#include "test.hpp"

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-320-caps", framework::CORE, 3, 2)
	{}

private:

	bool begin()
	{
		caps Caps(caps::CORE);

		bool Validated = true;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS >= 12;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS >= 12;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS >= 12;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS >= 1024;

		Validated = Validated && Caps.Limits.MAX_COMBINED_UNIFORM_BLOCKS >= 36;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BUFFER_BINDINGS >= 36;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BLOCK_SIZE >= 16384;

		std::uint64_t const CombinedVertUniformCount(Caps.Limits.MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedGeomUniformCount(Caps.Limits.MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedFragUniformCount(Caps.Limits.MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);

		std::uint64_t const VertUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS));
		std::uint64_t const GeomUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS));
		std::uint64_t const FragUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS));

		Validated = Validated && CombinedVertUniformCount <= VertUniformCount;
		Validated = Validated && CombinedGeomUniformCount <= GeomUniformCount;
		Validated = Validated && CombinedFragUniformCount <= FragUniformCount;

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
