#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-420/texture-conversion.vert");
	char const* FRAG_SHADER_SOURCE[2] = 
	{
		"gl-420/texture-conversion-normalized.frag", 
		"gl-420/texture-conversion-uint.frag"
	};
	char const* TEXTURE_DIFFUSE("kueken7_rgb8_unorm.dds");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(  0.f,   0.f), glm::vec2(0.25f, 0.75f)),
		glf::vertex_v2fv2f(glm::vec2(256.f,   0.f), glm::vec2(0.75f, 0.75f)),
		glf::vertex_v2fv2f(glm::vec2(256.f, 256.f), glm::vec2(0.75f, 0.25f)),
		glf::vertex_v2fv2f(glm::vec2(  0.f, 256.f), glm::vec2(0.25f, 0.25f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

	namespace texture
	{
		enum type
		{
			RGBA8, // GL_RGBA8
			RGBA8UI, // GL_RGBA8UI
			RGBA32F, // GL_RGBA32F
			RGBA8_SNORM, 
			MAX
		};
	}//namespace texture

	namespace program
	{
		enum type
		{
			NORM, 
			UINT, 
			MAX
		};
	}//namespace program

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

	GLenum const TextureInternalFormat[texture::MAX] = 
	{
		GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 
		GL_RGBA8UI,
		GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
		GL_RGBA8_SNORM
	};

	GLenum const TextureFormat[texture::MAX] = 
	{
		GL_RGB,
		GL_RGB_INTEGER,
		GL_RGB,
		GL_RGB
	};

	glm::ivec4 const Viewport[texture::MAX] = 
	{
		glm::ivec4(  0,   0, 256, 256),
		glm::ivec4(256,   0, 256, 256),
		glm::ivec4(256, 256, 256, 256),
		glm::ivec4(  0, 256, 256, 256)
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-420-texture-conversion", framework::CORE, 4, 2, glm::uvec2(512)),
		VertexArrayName(0)
	{}

private:
	std::array<GLuint, program::MAX> PipelineName;
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	GLuint VertexArrayName;

	bool initProgram()
	{
		bool Validated(true);
	
		glGenProgramPipelines(program::MAX, &PipelineName[0]);

		compiler Compiler;
		GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 420 --profile core");
		GLuint FragShaderNameNorm = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE[program::NORM], "--version 420 --profile core");
		GLuint FragShaderNameUint = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE[program::UINT], "--version 420 --profile core");
		Validated = Validated && Compiler.check();

		if(Validated)
		{
			ProgramName[program::NORM] = glCreateProgram();
			glProgramParameteri(ProgramName[program::NORM], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::NORM], VertShaderName);
			glAttachShader(ProgramName[program::NORM], FragShaderNameNorm);
			glLinkProgram(ProgramName[program::NORM]);
			Validated = Validated && Compiler.check_program(ProgramName[program::NORM]);
		}

		if(Validated)
		{
			ProgramName[program::UINT] = glCreateProgram();
			glProgramParameteri(ProgramName[program::UINT], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::UINT], VertShaderName);
			glAttachShader(ProgramName[program::UINT], FragShaderNameUint);
			glLinkProgram(ProgramName[program::UINT]);
			Validated = Validated && Compiler.check_program(ProgramName[program::UINT]);
		}

		if(Validated)
		{
			glUseProgramStages(PipelineName[program::NORM], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::NORM]);
			glUseProgramStages(PipelineName[program::UINT], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::UINT]);
		}

		return Validated;
	}

	bool initBuffer()
	{
		bool Validated(true);

		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return Validated;
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		assert(!Texture.empty());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(texture::MAX, &TextureName[0]);
		glActiveTexture(GL_TEXTURE0);

		for(std::size_t i = 0; i < texture::MAX; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, TextureName[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFormat[i] == GL_RGB_INTEGER ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFormat[i] == GL_RGB_INTEGER ? GL_NEAREST :  GL_LINEAR);
			glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()), TextureInternalFormat[i], GLsizei(Texture[0].extent().x), GLsizei(Texture[0].extent().y));

			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTexSubImage2D(GL_TEXTURE_2D, GLint(Level),
					0, 0,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					TextureFormat[i], GL_UNSIGNED_BYTE,
					Texture[Level].data());
			}
		}
	
		glBindTexture(GL_TEXTURE_2D, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

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

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		for(std::size_t i = 0; i < program::MAX; ++i)
			glDeleteProgram(ProgramName[i]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteProgramPipelines(program::MAX, &PipelineName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0,	sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::ortho(0.f, 256.f, 0.f, 256.f, 0.f, 16.f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glBindVertexArray(VertexArrayName);
		glActiveTexture(GL_TEXTURE0);

		glBindProgramPipeline(PipelineName[program::UINT]);
		{
			glViewportIndexedfv(0, &glm::vec4(Viewport[texture::RGBA8UI])[0]);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA8UI]);

			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0, 0);
		}

		glBindProgramPipeline(PipelineName[program::NORM]);
		{
			glViewportIndexedfv(0, &glm::vec4(Viewport[texture::RGBA32F])[0]);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA32F]);

			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0, 0);

			glViewportIndexedfv(0, &glm::vec4(Viewport[texture::RGBA8])[0]);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA8]);

			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0, 0);

			glViewportIndexedfv(0, &glm::vec4(Viewport[texture::RGBA8_SNORM])[0]);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA8_SNORM]);

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


