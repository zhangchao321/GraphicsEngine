#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-500/shader-invocation.vert");
	char const* FRAG_SHADER_SOURCE("gl-500/shader-invocation.frag");

	namespace program
	{
		enum type
		{
			VERTEX,
			FRAGMENT,
			MAX
		};
	}//namespace program

	namespace buffer
	{
		enum type
		{
			CONSTANT,
			TRANSFORM,
			VERTEX,
			MAX
		};
	}//namespace buffer

	std::size_t const QuadOverlapCount = 1;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-500-shader-invocation-nv", framework::CORE, 4, 4, glm::uvec2(1280, 720), glm::vec2(0), glm::vec2(0), 2, framework::RUN_ONLY),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0)
	{}

private:
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint VertexCount;
	std::array<GLuint, buffer::MAX> BufferName;

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
			glCreateProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(1, &VertexArrayName);
		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::POSITION);

		glVertexArrayVertexBuffer(VertexArrayName, 0, BufferName[buffer::VERTEX], 0, sizeof(glm::vec2));

		return true;
	}

	bool initBuffer()
	{
		glCreateBuffers(buffer::MAX, &BufferName[0]);

		glm::vec2 WindowSizeF(this->getWindowSize());
		glm::mat4 const MVP = glm::ortho(0.0f, WindowSizeF.x * 1.0f, 0.0f, WindowSizeF.y * 1.0f);
		//glm::mat4 const MVP(1.0f);//glm::ortho(0.0f, WindowSizeF.x * 1.0f, 0.0f, WindowSizeF.y * 1.0f);

		glNamedBufferStorage(BufferName[buffer::TRANSFORM], static_cast<GLsizeiptr>(sizeof(MVP)), &MVP[0], 0);

		std::array<GLint, 3> Constants;
		glGetIntegerv(GL_WARP_SIZE_NV, &Constants[0]);
		glGetIntegerv(GL_WARPS_PER_SM_NV, &Constants[1]);
		glGetIntegerv(GL_SM_COUNT_NV, &Constants[2]);

		glNamedBufferStorage(BufferName[buffer::CONSTANT], static_cast<GLsizeiptr>(sizeof(GLint) * Constants.size()), &Constants[0], 0);

		glm::uvec2 WindowSize(this->getWindowSize());

		glm::uint const WindowDiv = 1;

		this->VertexCount =  (WindowSize.x / WindowDiv) * (WindowSize.y / WindowDiv) * 6 * QuadOverlapCount;
		std::vector<glm::vec2> VertexPosition(this->VertexCount);
		for(std::size_t QuadCoordIndexY = 0, QuadCoordCountY = WindowSize.y / WindowDiv; QuadCoordIndexY < QuadCoordCountY; ++QuadCoordIndexY)
		for(std::size_t QuadCoordIndexX = 0, QuadCoordCountX = WindowSize.x / WindowDiv; QuadCoordIndexX < QuadCoordCountX; ++QuadCoordIndexX)
		{
			for(std::size_t QuadOverlapIndex = 0; QuadOverlapIndex < QuadOverlapCount; ++QuadOverlapIndex)
			{
				//std::size_t const QuadIndex = (QuadCoordIndexY + QuadCoordCountY * QuadCoordIndexX) * QuadOverlapCount + QuadOverlapIndex;
				std::size_t const QuadIndex = (QuadCoordIndexX + QuadCoordCountX * QuadCoordIndexY) * QuadOverlapCount + QuadOverlapIndex;
				VertexPosition[QuadIndex * 6 + 0] = glm::vec2(QuadCoordIndexX * 1 + 0, QuadCoordIndexY * 1 + 0);
				VertexPosition[QuadIndex * 6 + 1] = glm::vec2(QuadCoordIndexX * 1 + 1, QuadCoordIndexY * 1 + 0);
				VertexPosition[QuadIndex * 6 + 2] = glm::vec2(QuadCoordIndexX * 1 + 1, QuadCoordIndexY * 1 + 1);
				VertexPosition[QuadIndex * 6 + 3] = glm::vec2(QuadCoordIndexX * 1 + 0, QuadCoordIndexY * 1 + 0);
				VertexPosition[QuadIndex * 6 + 4] = glm::vec2(QuadCoordIndexX * 1 + 1, QuadCoordIndexY * 1 + 1);
				VertexPosition[QuadIndex * 6 + 5] = glm::vec2(QuadCoordIndexX * 1 + 0, QuadCoordIndexY * 1 + 1);
			}
		}

		glNamedBufferStorage(BufferName[buffer::VERTEX], static_cast<GLsizeiptr>(sizeof(glm::vec2) * VertexPosition.size()), &VertexPosition[0], 0);

		return true;
	}

	bool begin()
	{
		bool Validated = this->checkExtension("GL_NV_shader_thread_group");

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
		glDeleteProgramPipelines(1, &PipelineName);
		glDeleteProgram(ProgramName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::uvec2 WindowSize(this->getWindowSize());

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glBindProgramPipeline(PipelineName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::CONSTANT, BufferName[buffer::CONSTANT]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6 * this->VertexCount, 1, 0);

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
