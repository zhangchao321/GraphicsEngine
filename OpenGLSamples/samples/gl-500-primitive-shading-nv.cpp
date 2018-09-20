#include "test.hpp"

namespace
{
	char const* SAMPLE_VERT_SHADER("gl-500/primitive-shading-nv.vert");
	char const* SAMPLE_GEOM_SHADER("gl-500/primitive-shading-nv.geom");
	char const* SAMPLE_FRAG_SHADER("gl-500/primitive-shading-nv.frag");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fc4ub);
	glf::vertex_v2fc4ub const VertexData[VertexCount] =
	{
		glf::vertex_v2fc4ub(glm::vec2(-1.0f,-1.0f), glm::u8vec4(255,   0,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f,-1.0f), glm::u8vec4(255, 255, 255, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f, 1.0f), glm::u8vec4(  0, 255,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2(-1.0f, 1.0f), glm::u8vec4(  0,   0, 255, 255))
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
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-500-primitive-shading-nv", framework::CORE, 4, 5)
		, QueryName(0)
		, PipelineName(0)
	{}

private:
	GLuint QueryName;
	GLuint PipelineName;

	bool testError()
	{
		compiler Compiler;

		Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER, "--version 450 --profile core --define GEN_ERROR");

		return !Compiler.check();
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERT_SHADER, "--version 450 --profile core");
			GLuint GeomShaderName = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAG_SHADER, "--version 450 --profile core");

			ProgramName = glCreateProgram();
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, GeomShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);

			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glCreateProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(1, &VertexArrayName);
		
		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::COLOR, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::COLOR);

		glVertexArrayElementBuffer(VertexArrayName, BufferName[buffer::ELEMENT]);
		glVertexArrayVertexBuffer(VertexArrayName, 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fc4ub));

		return true;
	}

	bool initBuffer()
	{
		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT);
		glNamedBufferStorage(BufferName[buffer::ELEMENT], ElementSize, ElementData, 0);
		glNamedBufferStorage(BufferName[buffer::VERTEX], VertexSize, VertexData, 0);

		return true;
	}

	bool initQuery()
	{
		glGenQueries(1, &QueryName);

		int QueryBits(0);
		glGetQueryiv(GL_PRIMITIVES_GENERATED, GL_QUERY_COUNTER_BITS, &QueryBits);

		return QueryBits >= 32;
	}

	bool begin()
	{
		bool Validated = true;//testError();

		if(Validated)
			Validated = initQuery();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteProgram(ProgramName);
		glDeleteProgramPipelines(1, &PipelineName);

		return true;
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		{
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapNamedBufferRange(BufferName[buffer::TRANSFORM],
				0, sizeof(glm::mat4) * 1, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapNamedBuffer(BufferName[buffer::TRANSFORM]);
		}
	
		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glBindProgramPipeline(PipelineName);
		glBindVertexArray(VertexArrayName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

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

