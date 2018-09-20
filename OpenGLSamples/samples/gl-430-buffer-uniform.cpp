#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-430/buffer-uniform.vert");
	char const* FRAG_SHADER_SOURCE("gl-430/buffer-uniform.frag");

	struct vertex_v3fn3fc4f
	{
		vertex_v3fn3fc4f
		(
			glm::vec3 const & Position,
			glm::vec3 const & Texcoord,
			glm::vec4 const & Color
		) :
			Position(Position),
			Texcoord(Texcoord),
			Color(Color)
		{}

		glm::vec3 Position;
		glm::vec3 Texcoord;
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
		framework(argc, argv, "gl-430-buffer-uniform", framework::CORE, 4, 3),
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

	const char* GetTypeString(GLenum type)
	{
		switch(type)
		{
		case GL_INT:
			return "int";
		case GL_INT_VEC2:
			return "ivec2";
		case GL_INT_VEC3:
			return "ivec3";
		case GL_INT_VEC4:
			return "ivec4";

		case GL_UNSIGNED_INT:
			return "uint";
		case GL_UNSIGNED_INT_VEC2:
			return "uvec2";
		case GL_UNSIGNED_INT_VEC3:
			return "uvec3";
		case GL_UNSIGNED_INT_VEC4:
			return "uvec4";

		case GL_BOOL:
			return "bool";
		case GL_BOOL_VEC2:
			return "bvec2";
		case GL_BOOL_VEC3:
			return "bvec3";
		case GL_BOOL_VEC4:
			return "bvec4";

		case GL_FLOAT:
			return "float";
		case GL_FLOAT_VEC2:
			return "vec2";
		case GL_FLOAT_VEC3:
			return "vec3";
		case GL_FLOAT_VEC4:
			return "vec4";
		case GL_FLOAT_MAT2:
			return "mat2";
		case GL_FLOAT_MAT3:
			return "mat3";
		case GL_FLOAT_MAT4:
			return "mat4";
		case GL_FLOAT_MAT2x3:
			return "mat2x3";
		case GL_FLOAT_MAT2x4:
			return "mat2x4";
		case GL_FLOAT_MAT3x2:
			return "mat3x2";
		case GL_FLOAT_MAT3x4:
			return "mat3x4";
		case GL_FLOAT_MAT4x2:
			return "mat4x2";
		case GL_FLOAT_MAT4x3:
			return "mat4x3";

		case GL_DOUBLE:
			return "double";
		case GL_DOUBLE_VEC2:
			return "dvec2";
		case GL_DOUBLE_VEC3:
			return "dvec3";
		case GL_DOUBLE_VEC4:
			return "dvec4";
		case GL_DOUBLE_MAT2:
			return "dmat2";
		case GL_DOUBLE_MAT3:
			return "dmat3";
		case GL_DOUBLE_MAT4:
			return "dmat4";
		case GL_DOUBLE_MAT2x3:
			return "dmat2x3";
		case GL_DOUBLE_MAT2x4:
			return "dmat2x4";
		case GL_DOUBLE_MAT3x2:
			return "dmat3x2";
		case GL_DOUBLE_MAT3x4:
			return "dmat3x4";
		case GL_DOUBLE_MAT4x2:
			return "dmat4x2";
		case GL_DOUBLE_MAT4x3:
			return "dmat4x3";

		default:
			return "unknown";
		}
	}

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 430 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 430 --profile core");

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
			glUniformBlockBinding(ProgramName, this->UniformPerPass, uniform::PER_SCENE);
		}

		{
			GLint MaxNameLength = 0;
			glGetProgramInterfaceiv(ProgramName, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &MaxNameLength);
			std::vector<char> Name(MaxNameLength + 1, ' ');

			GLint ActiveResources = 0;
			glGetProgramInterfaceiv(ProgramName, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &ActiveResources);

			struct property
			{
				GLint Binding;
				GLsizei DataSize;
				GLsizei NumActiveVariables;
				GLint VertexShader;
				GLint ControlShader;
				GLint Evaluationhader;
				GLint GeometryShader;
				GLint FragmentShader;
				GLint ComputeShader;
			};

			for(GLint ResourceIndex = 0; ResourceIndex < ActiveResources; ++ResourceIndex)
			{
				glGetProgramResourceName(ProgramName, GL_UNIFORM_BLOCK, ResourceIndex, static_cast<GLsizei>(Name.size()), NULL, &Name[0]);

				GLenum const Properties[] =
				{
					GL_BUFFER_BINDING,
					GL_BUFFER_DATA_SIZE,
					GL_NUM_ACTIVE_VARIABLES,
					GL_REFERENCED_BY_VERTEX_SHADER,
					GL_REFERENCED_BY_TESS_CONTROL_SHADER,
					GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
					GL_REFERENCED_BY_GEOMETRY_SHADER,
					GL_REFERENCED_BY_FRAGMENT_SHADER,
					GL_REFERENCED_BY_COMPUTE_SHADER
				};

				property Params;
				GLsizei Length[sizeof(Properties) / sizeof(Properties[0])];

				glGetProgramResourceiv(ProgramName, GL_UNIFORM_BLOCK, ResourceIndex, sizeof(Properties) / sizeof(Properties[0]), Properties, sizeof(Params), Length, &Params.Binding);

				printf(
					"Uniform Block(binding %d, size %d, num var %d, name %s)\n",
					Params.Binding, Params.DataSize, Params.NumActiveVariables, &Name[0]);
			}

			GLint MaxNumActiveVariables = 0;
			glGetProgramInterfaceiv(ProgramName, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &MaxNumActiveVariables);
		}

		{
			GLint MaxNameLength = 0;
			glGetProgramInterfaceiv(ProgramName, GL_UNIFORM, GL_MAX_NAME_LENGTH, &MaxNameLength);
			std::vector<char> Name(MaxNameLength + 1, ' ');

			GLint ActiveResources = 0;
			glGetProgramInterfaceiv(ProgramName, GL_UNIFORM, GL_ACTIVE_RESOURCES, &ActiveResources);

			struct property
			{
				GLint Location;
				GLint BlockIndex;
				GLint Offset;
				GLint ArraySize;
				GLint ArrayStride;
				GLint Type;
				GLint MatrixStride;
				GLint IsRowMajor;
				GLint VertexShader;
				GLint ControlShader;
				GLint Evaluationhader;
				GLint GeometryShader;
				GLint FragmentShader;
				GLint ComputeShader;
			};

			for(GLint ResourceIndex = 0; ResourceIndex < ActiveResources; ++ResourceIndex)
			{
				glGetProgramResourceName(ProgramName, GL_UNIFORM, ResourceIndex, static_cast<GLsizei>(Name.size()), NULL, &Name[0]);

				GLenum const Properties[] =
				{
					GL_LOCATION,
					GL_BLOCK_INDEX,
					GL_OFFSET,
					GL_ARRAY_SIZE,
					GL_ARRAY_STRIDE,
					GL_TYPE,
					GL_MATRIX_STRIDE,
					GL_IS_ROW_MAJOR,
					GL_REFERENCED_BY_VERTEX_SHADER,
					GL_REFERENCED_BY_TESS_CONTROL_SHADER,
					GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
					GL_REFERENCED_BY_GEOMETRY_SHADER,
					GL_REFERENCED_BY_FRAGMENT_SHADER,
					GL_REFERENCED_BY_COMPUTE_SHADER
				};

				property Params;
				GLsizei Length[sizeof(Properties) / sizeof(Properties[0])];

				glGetProgramResourceiv(ProgramName, GL_UNIFORM, ResourceIndex, sizeof(Properties) / sizeof(Properties[0]), Properties, sizeof(Params), Length, &Params.Location);

				printf("Uniform %s (%s), location: %d, block: %d, offset: %d, array size %d, array stride %d\n", &Name[0], GetTypeString(Params.Type), Params.Location, Params.BlockIndex, Params.Offset, Params.ArraySize, Params.ArrayStride);
			}
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
