#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-320/buffer-uniform.vert");
	char const* FRAG_SHADER_SOURCE("gl-320/buffer-uniform.frag");

	struct vertex_v3fn3fc4f
	{
		vertex_v3fn3fc4f
		(
			glm::vec3 const & Position,
			glm::vec3 const & Normal,
			glm::vec4 const & Color
		) :
			Position(Position),
			Normal(Normal),
			Color(Color)
		{}

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec4 Color;
	};

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v3fn3fc4f);
	vertex_v3fn3fc4f const VertexData[VertexCount] =
	{
		vertex_v3fn3fc4f(glm::vec3(-1.0f,-1.0f, 0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3( 1.0f,-1.0f, 0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3( 1.0f, 1.0f, 0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3(-1.0f, 1.0f, 0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

/*
	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v3fn3fc4f);
	vertex_v3fn3fc4f const VertexData[VertexCount] =
	{
		vertex_v3fn3fc4f(glm::vec3(-1.000f, -0.732f, -0.732f), glm::normalize(glm::vec3(-1.000f, -0.732f, -0.732f)), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3( 1.000f, -0.732f, -0.732f), glm::normalize(glm::vec3( 1.000f, -0.732f, -0.732f)), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3( 0.000f,  1.000f, -0.732f), glm::normalize(glm::vec3( 0.000f,  1.000f, -0.732f)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
		vertex_v3fn3fc4f(glm::vec3( 0.000f,  0.000f,  1.000f), glm::normalize(glm::vec3( 0.000f,  0.000f,  1.000f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	};

	GLsizei const ElementCount(12);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 2, 1,
		0, 1, 3,
		1, 2, 3,
		2, 0, 3
	};
*/

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			PER_SCENE,
			PER_PASS,
			PER_DRAW,
			MAX
		};
	}//namespace buffer

	namespace uniform
	{
		enum type
		{
			PER_SCENE = 0,
			PER_PASS = 1,
			PER_DRAW = 2,
			LIGHT = 3
		};
	};

	struct material
	{
		glm::vec3 Ambient;
		float Padding1;
		glm::vec3 Diffuse;
		float Padding2;
		glm::vec3 Specular;
		float Shininess;
	};

	struct light
	{
		glm::vec3 Position;
	};

	struct transform
	{
		glm::mat4 P;
		glm::mat4 MV;
		glm::mat3 Normal;
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-buffer-uniform", framework::CORE, 3, 2),
		VertexArrayName(0),
		ProgramName(0),
		UniformPerDraw(0),
		UniformPerPass(0),
		UniformPerScene(0)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLint UniformPerDraw;
	GLint UniformPerPass;
	GLint UniformPerScene;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 150 --profile core");

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::NORMAL, "Normal");
			glBindAttribLocation(ProgramName, semantic::attr::COLOR, "Color");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			this->UniformPerDraw = glGetUniformBlockIndex(ProgramName, "per_draw");
			this->UniformPerPass = glGetUniformBlockIndex(ProgramName, "per_pass");
			this->UniformPerScene = glGetUniformBlockIndex(ProgramName, "per_scene");

			glUniformBlockBinding(ProgramName, this->UniformPerDraw, uniform::PER_DRAW);
			glUniformBlockBinding(ProgramName, this->UniformPerPass, uniform::PER_PASS);
			glUniformBlockBinding(ProgramName, this->UniformPerScene, uniform::PER_SCENE);
		}
	
		return Validated;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_v3fn3fc4f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_v3fn3fc4f), BUFFER_OFFSET(sizeof(glm::vec3)));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_v3fn3fc4f), BUFFER_OFFSET(sizeof(glm::vec3) + sizeof(glm::vec3)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::NORMAL);
			glEnableVertexAttribArray(semantic::attr::COLOR);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
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

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::PER_DRAW]);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		{
			light Light = {glm::vec3(0.0f, 0.0f, 100.f)};

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::PER_PASS]);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(Light), &Light, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		{
			material Material = {glm::vec3(0.7f, 0.0f, 0.0f), 0.0f, glm::vec3(0.0f, 0.5f, 0.0f), 0.0f, glm::vec3(0.0f, 0.0f, 0.5f), 128.0f};

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::PER_SCENE]);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &Material, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

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

		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return Validated;
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::PER_DRAW]);
			transform* Transform = static_cast<transform*>(glMapBufferRange(GL_UNIFORM_BUFFER,
				0, sizeof(transform), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			glm::mat4 const Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 const View = this->view();
			glm::mat4 const Model = glm::rotate(glm::mat4(1.0f), -glm::pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		
			Transform->MV = View * Model;
			Transform->P = Projection;
			Transform->Normal = glm::mat3(glm::transpose(glm::inverse(Transform->MV)));

			glUnmapBuffer(GL_UNIFORM_BUFFER);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)[0]);
		glClearBufferfv(GL_DEPTH, 0, &glm::vec1(1.0f)[0]);

		glUseProgram(ProgramName);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniform::PER_SCENE, BufferName[buffer::PER_SCENE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniform::PER_PASS, BufferName[buffer::PER_PASS]);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniform::PER_DRAW, BufferName[buffer::PER_DRAW]);
		glBindVertexArray(VertexArrayName);

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0);

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
