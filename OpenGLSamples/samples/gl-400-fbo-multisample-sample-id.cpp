#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_RENDER("gl-400/fbo-multisample-render.vert");
	char const* FRAG_SHADER_SOURCE_RENDER("gl-400/fbo-multisample-render.frag");
	char const* VERT_SHADER_SOURCE_BLIT("gl-400/fbo-multisample-blit.vert");
	char const* FRAG_SHADER_SOURCE_BLIT("gl-400/fbo-multisample-blit.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");
	glm::ivec2 const FRAMEBUFFER_SIZE(80, 60);

	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f))
	};

	namespace framebuffer
	{
		enum type
		{
			RENDER,
			RESOLVE,
			MAX
		};
	}//namespace framebuffer

	namespace program
	{
		enum type
		{
			RENDER,
			BLIT,
			MAX
		};
	}//namespace program

	namespace texture
	{
		enum type
		{
			TEXTURE,
			COLORBUFFER,
			MULTISAMPLE,
			MAX
		};
	}//namespace texture

	namespace buffer
	{
		enum type
		{
			VERTEX,
			MAX
		};
	}//namespace buffer
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-400-fbo-multisample-sample-id", framework::CORE, 4, 0)
	{}

private:
	std::array<GLint, program::MAX> UniformMVP;
	std::array<GLint, program::MAX> UniformDiffuse;
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, program::MAX> VertexArrayName;
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;

	bool initProgram()
	{
		bool Validated = true;

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_RENDER, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_RENDER, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[program::RENDER] = glCreateProgram();
			glAttachShader(ProgramName[program::RENDER], VertShaderName);
			glAttachShader(ProgramName[program::RENDER], FragShaderName);
			glLinkProgram(ProgramName[program::RENDER]);
			Validated = Validated && Compiler.check_program(ProgramName[program::RENDER]);
		}

		if(Validated)
		{
			UniformMVP[program::RENDER] = glGetUniformLocation(ProgramName[program::RENDER], "MVP");
			UniformDiffuse[program::RENDER] = glGetUniformLocation(ProgramName[program::RENDER], "Diffuse");
		}

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_BLIT, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_BLIT, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[program::BLIT] = glCreateProgram();
			glAttachShader(ProgramName[program::BLIT], VertShaderName);
			glAttachShader(ProgramName[program::BLIT], FragShaderName);
			glLinkProgram(ProgramName[program::BLIT]);
			Validated = Validated && Compiler.check_program(ProgramName[program::BLIT]);
		}

		if(Validated)
		{
			UniformMVP[program::BLIT] = glGetUniformLocation(ProgramName[program::BLIT], "MVP");
			UniformDiffuse[program::BLIT] = glGetUniformLocation(ProgramName[program::BLIT], "Diffuse");
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		assert(!Texture.empty());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glActiveTexture(GL_TEXTURE0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(texture::MAX, &this->TextureName[texture::TEXTURE]);

		glBindTexture(GL_TEXTURE_2D, this->TextureName[texture::TEXTURE]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		for(gli::texture2d::size_type Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}

		glBindTexture(GL_TEXTURE_2D, this->TextureName[texture::COLORBUFFER]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->TextureName[texture::MULTISAMPLE]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, GL_FALSE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(framebuffer::MAX, &this->FramebufferName[0]);

		glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferName[framebuffer::RENDER]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->TextureName[texture::MULTISAMPLE], 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferName[framebuffer::RESOLVE]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->TextureName[texture::COLORBUFFER], 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(program::MAX, &this->VertexArrayName[0]);

		glBindVertexArray(this->VertexArrayName[program::RENDER]);
			glBindBuffer(GL_ARRAY_BUFFER, this->BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayName[program::BLIT]);
		glBindVertexArray(0);

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
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteProgram(this->ProgramName[program::RENDER]);
		glDeleteProgram(this->ProgramName[program::BLIT]);
		glDeleteTextures(texture::MAX, &this->TextureName[0]);
		glDeleteBuffers(buffer::MAX, &this->BufferName[0]);
		glDeleteFramebuffers(framebuffer::MAX, &this->FramebufferName[0]);
		glDeleteVertexArrays(program::MAX, &this->VertexArrayName[0]);

		return true;
	}

	void renderFBO()
	{
		glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::rotate(glm::mat4(1), glm::pi<float>() * 0.1f, glm::normalize(glm::vec3(1)));
		glm::mat4 MVP = Perspective * this->view() * Model;

		glUseProgram(this->ProgramName[program::RENDER]);
		glUniformMatrix4fv(this->UniformMVP[program::RENDER], 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(this->UniformDiffuse[program::RENDER], 0);

		glViewport(0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
		glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferName[framebuffer::RENDER]);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TextureName[texture::TEXTURE]);
		glBindVertexArray(this->VertexArrayName[program::RENDER]);

		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	}

	bool render()
	{
		glm::vec2 const WindowSize(this->getWindowSize());

		// Pass 1, render the scene in a multisampled framebuffer
		renderFBO();

		// Resolved multisampling
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FramebufferName[framebuffer::RENDER]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FramebufferName[framebuffer::RESOLVE]);
		glBlitFramebuffer(
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y,
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FramebufferName[framebuffer::RESOLVE]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y,
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

