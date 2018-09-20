#include "test.hpp"

namespace
{
	char const* CS_SOURCE("gl-430/glsl-std140.comp");

	namespace semantics
	{
		enum type
		{
			INPUT,
			OUTPUT,
			MAX
		};
	}//namespace semantics

	GLsizei const DataSize = 16;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-430-glsl-std140", framework::CORE, 4, 3)
	{}

private:
	GLuint PipelineName;
	GLuint ProgramName;
	std::array<GLuint, semantics::MAX> BufferName;

	bool initProgram()
	{
		bool Validated(true);

		compiler Compiler;

		if(Validated)
		{
			GLuint ComputeShaderName = Compiler.create(GL_COMPUTE_SHADER, getDataDirectory() + CS_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, ComputeShaderName);
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glLinkProgram(ProgramName);
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glGenProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_COMPUTE_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initBuffer()
	{
		std::vector<glm::uint> Input(DataSize, 0);
		for(std::size_t i = 0, n = Input.size(); i < n; ++i)
			Input[i] = i;

		glGenBuffers(semantics::MAX, &BufferName[0]);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[semantics::INPUT]);
		glBufferData(GL_ARRAY_BUFFER, Input.size() * sizeof(glm::uint), &Input[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[semantics::OUTPUT]);
		glBufferData(GL_ARRAY_BUFFER, Input.size() * sizeof(glm::uint), nullptr, GL_STATIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	bool begin()
	{
		bool Validated(true);
		Validated = Validated && this->checkExtension("GL_ARB_compute_shader");

		this->logImplementationDependentLimit(GL_MAX_COMPUTE_UNIFORM_BLOCKS, "GL_MAX_COMPUTE_UNIFORM_BLOCKS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_IMAGE_UNIFORMS, "GL_MAX_COMPUTE_IMAGE_UNIFORMS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, "GL_MAX_COMPUTE_SHARED_MEMORY_SIZE");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_ATOMIC_COUNTERS, "GL_MAX_COMPUTE_ATOMIC_COUNTERS");
		this->logImplementationDependentLimit(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS");
		this->logImplementationDependentLimit(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS");

		//this->logImplementationDependentLimit(GL_MAX_COMPUTE_WORK_GROUP_COUNT, "GL_MAX_COMPUTE_WORK_GROUP_COUNT");
		//this->logImplementationDependentLimit(GL_MAX_COMPUTE_WORK_GROUP_SIZE, "GL_MAX_COMPUTE_WORK_GROUP_SIZE");

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteProgramPipelines(1, &PipelineName);
		glDeleteProgram(ProgramName);
		glDeleteBuffers(semantics::MAX, &BufferName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());
		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glBindProgramPipeline(PipelineName);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantics::INPUT, BufferName[semantics::INPUT]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantics::OUTPUT, BufferName[semantics::OUTPUT]);
		glDispatchCompute(1, 1, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		std::vector<GLuint> Results(DataSize);
		glBindBuffer(GL_COPY_READ_BUFFER, BufferName[semantics::OUTPUT]);
		glm::uint* Pointer = (glm::uint*)glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(glm::uint) * Results.size(), GL_MAP_READ_BIT);
		memcpy(&Results[0], Pointer, Results.size() * sizeof(glm::uint));
		glUnmapBuffer(GL_COPY_READ_BUFFER);

		std::array<glm::uint, 14> const Expect{0u,  4u,  5u,  6u,  8u,  9u,  10u};

		for(std::size_t i = 0, n = Expect.size(); i < n; ++i)
		{
			if(Expect[i] != Results[i])
				return false;
		}

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
