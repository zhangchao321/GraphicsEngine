#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE2("gl-330/image-2d.vert");
	char const* FRAG_SHADER_SOURCE2("gl-330/image-2d.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");

	struct vertex
	{
		vertex
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex);
	vertex const VertexData[VertexCount] =
	{
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f))
	};

	enum texture_type
	{
		TEXTURE_RGB8,
		TEXTURE_R,
		TEXTURE_G,
		TEXTURE_B,
		TEXTURE_MAX
	};

	namespace shader
	{
		enum type
		{
			VERT1,
			FRAG1,
			VERT2,
			FRAG2,
			MAX
		};
	}//namespace shader
	
	GLuint FramebufferName = 0;
	GLuint VertexArrayName = 0;
	GLuint ProgramNameSingle = 0;
	GLint UniformMVPSingle = 0;
	GLint UniformDiffuseSingle = 0;
	GLuint BufferName = 0;
	GLuint Texture2DName[TEXTURE_MAX];
	GLuint SamplerName = 0;
	glm::ivec4 Viewport[TEXTURE_MAX];
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-330-blend-rtt", framework::CORE, 3, 3)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;

		std::array<GLuint, shader::MAX> ShaderName;
		
		compiler Compiler;
		ShaderName[shader::VERT2] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE2, "--version 330 --profile core");
		ShaderName[shader::FRAG2] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE2, "--version 330 --profile core");
		Validated = Validated && Compiler.check();

		if(Validated)
		{
			ProgramNameSingle = glCreateProgram();
			glAttachShader(ProgramNameSingle, ShaderName[shader::VERT2]);
			glAttachShader(ProgramNameSingle, ShaderName[shader::FRAG2]);
			glLinkProgram(ProgramNameSingle);
			Validated = Validated && Compiler.check_program(ProgramNameSingle);
		}

		if(Validated)
		{
			UniformMVPSingle = glGetUniformLocation(ProgramNameSingle, "MVP");
			UniformDiffuseSingle = glGetUniformLocation(ProgramNameSingle, "Diffuse");
		}

		return this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool initSampler()
	{
		glGenSamplers(1, &SamplerName);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glSamplerParameterfv(SamplerName, GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
		glSamplerParameterf(SamplerName, GL_TEXTURE_MIN_LOD, -1000.f);
		glSamplerParameterf(SamplerName, GL_TEXTURE_MAX_LOD, 1000.f);
		glSamplerParameterf(SamplerName, GL_TEXTURE_LOD_BIAS, 0.0f);
		glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		return this->checkError("initSampler");
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(TEXTURE_MAX, Texture2DName);

		glBindTexture(GL_TEXTURE_2D, Texture2DName[TEXTURE_RGB8]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, GLint(0),
			Format.Internal,
			GLsizei(Texture.extent().x), GLsizei(Texture.extent().y),
			0,
			Format.External, Format.Type,
			Texture.data());

		glBindTexture(GL_TEXTURE_2D, Texture2DName[TEXTURE_R]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, Texture2DName[TEXTURE_G]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, Texture2DName[TEXTURE_B]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		for(int i = TEXTURE_R; i <= TEXTURE_B; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, Texture2DName[i]);
			glTexImage2D(GL_TEXTURE_2D, 0,
				GL_R8,
				GLsizei(Texture.extent().x), GLsizei(Texture.extent().y),
				0,
				GL_RGB, GL_UNSIGNED_BYTE,
				nullptr);
		}

		return this->checkError("initTexture");
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		for(std::size_t i = TEXTURE_R; i <= TEXTURE_B; ++i)
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GLenum(i - TEXTURE_R), Texture2DName[i], 0);

		GLenum DrawBuffers[3];
		DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
		DrawBuffers[1] = GL_COLOR_ATTACHMENT1;
		DrawBuffers[2] = GL_COLOR_ATTACHMENT2;

		glDrawBuffers(3, DrawBuffers);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return this->checkError("initFramebuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initBlend()
	{
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);

		glEnablei(GL_BLEND, 0);
		glEnablei(GL_BLEND, 1);
		glEnablei(GL_BLEND, 2);
		glEnablei(GL_BLEND, 3);

		glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		glColorMaski(1, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
		glColorMaski(2, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
		glColorMaski(3, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);

		return this->checkError("initBlend");
	}

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());
		Viewport[TEXTURE_RGB8] = glm::ivec4(0, 0, WindowSize >> 1);
		Viewport[TEXTURE_R] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		Viewport[TEXTURE_G] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		Viewport[TEXTURE_B] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated = true;

		if(Validated)
			Validated = initBlend();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initSampler();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramNameSingle);
		glDeleteTextures(TEXTURE_MAX, Texture2DName);
		glDeleteFramebuffers(1, &FramebufferName);
		glDeleteSamplers(1, &SamplerName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		// Pass 1
		{
			glm::mat4 Projection = glm::ortho(-2.0f, 2.0f,-1.5f, 1.5f,-1.0f, 1.0f);
			glm::mat4 View = glm::mat4(1.0f);
			glm::mat4 Model = glm::mat4(0.2f);
			glm::mat4 MVP = Projection * View * Model;

			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			glViewport(0, 0, WindowSize.x >> 1, WindowSize.y >> 1);
			glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);
			glClearBufferfv(GL_COLOR, 1, &glm::vec4(1.0f)[0]);
			glClearBufferfv(GL_COLOR, 2, &glm::vec4(1.0f)[0]);
			glClearBufferfv(GL_COLOR, 3, &glm::vec4(1.0f)[0]);
		}

		// Pass 2
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glUseProgram(ProgramNameSingle);

		{
			glm::mat4 Projection = glm::ortho(-2.0f, 2.0f,-1.5f, 1.5f, -1.0f, 1.0f);
			glm::mat4 View = glm::mat4(1.0f);
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;

			glUniformMatrix4fv(UniformMVPSingle, 1, GL_FALSE, &MVP[0][0]);
			glUniform1i(UniformDiffuseSingle, 0);
		}

		for(std::size_t i = 0; i < TEXTURE_MAX; ++i)
		{
			glViewport(Viewport[i].x, Viewport[i].y, Viewport[i].z, Viewport[i].w);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture2DName[i]);
			glBindSampler(0, SamplerName);
			glBindVertexArray(VertexArrayName);

			glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
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

