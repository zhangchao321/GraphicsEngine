#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE1("gl-500/fbo-layered-layer-nv.vert");
	char const* FRAG_SHADER_SOURCE1("gl-500/fbo-layered-layer-nv.frag");
	char const* VERT_SHADER_SOURCE2("gl-500/fbo-layered-viewport-nv.vert");
	char const* FRAG_SHADER_SOURCE2("gl-500/fbo-layered-viewport-nv.frag");
	glm::ivec2 const FRAMEBUFFER_SIZE(640, 480);

	GLsizei const VertexCount(3);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 3.0f,-1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 3.0f), glm::vec2(0.0f, 1.0f))
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace pipeline
	{
		enum type
		{
			LAYERING,
			VIEWPORT,
			MAX
		};
	}//namespace pipeline
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-500-fbo-layered-nv", framework::CORE, 4, 5)
		, FramebufferName(0)
		, SamplerName(0)
		, TextureColorbufferName(0)
	{
		this->VertexArrayName.fill(0);
		this->ProgramName.fill(0);
		this->PipelineName.fill(0);
		this->UniformMVP.fill(0);
		this->BufferName.fill(0);
	}

private:
	GLuint FramebufferName;
	GLuint SamplerName;
	GLuint TextureColorbufferName;
	std::array<GLuint, pipeline::MAX> VertexArrayName;
	std::array<GLuint, pipeline::MAX> ProgramName;
	std::array<GLuint, pipeline::MAX> PipelineName;
	std::array<GLint, pipeline::MAX> UniformMVP;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<glm::vec4, 4> Viewport;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE1, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE1, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::LAYERING] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::LAYERING], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::LAYERING], VertShaderName);
			glAttachShader(ProgramName[pipeline::LAYERING], FragShaderName);
			glLinkProgram(ProgramName[pipeline::LAYERING]);

			Validated = Validated && Compiler.check_program(ProgramName[pipeline::LAYERING]);
		}

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE2, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE2, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::VIEWPORT] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::VIEWPORT], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::VIEWPORT], VertShaderName);
			glAttachShader(ProgramName[pipeline::VIEWPORT], FragShaderName);
			glLinkProgram(ProgramName[pipeline::VIEWPORT]);

			Validated = Validated && Compiler.check_program(ProgramName[pipeline::VIEWPORT]);
		}

		if(Validated)
		{
			glCreateProgramPipelines(GLsizei(pipeline::MAX), &PipelineName[0]);
			glUseProgramStages(PipelineName[pipeline::VIEWPORT], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::VIEWPORT]);
			glUseProgramStages(PipelineName[pipeline::LAYERING], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::LAYERING]);
		}

		if(Validated)
		{
			for(std::size_t i = 0; i < pipeline::MAX; ++i)
				UniformMVP[i] = glGetUniformLocation(ProgramName[i], "MVP");
		}

		return Validated;
	}

	bool initBuffer()
	{
		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::VERTEX], VertexSize, VertexData, 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], sizeof(glm::mat4), nullptr, GL_MAP_WRITE_BIT);

		return true;
	}

	bool initTexture()
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &TextureColorbufferName);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
		glTextureParameteri(TextureColorbufferName, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
		glTextureStorage3D(TextureColorbufferName, static_cast<GLsizei>(1), GL_RGBA8, GLsizei(FRAMEBUFFER_SIZE.x), GLsizei(FRAMEBUFFER_SIZE.y), 4);

		return true;
	}

	bool initSampler()
	{
		glCreateSamplers(1, &SamplerName);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glSamplerParameterfv(SamplerName, GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
		glSamplerParameterf(SamplerName, GL_TEXTURE_MIN_LOD, -1000.f);
		glSamplerParameterf(SamplerName, GL_TEXTURE_MAX_LOD, 1000.f);
		glSamplerParameterf(SamplerName, GL_TEXTURE_LOD_BIAS, 0.0f);
		glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		return true;
	}

	bool initFramebuffer()
	{
		glCreateFramebuffers(1, &FramebufferName);
		glNamedFramebufferTexture(FramebufferName, GL_COLOR_ATTACHMENT0, TextureColorbufferName, 0);

		return glCheckNamedFramebufferStatus(FramebufferName, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(pipeline::MAX, &VertexArrayName[0]);

		glVertexArrayAttribBinding(VertexArrayName[pipeline::VIEWPORT], semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName[pipeline::VIEWPORT], semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName[pipeline::VIEWPORT], semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName[pipeline::VIEWPORT], semantic::attr::TEXCOORD, 0);
		glVertexArrayAttribFormat(VertexArrayName[pipeline::VIEWPORT], semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName[pipeline::VIEWPORT], semantic::attr::TEXCOORD);

		glVertexArrayVertexBuffer(VertexArrayName[pipeline::VIEWPORT], 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));

		return true;
	}

	bool begin()
	{
		bool Validated = true;
		Validated = Validated && this->checkExtension("GL_NV_viewport_array2");

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();
		if(Validated)
			Validated = initSampler();

		GLint const Border = 2;
		glm::vec2 const WindowSize(this->getWindowSize());
		this->Viewport[0] = glm::vec4(Border, Border, WindowSize * 0.5f - 2.0f * Border);
		this->Viewport[1] = glm::vec4((WindowSize.x * 0.5f) + Border, Border, WindowSize * 0.5f - 2.0f * Border);
		this->Viewport[2] = glm::vec4((WindowSize.x * 0.5f) + Border, (WindowSize.y * 0.5f) + 1, WindowSize * 0.5f - 2.0f * Border);
		this->Viewport[3] = glm::vec4(Border, (WindowSize.y * 0.5f) + Border, WindowSize * 0.5f - 2.0f * Border);

		return Validated;
	}

	bool end()
	{
		glDeleteVertexArrays(pipeline::MAX, &VertexArrayName[0]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(1, &TextureColorbufferName);
		glDeleteFramebuffers(1, &FramebufferName);
		glDeleteProgram(ProgramName[pipeline::VIEWPORT]);
		glDeleteProgram(ProgramName[pipeline::LAYERING]);
		glDeleteSamplers(1, &SamplerName);

		return true;
	}

	bool render()
	{
		{
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapNamedBufferRange(BufferName[buffer::TRANSFORM], 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, 1.0f,-1.0f, 1.0f, -1.0f);
			glm::mat4 View = glm::mat4(1.0f);
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;
		
			*Pointer = MVP;

			glUnmapNamedBuffer(BufferName[buffer::TRANSFORM]);
		}

		glBindBufferRange(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM], 0, sizeof(glm::mat4));

		// Pass 1
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			glBindProgramPipeline(PipelineName[pipeline::LAYERING]);
			glBindVertexArray(VertexArrayName[pipeline::LAYERING]);

			glViewportIndexedfv(0, &glm::vec4(0, 0, FRAMEBUFFER_SIZE)[0]);

			glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, VertexCount, 4, 0);
		}

		// Pass 2
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindProgramPipeline(PipelineName[pipeline::VIEWPORT]);
			glBindTextureUnit(0, TextureColorbufferName);
			glBindSampler(0, SamplerName);
			glBindVertexArray(VertexArrayName[pipeline::VIEWPORT]);

			glViewportArrayv(0, 4, &this->Viewport[0][0]);

			glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, VertexCount, 1, 0);
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

