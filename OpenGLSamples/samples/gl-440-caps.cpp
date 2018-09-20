#include "test.hpp"

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "gl-440-caps", framework::CORE, 4, 4)
	{}

private:
	bool checkCaps()
	{
		caps Caps(caps::CORE);

		GLint NumCompressedTextureFormats = 0;
		glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &NumCompressedTextureFormats);
		std::vector<GLint> CompressedTextureFormats(NumCompressedTextureFormats);
		glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, &CompressedTextureFormats[0]);

		if(this->checkExtension("GL_EXT_texture_compression_s3tc"))
		{
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGB_S3TC_DXT1_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) == CompressedTextureFormats.end())
				return false;
		}

		if(this->checkExtension("GL_EXT_texture_sRGB") && this->checkExtension("GL_EXT_texture_compression_s3tc"))
		{
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB_S3TC_DXT1_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT) == CompressedTextureFormats.end())
				return false;
			if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT) == CompressedTextureFormats.end())
				return false;
		}

		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RED_RGTC1) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SIGNED_RED_RGTC1) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RG_RGTC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SIGNED_RG_RGTC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGBA_BPTC_UNORM) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGB8_ETC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB8_ETC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RGBA8_ETC2_EAC) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_R11_EAC) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SIGNED_R11_EAC) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_RG11_EAC) == CompressedTextureFormats.end())
			return false;
		if(std::find(CompressedTextureFormats.begin(), CompressedTextureFormats.end(), GL_COMPRESSED_SIGNED_RG11_EAC) == CompressedTextureFormats.end())
			return false;

		return true;
	}

	bool begin()
	{
		return checkCaps();
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
