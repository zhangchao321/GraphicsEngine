#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-330/texture-2d.vert");
	char const* FRAG_SHADER_SOURCE("gl-330/texture-2d.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba_dxt5_unorm.dds");

	struct vertex
	{
		vertex
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount = 6;
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex);
	vertex const VertexData[VertexCount] =
	{
		vertex(glm::vec2(-1.0f,-1.0f) * 1.5f, glm::vec2(-2.0f, 2.0f)),
		vertex(glm::vec2( 1.0f,-1.0f) * 1.5f, glm::vec2( 2.0f, 2.0f)),
		vertex(glm::vec2( 1.0f, 1.0f) * 1.5f, glm::vec2( 2.0f,-2.0f)),
		vertex(glm::vec2( 1.0f, 1.0f) * 1.5f, glm::vec2( 2.0f,-2.0f)),
		vertex(glm::vec2(-1.0f, 1.0f) * 1.5f, glm::vec2(-2.0f,-2.0f)),
		vertex(glm::vec2(-1.0f,-1.0f) * 1.5f, glm::vec2(-2.0f, 2.0f))
	};

	namespace viewport
	{
		enum type
		{
			V00,
			V10,
			V11,
			V01,
			MAX
		};
	}//namespace viewport

	GLuint VertexArrayName;
	GLuint ProgramName;
	GLuint BufferName;
	GLuint Texture2DName;
	GLint UniformMVP;
	GLint UniformDiffuse;
	GLuint SamplerName[viewport::MAX];
	glm::ivec4 Viewport[viewport::MAX];
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-330-sampler-wrap", framework::CORE, 3, 3)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 330 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 330 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &BufferName);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	#define GL_MIRROR_CLAMP_TO_EDGE			0x8743
	#define GL_MIRROR_CLAMP_TO_BORDER_EXT	0x8912

	bool initSampler()
	{
		glGenSamplers(viewport::MAX, SamplerName);

		for(std::size_t i = 0; i < viewport::MAX; ++i)
		{
			glSamplerParameteri(SamplerName[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameteri(SamplerName[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glm::vec4 BorderColor(0.0f, 0.5f, 1.0f, 1.0f);
			glSamplerParameterfv(SamplerName[i], GL_TEXTURE_BORDER_COLOR, &BorderColor[0]);
		}

		glSamplerParameteri(SamplerName[viewport::V00], GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glSamplerParameteri(SamplerName[viewport::V10], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(SamplerName[viewport::V11], GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(SamplerName[viewport::V01], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glSamplerParameteri(SamplerName[viewport::V00], GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glSamplerParameteri(SamplerName[viewport::V10], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(SamplerName[viewport::V11], GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(SamplerName[viewport::V01], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		return this->checkError("initSampler");
	}

	bool initTexture()
	{
		glGenTextures(1, &Texture2DName);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2DName);

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

		return this->checkError("initTexture");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		Viewport[viewport::V00] = glm::ivec4(0, 0, WindowSize >> 1);
		Viewport[viewport::V10] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		Viewport[viewport::V11] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		Viewport[viewport::V01] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initSampler();
		if(Validated)
			Validated = initVertexArray();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteTextures(1, &Texture2DName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 1000.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(UniformDiffuse, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2DName);

		glBindVertexArray(VertexArrayName);

		for(std::size_t Index = 0; Index < viewport::MAX; ++Index)
		{
			glViewport(Viewport[Index].x, Viewport[Index].y, Viewport[Index].z, Viewport[Index].w);
			glBindSampler(0, SamplerName[Index]);
			glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
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
