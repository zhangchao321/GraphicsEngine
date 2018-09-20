#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_RENDER("gl-320/fbo-blend-points.vert");
	char const* FRAG_SHADER_SOURCE_RENDER("gl-320/fbo-blend-points.frag");
	char const* VERT_SHADER_SOURCE_SPLASH("gl-320/fbo-blend-points-blit.vert");
	char const* FRAG_SHADER_SOURCE_SPLASH("gl-320/fbo-blend-points-blit.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgb8_unorm.dds");

	GLsizei const VertexCount(8);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v4fc4f);
	float Scale = 0.2f;
	glf::vertex_v4fc4f const VertexData[VertexCount] =
	{
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2(-1.0f,-1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2( 1.0f,-1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.5f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2( 1.0f, 1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.5f, 0.0f, 1.0f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2(-1.0f, 1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2( 1.0f, 0.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.5f, 1.0f, 0.0f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2( 0.0f, 1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2(-1.0f, 0.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.4f, 0.6f, 0.5f, 1.0f)),
		glf::vertex_v4fc4f(glm::vec4(glm::normalize(glm::vec2( 0.0f,-1.0f)) * Scale, 0.0f, 1.0f), glm::vec4(0.5f, 0.4f, 0.6f, 1.0f)),
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

	namespace texture
	{
		enum type
		{
			COLORBUFFER,
			MAX
		};
	}//namespace texture
	
	namespace program
	{
		enum type
		{
			RENDER,
			SPLASH,
			MAX
		};
	}//namespace program

	namespace shader
	{
		enum type
		{
			VERT_TEXTURE,
			FRAG_TEXTURE,
			VERT_SPLASH,
			FRAG_SPLASH,
			MAX
		};
	}//namespace shader
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-fbo-blend-points", framework::CORE, 3, 2),
		FramebufferName(0),
		FramebufferScale(2),
		UniformTransform(-1),
		UniformDiffuse(-1)
	{}

private:
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, program::MAX> VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	GLuint FramebufferName;
	glm::uint FramebufferScale;
	GLint UniformTransform;
	GLint UniformDiffuse;

	bool initProgram()
	{
		bool Validated(true);

		compiler Compiler;

		std::array<GLuint, shader::MAX> ShaderName;

		if(Validated)
		{
			ShaderName[shader::VERT_TEXTURE] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_RENDER, "--version 150 --profile core");
			ShaderName[shader::FRAG_TEXTURE] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_RENDER, "--version 150 --profile core");

			ProgramName[program::RENDER] = glCreateProgram();
			glAttachShader(ProgramName[program::RENDER], ShaderName[shader::VERT_TEXTURE]);
			glAttachShader(ProgramName[program::RENDER], ShaderName[shader::FRAG_TEXTURE]);

			glBindAttribLocation(ProgramName[program::RENDER], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[program::RENDER], semantic::attr::COLOR, "Color");
			glBindFragDataLocation(ProgramName[program::RENDER], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::RENDER]);
		}

		if(Validated)
		{
			ShaderName[shader::VERT_SPLASH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_SPLASH, "--version 150 --profile core");
			ShaderName[shader::FRAG_SPLASH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_SPLASH, "--version 150 --profile core");

			ProgramName[program::SPLASH] = glCreateProgram();
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::VERT_SPLASH]);
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::FRAG_SPLASH]);

			glBindFragDataLocation(ProgramName[program::SPLASH], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::SPLASH]);
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName[program::RENDER]);
			Validated = Validated && Compiler.check_program(ProgramName[program::SPLASH]);
		}

		if(Validated)
		{
			UniformTransform = glGetUniformBlockIndex(ProgramName[program::RENDER], "transform");
			UniformDiffuse = glGetUniformLocation(ProgramName[program::SPLASH], "Diffuse");

			glUseProgram(ProgramName[program::RENDER]);
			glUniformBlockBinding(ProgramName[program::RENDER], UniformTransform, semantic::uniform::TRANSFORM0);

			glUseProgram(ProgramName[program::SPLASH]);
			glUniform1i(UniformDiffuse, 0);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		glm::ivec2 WindowSize(this->getWindowSize() * this->FramebufferScale);

		glGenTextures(texture::MAX, &TextureName[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, GLsizei(WindowSize.x), GLsizei(WindowSize.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		return Validated;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(program::MAX, &VertexArrayName[0]);
		glBindVertexArray(VertexArrayName[program::RENDER]);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v4fc4f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v4fc4f), BUFFER_OFFSET(sizeof(glm::vec4)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::COLOR);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayName[program::SPLASH]);
		glBindVertexArray(0);

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);

		if(!this->checkFramebuffer(FramebufferName))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool begin()
	{
		bool Validated(true);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
		//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteFramebuffers(1, &FramebufferName);
		glDeleteProgram(ProgramName[program::SPLASH]);
		glDeleteProgram(ProgramName[program::RENDER]);
		
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(GL_UNIFORM_BUFFER,
				0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		{
			glViewport(0, 0, static_cast<GLsizei>(WindowSize.x) * this->FramebufferScale, static_cast<GLsizei>(WindowSize.y) * this->FramebufferScale);

			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);
			glEnable(GL_FRAMEBUFFER_SRGB);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(ProgramName[program::RENDER]);

			glBindVertexArray(VertexArrayName[program::RENDER]);
			glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

			glDrawArraysInstanced(GL_POINTS, 0, 8, 1);

			glDisable(GL_BLEND);
		}

		{
			glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_FRAMEBUFFER_SRGB);

			glUseProgram(ProgramName[program::SPLASH]);

			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(VertexArrayName[program::SPLASH]);
			glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
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

