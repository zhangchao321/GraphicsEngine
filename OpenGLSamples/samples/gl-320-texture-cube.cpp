#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/texture-cube.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/texture-cube.frag");

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

	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader

	std::vector<GLuint> ShaderName(shader::MAX);
	GLuint VertexArrayName = 0;
	GLuint BufferName = 0;
	GLuint TextureName = 0;
	GLuint ProgramName = 0;
	GLint UniformMV = 0;
	GLint UniformMVP = 0;
	GLint UniformEnvironment = 0;
	GLint UniformCamera = 0;
	glm::ivec4 Viewport;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-texture-cube", framework::CORE, 3, 2, glm::vec2(glm::pi<float>() * 0.1f))
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			ShaderName[shader::VERT] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			ShaderName[shader::FRAG] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, ShaderName[shader::VERT]);
			glAttachShader(ProgramName, ShaderName[shader::FRAG]);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			UniformMV = glGetUniformLocation(ProgramName, "MV");
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformEnvironment = glGetUniformLocation(ProgramName, "Environment");
			UniformCamera = glGetUniformLocation(ProgramName, "Camera");
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		gli::texture_cube Texture(gli::FORMAT_RGBA8_UNORM_PACK8, gli::texture_cube::extent_type(2), 1);
		assert(!Texture.empty());

		Texture[0].clear<glm::u8vec4>(glm::u8vec4(255,   0,   0, 255));
		Texture[1].clear<glm::u8vec4>(glm::u8vec4(255, 128,   0, 255));
		Texture[2].clear<glm::u8vec4>(glm::u8vec4(255, 255,   0, 255));
		Texture[3].clear<glm::u8vec4>(glm::u8vec4(  0, 255,   0, 255));
		Texture[4].clear<glm::u8vec4>(glm::u8vec4(  0, 255, 255, 255));
		Texture[5].clear<glm::u8vec4>(glm::u8vec4(  0,   0, 255, 255));

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &TextureName);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureName);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_LOD, -1000.f);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LOD, 1000.f);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, 0.0f);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		for(gli::texture_cube::size_type Face = 0; Face < Texture.faces(); ++Face)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(Face),
				0,
				Format.Internal,
				static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y),
				0,
				Format.External, Format.Type,
				Texture[Face].data());
		}

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), BUFFER_OFFSET(0));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated = true;

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
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(1, &TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 2.0f / 3.0f, 0.1f, 1000.0f);
		glm::mat4 View = this->view();
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;
		glm::mat4 MV = View * Model;

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMV, 1, GL_FALSE, &MV[0][0]);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(UniformEnvironment, 0);
		glUniform3fv(UniformCamera, 1, &glm::vec3(0.0f, 0.0f, -this->cameraDistance())[0]);

		glBindVertexArray(VertexArrayName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureName);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glViewport(0, 0, WindowSize.x >> 1, WindowSize.y);

		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glViewport(WindowSize.x >> 1, 0, WindowSize.x >> 1, WindowSize.y);

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

