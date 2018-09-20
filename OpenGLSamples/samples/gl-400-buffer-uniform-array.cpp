#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-400/buffer-uniform-array.vert");
	char const* FRAG_SHADER_SOURCE("gl-400/buffer-uniform-array.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f) * 0.8f,
		glm::vec2( 1.0f,-1.0f) * 0.8f,
		glm::vec2( 1.0f, 1.0f) * 0.8f,
		glm::vec2(-1.0f, 1.0f) * 0.8f
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
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-buffer-uniform-array", framework::CORE, 4, 0),
		VertexArrayName(0),
		ProgramName(0)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLint UniformBufferAlignment;
	GLint UniformInstance;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 400 --profile core");

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			this->UniformInstance = glGetUniformLocation(ProgramName, "Instance");

			GLint UniformMaterial = glGetUniformBlockIndex(ProgramName, "material");
			GLint UniformTransform0 = glGetUniformBlockIndex(ProgramName, "transform[0]");
			GLint UniformTransform1 = glGetUniformBlockIndex(ProgramName, "transform[1]");

			glUniformBlockBinding(ProgramName, UniformMaterial, semantic::uniform::MATERIAL);
			glUniformBlockBinding(ProgramName, UniformTransform0, semantic::uniform::TRANSFORM0);
			glUniformBlockBinding(ProgramName, UniformTransform1, semantic::uniform::TRANSFORM1);
		}

		return Validated;
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

		return true;
	}

	bool initBuffer()
	{
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &this->UniformBufferAlignment);

		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::size_t UniformBufferSize = std::max<std::size_t>(this->UniformBufferAlignment, sizeof(glm::mat4)) * 2;

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glBufferData(GL_UNIFORM_BUFFER, UniformBufferSize, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		{
			glm::vec4 const Diffuse[2] =
			{
				glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
				glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)
			};

			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::MATERIAL]);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(Diffuse), &Diffuse[0][0], GL_STATIC_DRAW);
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

		std::size_t UniformBufferOffset = std::max<std::size_t>(this->UniformBufferAlignment, sizeof(glm::mat4));
		std::size_t UniformBufferRange = UniformBufferOffset * 2;

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::byte* Pointer = (glm::byte*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, UniformBufferRange, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glm::mat4 const Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 const Model0 = glm::translate(glm::mat4(1.0f), glm::vec3( 1, 0, 0));
			glm::mat4 const Model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0));
		
			*reinterpret_cast<glm::mat4*>(Pointer + UniformBufferOffset * 0) = Projection * this->view() * Model0;
			*reinterpret_cast<glm::mat4*>(Pointer + UniformBufferOffset * 1) = Projection * this->view() * Model1;
		
			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

		glUseProgram(ProgramName);

		// Attach the buffer to UBO binding point semantic::uniform::MATERIAL
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::MATERIAL, BufferName[buffer::MATERIAL]);
		glBindVertexArray(VertexArrayName);

		// Attach the buffer to UBO binding point semantic::uniform::TRANSFORM0
		glBindBufferRange(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM], 0, sizeof(glm::mat4));
		glBindBufferRange(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM1, BufferName[buffer::TRANSFORM], UniformBufferOffset, sizeof(glm::mat4));

		glUniform1i(this->UniformInstance, 0);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0);

		glUniform1i(this->UniformInstance, 1);
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
