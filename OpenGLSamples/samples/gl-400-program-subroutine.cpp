#include "test.hpp"

namespace
{
	char const* VERTEX_SHADER_SOURCE("gl-400/subroutine.vert");
	char const* FRAGMENT_SHADER_SOURCE("gl-400/subroutine.frag");
	char const* TEXTURE_DIFFUSE_RGB8("kueken7_rgba8_srgb.dds");
	char const* TEXTURE_DIFFUSE_DXT1("kueken7_rgb_dxt1_unorm.dds");

	GLsizei const VertexCount = 4;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount = 6;
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

	namespace texture
	{
		enum type
		{
			DXT1,
			RGB8,
			MAX
		};
	}//namespace texture

	GLuint ProgramName = 0;
	GLuint BufferName[buffer::MAX];
	GLuint TextureName[texture::MAX];
	GLuint VertexArrayName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
	GLint UniformRGB8 = 0;
	GLint UniformDXT1 = 0;
	GLint UniformDisplacement = 0;
	GLuint IndexDXT1 = 0;
	GLuint IndexRGB8 = 0;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-program-subroutine", framework::CORE, 4, 0, glm::vec2(0.0f))
	{}

private:
	bool initTest()
	{
		glEnable(GL_DEPTH_TEST);

		return this->checkError("initTest");
	}

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERTEX_SHADER_SOURCE);
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			GLint ProgramVertSubroutine(0);
			GLint ProgramFragSubroutine(0);
			glGetProgramStageiv(ProgramName, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &ProgramVertSubroutine);
			glGetProgramStageiv(ProgramName, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &ProgramFragSubroutine);

			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDXT1 = glGetUniformLocation(ProgramName, "DiffuseDXT1");
			UniformRGB8 = glGetUniformLocation(ProgramName, "DiffuseRGB8");
			UniformDisplacement = glGetUniformLocation(ProgramName, "Displacement");
			UniformDiffuse = glGetSubroutineUniformLocation(ProgramName, GL_FRAGMENT_SHADER, "Diffuse");
			IndexDXT1 = glGetSubroutineIndex(ProgramName, GL_FRAGMENT_SHADER, "diffuseLQ");
			IndexRGB8 = glGetSubroutineIndex(ProgramName, GL_FRAGMENT_SHADER, "diffuseHQ");
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, BufferName);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		glBindVertexArray(0);

		return true;
	}

	bool initTexture()
	{
		glGenTextures(texture::MAX, TextureName);

		gli::gl GL(gli::gl::PROFILE_GL33);

		{
			gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE_RGB8).c_str()));
			gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGB8]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels()));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
			gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::DXT1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels()));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, GLint(Level),
					Format.Internal,
					GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
					0, 
					GLsizei(Texture[Level].size()), 
					Texture[Level].data());
			}
		}

		return true;
	}

	bool begin()
	{
		bool Validated = true;

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

		this->logImplementationDependentLimit(GL_MAX_SUBROUTINES, "GL_MAX_SUBROUTINES");

		return Validated;
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(buffer::MAX, BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(texture::MAX, TextureName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));

		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH, 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGB8]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DXT1]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(UniformRGB8, 0);
		glUniform1i(UniformDXT1, 1);

		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);

		glUniform1f(UniformDisplacement,  1.1f);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &IndexDXT1);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, nullptr, 1, 0);

		glUniform1f(UniformDisplacement, -1.1f);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &IndexRGB8);
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

