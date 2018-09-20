#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_TEXTURE("gl-500/sample-location-render.vert");
	char const* FRAG_SHADER_SOURCE_TEXTURE("gl-500/sample-location-render.frag");
	char const* VERT_SHADER_SOURCE_SPLASH("gl-500/sample-location-splash.vert");
	char const* FRAG_SHADER_SOURCE_SPLASH("gl-500/sample-location-splash.frag");

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
			MAX
		};
	}//namespace texture
	
	namespace framebuffer
	{
		enum type
		{
			COLORBUFFER,
			RENDERBUFFER0,
			RENDERBUFFER1,
			RENDERBUFFER2,
			RENDERBUFFER3,
			MAX
		};
	}//namespace framebuffer

	namespace program
	{
		enum type
		{
			TEXTURE,
			SPLASH,
			MAX
		};
	}//namespace program

	namespace shader
	{
		enum type
		{
			VERT_TEXTURE,
			FRAG_TEXTURE,
			VERT_SPLASH,
			FRAG_SPLASH,
			MAX
		};
	}//namespace shader
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-500-sample-location-grid-nv", framework::CORE, 4, 5),
		FramebufferScale(3),
		UniformTransform(-1),
		VertexCount(0)
	{}

private:
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, program::MAX> VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<GLint, program::MAX> UniformDiffuse;
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	GLsizei VertexCount;
	glm::uint FramebufferScale;
	GLint UniformTransform;

	bool initProgram()
	{
		bool Validated(true);

		compiler Compiler;

		std::array<GLuint, shader::MAX> ShaderName;

		if(Validated)
		{
			ShaderName[shader::VERT_TEXTURE] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");
			ShaderName[shader::FRAG_TEXTURE] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");

			ProgramName[program::TEXTURE] = glCreateProgram();
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::VERT_TEXTURE]);
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::FRAG_TEXTURE]);

			glBindAttribLocation(ProgramName[program::TEXTURE], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[program::TEXTURE], semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName[program::TEXTURE], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::TEXTURE]);
		}
		
		if(Validated)
		{
			ShaderName[shader::VERT_SPLASH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_SPLASH, "--version 450 --profile core");
			ShaderName[shader::FRAG_SPLASH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_SPLASH, "--version 450 --profile core");

			ProgramName[program::SPLASH] = glCreateProgram();
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::VERT_SPLASH]);
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::FRAG_SPLASH]);

			glBindFragDataLocation(ProgramName[program::SPLASH], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::SPLASH]);
		}
	
		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName[program::TEXTURE]);
			Validated = Validated && Compiler.check_program(ProgramName[program::SPLASH]);
		}

		if(Validated)
		{
			UniformTransform = glGetUniformBlockIndex(ProgramName[program::TEXTURE], "transform");
			UniformDiffuse[program::TEXTURE] = glGetUniformLocation(ProgramName[program::TEXTURE], "Diffuse");
			UniformDiffuse[program::SPLASH] = glGetUniformLocation(ProgramName[program::SPLASH], "Diffuse");

			glUseProgram(ProgramName[program::TEXTURE]);
			glUniform1i(UniformDiffuse[program::TEXTURE], 0);
			glUniformBlockBinding(ProgramName[program::TEXTURE], UniformTransform, semantic::uniform::TRANSFORM0);

			glUseProgram(ProgramName[program::SPLASH]);
			glUniform1i(UniformDiffuse[program::SPLASH], 0);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		std::array<glm::vec2, 36> Data;
		for(std::size_t i = 0; i < Data.size(); ++i)
		{
			float Angle = glm::pi<float>() * 2.0f * float(i) / Data.size();
			Data[i] = glm::normalize(glm::vec2(glm::sin(Angle), glm::cos(Angle)));
		}
		this->VertexCount = 18;//static_cast<GLsizei>(Data.size() - 8);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::ELEMENT], ElementSize, ElementData, 0);
		glNamedBufferStorage(BufferName[buffer::VERTEX], Data.size() * sizeof(glm::vec2), &Data[0], 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		glm::ivec2 WindowSize(this->getWindowSize() >> this->FramebufferScale);

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureName[texture::COLORBUFFER]);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureStorage2D(TextureName[texture::COLORBUFFER], 1, GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &TextureName[texture::RENDERBUFFER]);
		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureStorage2DMultisample(TextureName[texture::RENDERBUFFER], 4, GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y), GL_TRUE);

		return Validated;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(program::MAX, &VertexArrayName[0]);

		glVertexArrayAttribBinding(VertexArrayName[program::TEXTURE], semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName[program::TEXTURE], semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName[program::TEXTURE], semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD, 0);
		glVertexArrayAttribFormat(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD);

		glVertexArrayElementBuffer(VertexArrayName[program::TEXTURE], BufferName[buffer::ELEMENT]);
		glVertexArrayVertexBuffer(VertexArrayName[program::TEXTURE], 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));

		return true;
	}

	bool initFramebuffer()
	{
		typedef std::array<glm::vec2, 8> sampleLocations;

		static glm::vec2 SamplesPositions16[] =
		{
			glm::vec2( 1.f,  0.f) / 16.f,
			glm::vec2( 4.f,  1.f) / 16.f,
			glm::vec2( 3.f,  6.f) / 16.f,
			glm::vec2( 7.f,  5.f) / 16.f,
			glm::vec2( 8.f,  1.f) / 16.f,
			glm::vec2(11.f,  3.f) / 16.f,
			glm::vec2(12.f,  7.f) / 16.f,
			glm::vec2(15.f,  4.f) / 16.f,
			glm::vec2( 0.f,  8.f) / 16.f,
			glm::vec2( 5.f, 10.f) / 16.f,
			glm::vec2( 2.f, 12.f) / 16.f,
			glm::vec2( 6.f, 14.f) / 16.f,
			glm::vec2( 9.f,  9.f) / 16.f,
			glm::vec2(13.f, 11.f) / 16.f,
			glm::vec2(10.f, 13.f) / 16.f,
			glm::vec2(14.f, 15.f) / 16.f
		};

		GLint SubPixelBits(0);
		glm::ivec2 PixelGrid(0);
		GLint TableSize(0);

		glGetIntegerv(GL_SAMPLE_LOCATION_SUBPIXEL_BITS_NV, &SubPixelBits);
		glGetIntegerv(GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_NV, &PixelGrid.x);
		glGetIntegerv(GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_NV, &PixelGrid.y);
		glGetIntegerv(GL_PROGRAMMABLE_SAMPLE_LOCATION_TABLE_SIZE_NV, &TableSize);

		glGenFramebuffers(framebuffer::MAX, &FramebufferName[0]);

		for(int FramebufferIndex = 0; FramebufferIndex < 4; ++FramebufferIndex)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER0 + FramebufferIndex]);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::RENDERBUFFER], 0);
			glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_PROGRAMMABLE_SAMPLE_LOCATIONS_NV, FramebufferIndex == 0 ? GL_FALSE : GL_TRUE);
			glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_SAMPLE_LOCATION_PIXEL_GRID_NV, GL_TRUE);
			glFramebufferSampleLocationsfvNV(GL_FRAMEBUFFER, 0, TableSize, &SamplesPositions16[0][0]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[texture::COLORBUFFER]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);

		for(int FramebufferIndex = 0; FramebufferIndex < framebuffer::MAX; ++FramebufferIndex)
			if(!this->checkFramebuffer(FramebufferName[FramebufferIndex]))
				return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool begin()
	{
		bool Validated(true);
		Validated = Validated && this->checkExtension("GL_NV_sample_locations");

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
		glDeleteFramebuffers(texture::MAX, &FramebufferName[0]);
		glDeleteProgram(ProgramName[program::SPLASH]);
		glDeleteProgram(ProgramName[program::TEXTURE]);
		
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapNamedBufferRange(BufferName[buffer::TRANSFORM], 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapNamedBuffer(BufferName[buffer::TRANSFORM]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER0]);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

		glUseProgram(ProgramName[program::TEXTURE]);

		glBindVertexArray(VertexArrayName[program::TEXTURE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glm::vec2 ViewportSize(glm::vec2(this->getWindowSize() >> this->FramebufferScale) * 0.5f);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER0]);
		glViewportIndexedf(0, 0, 0, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER1]);
		glViewportIndexedf(0, ViewportSize.x, 0, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER2]);
		glViewportIndexedf(0, ViewportSize.x, ViewportSize.y, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER3]);
		glViewportIndexedf(0, 0, ViewportSize.y, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		// Blit
		glBlitNamedFramebuffer(FramebufferName[framebuffer::RENDERBUFFER0], FramebufferName[framebuffer::COLORBUFFER],
			0, 0, static_cast<GLsizei>(WindowSize.x) >> this->FramebufferScale, static_cast<GLsizei>(WindowSize.y) >> this->FramebufferScale, 
			0, 0, static_cast<GLsizei>(WindowSize.x) >> this->FramebufferScale, static_cast<GLsizei>(WindowSize.y) >> this->FramebufferScale, 
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(ProgramName[program::SPLASH]);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VertexArrayName[program::SPLASH]);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

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

