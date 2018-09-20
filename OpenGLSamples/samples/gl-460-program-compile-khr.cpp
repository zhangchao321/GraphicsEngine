#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-460/program-compile-khr.vert");
	char const* FRAG_SHADER_SOURCE("gl-460/program-compile-khr.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
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
			MATERIAL,
			MAX
		};
	}//namespace buffer

	namespace program
	{
		enum type
		{
			USED,
			MAX
		};
	}//namespace buffer
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-460-program-compile-khr", framework::CORE, 4, 5),
		VertexArrayName(0),
		UniformTransform(0),
		UniformMaterial(0)
	{}

private:
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint VertexArrayName;
	GLint UniformTransform;
	GLint UniformMaterial;

	bool initProgram()
	{
		bool Validated = true;

		glMaxShaderCompilerThreadsARB(1);

		GLint MaxShaderCompilerThreads = 0;
		glGetIntegerv(GL_MAX_SHADER_COMPILER_THREADS_ARB, &MaxShaderCompilerThreads);
		if (MaxShaderCompilerThreads != 1)
			return false;

		compiler Compiler;
	
		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 450 --profile core");

			ProgramName[program::USED] = glCreateProgram();
			glAttachShader(ProgramName[program::USED], VertShaderName);
			glAttachShader(ProgramName[program::USED], FragShaderName);

			glBindAttribLocation(ProgramName[program::USED], semantic::attr::POSITION, "Position");
			glBindFragDataLocation(ProgramName[program::USED], semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName[program::USED]);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName[program::USED]);
		}

		if(Validated)
		{
			UniformMaterial = glGetUniformBlockIndex(ProgramName[program::USED], "material");
			UniformTransform = glGetUniformBlockIndex(ProgramName[program::USED], "transform");
		}

		GLint ActiveUniformBlocks(0);
		glGetProgramiv(ProgramName[program::USED], GL_ACTIVE_UNIFORM_BLOCKS, &ActiveUniformBlocks);

		for(GLint i = 0; i < ActiveUniformBlocks; ++i)
		{
			char Name[128];
			memset(Name, '\0', sizeof(Name));
			GLsizei Length(0);

			glGetActiveUniformBlockName(ProgramName[program::USED], i, GLsizei(sizeof(Name)), &Length, Name);

			std::string StringName(Name);

			Validated = Validated && (StringName == std::string("material") || StringName == std::string("transform"));
		}

		GLint ActiveUniform(0);
		glGetProgramiv(ProgramName[program::USED], GL_ACTIVE_UNIFORMS, &ActiveUniform);

		for(GLint i = 0; i < ActiveUniformBlocks; ++i)
		{
			char Name[128];
			memset(Name, '\0', sizeof(Name));
			GLsizei Length(0);

			glGetActiveUniformName(ProgramName[program::USED], i, GLsizei(sizeof(Name)), &Length, Name);

			std::string StringName(Name);

			Validated = Validated && (
				StringName == std::string("material.Diffuse") || 
				StringName == std::string("transform.MVP"));
		}
	
		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		GLint UniformBlockSize = 0;

		{
			glGetActiveUniformBlockiv(ProgramName[program::USED], UniformTransform, GL_UNIFORM_BLOCK_DATA_SIZE, &UniformBlockSize);

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		{
			glm::vec4 Diffuse(1.0f, 0.5f, 0.0f, 1.0f);

			glGetActiveUniformBlockiv(ProgramName[program::USED], UniformMaterial, GL_UNIFORM_BLOCK_DATA_SIZE, &UniformBlockSize);

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::MATERIAL]);
			glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, &Diffuse[0], GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		return this->checkError("initBuffer");
	}

	bool begin()
	{
		bool Validated = this->checkExtension("GL_ARB_parallel_shader_compile");

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
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName[program::USED]);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0,	sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * this->view() * Model;
		
			*Pointer = MVP;
		
			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glUseProgram(ProgramName[program::USED]);
		glUniformBlockBinding(ProgramName[program::USED], UniformTransform, semantic::uniform::TRANSFORM0);
		glUniformBlockBinding(ProgramName[program::USED], UniformMaterial, semantic::uniform::MATERIAL);

		// Attach the buffer to UBO binding point semantic::uniform::TRANSFORM0
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		// Attach the buffer to UBO binding point semantic::uniform::MATERIAL
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::MATERIAL, BufferName[buffer::MATERIAL]);

		glBindVertexArray(VertexArrayName);
		glDrawElementsInstanced(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 1);

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
