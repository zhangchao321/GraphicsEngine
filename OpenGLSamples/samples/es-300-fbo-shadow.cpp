#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_DEPTH("es-300/fbo-shadow-depth.vert");
	char const* FRAG_SHADER_SOURCE_DEPTH("es-300/fbo-shadow-depth.frag");
	char const* VERT_SHADER_SOURCE_RENDER("es-300/fbo-shadow-render.vert");
	char const* FRAG_SHADER_SOURCE_RENDER("es-300/fbo-shadow-render.frag");

	GLsizei const VertexCount(8);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v3fv4u8);
	glf::vertex_v3fv4u8 const VertexData[VertexCount] =
	{
		glf::vertex_v3fv4u8(glm::vec3(-1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255))
	};

	GLsizei const ElementCount(12);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0,
		4, 5, 6, 
		6, 7, 4,
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
			COLORBUFFER,
			DEPTHBUFFER,
			SHADOWMAP,
			MAX
		};
	}//namespace texture
	
	namespace program
	{
		enum type
		{
			DEPTH,
			RENDER,
			MAX
		};
	}//namespace program

	namespace framebuffer
	{
		enum type
		{
			FRAMEBUFFER,
			SHADOW,
			MAX
		};
	}//namespace framebuffer

	namespace shader
	{
		enum type
		{
			VERT_RENDER,
			FRAG_RENDER,
			VERT_DEPTH,
			FRAG_DEPTH,
			MAX
		};
	}//namespace shader

	glm::ivec2 const ShadowSize(64, 64);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "es-300-fbo-shadow", framework::ES, 3, 0, glm::vec2(0.0f, -glm::pi<float>() * 0.3f))
	{}

private:
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, program::MAX> VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	GLint UniformRenderMVP;
	GLint UniformRenderDepthBiasMVP;
	GLint UniformRenderShadow;
	GLint UniformDepthMVP;

	bool initProgram()
	{
		bool Validated(true);
	
		std::vector<GLuint> ShaderName(shader::MAX);
		
		if(Validated)
		{
			compiler Compiler;
			ShaderName[shader::VERT_RENDER] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_RENDER);
			ShaderName[shader::FRAG_RENDER] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_RENDER);
			Validated = Validated && Compiler.check();

			ProgramName[program::RENDER] = glCreateProgram();
			glAttachShader(ProgramName[program::RENDER], ShaderName[shader::VERT_RENDER]);
			glAttachShader(ProgramName[program::RENDER], ShaderName[shader::FRAG_RENDER]);
			glBindAttribLocation(ProgramName[program::RENDER], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[program::RENDER], semantic::attr::COLOR, "Color");
			glLinkProgram(ProgramName[program::RENDER]);

			Validated = Validated && Compiler.check_program(ProgramName[program::RENDER]);
		}

		if(Validated)
		{
			UniformRenderShadow = glGetUniformLocation(ProgramName[program::RENDER], "Shadow");
			UniformRenderMVP = glGetUniformLocation(ProgramName[program::RENDER], "MVP");
			UniformRenderDepthBiasMVP = glGetUniformLocation(ProgramName[program::RENDER], "DepthBiasMVP");
		}

		if(Validated)
		{
			compiler Compiler;
			ShaderName[shader::VERT_DEPTH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_DEPTH);
			ShaderName[shader::FRAG_DEPTH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_DEPTH);
			Validated = Validated && Compiler.check();

			ProgramName[program::DEPTH] = glCreateProgram();
			glAttachShader(ProgramName[program::DEPTH], ShaderName[shader::VERT_DEPTH]);
			glAttachShader(ProgramName[program::DEPTH], ShaderName[shader::FRAG_DEPTH]);
			glBindAttribLocation(ProgramName[program::DEPTH], semantic::attr::POSITION, "Position");
			glLinkProgram(ProgramName[program::DEPTH]);

			Validated = Validated && Compiler.check_program(ProgramName[program::DEPTH]);
		}

		if(Validated)
			UniformDepthMVP = glGetUniformLocation(ProgramName[program::DEPTH], "MVP");

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
		glGenTextures(texture::MAX, &TextureName[0]);

		glm::ivec2 WindowSize(this->getWindowSize());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::DEPTHBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_DEPTH_COMPONENT24, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::SHADOWMAP]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_DEPTH_COMPONENT24, GLsizei(ShadowSize.x), GLsizei(ShadowSize.y));

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(program::MAX, &VertexArrayName[0]);
		glBindVertexArray(VertexArrayName[program::RENDER]);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv4u8), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glf::vertex_v3fv4u8), BUFFER_OFFSET(sizeof(glm::vec3)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::COLOR);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(framebuffer::MAX, &FramebufferName[0]);

		GLenum const BuffersRender = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::FRAMEBUFFER]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureName[texture::DEPTHBUFFER], 0);
		glDrawBuffers(1, &BuffersRender);
		if(!this->checkFramebuffer(FramebufferName[framebuffer::FRAMEBUFFER]))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::SHADOW]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureName[texture::SHADOWMAP], 0);
		if(!this->checkFramebuffer(FramebufferName[framebuffer::SHADOW]))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

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
		if(Validated)
			Validated = initFramebuffer();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		for(std::size_t i = 0; i < program::MAX; ++i)
			glDeleteProgram(ProgramName[i]);

		glDeleteFramebuffers(framebuffer::MAX, &FramebufferName[0]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);

		return this->checkError("end");
	}

	void renderShadow()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glViewport(0, 0, ShadowSize.x, ShadowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::SHADOW]);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);

		glBindVertexArray(VertexArrayName[program::RENDER]);
		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr);

		glDisable(GL_DEPTH_TEST);

		this->checkError("renderShadow");
	}

	void renderFramebuffer()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glm::ivec2 WindowSize(this->getWindowSize());
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName[texture::SHADOWMAP]);

		glBindVertexArray(VertexArrayName[program::RENDER]);
		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr);

		glDisable(GL_DEPTH_TEST);
		
		this->checkError("renderFramebuffer");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Model = glm::mat4(1.0f);

		glm::mat4 DepthProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f,-4.0f, 8.0f);
		glm::mat4 DepthView = glm::lookAt(glm::vec3(0.5, 1.0, 2.0), glm::vec3(0), glm::vec3(0, 0, 1));
		glm::mat4 DepthMVP = DepthProjection * DepthView * Model;

		glm::mat4 BiasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

		glm::mat4 DepthMVPBias = BiasMatrix * DepthMVP;

		glm::mat4 RenderProjection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 10.0f);
		glm::mat4 RenderMVP = RenderProjection * this->view() * Model;

		glUseProgram(ProgramName[program::DEPTH]);
		glUniformMatrix4fv(this->UniformDepthMVP, 1, GL_FALSE, &DepthMVP[0][0]);

		renderShadow();

		glUseProgram(ProgramName[program::RENDER]);
		glUniform1i(this->UniformRenderShadow, 0);
		glUniformMatrix4fv(this->UniformRenderMVP, 1, GL_FALSE, &RenderMVP[0][0]);
		glUniformMatrix4fv(this->UniformRenderDepthBiasMVP, 1, GL_FALSE, &DepthMVPBias[0][0]);

		renderFramebuffer();

		return this->checkError("render");
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	sample Sample(argc, argv);
	Error += Sample();

	return Error;
}

