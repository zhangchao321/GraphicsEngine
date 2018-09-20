#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-440/texture-2d.vert");
	char const* FRAG_SHADER_SOURCE("gl-440/texture-2d.frag");
	char const* TEXTURE_DIFFUSE_RGB_ETC2_SRGB("kueken7_rgb_etc2_srgb.ktx");
	char const* TEXTURE_DIFFUSE_RGB9E5_UFLOAT("kueken7_rgb9e5_ufloat.ktx");
	char const* TEXTURE_DIFFUSE_RGBA_DXT5_UNORM("kueken7_rgba_dxt5_unorm.ktx");
	char const* TEXTURE_DIFFUSE_RG_EAC_UNORM("kueken7_rg_eac_snorm.ktx");

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
			RGB10,
			BC1,
			BC3,
			MAX
		};
	}//namespace texture
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-440-texture-compressed", framework::CORE, 4, 3),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0),
		SamplerName(0)
	{}

private:
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint SamplerName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<glm::ivec4, texture::MAX> Viewport;

	bool initProgram()
	{
		bool Validated(true);

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
		{
			glGenProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
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
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		gli::gl GL(gli::gl::PROFILE_GL33);

		glGenTextures(texture::MAX, &TextureName[0]);

		{
			gli::texture Texture = gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RGBA_DXT5_UNORM));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName[texture::BC1]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexStorage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Texture.levels()),
				Format.Internal,
				static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y), static_cast<GLsizei>(1));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				gli::image::extent_type const Extent = Texture.extent(Level);

				glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Level),
					0, 0, 0,
					static_cast<GLsizei>(Extent.x), static_cast<GLsizei>(Extent.y), static_cast<GLsizei>(1),
					Format.Internal,
					static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(0, 0, Level));
			}
		}

		{
			gli::texture Texture = gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RG_EAC_UNORM));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName[texture::BC3]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexStorage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Texture.levels()),
				Format.Internal,
				static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y), static_cast<GLsizei>(1));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Level),
					0, 0, 0,
					static_cast<GLsizei>(Texture.extent(Level).x), static_cast<GLsizei>(Texture.extent(Level).y), static_cast<GLsizei>(1),
					Format.Internal,
					static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(0, 0, Level));
			}
		}

		{
			gli::texture Texture = gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RGB9E5_UFLOAT));
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName[texture::RGB8]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexStorage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Texture.levels()),
				Format.Internal,
				static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y), static_cast<GLsizei>(1));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Level),
					0, 0, 0,
					static_cast<GLsizei>(Texture.extent(Level).x), static_cast<GLsizei>(Texture.extent(Level).y), static_cast<GLsizei>(1),
					Format.External, Format.Type,
					Texture.data(0, 0, Level));
			}
		}

		{
			gli::texture Texture = gli::load((getDataDirectory() + TEXTURE_DIFFUSE_RGB_ETC2_SRGB).c_str());
			assert(!Texture.empty());
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName[texture::RGB10]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexStorage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Texture.levels()),
				Format.Internal,
				static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y), static_cast<GLsizei>(1));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(Level),
					0, 0, 0,
					static_cast<GLsizei>(Texture.extent(Level).x), static_cast<GLsizei>(Texture.extent(Level).y), static_cast<GLsizei>(1),
					Format.Internal,
					static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(0, 0, Level));
			}
		}

		glBindTextures(0, 1, 0);

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glVertexAttribBinding(semantic::attr::POSITION, 0);
			glVertexAttribFormat(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);

			glVertexAttribBinding(semantic::attr::TEXCOORD, 0);
			glVertexAttribFormat(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
			glBindVertexBuffer(0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));
		glBindVertexArray(0);

		return true;
	}

	bool initSampler()
	{
		glGenSamplers(1, &SamplerName);
		glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
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

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		Viewport[texture::RGB8] = glm::ivec4(0, 0, WindowSize >> 1);
		Viewport[texture::RGB10] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		Viewport[texture::BC1] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		Viewport[texture::BC3] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated = this->checkExtension("GL_ARB_multi_bind");

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
		glm::uvec2 WindowSize = this->getWindowSize();

		// Asynchrone update of the uniform buffer
		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 1000.0f);
			glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));

			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		// Clear the color buffer
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)[0]);

		glBindProgramPipeline(PipelineName);
		glBindBuffersBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, 1, &BufferName[buffer::TRANSFORM]);
		glBindSamplers(0, 1, &SamplerName);
		glBindVertexArray(VertexArrayName);

		// Draw each texture in different viewports
		for(std::size_t Index = 0; Index < texture::MAX; ++Index)
		{
			glViewportIndexedfv(0, &glm::vec4(Viewport[Index])[0]);
			glBindTextures(0, 1, &TextureName[Index]);

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

