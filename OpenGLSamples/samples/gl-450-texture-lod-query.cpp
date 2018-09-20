#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-450/texture-lod-query.vert");
	char const* FRAG_SHADER_LIBRARY("gl-450/sampler-library.frag");
	char const* FRAG_SHADER_SOURCE_PROG("gl-450/texture-lod-query.frag");
	char const* FRAG_SHADER_SOURCE_FUNC("gl-450/texture-lod-query-builtin.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");
	float const FRAMEBUFFER_SCALE = 0.125f;

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-256.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-256.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 256.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 256.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
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

	namespace program
	{
		enum type
		{
			FUNC,
			PROG,
			MAX
		};
	}//namespace program

	namespace texture
	{
		enum type
		{
			DIFFUSE,
			COLORBUFFER,
			MAX
		};
	}//namespace texture
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-450-texture-lod-query", framework::CORE, 4, 5, glm::uvec2(640, 480), glm::vec2(0, 0), glm::vec2(0, 8)),
		VertexArrayName(0)
	{
		this->setupView(true, true, false);
	}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, program::MAX> PipelineName;
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, texture::MAX> TextureName;
	GLuint VertexArrayName;
	GLuint FramebufferName;

	bool initProgram()
	{
		bool Validated = true;

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 450 --profile core");
			GLuint FragShaderFuncName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_FUNC, "--version 450 --profile core");
			GLuint FragShaderProgName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_PROG, "--version 450 --profile core");
			GLuint LibShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_LIBRARY, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[program::PROG] = glCreateProgram();
			glProgramParameteri(ProgramName[program::PROG] , GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::PROG], VertShaderName);
			glAttachShader(ProgramName[program::PROG], LibShaderName);
			glAttachShader(ProgramName[program::PROG], FragShaderProgName);
			glLinkProgram(ProgramName[program::PROG]);

			ProgramName[program::FUNC] = glCreateProgram();
			glProgramParameteri(ProgramName[program::FUNC] , GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::FUNC], VertShaderName);
			glAttachShader(ProgramName[program::FUNC], FragShaderFuncName);
			glLinkProgram(ProgramName[program::FUNC]);

			Validated = Validated && Compiler.check_program(ProgramName[program::PROG]);
			Validated = Validated && Compiler.check_program(ProgramName[program::FUNC]);
		}

		if(Validated)
		{
			glGenProgramPipelines(program::MAX, &PipelineName[0]);
			glUseProgramStages(PipelineName[program::PROG], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::PROG]);
			glUseProgramStages(PipelineName[program::FUNC], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::FUNC]);
		}

		return Validated;
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

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		glGenTextures(texture::MAX, &TextureName[0]);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
		glTexStorage2D(GL_TEXTURE_2D, gli::levels(2048), GL_RGBA8, 2048, 2048);

		glm::vec2 WindowSize(this->getWindowSize());

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA8, GLsizei(WindowSize.x * FRAMEBUFFER_SCALE), GLsizei(WindowSize.y * FRAMEBUFFER_SCALE));

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		if(!this->checkFramebuffer(FramebufferName))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteProgram(ProgramName[program::FUNC]);
		glDeleteProgram(ProgramName[program::PROG]);
		glDeleteProgramPipelines(program::MAX, &PipelineName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 const WindowSize(this->getWindowSize());
		glm::vec2 const FramebufferSize(WindowSize * FRAMEBUFFER_SCALE);

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapBufferRange(GL_UNIFORM_BUFFER,
				0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			glm::mat4 const Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x * 0.5f / WindowSize.y, 0.1f, 1000.0f);
			glm::mat4 const Model = glm::rotate(glm::mat4(1.0f), -glm::pi<float>() * 0.47f, glm::vec3(1, 0, 0));

			*Pointer = Projection * this->view() * glm::scale(Model, glm::vec3(4.f));

			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferName);
		glViewportIndexedf(0, 0, 0, FramebufferSize.x, FramebufferSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);

		glBindProgramPipeline(PipelineName[program::PROG]);
		glViewportIndexedf(0, FramebufferSize.x * 0.5f * 0.0f, 0, FramebufferSize.x * 0.5f, FramebufferSize.y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0, 0);

		glBindProgramPipeline(PipelineName[program::FUNC]);
		glViewportIndexedf(0, FramebufferSize.x * 0.5f * 1.0f, 0, FramebufferSize.x * 0.5f, FramebufferSize.y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0, 0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FramebufferName);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, FramebufferSize.x, FramebufferSize.y,
			0, 0, WindowSize.x, WindowSize.y,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

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



