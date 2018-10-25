#include "framework.hpp"

namespace ES200
{
	class ES200DrawElementsSample :public FrameWork
	{
	public:
		ES200DrawElementsSample(int argc, char *argv[])
			:FrameWork(argc, argv, "ES200DrawElements", FrameWork::ES, 2, 0)
		{

		}

	protected:
		bool begin() override; 
		bool end() override;
		bool render() override;
	private:
		void initProgram();
		void initBuffer();
	private:
		GLuint vao;
		GLuint programName;
		GLuint uniformMVP;
		GLuint elementCount;
		GLuint uniformDiffuse;
		GLuint buffers[2];
	};

	bool ES200DrawElementsSample::begin()
	{
		bool Validated(true);

		char const* Vendor = (char const *)glGetString(GL_VENDOR);
		printf(Vendor);
		char const* Renderer = (char const *)glGetString(GL_RENDERER);
		printf(Renderer);
		char const* Version = (char const *)glGetString(GL_VERSION);
		printf(Version);
		char const* Extensions = (char const *)glGetString(GL_EXTENSIONS);
		printf(Extensions);

		initProgram();
		initBuffer();

		return Validated;
	}

	bool ES200DrawElementsSample::end()
	{
		glDeleteVertexArrays(1,&vao);
		glDeleteBuffers(2, buffers);
		return true;
	}

	bool ES200DrawElementsSample::render()
	{ 
		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		glm::uvec2 WindowSize = this->getWindowSize();
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		// Clear color buffer with black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepthf(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(programName);
		glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glEnableVertexAttribArray(semantic::attr::POSITION);
		glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(semantic::attr::POSITION);

		glBindVertexArray(0);
		glUseProgram(0);
		return true;
	}

	void ES200DrawElementsSample::initProgram()
	{
		bool Validated = true;

		// Create program
		if (Validated)
		{
			Compiler compiler;
			GLuint VertShaderName = compiler.create(GL_VERTEX_SHADER, getDataDirectory() + "ES200/flat-color.vert");
			GLuint FragShaderName = compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + "ES200/flat-color.frag");

			programName = glCreateProgram();
			glAttachShader(programName, VertShaderName);
			glAttachShader(programName, FragShaderName);

			glBindAttribLocation(programName, semantic::attr::POSITION, "Position");
			glLinkProgram(programName);

			Validated = Validated && compiler.check();
			Validated = Validated && compiler.check_program(programName);
		}

		// Get variables locations
		if (Validated)
		{
			uniformMVP = glGetUniformLocation(programName, "MVP");
			uniformDiffuse = glGetUniformLocation(programName, "Diffuse");
		}

		// Set some variables 
		if (Validated)
		{
			// Bind the program for use
			glUseProgram(programName);

			// Set uniform value
			glUniform4fv(uniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

			// Unbind the program
			glUseProgram(0);
		} 
	}

	void ES200DrawElementsSample::initBuffer()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glm::uint16 elementData[] = {
			0,1,2,
			0,2,3
		};
		elementCount = sizeof(elementData) / sizeof(glm::uint16);

		glm::float32 postionData[] =
		{
			-1.0,-1.0,
			1.0,-1.0,
			1.0,1.0,
			-1.0,1.0
		};
		
		glGenBuffers(2, &buffers[0]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::float32)*8, postionData, GL_STATIC_DRAW);
		glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint16)*6,elementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

int main(int argc, char * argv[])
{
	int error = 0;
	ES200::ES200DrawElementsSample sample(argc, argv);
	error = sample();
	return error;
}
