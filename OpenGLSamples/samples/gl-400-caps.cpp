#include "test.hpp"

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-400-caps", framework::CORE, 4, 0)
	{}

private:

	bool begin()
	{
		caps Caps(caps::CORE);

		bool Validated = true;

		Validated = Validated && Caps.Limits.MAX_PATCH_VERTICES >= 32;
		Validated = Validated && Caps.Limits.MAX_TESS_GEN_LEVEL >= 64;

		Validated = Validated && Caps.Limits.MAX_TEXTURE_BUFFER_SIZE >= 65536;
		Validated = Validated && Caps.Values.MAX_TEXTURE_SIZE >= 16384;
		Validated = Validated && Caps.Values.MAX_3D_TEXTURE_SIZE >= 2048;
		Validated = Validated && Caps.Values.MAX_CUBE_MAP_TEXTURE_SIZE >= 16384;
		Validated = Validated && Caps.Limits.MAX_TEXTURE_IMAGE_UNITS >= 16;

		return Validated;
	}

	bool end()
	{
		return true;
	}

	bool render()
	{
		glm::uvec2 WindowSize = this->getWindowSize();

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

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
