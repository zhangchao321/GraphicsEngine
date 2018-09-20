#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/fbo-multisample.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/fbo-multisample.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");
	glm::ivec2 const FRAMEBUFFER_SIZE(160, 120);

	// With DDS textures, v texture coordinate are reversed, from top to bottom
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

	namespace texture
	{
		enum type
		{
			DIFFUSE,
			COLORBUFFER,
			MULTISAMPLE,
			MAX
		};
	}//namespace texture

	namespace framebuffer
	{
		enum type
		{
			RENDER,
			RESOLVE,
			MAX
		};
	}//namespace framebuffer

	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-320-fbo-multisample", framework::CORE, 3, 2, glm::vec2(glm::pi<float>() * 0.2f))
		, VertexArrayName(0)
		, BufferName(0)
		, ProgramName(0)
		, UniformMVP(0)
		, UniformDiffuse(0)
	{}

private:
	std::array<GLuint, texture::MAX> TextureName;
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	GLuint VertexArrayName;
	GLuint BufferName;
	GLuint ProgramName;
	GLint UniformMVP;
	GLint UniformDiffuse;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");;
	}

	bool initTexture()
	{
		glGenTextures(texture::MAX, &TextureName[0]);

		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TextureName[texture::MULTISAMPLE]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, Format.Internal, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, GL_TRUE); // The second parameter is the number of samples.
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		return this->checkError("initTexture");
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(framebuffer::MAX, &FramebufferName[0]);

		GLenum const BuffersRender = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDER]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::MULTISAMPLE], 0);
		glDrawBuffers(1, &BuffersRender);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		GLenum const BuffersResolve = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RESOLVE]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);
		glDrawBuffers(1, &BuffersResolve);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return this->checkError("initFramebuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
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

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteFramebuffers(framebuffer::MAX, &FramebufferName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	void renderFBO(GLuint Framebuffer)
	{
		glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		glm::mat4 MVP = Perspective * this->view() * Model;

		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glViewport(0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);

		glBindVertexArray(VertexArrayName);

		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	}

	void renderFB(GLuint TextureName)
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::ortho(-1.1f, 1.1f, 1.1f,-1.1f, 0.0f, 10.0f);
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -this->cameraDistance() * 0.1));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glBindVertexArray(VertexArrayName);

		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
		
		this->checkError("renderFB");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		// Clear the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glUseProgram(ProgramName);
		glUniform1i(UniformDiffuse, 0);

		// Pass 1
		// Render the scene in a multisampled framebuffer
		glEnable(GL_MULTISAMPLE);
		renderFBO(FramebufferName[framebuffer::RENDER]);
		glDisable(GL_MULTISAMPLE);

		// Resolved multisampling
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FramebufferName[framebuffer::RENDER]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferName[framebuffer::RESOLVE]);
		glBlitFramebuffer(
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Pass 2
		// Render the colorbuffer from the multisampled framebuffer
		renderFB(TextureName[texture::COLORBUFFER]);

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

