#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE("gl-500/texture-bindless.vert");
	char const* FRAG_SHADER_SOURCE("gl-500/texture-bindless.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
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
			INDIRECT,
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
		framework(argc, argv, "gl-500-texture-bindless-arb", framework::CORE, 4, 5),
		PipelineName(0),
		ProgramName(0),
		VertexArrayName(0),
		TextureName(0),
		TextureHandle(0),
		UniformPointer(nullptr)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint VertexArrayName;
	GLuint TextureName;
	GLuint64 TextureHandle;
	glm::mat4* UniformPointer;

	bool initProgram()
	{
		bool Validated(true);
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 450 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 450 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glGenProgramPipelines(1, &PipelineName);
			glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferName[buffer::VERTEX]);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, VertexSize, VertexData, 0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLsizeiptr UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferStorage(GL_UNIFORM_BUFFER, UniformBlockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		this->UniformPointer = (glm::mat4*)glMapBufferRange(GL_UNIFORM_BUFFER,
			0, sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::MATERIAL]);
		glBufferStorage(GL_UNIFORM_BUFFER, sizeof(TextureHandle), &TextureHandle, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		DrawElementsIndirectCommand Command(ElementCount, 1, 0, 0, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, this->BufferName[buffer::INDIRECT]);
		glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(Command), &Command, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));
		assert(!Texture.empty());
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &TextureName);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexStorage2D(GL_TEXTURE_2D, static_cast<GLint>(Texture.levels()),
			Format.Internal, static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y));

		for(gli::texture2d::size_type Level(0); Level < Texture.levels(); ++Level)
		{
			glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(Level),
				0, 0,
				static_cast<GLsizei>(Texture[Level].extent().x), static_cast<GLsizei>(Texture[Level].extent().y),
				Format.External, Format.Type,
				Texture[Level].data());
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		// Query the texture handle and make the texture resident
		this->TextureHandle = glGetTextureHandleARB(this->TextureName);
		glMakeTextureHandleResidentARB(this->TextureHandle);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Validated;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
	}

	bool begin()
	{
		bool Validated(true);
		Validated = Validated && this->checkExtension("GL_ARB_bindless_texture");

		this->sync(framework::ASYNC);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();

		if(Validated)
		{
			glm::vec2 WindowSize(this->getWindowSize());

			glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

			glBindProgramPipeline(PipelineName);
			glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
			glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::MATERIAL, BufferName[buffer::MATERIAL]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantic::storage::VERTEX, BufferName[buffer::VERTEX]);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, BufferName[buffer::INDIRECT]);
			glBindVertexArray(VertexArrayName);
		}

		return Validated;
	}

	bool end()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		
		glDeleteProgramPipelines(1, &PipelineName);
		glDeleteProgram(ProgramName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(1, &TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, WindowSize.x, WindowSize.y, 0.1f, 100.0f);
			*this->UniformPointer = Projection * this->view() * glm::mat4(1.0f);
		}

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0), 1, sizeof(DrawElementsIndirectCommand));

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

