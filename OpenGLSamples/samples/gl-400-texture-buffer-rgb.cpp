#include "test.hpp"

namespace
{
	char const* VERTEX_SHADER_SOURCE("gl-400/texture-buffer.vert");
	char const* FRAGMENT_SHADER_SOURCE("gl-400/texture-buffer.frag");

	GLsizei const VertexCount = 4;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const VertexData[VertexCount] = 
	{
		glm::vec2(-1.0f,-1.0f), 
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] = 
	{
		0, 1, 2, 
		2, 3, 0
	};

	enum buffer_type
	{
		BUFFER_VERTEX,
		BUFFER_ELEMENT,
		BUFFER_DISPLACEMENT,
		BUFFER_DIFFUSE,
		BUFFER_MAX
	};

	enum texture_type
	{
		TEXTURE_DISPLACEMENT,
		TEXTURE_DIFFUSE,
		TEXTURE_MAX
	};

	GLuint BufferName[BUFFER_MAX];
	GLuint TextureName[TEXTURE_MAX];
	GLuint VertexArrayName = 0;
	GLuint ProgramName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
	GLint UniformDisplacement = 0;
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-410-texture-buffer-rgb", framework::CORE, 4, 0, glm::vec2(glm::pi<float>() * 0.2f))
	{}

private:
	bool initTest()
	{
		bool Validated = true;
		glEnable(GL_DEPTH_TEST);

		return Validated && this->checkError("initTest");
	}

	bool initProgram()
	{
		bool Validated = true;

		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERTEX_SHADER_SOURCE);
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
			UniformDisplacement = glGetUniformLocation(ProgramName, "Displacement");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(BUFFER_MAX, BufferName);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glm::vec3 Displacement[5] = 
		{
			glm::vec3( 0.1f, 0.3f,-1.0f), 
			glm::vec3(-0.5f, 0.0f,-0.5f),
			glm::vec3(-0.2f,-0.2f, 0.0f),
			glm::vec3( 0.3f, 0.2f, 0.5f),
			glm::vec3( 0.1f,-0.3f, 1.0f)
		};

		glBindBuffer(GL_TEXTURE_BUFFER, BufferName[BUFFER_DISPLACEMENT]);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(Displacement), Displacement, GL_STATIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		glm::vec3 Diffuse[5] = 
		{
			glm::vec3(1.0f, 0.0f, 0.0f), 
			glm::vec3(1.0f, 0.5f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		};	

		glBindBuffer(GL_TEXTURE_BUFFER, BufferName[BUFFER_DIFFUSE]);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(Diffuse), Diffuse, GL_STATIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool initTexture()
	{
		glGenTextures(TEXTURE_MAX, TextureName);

		glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DISPLACEMENT]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, BufferName[BUFFER_DISPLACEMENT]);
		glBindTexture(GL_TEXTURE_BUFFER, 0);

		glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DIFFUSE]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, BufferName[BUFFER_DIFFUSE]);
		glBindTexture(GL_TEXTURE_BUFFER, 0);	

		return this->checkError("initTextureBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
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
			Validated = initTexture();
		if(Validated)
			Validated = initVertexArray();

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteTextures(TEXTURE_MAX, TextureName);
		glDeleteBuffers(BUFFER_MAX, BufferName);
		glDeleteProgram(ProgramName);
		glDeleteVertexArrays(1, &VertexArrayName);

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
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(UniformDisplacement, 0);
		glUniform1i(UniformDiffuse, 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DISPLACEMENT]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DIFFUSE]);

		glBindVertexArray(VertexArrayName);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 5, 0);

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

