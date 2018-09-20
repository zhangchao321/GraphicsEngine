#include "test.hpp"

namespace
{
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");

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
			OFFSET,
			BICUBIC,
			MAX
		};
	}//namespace program

	char const* SHADER_VERT("gl-320/texture-offset.vert");
	char const* SHADER_FRAG[program::MAX] = 
	{
		"gl-320/texture-offset.frag",
		"gl-320/texture-offset-bicubic.frag",
	};

	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			FRAG_BICUBIC,
			MAX
		};
	}//namespace shader

	GLuint VertexArrayName(0);
	GLuint TextureName(0);
	std::vector<GLuint> BufferName(buffer::MAX);
	std::vector<GLuint> ProgramName(program::MAX);
	std::vector<GLint> UniformMVP(program::MAX);
	std::vector<GLint> UniformDiffuse(program::MAX);
	GLint UniformOffset;
	std::vector<glm::vec4> Viewport(program::MAX);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-texture-offset", framework::CORE, 3, 2)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;

		std::array<GLuint, shader::MAX> ShaderName;
		
		compiler Compiler;
		ShaderName[shader::VERT] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SHADER_VERT, "--version 150 --profile core");
		ShaderName[shader::FRAG] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SHADER_FRAG[program::OFFSET], "--version 150 --profile core");
		ShaderName[shader::FRAG_BICUBIC] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SHADER_FRAG[program::BICUBIC], "--version 150 --profile core");
		Validated = Validated && Compiler.check();

		for(std::size_t i = 0; i < program::MAX; ++i)
		{
			ProgramName[i] = glCreateProgram();
			glAttachShader(ProgramName[i], ShaderName[shader::VERT]);
			glAttachShader(ProgramName[i], ShaderName[shader::FRAG + i]);
			
			glBindAttribLocation(ProgramName[i], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[i], semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName[i], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[i]);
			Validated = Validated && Compiler.check_program(ProgramName[i]);
		
			UniformMVP[i] = glGetUniformLocation(ProgramName[i], "MVP");
			UniformDiffuse[i] = glGetUniformLocation(ProgramName[i], "Diffuse");
			if(i == program::OFFSET)
				UniformOffset = glGetUniformLocation(ProgramName[program::OFFSET], "Offset");
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

		return true;
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glGenTextures(1, &TextureName);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}

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

		glm::ivec2 WindowSize(this->getWindowSize());
		int const Border(1);
		glm::ivec2 const ViewportSize = WindowSize / 2 - Border * 2;

		Viewport[program::OFFSET] = glm::vec4(Border, Border, WindowSize.x / 2 - Border * 2, WindowSize.y - Border * 2);
		Viewport[program::BICUBIC] = glm::vec4(Border + WindowSize.x / 2, Border, WindowSize.x / 2 - Border * 2, WindowSize.y - Border * 2);

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
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		for(int i = 0; i < program::MAX; ++i)
			glDeleteProgram(ProgramName[i]);
		glDeleteTextures(1, &TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 2.0f / 3.0f, 0.1f, 1000.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);

		glBindVertexArray(VertexArrayName);

		glUseProgram(ProgramName[program::OFFSET]);
		glUniform2iv(UniformOffset, 1, &glm::ivec2(48, -80)[0]);

		for(std::size_t i = 0; i < program::MAX; ++i)
		{
			glUseProgram(ProgramName[i]);
			glUniformMatrix4fv(UniformMVP[i], 1, GL_FALSE, &MVP[0][0]);
			glUniform1i(UniformDiffuse[i], 0);

			glViewport(GLint(Viewport[i].x), GLint(Viewport[i].y), GLsizei(Viewport[i].z), GLsizei(Viewport[i].w));

			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, nullptr, 1, 0);
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

