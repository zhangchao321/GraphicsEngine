#include "test.hpp"

namespace
{
	std::string const SAMPLE_VERT_SHADER1("gl-400/tess.vert");
	std::string const SAMPLE_CONT_SHADER1("gl-400/tess.cont");
	std::string const SAMPLE_EVAL_SHADER1("gl-400/tess.eval");
	std::string const SAMPLE_GEOM_SHADER1("gl-400/tess.geom");
	std::string const SAMPLE_FRAG_SHADER1("gl-400/tess.frag");
	std::string const SAMPLE_VERT_SHADER2("gl-400/smooth-shading-geom.vert");
	std::string const SAMPLE_GEOM_SHADER2("gl-400/smooth-shading-geom.geom");
	std::string const SAMPLE_FRAG_SHADER2("gl-400/smooth-shading-geom.frag");

	GLsizei const VertexCount = 4;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fc4ub);
	glf::vertex_v2fc4ub const VertexData[VertexCount] =
	{
		glf::vertex_v2fc4ub(glm::vec2(-1.0f,-1.0f), glm::u8vec4(255,   0,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f,-1.0f), glm::u8vec4(255, 255, 255, 255)),
		glf::vertex_v2fc4ub(glm::vec2( 1.0f, 1.0f), glm::u8vec4(  0, 255,   0, 255)),
		glf::vertex_v2fc4ub(glm::vec2(-1.0f, 1.0f), glm::u8vec4(  0,   0, 255, 255))
		//glf::vertex_v2fc4ub(glm::vec2(-1.0f,-1.0f), glm::u8vec4(255, 148,  17, 255)),
		//glf::vertex_v2fc4ub(glm::vec2( 1.0f,-1.0f), glm::u8vec4( 17, 255,  58, 255)),
		//glf::vertex_v2fc4ub(glm::vec2( 1.0f, 1.0f), glm::u8vec4( 17, 219, 255, 255)),
		//glf::vertex_v2fc4ub(glm::vec2(-1.0f, 1.0f), glm::u8vec4(255,  17, 232, 255))
		//glf::vertex_v2fc4ub(glm::vec2(-1.0f,-1.0f), glm::u8vec4(242, 136,   0, 255)),
		//glf::vertex_v2fc4ub(glm::vec2( 1.0f,-1.0f), glm::u8vec4(  0, 202,  36, 255)),
		//glf::vertex_v2fc4ub(glm::vec2( 1.0f, 1.0f), glm::u8vec4(  0, 147, 175, 255)),
		//glf::vertex_v2fc4ub(glm::vec2(-1.0f, 1.0f), glm::u8vec4(185,   0, 166, 255))
	};

	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	GLuint ProgramName[2] = {0, 0};
	GLuint ElementBufferName = 0;
	GLuint ArrayBufferName = 0;
	GLuint VertexArrayName = 0;
	GLint UniformMVP[2] = {0, 0};
	GLint UniformDiffuse[2] = {0, 0};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-primitive-smooth-shading", framework::CORE, 4, 0)
	{}

private:
	bool initProgram()
	{
		compiler Compiler;

		bool Validated = true;
		if(Validated)
		{
			GLuint VertShader = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERT_SHADER1);
			GLuint ContShader = Compiler.create(GL_TESS_CONTROL_SHADER, getDataDirectory() + SAMPLE_CONT_SHADER1);
			GLuint EvalShader = Compiler.create(GL_TESS_EVALUATION_SHADER, getDataDirectory() + SAMPLE_EVAL_SHADER1);
			GLuint GeomShader = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER1);
			GLuint FragShader = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAG_SHADER1);

			ProgramName[0] = glCreateProgram();
			glAttachShader(ProgramName[0], VertShader);
			glAttachShader(ProgramName[0], GeomShader);
			glAttachShader(ProgramName[0], ContShader);
			glAttachShader(ProgramName[0], EvalShader);
			glAttachShader(ProgramName[0], FragShader);
			glLinkProgram(ProgramName[0]);
		}

		if(Validated)
		{
			GLuint VertShader = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + SAMPLE_VERT_SHADER2);
			GLuint GeomShader = Compiler.create(GL_GEOMETRY_SHADER, getDataDirectory() + SAMPLE_GEOM_SHADER2);
			GLuint FragShader = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + SAMPLE_FRAG_SHADER2);

			ProgramName[1] = glCreateProgram();
			glAttachShader(ProgramName[1], VertShader);
			glAttachShader(ProgramName[1], GeomShader);
			glAttachShader(ProgramName[1], FragShader);
			glLinkProgram(ProgramName[1]);
		}

		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName[0]);
			Validated = Validated && Compiler.check_program(ProgramName[1]);
		}

		if(Validated)
		{
			UniformMVP[0] = glGetUniformLocation(ProgramName[0], "MVP");
			UniformMVP[1] = glGetUniformLocation(ProgramName[1], "MVP");
		}

		return Validated && this->checkError("initProgram");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fc4ub), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glf::vertex_v2fc4ub), BUFFER_OFFSET(sizeof(glm::vec2)));
			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initBuffer()
	{
		glGenBuffers(1, &ElementBufferName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ArrayBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initBuffer");
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

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteVertexArrays(1, &VertexArrayName);
		glDeleteBuffers(1, &ArrayBufferName);
		glDeleteBuffers(1, &ElementBufferName);
		glDeleteProgram(ProgramName[0]);
		glDeleteProgram(ProgramName[1]);

		return this->checkError("end");
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x * 0.5f / WindowSize.y, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;
	
		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x * 0.5f), static_cast<GLsizei>(WindowSize.y));
		glUseProgram(ProgramName[0]);
		glUniformMatrix4fv(UniformMVP[0], 1, GL_FALSE, &MVP[0][0]);

		glPatchParameteri(GL_PATCH_VERTICES, VertexCount);
		glDrawArraysInstanced(GL_PATCHES, 0, VertexCount, 1);
	
		glViewport(static_cast<GLint>(WindowSize.x * 0.5f), 0, static_cast<GLsizei>(WindowSize.x * 0.5f), static_cast<GLsizei>(WindowSize.y));
		glUseProgram(ProgramName[1]);
		glUniformMatrix4fv(UniformMVP[1], 1, GL_FALSE, &MVP[0][0]);

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 1, 0);

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

