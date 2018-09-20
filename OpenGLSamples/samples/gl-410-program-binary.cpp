#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-410/binary.vert");
	char const* GEOM_SHADER_SOURCE("gl-410/binary.geom");
	char const* FRAG_SHADER_SOURCE("gl-410/binary.frag");
	char const* VERT_PROGRAM_BINARY("gl-410/binary.vert.bin");
	char const* GEOM_PROGRAM_BINARY("gl-410/binary.geom.bin");
	char const* FRAG_PROGRAM_BINARY("gl-410/binary.frag.bin");
	char const* TEXTURE_DIFFUSE("kueken7_rgba_dxt5_unorm.dds");

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
			MAX
		};
	}//namespace buffer

	namespace program
	{
		enum type
		{
			VERT,
			GEOM,
			FRAG,
			MAX
		};
	}//namespace program

	GLuint PipelineName(0);
	GLuint ProgramName[program::MAX] = {0, 0, 0};
	GLuint BufferName[buffer::MAX] = {0, 0};
	GLuint VertexArrayName(0);
	GLint UniformMVP(0);
	GLint UniformDiffuse(0);
	GLuint Texture2DName(0);
}//namespace

static const GLenum GL_PROGRAM_BINARY_SPIR_V = 0xFFFF;

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-410-program-binary", framework::CORE, 4, 1)
	{}

private:
	bool is_valid_binary_format(GLenum Format)
	{
		GLint NumProgramBinaryFormats = 0;
		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &NumProgramBinaryFormats);
		std::vector<GLint> ProgramBinaryFormats(static_cast<std::size_t>(NumProgramBinaryFormats));
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, &ProgramBinaryFormats[0]);

		for(GLint Index = 0; Index < NumProgramBinaryFormats; ++Index)
		{
			if(static_cast<GLenum>(ProgramBinaryFormats[Index]) == Format)
				return true;
		}

		return false;
	}

	bool save_program(GLuint ProgramName, std::string const & String)
	{
		GLint Size(0);
		GLenum Format(0);

		glGetProgramiv(ProgramName, GL_PROGRAM_BINARY_LENGTH, &Size);
		std::vector<glm::byte> Data(Size);
		glGetProgramBinary(ProgramName, Size, nullptr, &Format, &Data[0]);
		save_binary(String, Format, Data, Size);

		return true;
	}

	bool init_program()
	{
		bool Validated = true;
		GLint Success = 0;

		glGenProgramPipelines(1, &PipelineName);

		ProgramName[program::VERT] = glCreateProgram();
		glProgramParameteri(ProgramName[program::VERT], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glProgramParameteri(ProgramName[program::VERT], GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

		ProgramName[program::GEOM] = glCreateProgram();
		glProgramParameteri(ProgramName[program::GEOM], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glProgramParameteri(ProgramName[program::GEOM], GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

		ProgramName[program::FRAG] = glCreateProgram();
		glProgramParameteri(ProgramName[program::FRAG], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glProgramParameteri(ProgramName[program::FRAG], GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

		{
			GLenum Format = 0;
			GLint Size = 0;
			std::vector<glm::byte> Data;
			if(load_binary(getDataDirectory() + VERT_PROGRAM_BINARY, Format, Data, Size))
			{
				if(is_valid_binary_format(Format))
				{
					glProgramBinary(ProgramName[program::VERT], Format, &Data[0], Size);
					glGetProgramiv(ProgramName[program::VERT], GL_LINK_STATUS, &Success);
					Validated = Validated && Success == GL_TRUE;
				}
			}
		}

		{
			GLenum Format = 0;
			GLint Size = 0;
			std::vector<glm::byte> Data;
			if(load_binary(getDataDirectory() + GEOM_PROGRAM_BINARY, Format, Data, Size))
			{
				if(is_valid_binary_format(Format))
				{
					glProgramBinary(ProgramName[program::GEOM], Format, &Data[0], Size);
					glGetProgramiv(ProgramName[program::GEOM], GL_LINK_STATUS, &Success);
					Validated = Validated && Success == GL_TRUE;
				}
			}
		}

		{
			GLenum Format = 0;
			GLint Size = 0;
			std::vector<glm::byte> Data;
			if(load_binary(getDataDirectory() + FRAG_PROGRAM_BINARY, Format, Data, Size))
			{
				if(is_valid_binary_format(Format))
				{
					glProgramBinary(ProgramName[program::FRAG], Format, &Data[0], Size);
					glGetProgramiv(ProgramName[program::FRAG], GL_LINK_STATUS, &Success);
					Validated = Validated && Success == GL_TRUE;
				}
			}
		}

		compiler Compiler;

		if(Validated && !Success)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE);

			glAttachShader(ProgramName[program::VERT], VertShaderName);
			glLinkProgram(ProgramName[program::VERT]);
		}

		if(Validated && !Success)
		{
			GLuint GeomShaderName = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + GEOM_SHADER_SOURCE);

			glAttachShader(ProgramName[program::GEOM], GeomShaderName);
			glLinkProgram(ProgramName[program::GEOM]);
		}

		if(Validated && !Success)
		{
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE);

			glAttachShader(ProgramName[program::FRAG], FragShaderName);
			glLinkProgram(ProgramName[program::FRAG]);
		}

		if(Validated)
		{
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT, ProgramName[program::VERT]);
			glUseProgramStages(PipelineName, GL_GEOMETRY_SHADER_BIT, ProgramName[program::GEOM]);
			glUseProgramStages(PipelineName, GL_FRAGMENT_SHADER_BIT, ProgramName[program::FRAG]);
			Validated = Validated && this->checkError("initProgram - stage");
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check_program(ProgramName[program::VERT]);
			Validated = Validated && Compiler.check_program(ProgramName[program::GEOM]);
			Validated = Validated && Compiler.check_program(ProgramName[program::FRAG]);
		}

		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName[program::VERT], "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName[program::FRAG], "Diffuse");
		}

		save_program(ProgramName[program::VERT], getDataDirectory() + VERT_PROGRAM_BINARY);
		save_program(ProgramName[program::GEOM], getDataDirectory() + GEOM_PROGRAM_BINARY);
		save_program(ProgramName[program::FRAG], getDataDirectory() + FRAG_PROGRAM_BINARY);

		return Validated;
	}

	bool init_texture()
	{
		glGenTextures(1, &Texture2DName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2DName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glCompressedTexImage2D(
				GL_TEXTURE_2D,
				GLint(Level),
				GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
				GLsizei(Texture[Level].extent().x),
				GLsizei(Texture[Level].extent().y),
				0, 
				GLsizei(Texture[Level].size()), 
				Texture[Level].data());
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		return this->checkError("initTexture");
	}

	bool init_buffer()
	{
		glGenBuffers(buffer::MAX, BufferName);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool init_vertexarray()
	{
		glGenVertexArrays(1, &VertexArrayName);

		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
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
		GLint NumProgramBinaryFormats(0);
		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &NumProgramBinaryFormats);

		std::vector<GLint> ProgramBinaryFormats(static_cast<std::size_t>(NumProgramBinaryFormats));
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, &ProgramBinaryFormats[0]);

		bool Validated = NumProgramBinaryFormats > 0 ? true : false;
		
		if(Validated)
			Validated = init_program();
		if(Validated)
			Validated = init_buffer();
		if(Validated)
			Validated = init_vertexarray();
		if(Validated)
			Validated = init_texture();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, BufferName);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteTextures(1, &Texture2DName);
		for(std::size_t i = 0; i < program::MAX; ++i)
			glDeleteProgram(ProgramName[i]);
		glDeleteProgramPipelines(1, &PipelineName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glProgramUniformMatrix4fv(ProgramName[program::VERT], UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glProgramUniform1i(ProgramName[program::FRAG], UniformDiffuse, 0);

		glViewportIndexedfv(0, &glm::vec4(0, 0, WindowSize.x, WindowSize.y)[0]);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glBindProgramPipeline(PipelineName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2DName);

		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]); //!\ Need to be called after glBindVertexArray
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, nullptr, 1, 0);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	// Execute the framework twice to framework the generated binary
	for(std::size_t i = 0; i < 2; ++i)
	{
		sample Sample(argc, argv);
		Error += Sample();
	}

	return Error;
}
