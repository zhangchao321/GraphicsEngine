#include "test.hpp"

namespace
{
	char const* VERTEX_SHADER_SOURCE("es-300/flat-color.vert");
	char const* FRAGMENT_SHADER_SOURCE("es-300/flat-color.frag");
	char const* FRAGMENT_SHADER_SOURCE_FAIL("es-300/flat-color-fail.frag");

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(glm::uint32);
	glm::uint32 const ElementData[ElementCount] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLuint VertexArrayName = 0;
	GLuint ProgramName = 0;
	GLuint ArrayBufferName = 0;
	GLuint ElementBufferName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "es-300-draw-elements", framework::ES, 3, 0)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;

		// Check fail positive
		if(Validated)
		{
			compiler Compiler;
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE_FAIL);

			Validated = Validated && !Compiler.check();
		}

		// Create program
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERTEX_SHADER_SOURCE);
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		// Set some variables 
		if(Validated)
		{
			// Bind the program for use
			glUseProgram(ProgramName);

			// Set uniform value
			glUniform4fv(UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

			// Unbind the program
			glUseProgram(0);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &ArrayBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ElementBufferName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
				glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);

			glEnableVertexAttribArray(semantic::attr::POSITION);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool begin()
	{
		bool Validated(true);

		char const* Vendor = (char const *)glGetString(GL_VENDOR);
		printf(Vendor);
		char const* Renderer = (char const *)glGetString(GL_RENDERER);
		printf(Renderer);
		char const* Version = (char const *)glGetString(GL_VERSION);
		printf(Version);
		char const* Extensions = (char const *)glGetString(GL_EXTENSIONS);
		printf(Extensions);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		// Delete objects
		glDeleteBuffers(1, &ArrayBufferName);
		glDeleteBuffers(1, &ElementBufferName);
		glDeleteProgram(ProgramName);

		return true;
	}

	bool render()
	{
		GLenum const Buffers = GL_BACK;
		glDrawBuffers(1, &Buffers);

		// Compute the MVP (Model View Projection matrix)
		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		// Set the display viewport
		glm::uvec2 WindowSize = this->getWindowSize();
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		// Clear color buffer with black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepthf(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind program
		glUseProgram(ProgramName);

		// Set the value of MVP uniform.
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(VertexArrayName);

		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);

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






