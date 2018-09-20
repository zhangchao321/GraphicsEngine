///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Samples Pack (ogl-samples.g-truc.net)
///
/// Copyright (c) 2004 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////

#include "test.hpp"

namespace
{
	char const * VERT_SHADER_SOURCE_TEXTURE("gl-500/sample-location-render.vert");
	char const * FRAG_SHADER_SOURCE_TEXTURE("gl-500/sample-location-render.frag");
	char const * VERT_SHADER_SOURCE_SPLASH("gl-500/sample-location-splash.vert");
	char const * FRAG_SHADER_SOURCE_SPLASH("gl-500/sample-location-splash.frag");

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
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace texture
	{
		enum type
		{
			COLORBUFFER,
			RENDERBUFFER,
			MAX
		};
	}//namespace texture
	
	namespace framebuffer
	{
		enum type
		{
			COLORBUFFER,
			RENDERBUFFER0,
			RENDERBUFFER1,
			RENDERBUFFER2,
			RENDERBUFFER3,
			MAX
		};
	}//namespace framebuffer

	namespace program
	{
		enum type
		{
			TEXTURE,
			SPLASH,
			MAX
		};
	}//namespace program

	namespace shader
	{
		enum type
		{
			VERT_TEXTURE,
			FRAG_TEXTURE,
			VERT_SPLASH,
			FRAG_SPLASH,
			MAX
		};
	}//namespace shader
}//namespace

class instance : public test
{
public:
	instance(int argc, char* argv[]) :
		test(argc, argv, "gl-500-sample-location-nv", test::CORE, 4, 5),
		FramebufferScale(3),
		UniformTransform(-1),
		VertexCount(0)
	{}

private:
	std::array<GLuint, program::MAX> PipelineName;
	std::array<GLuint, program::MAX> ProgramName;
	std::array<GLuint, program::MAX> VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<GLint, program::MAX> UniformDiffuse;
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	GLsizei VertexCount;
	glm::uint FramebufferScale;
	GLint UniformTransform;

	bool initProgram()
	{
		bool Validated(true);

		compiler Compiler;

		std::array<GLuint, shader::MAX> ShaderName;

		if(Validated)
		{
			ShaderName[shader::VERT_TEXTURE] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");
			ShaderName[shader::FRAG_TEXTURE] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_TEXTURE, "--version 450 --profile core");

			ProgramName[program::TEXTURE] = glCreateProgram();
			glProgramParameteri(ProgramName[program::TEXTURE], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::VERT_TEXTURE]);
			glAttachShader(ProgramName[program::TEXTURE], ShaderName[shader::FRAG_TEXTURE]);
			glLinkProgram(ProgramName[program::TEXTURE]);
		}
		
		if(Validated)
		{
			ShaderName[shader::VERT_SPLASH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_SPLASH, "--version 450 --profile core");
			ShaderName[shader::FRAG_SPLASH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_SPLASH, "--version 450 --profile core");

			ProgramName[program::SPLASH] = glCreateProgram();
			glProgramParameteri(ProgramName[program::SPLASH], GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::VERT_SPLASH]);
			glAttachShader(ProgramName[program::SPLASH], ShaderName[shader::FRAG_SPLASH]);
			glLinkProgram(ProgramName[program::SPLASH]);
		}
	
		if(Validated)
		{
			Validated = Validated && Compiler.check();
			Validated = Validated && Compiler.check_program(ProgramName[program::TEXTURE]);
			Validated = Validated && Compiler.check_program(ProgramName[program::SPLASH]);
		}

		if(Validated)
		{
			glCreateProgramPipelines(program::MAX, &PipelineName[0]);
			glUseProgramStages(PipelineName[program::TEXTURE], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::TEXTURE]);
			glUseProgramStages(PipelineName[program::SPLASH], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::SPLASH]);
		}

		return Validated && this->checkError("initProgram");
	}

	bool initBuffer()
	{
		std::array<glm::vec2, 36> Data;
		for(std::size_t i = 0; i < Data.size(); ++i)
		{
			float Angle = glm::pi<float>() * 2.0f * float(i) / Data.size();
			Data[i] = glm::normalize(glm::vec2(glm::sin(Angle), glm::cos(Angle)));
		}
		this->VertexCount = 18;//static_cast<GLsizei>(Data.size() - 8);

		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::VERTEX], Data.size() * sizeof(glm::vec2), &Data[0], 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT);

		return true;
	}

	bool initTexture()
	{
		bool Validated(true);

		glm::ivec2 WindowSize(this->getWindowSize() >> this->FramebufferScale);

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &TextureName[texture::COLORBUFFER]);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(TextureName[texture::COLORBUFFER], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureStorage3D(TextureName[texture::COLORBUFFER], 1, GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y), 1);

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 1, &TextureName[texture::RENDERBUFFER]);
		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(TextureName[texture::RENDERBUFFER], GL_TEXTURE_MAX_LEVEL, 0);
		glTextureStorage3DMultisample(TextureName[texture::RENDERBUFFER], 8, GL_RGBA8, GLsizei(WindowSize.x), GLsizei(WindowSize.y), 1, GL_TRUE);

		return Validated;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(program::MAX, &VertexArrayName[0]);

		glVertexArrayAttribBinding(VertexArrayName[program::TEXTURE], semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName[program::TEXTURE], semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName[program::TEXTURE], semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD, 0);
		glVertexArrayAttribFormat(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName[program::TEXTURE], semantic::attr::TEXCOORD);

		glVertexArrayVertexBuffer(VertexArrayName[program::TEXTURE], 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));

		return true;
	}

	bool initFramebuffer()
	{
		typedef std::array<glm::vec2, 8> sampleLocations;

		static glm::vec2 SamplesPositions8[][8] =
		{
			{
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f)
			},
			{
				glm::vec2(0.0f / 7.0f),
				glm::vec2(1.0f / 7.0f),
				glm::vec2(2.0f / 7.0f),
				glm::vec2(3.0f / 7.0f),
				glm::vec2(4.0f / 7.0f),
				glm::vec2(5.0f / 7.0f),
				glm::vec2(6.0f / 7.0f),
				glm::vec2(7.0f / 7.0f),
			},
			{
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f),
				glm::vec2(0.00f, 0.00f)
			},
			{
				glm::vec2(0.75f, 0.25f),
				glm::vec2(1.00f, 0.75f),
				glm::vec2(0.25f, 1.00f),
				glm::vec2(0.00f, 0.25f),
				glm::vec2(0.25f, 0.25f),
				glm::vec2(1.00f, 0.25f),
				glm::vec2(0.75f, 1.00f),
				glm::vec2(0.00f, 0.75f)
			},
			{
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f),
				glm::vec2(0.50f, 0.50f)
			},
		};

		sampleLocations SampleLocationsRead;

		glCreateFramebuffers(framebuffer::MAX, &FramebufferName[0]);

		for(int FramebufferIndex = 0; FramebufferIndex < 4; ++FramebufferIndex)
		{
			GLuint currentFramebufferName = FramebufferName[framebuffer::RENDERBUFFER0 + FramebufferIndex];
			glNamedFramebufferTexture(currentFramebufferName, GL_COLOR_ATTACHMENT0, TextureName[texture::RENDERBUFFER], 0);

			GLint Samples = 0;
			glGetNamedFramebufferParameteriv(currentFramebufferName, GL_SAMPLES, &Samples);

			//glGetIntegerv(GL_SAMPLES, &Samples);
/*
			if(FramebufferIndex == 0)
			{
				for(GLint i = 0; i < Samples; ++i)
				{
					glGetMultisamplefv(GL_SAMPLE_POSITION, i, &SamplesPositions8[0][i][0]);
					//glGetMultisamplefv(GL_SAMPLE_POSITION, i, &SampleLocationsRead[i][0]);
				}
			}
*/
			glNamedFramebufferParameteri(currentFramebufferName, GL_FRAMEBUFFER_PROGRAMMABLE_SAMPLE_LOCATIONS_NV, GL_TRUE);
			glNamedFramebufferParameteri(currentFramebufferName, GL_FRAMEBUFFER_SAMPLE_LOCATION_PIXEL_GRID_NV, GL_FALSE);
			glNamedFramebufferSampleLocationsfvNV(currentFramebufferName, 0, Samples, &SamplesPositions8[FramebufferIndex][0][0]);
		}

		//for(std::size_t i = 0; i < SampleLocationsRead.size(); ++i)
		//	SampleLocationsRead[i] *= 16.0f;

		GLint SubPixelBits(0);
		glm::ivec2 PixelGrid(0);
		GLint TableSize(0);

		glGetIntegerv(GL_SAMPLE_LOCATION_SUBPIXEL_BITS_NV, &SubPixelBits);
		glGetIntegerv(GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_NV, &PixelGrid.x);
		glGetIntegerv(GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_NV, &PixelGrid.y);
		glGetIntegerv(GL_PROGRAMMABLE_SAMPLE_LOCATION_TABLE_SIZE_NV, &TableSize);

		glNamedFramebufferTexture(FramebufferName[texture::COLORBUFFER], GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);

		for(int FramebufferIndex = 0; FramebufferIndex < framebuffer::MAX; ++FramebufferIndex)
			if(!this->checkFramebuffer(FramebufferName[FramebufferIndex]))
				return false;
		return true;
	}

	bool begin()
	{
		bool Validated(true);
		Validated = Validated && this->checkExtension("GL_NV_sample_locations");
		Validated = Validated && this->checkExtension("GL_NV_internalformat_sample_query");

		// Obtain supported sample count for a format:
		GLint num_sample_counts = 0;
		//GLenum ifmt = GL_RGBA8;
		GLenum ifmt = GL_RGBA32F;
		GLenum target = GL_TEXTURE_2D_MULTISAMPLE;
		glGetInternalformativ(target, ifmt, GL_NUM_SAMPLE_COUNTS, 1, &num_sample_counts);

		// get the list of supported samples for this format
		std::vector<GLint> samples;
		samples.resize(num_sample_counts);
		glGetInternalformativ(target, ifmt, GL_SAMPLES, num_sample_counts, &samples[0]);

		// loop over the supported formats and get per-sample properties
		for (int i=0; i<num_sample_counts; i++)
		{
			GLint multisample;
			GLint ss_scale_x, ss_scale_y;
			GLint conformant;
			glGetInternalformatSampleivNV(target, ifmt, samples[i], GL_MULTISAMPLES_NV, 1, &multisample);
			glGetInternalformatSampleivNV(target, ifmt, samples[i], GL_SUPERSAMPLE_SCALE_X_NV, 1, &ss_scale_x);
			glGetInternalformatSampleivNV(target, ifmt, samples[i], GL_SUPERSAMPLE_SCALE_Y_NV, 1, &ss_scale_y);
			glGetInternalformatSampleivNV(target, ifmt, samples[i], GL_CONFORMANT_NV, 1, &conformant);
			
			// do something with this information :-)
			continue;
		}

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteFramebuffers(texture::MAX, &FramebufferName[0]);
		glDeleteProgram(ProgramName[program::SPLASH]);
		glDeleteProgram(ProgramName[program::TEXTURE]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);
		glDeleteProgramPipelines(program::MAX, &PipelineName[0]);

		return true;
	}

	bool render()
	{
		glm::vec2 WindowSize(this->getWindowSize());

		{
			glm::mat4* Pointer = static_cast<glm::mat4*>(glMapNamedBufferRange(BufferName[buffer::TRANSFORM], 0, sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

			//glm::mat4 Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, 640.f, 480.f, 0.1f, 100.0f);
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, WindowSize.x / WindowSize.y, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1.0f);
		
			*Pointer = Projection * this->view() * Model;

			// Make sure the uniform buffer is uploaded
			glUnmapNamedBuffer(BufferName[buffer::TRANSFORM]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER0]);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

		glBindProgramPipeline(PipelineName[program::TEXTURE]);
		glBindVertexArray(VertexArrayName[program::TEXTURE]);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

		glm::vec2 ViewportSize(glm::vec2(this->getWindowSize() >> this->FramebufferScale) * 0.5f);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER0]);
		glViewportIndexedf(0, 0, 0, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER1]);
		glViewportIndexedf(0, ViewportSize.x, 0, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER2]);
		glViewportIndexedf(0, ViewportSize.x, ViewportSize.y, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDERBUFFER3]);
		glViewportIndexedf(0, 0, ViewportSize.y, ViewportSize.x, ViewportSize.y);
		glDrawArraysInstanced(GL_LINE_LOOP, 0, this->VertexCount, 1);

		// Blit
		glBlitNamedFramebuffer(FramebufferName[framebuffer::RENDERBUFFER0], FramebufferName[framebuffer::COLORBUFFER],
			0, 0, static_cast<GLsizei>(WindowSize.x) >> this->FramebufferScale, static_cast<GLsizei>(WindowSize.y) >> this->FramebufferScale, 
			0, 0, static_cast<GLsizei>(WindowSize.x) >> this->FramebufferScale, static_cast<GLsizei>(WindowSize.y) >> this->FramebufferScale, 
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glViewportIndexedf(0, 0, 0, WindowSize.x, WindowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(VertexArrayName[program::SPLASH]);
		glBindTextureUnit(0, TextureName[texture::COLORBUFFER]);
		glBindProgramPipeline(PipelineName[program::SPLASH]);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	instance Test(argc, argv);
	Error += Test();

	return Error;
}

