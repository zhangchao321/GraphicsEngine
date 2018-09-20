#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-460/atomic-counter.vert");
	char const* FRAG_SHADER_SOURCE("gl-460/atomic-counter.frag");

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
		framework(argc, argv, "gl-460-atomic-counter", framework::CORE, 4, 6, glm::uvec2(1280, 720), glm::vec2(0), glm::vec2(0), 2, framework::RUN_ONLY),
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
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 460 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 460 --profile core");
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
		bool Validated(true);

		GLint MaxVertexAtomicCounterBuffers(0);
		GLint MaxControlAtomicCounterBuffers(0);
		GLint MaxEvaluationAtomicCounterBuffers(0);
		GLint MaxGeometryAtomicCounterBuffers(0);
		GLint MaxFragmentAtomicCounterBuffers(0);
		GLint MaxCombinedAtomicCounterBuffers(0);

		glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, &MaxVertexAtomicCounterBuffers);
		glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS, &MaxControlAtomicCounterBuffers);
		glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS, &MaxEvaluationAtomicCounterBuffers);
		glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS, &MaxGeometryAtomicCounterBuffers);
		glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, &MaxFragmentAtomicCounterBuffers);
		glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, &MaxCombinedAtomicCounterBuffers);

		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, BufferName[buffer::ATOMIC_COUNTER]);
		glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_MAP_WRITE_BIT);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		return Validated;
	}

	bool initVertexArray()
	{
		bool Validated(true);

		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
		glBindVertexArray(0);

		return Validated;
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

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, BufferName[buffer::ATOMIC_COUNTER]);
		glm::uint Data(0);
		glClearBufferSubData(GL_ATOMIC_COUNTER_BUFFER, GL_R8UI, 0, sizeof(glm::uint), GL_RGBA, GL_UNSIGNED_INT, &Data);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

		glBindProgramPipeline(PipelineName);
		glBindVertexArray(VertexArrayName);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, BufferName[buffer::ATOMIC_COUNTER]);

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

