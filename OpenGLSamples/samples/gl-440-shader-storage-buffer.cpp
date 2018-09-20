#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-440/shader-storage-buffer.vert");
	char const* FRAG_SHADER_SOURCE("gl-440/shader-storage-buffer.frag");

	namespace buffer
	{
		enum type
		{
			ATOMIC_COUNTER,
			MAX
		};
	}//namespace buffer
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-440-shader-storage-buffer", framework::CORE, 4, 4, glm::uvec2(2400, 1200), glm::vec2(0), glm::vec2(0), 2, framework::RUN_ONLY),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;

	bool initProgram()
	{
		bool Validated(true);
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 440 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 440 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glGenProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferName[buffer::ATOMIC_COUNTER]);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), nullptr, GL_MAP_WRITE_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);
		glDeleteProgramPipelines(1, &PipelineName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferName[buffer::ATOMIC_COUNTER]);
		glm::uint Data(0);
		glClearBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_R8UI, 0, sizeof(glm::uint), GL_RGBA, GL_UNSIGNED_INT, &Data);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

		glBindProgramPipeline(PipelineName);
		glBindVertexArray(VertexArrayName);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BufferName[buffer::ATOMIC_COUNTER]);

		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 3, 1, 0);

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

