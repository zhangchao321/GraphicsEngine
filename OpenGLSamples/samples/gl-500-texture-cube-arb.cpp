#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-500/texture-cube.vert");
	char const* FRAG_SHADER_SOURCE("gl-500/texture-cube.frag");

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const VertexData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(-1.0f,-1.0f)
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

	namespace sampler
	{
		enum type
		{
			SEAMLESS,
			NON_SEAMLESS,
			MAX
		};
	}//namespace sampler

	struct transform
	{
		glm::mat4 MVP;
		glm::mat4 MV;
		glm::vec3 Camera;
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-500-texture-cube-arb", framework::CORE, 4, 2, glm::vec2(glm::pi<float>() * 0.1f)),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0),
		TextureName(0),
		TransformPointer(nullptr)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, sampler::MAX> SamplerName;
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint TextureName;
	transform* TransformPointer;

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

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferName[buffer::VERTEX]);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, VertexSize, VertexData, 0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(transform)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferStorage(GL_UNIFORM_BUFFER, UniformBlockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initSampler()
	{
		glGenSamplers(sampler::MAX, &SamplerName[0]);

		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glSamplerParameterfv(SamplerName[sampler::SEAMLESS], GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
		glSamplerParameterf(SamplerName[sampler::SEAMLESS], GL_TEXTURE_MIN_LOD, -1000.f);
		glSamplerParameterf(SamplerName[sampler::SEAMLESS], GL_TEXTURE_MAX_LOD, 1000.f);
		glSamplerParameterf(SamplerName[sampler::SEAMLESS], GL_TEXTURE_LOD_BIAS, 0.0f);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glSamplerParameterf(SamplerName[sampler::SEAMLESS], GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		glSamplerParameteri(SamplerName[sampler::SEAMLESS], GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glSamplerParameterfv(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
		glSamplerParameterf(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_MIN_LOD, -1000.f);
		glSamplerParameterf(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_MAX_LOD, 1000.f);
		glSamplerParameterf(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_LOD_BIAS, 0.0f);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glSamplerParameterf(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		glSamplerParameteri(SamplerName[sampler::NON_SEAMLESS], GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_FALSE);

		return true;
	}

	bool initTexture()
	{
		gli::texture_cube Texture(gli::FORMAT_RGBA8_UNORM_PACK8, gli::texture_cube::extent_type(2), 1);
		assert(!Texture.empty());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

		Texture[0].clear<glm::u8vec4>(glm::u8vec4(255,   0,   0, 255));
		Texture[1].clear<glm::u8vec4>(glm::u8vec4(255, 128,   0, 255));
		Texture[2].clear<glm::u8vec4>(glm::u8vec4(255, 255,   0, 255));
		Texture[3].clear<glm::u8vec4>(glm::u8vec4(  0, 255,   0, 255));
		Texture[4].clear<glm::u8vec4>(glm::u8vec4(  0, 255, 255, 255));
		Texture[5].clear<glm::u8vec4>(glm::u8vec4(  0,   0, 255, 255));

		glGenTextures(1, &TextureName);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, TextureName);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

		glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, GLint(Texture.levels()),
			Format.Internal, GLsizei(Texture.extent().x), GLsizei(Texture.extent().y), GLsizei(Texture.faces()));

		glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0,
			0, 0, 0,
			static_cast<GLsizei>(Texture.extent().x),
			static_cast<GLsizei>(Texture.extent().y),
			static_cast<GLsizei>(Texture.faces()),
			Format.External, Format.Type,
			Texture.data());

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), BUFFER_OFFSET(0));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated = true;
		Validated = Validated && this->checkExtension("GL_ARB_seamless_cubemap_per_texture");
		Validated = Validated && this->checkExtension("GL_ARB_shader_storage_buffer_object");
		Validated = Validated && this->checkExtension("GL_ARB_buffer_storage");
		Validated = Validated && this->checkExtension("GL_ARB_multi_bind");

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

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		TransformPointer = reinterpret_cast<transform*>(glMapBufferRange(GL_UNIFORM_BUFFER,
			0, sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		return Validated;
	}

	bool end()
	{
		if(!TransformPointer)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			TransformPointer = nullptr;
		}

		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);
		glDeleteTextures(1, &TextureName);
		glDeleteSamplers(sampler::MAX, &SamplerName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x * 0.5f / WindowSize.y, 0.1f, 1000.0f);
			glm::mat4 View = this->view();
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;
			glm::mat4 MV = View * Model;

			TransformPointer->MVP = Projection * View * Model;
			TransformPointer->MV = View * Model;
			TransformPointer->Camera = glm::vec3(0.0f, 0.0f, -this->cameraDistance());
		}

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

		glBindProgramPipeline(PipelineName);
		glBindTextures(semantic::sampler::DIFFUSE, 1, &TextureName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		// Left side: seamless cubemap filtering
		glViewportIndexedf(0, 0, 0, WindowSize.x * 0.5f, WindowSize.y);
		glBindSamplers(semantic::sampler::DIFFUSE, 1, &SamplerName[sampler::SEAMLESS]);

		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, VertexCount, 1, 0);

		// Right side: per face cubemap filtering
		glViewportIndexedf(0, WindowSize.x * 0.5f, 0, WindowSize.x * 0.5f, WindowSize.y);
		glBindSamplers(semantic::sampler::DIFFUSE, 1, &SamplerName[sampler::NON_SEAMLESS]);

		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, VertexCount, 1, 0);

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
