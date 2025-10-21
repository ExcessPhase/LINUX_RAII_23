#include "linux.h"
#include <iostream>
#include <string_view>
#include <sys/stat.h>


int main(int, char**argv)
{	using namespace foelsche::linux_ns;

		/// opening a file
	auto sFdAndMap = foelsche::linux_ns::open(argv[1], O_RDONLY).and_then(
		[](foelsche::linux_ns::close fd) -> std::expected<std::pair<foelsche::linux_ns::close, std::size_t>, std::system_error>
		{	struct stat st;
			if (fstat(fd.m_i, &st) < 0)
				return std::unexpected(std::system_error(errno, std::generic_category(), "fstat failed"));
			return std::make_pair(std::move(fd), st.st_size);
		}).and_then(
			[](std::pair<foelsche::linux_ns::close, std::size_t> fdAndSize)
			{	return foelsche::linux_ns::mmap(
					nullptr,
					fdAndSize.second,
					PROT_READ,
					MAP_SHARED,
					fdAndSize.first.m_i,
					0
				).transform(
					[&](foelsche::linux_ns::munmap map)
					{	return std::make_pair(std::move(fdAndSize.first), std::move(map));
					}
				);
			}
		);
				/// checking for success
	if (sFdAndMap.has_value())
	{	//std::cout << "successful mmap!" << std::endl;
			/// and printing the file contents
		std::cout << std::string_view(
			static_cast<const char*>(sFdAndMap.value().second.m_p),
			sFdAndMap.value().second.m_iL
		) << std::endl;
	}
	else
	{	std::cout << sFdAndMap.error().what() << std::endl;
		return 1;
	}
}
#if 0
using namespace foelsche::linux_ns;

auto sFdAndMap = open("file.dat", O_RDONLY)
    .and_then([](close fd) {
        struct stat st;
        if (fstat(fd.m_i, &st) < 0)
            return std::unexpected(std::system_error(errno, std::generic_category(), "fstat failed"));
        return std::expected<std::pair<close, std::size_t>, std::system_error>{std::make_pair(std::move(fd), st.st_size)};
    })
    .and_then([](std::pair<close, std::size_t> fdAndSize) {
        return mmap(nullptr, fdAndSize.second, PROT_READ, MAP_SHARED, fdAndSize.first.m_i, 0)
            .transform([&](munmap map) {
                return std::make_pair(std::move(fdAndSize.first), std::move(map));
            });
    });
#endif
