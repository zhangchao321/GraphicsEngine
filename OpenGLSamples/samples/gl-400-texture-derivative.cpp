#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_TEXTURE("gl-400/texture-derivative2.vert");
	char const* FRAG_SHADER_SOURCE_TEXTURE("gl-400/texture-derivative2.frag");
	char const* VERT_SHADER_SOURCE_SPLASH("gl-400/texture-derivative1.vert");
	char const* FRAG_SHADER_SOURCE_SPLASH("gl-400/texture-derivative1.frag");
	int const FRAMEBUFFER_SIZE(8);
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-2.0f,-2.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 2.0f,-2.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 2.0f, 2.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-2.0f, 2.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
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
			DIFFUSE,
			COLORBUFFER,
			RENDERBUFFER,
			MAX
		};
	}//namespace texture
	
	namespace program
	{
		enum type
		{
			TEXTURE,
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

	GLuint FramebufferName(0);
	std::vector<GLuint> ShaderName(shader::MAX);
	std::vector<GLuint> ProgramName(program::MAX);
	std::vector<GLuint> VertexArrayName(program::MAX);
	std::vector<GLuint> BufferName(buffer::MAX);
	std::vector<GLuint> TextureName(texture::MAX);
	std::vector<GLint> UniformDiffuse(program::MAX);
	GLint UniformTransform(0);
	GLint UniformUseGrad(0);
	GLint UniformFramebufferSize(0);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-texture-derivative", framework::CORE, 4, 0, glm::vec2(glm::pi<float>(), -glm::pi<float>() * 0.45f))
	{}

private:
	bool initProgram()
	{
		bool Validated(true);

		compiler Compiler;

		if(Validated)
		{
			ShaderName[shader::VERT_TEXTURE] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_TEXTURE, "--version 400 --profile core");
			ShaderName[shader::FRAG_TEXTURE] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_TEXTURE, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[program::TEXTURE] = glCreateProgram();
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::VERT_TEXTURE]);
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::FRAG_TEXTURE]);

			glBindAttribLocation(ProgramName[program::TEXTURE], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[program::TEXTURE], semantic::attr::TEXCOORD, "Texcoord");
			glBindFragDataLocation(ProgramName[program::TEXTURE], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::TEXTURE]);

			Validated = Validated && Compiler.check_program(ProgramName[program::TEXTURE]);
		}

		if(Validated)
		{
			UniformTransform = glGetUniformBlockIndex(ProgramName[program::TEXTURE], "transform");
			UniformDiffuse[program::TEXTURE] = glGetUniformLocation(ProgramName[program::TEXTURE], "Diffuse");
			UniformUseGrad = glGetUniformLocation(ProgramName[program::TEXTURE], "UseGrad");
		}
		
		if(Validated)
		{
			ShaderName[shader::VERT_SPLASH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_SPLASH, "--version 400 --profile core");
			ShaderName[shader::FRAG_SPLASH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_SPLASH, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName[program::SPLASH] = glCreateProgram();
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::VERT_SPLASH]);
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::FRAG_SPLASH]);

			glBindFragDataLocation(ProgramName[program::SPLASH], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::SPLASH]);

			Validated = Validated && Compiler.check_program(ProgramName[program::SPLASH]);
		}
	
		if(Validated)
		{
			UniformDiffuse[program::SPLASH] = glGetUniformLocation(ProgramName[program::SPLASH], "Diffuse");
			UniformFramebufferSize = glGetUniformLocation(ProgramName[program::SPLASH], "FramebufferSize");
		}

		return Validated && this->checkError("initProgram");
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
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		std::size_t TextureSize(128);
		gli::texture2d Texture(gli::FORMAT_RGBA8_UNORM_PACK8, gli::texture2d::extent_type(TextureSize));
		Texture[0].clear(glm::u8vec4(255, 255, 255, 255));
		Texture[1].clear(glm::u8vec4(224, 224, 224, 255));
		Texture[2].clear(glm::u8vec4(192, 192, 192, 255));
		Texture[3].clear(glm::u8vec4(160, 160, 160, 255));
		Texture[4].clear(glm::u8vec4(128, 128, 128, 255));
		Texture[5].clear(glm::u8vec4( 96,  96,  96, 255));
		Texture[6].clear(glm::u8vec4( 64,  64,  64, 255));
		Texture[7].clear(glm::u8vec4( 32,  32,  32, 255));

		assert(!Texture.empty());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const& Format = GL.translate(Texture.format(), Texture.swizzles());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(texture::MAX, &TextureName[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 2));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexImage2D(GL_TEXTURE_2D, GLint(Level),
				Format.Internal,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				0,
				Format.External, Format.Type,
				Texture[Level].data());
		}

		glm::ivec2 WindowSize(this->getWindowSize());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, GLint(0), GL_RGBA8, GLsizei(WindowSize.x / FRAMEBUFFER_SIZE), GLsizei(WindowSize.y / FRAMEBUFFER_SIZE), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::RENDERBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, GLint(0), GL_DEPTH_COMPONENT24, GLsizei(WindowSize.x / FRAMEBUFFER_SIZE), GLsizei(WindowSize.y / FRAMEBUFFER_SIZE), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return Validated;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(program::MAX, &VertexArrayName[0]);
		glBindVertexArray(VertexArrayName[program::TEXTURE]);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
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
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureName[texture::RENDERBUFFER], 0);

		if(!this->checkFramebuffer(FramebufferName))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool begin()
	{
		bool Validated(true);

		float MinFragmentInterpolationOffset(0);
		float MaxFragmentInterpolationOffset(0);
		float FragmentInterpolationOffsetBits(0);
		glGetFloatv(GL_MIN_FRAGMENT_INTERPOLATION_OFFSET, &MinFragmentInterpolationOffset);
		glGetFloatv(GL_MAX_FRAGMENT_INTERPOLATION_OFFSET, &MaxFragmentInterpolationOffset);
		glGetFloatv(GL_FRAGMENT_INTERPOLATION_OFFSET_BITS, &FragmentInterpolationOffsetBits);

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
		glDeleteProgram(ProgramName[program::TEXTURE]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);

		return true;
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());
		glm::ivec2 FramebufferSize = WindowSize / FRAMEBUFFER_SIZE;

		// Update of the uniform buffer
		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.2f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);

			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glViewport(0, 0, FramebufferSize.x, FramebufferSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glUseProgram(ProgramName[program::TEXTURE]);
		glUniform1i(UniformDiffuse[program::TEXTURE], 0);
		glUniformBlockBinding(ProgramName[program::TEXTURE], UniformTransform, semantic::uniform::TRANSFORM0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
		glBindVertexArray(VertexArrayName[program::TEXTURE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glEnable(GL_SCISSOR_TEST);
			glScissor(0, 0, FramebufferSize.x / 2, FramebufferSize.y);
			glUniform1i(UniformUseGrad, 1);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 2, 0);

			glScissor(FramebufferSize.x / 2, 0, FramebufferSize.x / 2, FramebufferSize.y);
			glUniform1i(UniformUseGrad, 0);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 2, 0);
		glDisable(GL_SCISSOR_TEST);

		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(ProgramName[program::SPLASH]);
		glUniform1i(UniformDiffuse[program::SPLASH], 0);
		glUniform1f(UniformFramebufferSize, static_cast<float>(FRAMEBUFFER_SIZE));

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VertexArrayName[program::SPLASH]);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);

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

