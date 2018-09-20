#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-330/texture-format-packed.vert");
	char const* FRAG_SHADER_SOURCE("gl-330/texture-format-packed.frag");

	GLsizei const VertexCount(3);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 3.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 3.0f,-1.0f), glm::vec2(1.0f, 0.0f)),
	};

	namespace texture
	{
		enum type
		{
			RGBA4,
			RGBA4_REV,
			BGRA4,
			BGRA4_REV,
			MAX
		};
	}//namespace texture

	glm::ivec4 const Viewport[texture::MAX] =
	{
		glm::ivec4(  0,   0, 320, 240),
		glm::ivec4(320,   0, 320, 240),
		glm::ivec4(  0, 240, 320, 240),
		glm::ivec4(320, 240, 320, 240)
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-330-texture-format-packed", framework::CORE, 3, 3)
		, VertexArrayName(0)
		, ProgramName(0)
		, BufferName(0)
		, UniformMVP(-1)
		, UniformDiffuse(-1)
	{}

private:
	GLuint VertexArrayName;
	GLuint ProgramName;
	GLuint BufferName;
	GLint UniformMVP;
	GLint UniformDiffuse;
	std::array<GLuint, texture::MAX> TextureName;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;
		GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
		GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");
		Validated = Validated && Compiler.check();

		this->ProgramName = glCreateProgram();
		glAttachShader(this->ProgramName, VertShaderName);
		glAttachShader(this->ProgramName, FragShaderName);

		glBindAttribLocation(this->ProgramName, semantic::attr::POSITION, "Position");
		glBindAttribLocation(this->ProgramName, semantic::attr::TEXCOORD, "Texcoord");
		glBindFragDataLocation(this->ProgramName, semantic::frag::COLOR, "Color");
		glLinkProgram(this->ProgramName);
		Validated = Validated && Compiler.check_program(this->ProgramName);

		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		Validated = Validated && (UniformMVP != -1);
		UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		Validated = Validated && (UniformDiffuse != -1);

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");;
	}

	bool initTexture()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(texture::MAX, &this->TextureName[0]);

		GLushort const DataRGBA4 = 0xF80C;
		GLushort const DataRGBA4REV = 0xC08F;
		GLushort const DataBGRA4 = 0x08FC;
		GLushort const DataBGRA4REV = 0xCF80;

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA4]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(0), GL_RGBA4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, &DataRGBA4);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA4_REV]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(0), GL_RGBA4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV, &DataRGBA4REV);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::BGRA4]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
		glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(0), GL_RGBA4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, &DataBGRA4);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::BGRA4_REV]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
		glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(0), GL_RGBA4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV, &DataBGRA4REV);

		return this->checkError("initTexture");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
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
		bool Validated = true;

		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
		glm::mat4 MVP = Projection * this->view() * Model;

		glUseProgram(ProgramName);
		glUniform1i(UniformDiffuse, 0);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayName);
		glActiveTexture(GL_TEXTURE0);

		glViewport(Viewport[0].x, Viewport[0].y, Viewport[0].z, Viewport[0].w);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA4]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

		glViewport(Viewport[1].x, Viewport[1].y, Viewport[1].z, Viewport[1].w);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RGBA4_REV]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

		glViewport(Viewport[2].x, Viewport[2].y, Viewport[2].z, Viewport[2].w);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::BGRA4]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

		glViewport(Viewport[3].x, Viewport[3].y, Viewport[3].z, Viewport[3].w);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::BGRA4_REV]);
		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

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

