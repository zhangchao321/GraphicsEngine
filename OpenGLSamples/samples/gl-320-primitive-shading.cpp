#include "test.hpp"

namespace
{
	char const* SAMPLE_VERT_SHADER("gl-320/primitive-shading.vert");
	char const* SAMPLE_GEOM_SHADER("gl-320/primitive-shading.geom");
	char const* SAMPLE_FRAG_SHADER("gl-320/primitive-shading.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fc4ub);
	glf::vertex_v2fc4ub const VertexData[VertexCount] =
	{
		glf::vertex_v2fc4ub(glm::vec2(-1.0f,-1.0f), glm::u8vec4(255,   0,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f,-1.0f), glm::u8vec4(255, 255, 255, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f, 1.0f), glm::u8vec4(  0, 255,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2(-1.0f, 1.0f), glm::u8vec4(  0,   0, 255, 255))
	};

	GLsizei const ColorCount(3);
	GLsizeiptr const ColorSize = ColorCount * sizeof(glm::vec4);
	glm::vec4 const ColorData[ColorCount] =
	{
		glm::vec4(0.5, 0.5, 0.5, 1.0),
		glm::vec4(0.7, 0.7, 0.7, 1.0),
		glm::vec4(0.3, 0.3, 0.3, 1.0)
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
			CONSTANT,
			MAX
		};
	}//namespace buffer

	GLuint ProgramName(0);
	GLuint VertexArrayName(0);
	std::vector<GLuint> BufferName(buffer::MAX);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-primitive-shading", framework::CORE, 3, 2),
		QueryName(0)
	{}

private:
	GLuint QueryName;

	bool testError()
	{
		compiler Compiler;

		Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER, "--version 150 --profile core --define GEN_ERROR");

		return !Compiler.check();
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERT_SHADER, "--version 150 --profile core");
			GLuint GeomShaderName = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER, "--version 150 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAG_SHADER, "--version 150 --profile core");

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, GeomShaderName);
			glAttachShader(ProgramName, FragShaderName);

			glBindAttribLocation(ProgramName, semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName, semantic::attr::COLOR, "Color");
			glBindFragDataLocation(ProgramName, semantic::frag::COLOR, "Color");
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glUseProgram(ProgramName);
			glUniformBlockBinding(ProgramName, glGetUniformBlockIndex(ProgramName, "transform"), semantic::uniform::TRANSFORM0);
			glUniformBlockBinding(ProgramName, glGetUniformBlockIndex(ProgramName, "constant"), semantic::uniform::CONSTANT);
			glUseProgram(0);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fc4ub), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glf::vertex_v2fc4ub), BUFFER_OFFSET(sizeof(glm::vec2)));
			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::CONSTANT]);
		glBufferData(GL_UNIFORM_BUFFER, ColorSize, ColorData, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return this->checkError("initBuffer");
	}

	bool initQuery()
	{
		glGenQueries(1, &QueryName);

		int QueryBits(0);
		glGetQueryiv(GL_PRIMITIVES_GENERATED, GL_QUERY_COUNTER_BITS, &QueryBits);

		bool Validated = QueryBits >= 32;

		return Validated && this->checkError("initQuery");
	}

	bool begin()
	{
		bool Validated = testError();

		if(Validated)
			Validated = initQuery();
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
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteProgram(ProgramName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		{
			glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
			glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 1,
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}
	
		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glUseProgram(ProgramName);

		glBindVertexArray(VertexArrayName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::CONSTANT, BufferName[buffer::CONSTANT]);

		glBeginQuery(GL_PRIMITIVES_GENERATED, QueryName); 
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0);
		glEndQuery(GL_PRIMITIVES_GENERATED); 

		GLuint64 PrimitivesGenerated = 0;
		glGetQueryObjectui64v(this->QueryName, GL_QUERY_RESULT, &PrimitivesGenerated);

		return PrimitivesGenerated > 0;
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	sample Sample(argc, argv);
	Error += Sample();

	return Error;
}

