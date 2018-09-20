#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/draw-range-elements.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/draw-range-elements.frag");

	GLsizei const VertexCount(8);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const VertexData[VertexCount] =
	{
		glm::vec2(-0.4f,-0.6f),
		glm::vec2( 0.5f,-0.4f),
		glm::vec2( 0.6f, 0.4f),
		glm::vec2(-0.5f, 0.5f),
		glm::vec2(-0.5f,-0.5f),
		glm::vec2( 0.5f,-0.5f),
		glm::vec2( 0.5f, 0.5f),
		glm::vec2(-0.5f, 0.5f)
	};

	GLsizei const ElementCount(12);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	GLuint ProgramName(0);
	GLuint VertexArrayName(0);
	std::vector<GLuint> BufferName(buffer::MAX);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-draw-range-elements", framework::CORE, 3, 2),
		VertexArrayName(0),
		ProgramName(0),
		UniformTransform(-1)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLint UniformTransform;

	bool initTest()
	{
		bool Validated = true;
		glEnable(GL_DEPTH_TEST);

		return Validated && this->checkError("initTest");
	}

	bool initProgram()
	{
		bool Validated = true;
	
		// Create program
		if(Validated)
		{
			compiler Compiler;
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
			glUniformBlockBinding(ProgramName, glGetUniformBlockIndex(ProgramName, "transform"), semantic::uniform::TRANSFORM0);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initTest();
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
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / 3.0f / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);

			*Pointer = Projection * this->view() * Model;
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));

		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH, 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

		glUseProgram(ProgramName);

		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		glViewport(static_cast<GLint>(WindowSize.x * 0 / 3), 0, static_cast<GLsizei>(WindowSize.x / 3), static_cast<GLsizei>(WindowSize.y));
		glDrawElements(GL_TRIANGLES, ElementCount / 2, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

		glViewport(static_cast<GLint>(WindowSize.x * 1 / 3), 0, static_cast<GLsizei>(WindowSize.x / 3), static_cast<GLsizei>(WindowSize.y));
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount / 2, GL_UNSIGNED_SHORT, BUFFER_OFFSET(sizeof(GLushort) * ElementCount / 2), 1, 0);

		glViewport(static_cast<GLint>(WindowSize.x * 2 / 3), 0, static_cast<GLsizei>(WindowSize.x / 3), static_cast<GLsizei>(WindowSize.y));
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount / 2, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0), 1, VertexCount / 2);

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


