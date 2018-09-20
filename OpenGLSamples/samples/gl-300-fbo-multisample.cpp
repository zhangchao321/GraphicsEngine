#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-300/image-2d.vert");
	char const* FRAG_SHADER_SOURCE("gl-300/image-2d.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");
	glm::ivec2 const FRAMEBUFFER_SIZE(160, 120);

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount = 6;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-2.0f,-1.5f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 2.0f,-1.5f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 2.0f, 1.5f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 2.0f, 1.5f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(-2.0f, 1.5f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(-2.0f,-1.5f), glm::vec2(0.0f, 0.0f))
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-300-fbo-multisample", framework::COMPATIBILITY, 3, 0),
		ProgramName(0),
		VertexArrayName(0),
		BufferName(0),
		TextureName(0),
		ColorRenderbufferName(0),
		ColorTextureName(0),
		FramebufferRenderName(0),
		FramebufferResolveName(0),
		UniformMVP(-1),
		UniformDiffuse(-1)
	{}

private:
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint BufferName;
	GLuint TextureName;
	GLuint ColorRenderbufferName;
	GLuint ColorTextureName;
	GLuint FramebufferRenderName;
	GLuint FramebufferResolveName;
	GLint UniformMVP;
	GLint UniformDiffuse;

	bool initProgram()
	{
		bool Validated = true;
	
		compiler Compiler;

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE);
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::TEXCOORD, "Texcoord");
			glLinkProgram(ProgramName);

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
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		gli::gl GL(gli::gl::PROFILE_GL32);

		glGenTextures(1, &TextureName);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}

		return this->checkError("initTexture");
	}

	bool validateRenderbuffer(GLuint Name, GLint ExpectedWidth, GLint ExpectedHeight, GLint ExpectedSamples, GLint ExpectedFormat)
	{
		GLint QueriedWidth = 0, QueriedHeight = 0, QueriedSamples = 0, QueriedFormat = 0;
		glBindRenderbuffer(GL_RENDERBUFFER, Name);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &QueriedWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &QueriedHeight);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &QueriedSamples);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &QueriedFormat);

		if (QueriedWidth != ExpectedWidth || QueriedHeight != ExpectedHeight)
			return false;
		if (QueriedSamples != ExpectedSamples)
			return false;
		if (QueriedFormat != ExpectedFormat)
			return false;

		return true;
	}

	bool initFramebuffer()
	{
		glGenRenderbuffers(1, &ColorRenderbufferName);
		glBindRenderbuffer(GL_RENDERBUFFER, ColorRenderbufferName);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
		// The second parameter is the number of samples.

		if (!validateRenderbuffer(ColorRenderbufferName, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 8, GL_RGBA8))
			return false;

		glGenFramebuffers(1, &FramebufferRenderName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferRenderName);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRenderbufferName);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenTextures(1, &ColorTextureName);
		glBindTexture(GL_TEXTURE_2D, ColorTextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glGenFramebuffers(1, &FramebufferResolveName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferResolveName);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureName, 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	void renderFBO(GLuint Framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Perspective * this->view() * Model;
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glViewport(0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);

		glBindVertexArray(VertexArrayName);
		glDrawArrays(GL_TRIANGLES, 0, VertexCount);

		this->checkError("renderFBO");
	}

	void renderFB(GLuint Texture2DName)
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Perspective * this->view() * Model;
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2DName);

		glBindVertexArray(VertexArrayName);
		glDrawArrays(GL_TRIANGLES, 0, VertexCount);
		
		this->checkError("renderFB");
	}

	bool begin()
	{
		bool Validated = true;

		caps Caps(caps::COMPATIBILITY);

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
		glDeleteTextures(1, &TextureName);
		glDeleteTextures(1, &ColorTextureName);
		glDeleteRenderbuffers(1, &ColorRenderbufferName);
		glDeleteFramebuffers(1, &FramebufferRenderName);
		glDeleteFramebuffers(1, &FramebufferResolveName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		// Clear the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glUseProgram(ProgramName);
		glUniform1i(UniformDiffuse, 0);

		// Pass 1
		// Render the scene in a multisampled framebuffer
		glEnable(GL_MULTISAMPLE);
		renderFBO(FramebufferRenderName);
		glDisable(GL_MULTISAMPLE);

		// Resolved multisampling
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FramebufferRenderName);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferResolveName);
		glBlitFramebuffer(
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
			0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glm::ivec2 WindowSize(this->getWindowSize());

		// Pass 2
		// Render the colorbuffer from the multisampled framebuffer
		glViewport(0, 0, WindowSize.x, WindowSize.y);
		renderFB(ColorTextureName);

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

