#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-400/flat-color.vert");
	char const* FRAG_SHADER_SOURCE("gl-400/flat-color.frag");
	
	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(glm::uint32);
	glm::uint32 const ElementData[ElementCount] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLsizei const VertexCount = 4;
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	struct DrawArraysIndirectCommand
	{
		GLuint count;
		GLuint primCount;
		GLuint first;
		GLuint reservedMustBeZero;
	};

	struct DrawElementsIndirectCommand
	{
		GLuint count;
		GLuint primCount;
		GLuint firstIndex;
		GLint  baseVertex;
		GLuint reservedMustBeZero;
	};

	GLuint VertexArrayName = 0;
	GLuint ProgramName = 0;
	GLuint ArrayBufferName = 0;
	GLuint IndirectBufferName = 0;
	GLuint ElementBufferName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-draw-indirect", framework::CORE, 4, 0)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initIndirectBuffer()
	{
		DrawElementsIndirectCommand Command(ElementCount, 1, 0, 0, 0);

		glGenBuffers(1, &IndirectBufferName);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBufferName);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command), &Command, GL_STATIC_READ);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		return this->checkError("initIndirectBuffer");
	}

	bool initArrayBuffer()
	{
		glGenBuffers(1, &ArrayBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ElementBufferName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return this->checkError("initArrayBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
				glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			glEnableVertexAttribArray(semantic::attr::POSITION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initArrayBuffer();
		if(Validated)
			Validated = initIndirectBuffer();
		if(Validated)
			Validated = initVertexArray();
		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &ArrayBufferName);
		glDeleteBuffers(1, &IndirectBufferName);
		glDeleteBuffers(1, &ElementBufferName);
		glDeleteProgram(ProgramName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform4fv(UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBufferName);

		glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);

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

