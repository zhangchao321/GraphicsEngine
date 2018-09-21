#include "buffer.hpp"
#include <memory>
#include <cassert>

namespace
{
	std::size_t const CHUNK_SIZE(65536);
	std::size_t const SLICE_SIZE(67108864);
	std::size_t const PAGE_SIZE(16384);

	GLenum make_usage(std::uint32_t Flags)
	{
		return 0;
	}

	class manager
	{
	public:

	private:

	};

	static std::unique_ptr<manager> Manager;


	bool const hasImmutableStorage = true;
}//namespace

namespace gl
{
	map_ptr::map_ptr(Name_t Name, std::size_t Offset, std::size_t Length, std::uint32_t Access) :
		Name(Name),
		Offset(Offset),
		Length(Length),
		Access(Access)
	{
		glMapNamedBufferRangeEXT(Name, Offset, Length, Access);
	}

	map_ptr::~map_ptr()
	{
		this->unmap();
	}

	void map_ptr::flush(std::size_t Offset, std::size_t Length) const
	{
		glFlushMappedNamedBufferRangeEXT(this->Name, Offset, Length);
	}

	void map_ptr::unmap() const
	{
		glUnmapNamedBufferEXT(this->Name);
	}

	Buffer::Buffer(std::size_t Size, const void* Data, std::uint32_t Flags)
	{
		glGenBuffers(1, &this->Name);
		glNamedBufferStorageEXT(this->Name, static_cast<GLsizeiptr>(Size), Data, Flags);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &this->Name);
		this->Name = 0;	
	}

	map_ptr const & Buffer::map(GLintptr Offset, GLsizeiptr Length, GLbitfield Flags)
	{
		return map_ptr(this->Name, Offset, Length, Flags);
	} 
}//namespace gl

