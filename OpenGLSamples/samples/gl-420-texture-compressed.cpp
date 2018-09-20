#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-420/texture-2d.vert");
	char const* FRAG_SHADER_SOURCE("gl-420/texture-2d.frag");
	char const* TEXTURE_DIFFUSE_DXT5_SRGB("kueken7_rgba_dxt5_srgb.dds");
	char const* TEXTURE_DIFFUSE_DXT5_UNORM("kueken7_rgba_dxt5_unorm.dds");
	char const* TEXTURE_DIFFUSE_RGB8_SNORM("kueken7_rgba8_snorm.dds");
	char const* TEXTURE_DIFFUSE_RGB5E5_UFLOAT("kueken7_rgb9e5_ufloat.dds");

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
			RGB8,
			DXT5,
			RGTC,
			BPTC,
			MAX
		};
	}//namespace texture
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-420-texture-compressed", framework::CORE, 4, 2, true),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0),
		SamplerName(0)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<glm::ivec4, texture::MAX> Viewport;
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint SamplerName;

	bool initProgram()
	{
		bool Validated(true);
	
		glGenProgramPipelines(1, &PipelineName);

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 420 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 420 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);

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
		gli::gl GL(gli::gl::PROFILE_GL33);

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(texture::MAX, &TextureName[0]);

		{
			gli::texture2d Texture(gli::load((getDataDirectory() + TEXTURE_DIFFUSE_DXT5_SRGB).c_str()));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D, TextureName[texture::BPTC]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()),
				Format.Internal, GLsizei(Texture.extent().x), GLsizei(Texture.extent().y));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexSubImage2D(GL_TEXTURE_2D, GLint(Level),
					0, 0,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					Format.Internal,
					GLsizei(Texture[Level].size()),
					Texture[Level].data());
			}
		}

		{
			gli::texture2d Texture(gli::load((getDataDirectory() + TEXTURE_DIFFUSE_DXT5_UNORM).c_str()));
			assert(!Texture.empty());
			gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D, TextureName[texture::DXT5]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()),
				Format.Internal, GLsizei(Texture.extent().x), GLsizei(Texture.extent().y));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexSubImage2D(GL_TEXTURE_2D, GLint(Level),
					0, 0,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					Format.Internal,
					GLsizei(Texture[Level].size()), 
					Texture[Level].data());
			}
		}

		{
			gli::texture2d Texture(gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RGB5E5_UFLOAT).c_str()));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGTC]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()),
				Format.Internal, GLsizei(Texture.extent().x), GLsizei(Texture.extent().y));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTexSubImage2D(GL_TEXTURE_2D, GLint(Level),
					0, 0,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					Format.External, Format.Type,
					Texture[Level].data());
			}
		}

		{
			gli::texture2d Texture(gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RGB8_SNORM).c_str()));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGB8]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()),
				Format.Internal, GLsizei(Texture.extent().x), GLsizei(Texture.extent().y));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTexSubImage2D(GL_TEXTURE_2D, GLint(Level),
					0, 0,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					Format.External, Format.Type,
					Texture[Level].data());
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

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

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
	}

	bool initSampler()
	{
		glGenSamplers(1, &SamplerName);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

		return true;
	}

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		Viewport[texture::RGB8] = glm::ivec4(0, 0, WindowSize >> 1);
		Viewport[texture::DXT5] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		Viewport[texture::RGTC] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		Viewport[texture::BPTC] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated(true);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initSampler();

		return Validated;
	}

	bool end()
	{
		glDeleteProgramPipelines(1, &PipelineName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteSamplers(1, &SamplerName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 1000.0f);
			glm::mat4 Model = glm::mat4(1.0f);

			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		// Clear the color buffer
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// Bind rendering objects
		glBindProgramPipeline(PipelineName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindSampler(0, SamplerName);
		glBindVertexArray(VertexArrayName);

		// Draw each texture in different viewports
		for(std::size_t Index = 0; Index < texture::MAX; ++Index)
		{
			glViewportIndexedfv(0, &glm::vec4(Viewport[Index])[0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[Index]);

			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0, 0);
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

