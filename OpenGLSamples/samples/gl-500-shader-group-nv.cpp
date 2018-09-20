#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_TEXTURE("gl-500/shader-group-nv.vert");
	char const* FRAG_SHADER_SOURCE_TEXTURE("gl-500/shader-group-nv.frag");
	char const* VERT_SHADER_SOURCE_BLIT("gl-500/shader-group-blit-nv.vert");
	char const* FRAG_SHADER_SOURCE_BLIT("gl-500/shader-group-blit-nv.frag");

	float const FRAMEBUFFER_SCALE = 1.0f / 8.0f;

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
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

	namespace texture
	{
		enum type
		{
			COLORBUFFER,
			RENDERBUFFER,
			INVOCATION_COUNT,
			MAX
		};
	}//namespace texture
	
	namespace pipeline
	{
		enum type
		{
			TEXTURE,
			SPLASH,
			MAX
		};
	}//namespace pipeline

	GLuint FramebufferName(0);
	std::vector<GLuint> ProgramName(pipeline::MAX);
	std::vector<GLuint> VertexArrayName(pipeline::MAX);
	std::vector<GLuint> BufferName(buffer::MAX);
	std::vector<GLuint> TextureName(texture::MAX);
	std::vector<GLuint> PipelineName(pipeline::MAX);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-500-shader-group-nv", framework::CORE, 4, 5, glm::vec2(glm::pi<float>() * 0.2f))
	{}

private:
	bool initProgram()
	{
		bool Validated(true);

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::TEXTURE] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::TEXTURE], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::TEXTURE], VertShaderName);
			glAttachShader(ProgramName[pipeline::TEXTURE], FragShaderName);
			glLinkProgram(ProgramName[pipeline::TEXTURE]);

			Validated = Validated && Compiler.check_program(ProgramName[pipeline::TEXTURE]);
		}

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_BLIT, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_BLIT, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[pipeline::SPLASH] = glCreateProgram();
			glProgramParameteri(ProgramName[pipeline::SPLASH], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[pipeline::SPLASH], VertShaderName);
			glAttachShader(ProgramName[pipeline::SPLASH], FragShaderName);
			glLinkProgram(ProgramName[pipeline::SPLASH]);

			Validated = Validated && Compiler.check_program(ProgramName[pipeline::SPLASH]);
		}

		if(Validated)
		{
			glGenProgramPipelines(pipeline::MAX, &PipelineName[0]);
			glUseProgramStages(PipelineName[pipeline::TEXTURE], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::TEXTURE]);
			glUseProgramStages(PipelineName[pipeline::SPLASH], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[pipeline::SPLASH]);
		}

		return Validated;
	}

	bool initBuffer()
	{
		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::ELEMENT], ElementSize, ElementData, 0);
		glNamedBufferStorage(BufferName[buffer::VERTEX], VertexSize, VertexData, 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT);

		return true;
	}

	bool initTexture()
	{
		glCreateTextures(GL_TEXTURE_2D, texture::MAX, &TextureName[0]);

		glm::vec2 const FramwbufferSize = glm::vec2(this->getWindowSize()) * FRAMEBUFFER_SCALE;

		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureStorage2D(TextureName[texture::COLORBUFFER], GLint(1), GL_RGBA8, GLsizei(FramwbufferSize.x), GLsizei(FramwbufferSize.y));

		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureStorage2D(TextureName[texture::RENDERBUFFER], GLint(1), GL_DEPTH_COMPONENT24, GLsizei(FramwbufferSize.x), GLsizei(FramwbufferSize.y));

		glTextureParameteri(TextureName[texture::INVOCATION_COUNT], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::INVOCATION_COUNT], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureStorage2D(TextureName[texture::INVOCATION_COUNT], GLint(1), GL_R32UI, GLsizei(FramwbufferSize.x), GLsizei(FramwbufferSize.y));

		return true;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(pipeline::MAX, &VertexArrayName[0]);

		glVertexArrayAttribBinding(VertexArrayName[pipeline::TEXTURE], semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName[pipeline::TEXTURE], semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName[pipeline::TEXTURE], semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName[pipeline::TEXTURE], semantic::attr::TEXCOORD, 0);
		glVertexArrayAttribFormat(VertexArrayName[pipeline::TEXTURE], semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName[pipeline::TEXTURE], semantic::attr::TEXCOORD);

		glVertexArrayElementBuffer(VertexArrayName[pipeline::TEXTURE], BufferName[buffer::ELEMENT]);
		glVertexArrayVertexBuffer(VertexArrayName[pipeline::TEXTURE], 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));

		return true;
	}

	bool initFramebuffer()
	{
		glCreateFramebuffers(1, &FramebufferName);
		glNamedFramebufferTexture(FramebufferName, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);
		glNamedFramebufferTexture(FramebufferName, GL_DEPTH_ATTACHMENT, TextureName[texture::RENDERBUFFER], 0);

		return glCheckNamedFramebufferStatus(FramebufferName, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	bool begin()
	{
		bool Validated = this->checkExtension("GL_NV_shader_thread_group");

		caps Caps(caps::CORE);

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

		return Validated;
	}

	bool end()
	{
		glDeleteProgramPipelines(pipeline::MAX, &PipelineName[0]);
		glDeleteProgram(ProgramName[pipeline::SPLASH]);
		glDeleteProgram(ProgramName[pipeline::TEXTURE]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteFramebuffers(1, &FramebufferName);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(pipeline::MAX, &VertexArrayName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glm::mat4* Pointer = reinterpret_cast<glm::mat4*>(glMapNamedBufferRange(BufferName[buffer::TRANSFORM],
				0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapNamedBuffer(BufferName[buffer::TRANSFORM]);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glClearTexImage(TextureName[texture::COLORBUFFER], 0, GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(255, 255, 255, 255)[0]);
		glClearTexImage(TextureName[texture::RENDERBUFFER], 0, GL_DEPTH_COMPONENT, GL_FLOAT, &glm::vec1(1.0)[0]);
		glClearTexImage(TextureName[texture::INVOCATION_COUNT], 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &glm::u32vec1(0)[0]);

		glViewportIndexedf(0, 0, 0, WindowSize.x * FRAMEBUFFER_SCALE, WindowSize.y * FRAMEBUFFER_SCALE);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glBindProgramPipeline(PipelineName[pipeline::TEXTURE]);
		glBindVertexArray(VertexArrayName[pipeline::TEXTURE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindImageTexture(0, TextureName[texture::INVOCATION_COUNT], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 5, 0, 0);

		glDisable(GL_DEPTH_TEST);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindProgramPipeline(PipelineName[pipeline::SPLASH]);
		glBindVertexArray(VertexArrayName[pipeline::SPLASH]);
		glBindImageTexture(0, TextureName[texture::INVOCATION_COUNT], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);

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


