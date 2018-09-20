#include "test.hpp"

namespace
{
	std::string const SAMPLE_VERT_SHADER("gl-450/primitive-tessellation-eval.vert");
	std::string const SAMPLE_EVAL_SHADER("gl-450/primitive-tessellation-eval.eval");
	std::string const SAMPLE_FRAG_SHADER("gl-450/primitive-tessellation-eval.frag");

	GLsizei const VertexCount = 1;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fc4f);
	glf::vertex_v2fc4f const VertexData[VertexCount] =
	{
		glf::vertex_v2fc4f(glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
	};

	namespace program
	{
		enum type
		{
			VERT,
			EVAL,
			FRAG,
			MAX
		};
	}//namespace program
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-450-primitive-tessellation-eval", framework::CORE, 4, 5)
		, ProgramName(0)
		, PipelineName(0)
		, ArrayBufferName(0)
		, VertexArrayName(0)
		, UniformMVP(-1)
	{}

private:
	GLuint ProgramName;
	GLuint PipelineName;
	GLuint ArrayBufferName;
	GLuint VertexArrayName;
	GLint UniformMVP;

	bool initProgram()
	{
		bool Validated = true;
	
		glGenProgramPipelines(1, &PipelineName);
		glBindProgramPipeline(PipelineName);
		glBindProgramPipeline(0);

		// Create program
		if(Validated)
		{
			compiler Compiler;
			GLuint ShaderName[] = {
				Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERT_SHADER, "--version 450 --profile core"),
				Compiler.create(GL_TESS_EVALUATION_SHADER, getDataDirectory() + SAMPLE_EVAL_SHADER, "--version 450 --profile core"),
				Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAG_SHADER, "--version 450 --profile core")};
			Validated = Validated && Compiler.check();

			this->ProgramName = glCreateProgram();
			glProgramParameteri(this->ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);

			for(std::size_t i = 0; i < program::MAX; ++i)
				glAttachShader(this->ProgramName, ShaderName[i]);

			glLinkProgram(this->ProgramName);

			Validated = Validated && Compiler.check_program(this->ProgramName);
		}

		if(Validated)
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);

		if(Validated)
			UniformMVP = glGetUniformLocation(this->ProgramName, "MVP");

		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(1, &this->VertexArrayName);

		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::COLOR, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::COLOR);

		glVertexArrayVertexBuffer(VertexArrayName, 0, ArrayBufferName, 0, sizeof(glf::vertex_v2fc4f));

		return this->checkError("initVertexArray");
	}

	bool initBuffer()
	{
		glCreateBuffers(1, &ArrayBufferName);
		glNamedBufferStorage(ArrayBufferName, VertexSize, VertexData, 0);

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

		return Validated;
	}

	bool end()
	{
		glDeleteProgramPipelines(1, &this->PipelineName);
		glDeleteVertexArrays(1, &this->VertexArrayName);
		glDeleteBuffers(1, &this->ArrayBufferName);
		glDeleteProgram(this->ProgramName);

		return true;
	}

	bool render()
	{
		glm::vec2 const WindowSize(this->getWindowSize());

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glm::mat4 const Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
		glm::mat4 const Model = glm::mat4(1.0f);
		glm::mat4 const MVP = Projection * this->view() * Model;

		glProgramUniformMatrix4fv(this->ProgramName, this->UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glViewportIndexedfv(0, &glm::vec4(0, 0, WindowSize.x, WindowSize.y)[0]);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glBindProgramPipeline(this->PipelineName);

		glBindVertexArray(this->VertexArrayName);
		float const InnerLevel[] = {16.f, 16.f};
		float const OuterLevel[] = {8.f, 8.f, 8.f, 8.f};
		glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, InnerLevel);
		glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, OuterLevel);
		glPatchParameteri(GL_PATCH_VERTICES, VertexCount);
		glDrawArraysInstancedBaseInstance(GL_PATCHES, 0, VertexCount, 1, 0);

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

