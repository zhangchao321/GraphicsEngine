#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include "framework.hpp"

namespace gl
{
	typedef std::uint32_t Name_t;

	class Noncopyable
	{
	protected:
		Noncopyable() = default;
		~Noncopyable() = default;

		Noncopyable(Noncopyable const &) = delete;
		Noncopyable& operator=(Noncopyable const &) = delete;
	};

	class Buffer;

	class map_ptr : Noncopyable
	{
		friend class Buffer;

	private:
		map_ptr(Name_t Name, std::size_t Offset, std::size_t Length, std::uint32_t Access);
		map_ptr(map_ptr const &);
		map_ptr& operator=(map_ptr const &);

	public:
		~map_ptr();

		void flush(std::size_t Offset, std::size_t Length) const;
		void unmap() const;

	private:
		Name_t const Name;
		std::size_t const Offset;
		std::size_t const Length;
		std::uint32_t const Access;
	};

	class Buffer : Noncopyable
	{
	public:
		Buffer(std::size_t Size, const void* Data, std::uint32_t Flags);
		~Buffer();

		map_ptr const & map(GLintptr Offset, GLsizeiptr Length, GLbitfield Flags);

	private:
		Name_t Name;
	};
	 
}//namespace gl
