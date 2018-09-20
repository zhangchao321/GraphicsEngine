#include "test.hpp"

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-430-caps", framework::CORE, 4, 3)
	{}

private:

	bool begin()
	{
		caps Caps(caps::CORE);

		bool Validated = true;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS >= 14;
		Validated = Validated && Caps.Limits.MAX_TESS_CONTROL_UNIFORM_BLOCKS >= 14;
		Validated = Validated && Caps.Limits.MAX_TESS_EVALUATION_UNIFORM_BLOCKS >= 14;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS >= 14;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS >= 14;
		Validated = Validated && Caps.Limits.MAX_COMPUTE_UNIFORM_BLOCKS >= 14;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_TESS_CONTROL_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_TESS_EVALUATION_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS >= 512;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_COMPUTE_UNIFORM_COMPONENTS >= 512;

		Validated = Validated && Caps.Limits.MAX_COMBINED_UNIFORM_BLOCKS >= 70;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BUFFER_BINDINGS >= 84;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BLOCK_SIZE >= 16384;
		Validated = Validated && Caps.Limits.MAX_SHADER_STORAGE_BLOCK_SIZE >= (1 << 24);

		std::uint64_t const CombinedVertUniformCount(Caps.Limits.MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedContUniformCount(Caps.Limits.MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedEvalUniformCount(Caps.Limits.MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedGeomUniformCount(Caps.Limits.MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedFragUniformCount(Caps.Limits.MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
		std::uint64_t const CombinedCompUniformCount(Caps.Limits.MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS);

		std::uint64_t const VertUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS));
		std::uint64_t const ContUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_TESS_CONTROL_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_TESS_CONTROL_UNIFORM_COMPONENTS));
		std::uint64_t const EvalUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_TESS_EVALUATION_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_TESS_EVALUATION_UNIFORM_COMPONENTS));
		std::uint64_t const GeomUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS));
		std::uint64_t const FragUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS));
		std::uint64_t const CompUniformCount((static_cast<std::uint64_t>(Caps.Limits.MAX_COMPUTE_UNIFORM_BLOCKS) * static_cast<std::uint64_t>(Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) / 4) + static_cast<std::uint64_t>(Caps.Limits.MAX_COMPUTE_UNIFORM_COMPONENTS));

		Validated = Validated && CombinedVertUniformCount <= VertUniformCount;
		Validated = Validated && CombinedContUniformCount <= ContUniformCount;
		Validated = Validated && CombinedEvalUniformCount <= EvalUniformCount;
		Validated = Validated && CombinedGeomUniformCount <= GeomUniformCount;
		Validated = Validated && CombinedFragUniformCount <= FragUniformCount;
		Validated = Validated && CombinedCompUniformCount <= CompUniformCount;
		
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
