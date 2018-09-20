#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-330/flat-color.vert");
	char const* FRAG_SHADER_SOURCE("gl-330/flat-color.frag");

	GLsizei const VertexCount(6);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(-1.0f,-1.0f)
	};

	GLuint VertexArrayName = 0;
	GLuint ProgramName = 0;
	GLuint BufferName = 0;
	GLuint QueryName = 0;
	GLint UniformMVP = 0;
	GLint UniformColor = 0;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-330-query-timer", framework::CORE, 3, 3)
	{}

private:
	bool initQuery()
	{
		glGenQueries(1, &QueryName);

		int QueryBits(0);
		glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &QueryBits);

		bool Validated = QueryBits >= 30;

		return Validated && this->checkError("initQuery");
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 330 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 330 --profile core");
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
			UniformColor = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated && this->checkError("initProgram");
	}

	// Buffer update using glBufferSubData
	bool initArrayBuffer()
	{
		// Generate a buffer object
		glGenBuffers(1, &BufferName);

		// Bind the buffer for use
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);

		// Reserve buffer memory but and copy the values
		glBufferData(GL_ARRAY_BUFFER, PositionSize, &PositionData[0][0], GL_STATIC_DRAW);

		// Unbind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initArrayBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
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
			Validated = initVertexArray();
		if(Validated)
			Validated = initQuery();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
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

		// Beginning of the time query
		glBeginQuery(GL_TIME_ELAPSED, QueryName);

		// Set the display viewport
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		// Clear color buffer with black
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		// Bind program
		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform4fv(UniformColor, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glBindVertexArray(VertexArrayName);
		glDrawArrays(GL_TRIANGLES, 0, VertexCount);

		// Unbind program
		glUseProgram(0);

		// End of the time query
		glEndQuery(GL_TIME_ELAPSED);

		// Get the count of samples. 
		// If the result of the query isn't here yet, we wait here...
		GLuint Time = 0;
		glGetQueryObjectuiv(QueryName, GL_QUERY_RESULT, &Time);
		fprintf(stdout, "Time: %f ms   \r", Time / 1000.f / 1000.f);

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
