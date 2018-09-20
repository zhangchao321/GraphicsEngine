#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-430/program-subroutine.vert");
	char const* GEOM_SHADER_SOURCE("gl-430/program-subroutine.geom");
	char const* FRAG_SHADER_SOURCE("gl-430/program-subroutine.frag");
	char const* TEXTURE_DIFFUSE_RGB8("kueken7_rgba8_srgb.dds");
	char const* TEXTURE_DIFFUSE_DXT1("kueken7_rgb_dxt1_unorm.dds");

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
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLuint);
	GLuint const ElementData[ElementCount] =
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
			DXT1,
			RGB8,
			MAX
		};
	}//namespace texture

	namespace sementics{
	namespace sampler
	{
		enum type
		{
			DXT1 = 0,
			RGB8 = 1,
			MAX
		};
	}//namespace sampler

	namespace sampling
	{
		enum type
		{
			DXT1 = 0,
			RGB8 = 1,
			MAX
		};
	}//namespace sampling
	}//namespace sementics
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-430-program-subroutine", framework::CORE, 4, 3),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0)
	{}

private:
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<GLuint, buffer::MAX> BufferName;

	bool initTest()
	{
		glEnable(GL_DEPTH_TEST);

		return this->checkError("initTest");
	}

	bool initProgram()
	{
		bool Validated = true;
	
		glGenProgramPipelines(1, &PipelineName);

		// Create shaders
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE);
			GLuint GeomShaderName = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + GEOM_SHADER_SOURCE);
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE);
			Validated = Validated && Compiler.check();

			if(Validated)
			{
				ProgramName = glCreateProgram();
				glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
				glAttachShader(ProgramName, VertShaderName);
				glAttachShader(ProgramName, GeomShaderName);
				glAttachShader(ProgramName, FragShaderName);
				glLinkProgram(ProgramName);
				Validated = Validated && Compiler.check_program(ProgramName);
			}
		}

		if(Validated)
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);

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

		GLint UniformBufferOffset(0);

		glGetIntegerv(
			GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
			&UniformBufferOffset);

		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_COPY);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

	bool initTexture()
	{
		gli::gl GL(gli::gl::PROFILE_GL33);

		glGenTextures(texture::MAX, &TextureName[0]);

		{
			gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE_RGB8).c_str()));
			assert(!Texture.empty());

			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGB8]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels()));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					0,
					Format.External, Format.Type,
					Texture[Level].data());
			}
		}

		{
			gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE_DXT1).c_str()));
			assert(!Texture.empty());

			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::DXT1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels()));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].extent().x),
					GLsizei(Texture[Level].extent().y),
					0, 
					GLsizei(Texture[Level].size()), 
					Texture[Level].data());
			}
		}

		return true;
	}

	bool begin()
	{
		bool Validated(true);
		Validated = Validated && this->checkExtension("GL_ARB_explicit_uniform_location");

		if(Validated)
			Validated = initTest();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();

		return Validated;
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteProgram(ProgramName);
		glDeleteProgramPipelines(1, &PipelineName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0,	sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x * 0.5f / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH, 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0 + sementics::sampler::RGB8);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGB8]);

		glActiveTexture(GL_TEXTURE0 + sementics::sampler::DXT1);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DXT1]);

		glBindProgramPipeline(PipelineName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		glUseProgram(ProgramName);
		std::vector<GLuint> Index(2);
		Index[0] = sementics::sampling::RGB8;
		Index[1] = sementics::sampling::DXT1;
		
		glViewportIndexedf(0, 0, 0, GLfloat(WindowSize.x) / 2.0f, GLfloat(WindowSize.y));
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &Index[0]);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, nullptr, 1, 0);

		glViewportIndexedf(0, GLfloat(WindowSize.x) / 2.0f, 0, GLfloat(WindowSize.x) / 2.0f, GLfloat(WindowSize.y));
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &Index[1]);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, nullptr, 1, 0);
		
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

