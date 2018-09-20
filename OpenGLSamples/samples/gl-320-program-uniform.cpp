#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/program-uniform.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/program-uniform.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MATERIAL,
			MAX
		};
	}//namespace buffer
	
	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader
	
	GLuint ProgramName(0);
	std::vector<GLuint> BufferName(buffer::MAX);
	GLuint VertexArrayName(0);
	GLint UniformTransform(0);
	GLint UniformDiffuse0(0);
	GLint UniformDiffuse1(0);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-program-uniform", framework::CORE, 3, 2)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;
	
		// Create program
		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			
			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformDiffuse0 = glGetUniformLocation(ProgramName, "Diffuse[0]");
			UniformDiffuse1 = glGetUniformLocation(ProgramName, "Diffuse[1]");
			UniformTransform = glGetUniformBlockIndex(ProgramName, "transform");
		}
	
		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		// Build a vertex array object
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initBuffer()
	{
		// Generate buffer objects
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBlockSize = 0;

		{
			glGetActiveUniformBlockiv(
				ProgramName, 
				UniformTransform,
				GL_UNIFORM_BLOCK_DATA_SIZE,
				&UniformBlockSize);

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		return this->checkError("initBuffer");
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0,	sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * this->view() * Model;
		
			*Pointer = MVP;
		
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glUseProgram(ProgramName);
		glUniformBlockBinding(ProgramName, UniformTransform, semantic::uniform::TRANSFORM0);
	
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glm::vec4 Diffuse[2] = {glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)};

		glUniform4fv(UniformDiffuse0, 1, &Diffuse[0][0]);
		glUniform4fv(UniformDiffuse1, 1, &Diffuse[1][0]);
		//glUniform4fv(UniformDiffuse + 0, 1, &Diffuse[0][0]);
		//glUniform4fv(UniformDiffuse + 1, 1, &Diffuse[1][0]);

		glBindVertexArray(VertexArrayName);
		glDrawElementsInstanced(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 1);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	sample Sample(argc, argv);
	Error += Sample();

	return Error;
}

