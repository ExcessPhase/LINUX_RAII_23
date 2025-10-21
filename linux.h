#pragma once
#include <sys/mman.h>
#include <fcntl.h>
#include <expected>
#include <format>
#include <unistd.h>
namespace foelsche::linux_ns
{
	/// a RAII wrapper for a FD
struct close
{	int m_i;
	close(void) = delete;
	close(const int _i)
		:m_i(_i)
	{
	}
	close(const close&) = delete;
	close(close&&_r)
		:m_i(_r.m_i)
	{	_r.m_i = -1;
	}
	close &operator=(const close&) = delete;
	close &operator=(close&&_r)
	{	if (&_r == this)
			return *this;
		if (m_i >= 0)
			::close(m_i);
		m_i = _r.m_i;
		_r.m_i = -1;
		return *this;
	}
	~close(void)
	{	if (m_i >= 0)
			::close(m_i);
	}
};
	/// wraps the open system call
std::expected<close, std::system_error> open(const char *const _p, const int _iFlags, const mode_t _iMode = 00600)
{	if (const auto i = ::open(_p, _iFlags, _iMode); i < 0)
		return std::unexpected(
			std::system_error(
				errno,
				std::generic_category(),
				std::format(
					"open(\"{}\") failed",
					_p
				)
			)
		);
	else
		return i;
}
	/// a RAII wrapper for a memory mapping
struct munmap
{	void *m_p;
	std::size_t m_iL;
	munmap(void) = delete;
	munmap(void *const _p, const int _iL)
		:m_p(_p),
		m_iL(_iL)
	{
	}
	munmap(const munmap&) = delete;
	munmap(munmap&&_r)
		:m_p(_r.m_p),
		m_iL(_r.m_iL)
	{	_r.m_p = nullptr;
		_r.m_iL = 0;
	}
	munmap &operator=(const munmap&) = delete;
	munmap &operator=(munmap&&_r)
	{	if (&_r == this)
			return *this;
		if (m_p && m_iL)
			::munmap(m_p, m_iL);
		m_p = _r.m_p;
		m_iL = _r.m_iL;
		_r.m_p = nullptr;
		_r.m_iL = 0;
		return *this;
	}
	~munmap(void)
	{	if (m_p && m_iL)
			::munmap(m_p, m_iL);
	}
};
	/// wraps the mmap() system call
std::expected<munmap, std::system_error> mmap(void*const _pA, const std::size_t _iL, const int _iProt, const int _iFlags, const int _iFD, const std::ptrdiff_t _iOffset)
{	if (const auto p = ::mmap(_pA, _iL, _iProt, _iFlags, _iFD, _iOffset); !p)
		return std::unexpected(
			std::system_error(
				errno,
				std::generic_category(),
				"mmap() failed"
			)
		);
	else
		return munmap(p, _iL);
}
}
