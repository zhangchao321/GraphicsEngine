#include "test.hpp"

namespace
{
	std::string const SAMPLE_VERTEX_SHADER("gl-400/tess-struct.vert");
	std::string const SAMPLE_CONTROL_SHADER("gl-400/tess-struct.cont");
	std::string const SAMPLE_EVALUATION_SHADER("gl-400/tess-struct.eval");
	std::string const SAMPLE_GEOMETRY_SHADER("gl-400/tess-struct.geom");
	std::string const SAMPLE_FRAGMENT_SHADER("gl-400/tess-struct.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fc4f);
	glf::vertex_v2fc4f const VertexData[VertexCount] =
	{
		glf::vertex_v2fc4f(glm::vec2(-1.0f,-1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		glf::vertex_v2fc4f(glm::vec2( 1.0f,-1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)),
		glf::vertex_v2fc4f(glm::vec2( 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		glf::vertex_v2fc4f(glm::vec2(-1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
	};

	GLuint ProgramName(0);
	GLuint ArrayBufferName(0);
	GLuint VertexArrayName(0);
	GLint UniformMVP(0);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-program-varying-structs", framework::CORE, 4, 0)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
	
		// Create program
		if(Validated)
		{
			compiler Compiler;
			GLuint VertexShader = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERTEX_SHADER);
			GLuint ControlShader = Compiler.create(GL_TESS_CONTROL_SHADER, getDataDirectory() + SAMPLE_CONTROL_SHADER);
			GLuint EvaluationShader = Compiler.create(GL_TESS_EVALUATION_SHADER, getDataDirectory() + SAMPLE_EVALUATION_SHADER);
			GLuint GeometryShader = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOMETRY_SHADER);
			GLuint FragmentShader = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAGMENT_SHADER);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertexShader);
			glAttachShader(ProgramName, ControlShader);
			glAttachShader(ProgramName, EvaluationShader);
			glAttachShader(ProgramName, GeometryShader);
			glAttachShader(ProgramName, FragmentShader);
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		// Build a vertex array object
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fc4f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fc4f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::COLOR);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initArrayBuffer()
	{
		glGenBuffers(1, &ArrayBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initArrayBuffer");
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initArrayBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(1, &ArrayBufferName);
		glDeleteProgram(ProgramName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(VertexArrayName);
		glPatchParameteri(GL_PATCH_VERTICES, VertexCount);
		glDrawArraysInstanced(GL_PATCHES, 0, VertexCount, 1);

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

