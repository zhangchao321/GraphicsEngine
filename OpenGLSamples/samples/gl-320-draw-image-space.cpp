#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/draw-image-space.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/draw-image-space.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-320-draw-image-space", framework::CORE, 3, 2, glm::vec2(glm::pi<float>() * 0.2f))
		, ProgramName(0)
		, VertexArrayName(0)
		, TextureName(0)
		, UniformDiffuse(0)
	{}

private:
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint TextureName;
	GLint UniformDiffuse;

	bool initProgram()
	{
		bool Validated(true);
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");
			Validated = Validated && Compiler.check();

			this->ProgramName = glCreateProgram();
			glAttachShader(this->ProgramName, VertShaderName);
			glAttachShader(this->ProgramName, FragShaderName);

			glBindFragDataLocation(this->ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(this->ProgramName);

			this->UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(this->ProgramName);
		}

		return Validated;
	}

	bool initTexture()
	{
		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		assert(!Texture.empty());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &TextureName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		for(gli::texture2d::size_type Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}
	
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated(true);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();

		return Validated;
	}

	bool end()
	{
		glDeleteProgram(this->ProgramName);
		glDeleteTextures(1, &this->TextureName);
		glDeleteVertexArrays(1, &this->VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 const WindowSize(this->getWindowSize());
		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));

		glUseProgram(this->ProgramName);
		glUniform1i(this->UniformDiffuse, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TextureName);
		glBindVertexArray(this->VertexArrayName);

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

