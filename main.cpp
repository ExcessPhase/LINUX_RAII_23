#include "linux.h"
#include <iostream>
#include <string_view>
#include <sys/stat.h>


int main(int, char**argv)
{	using namespace foelsche::linux_ns;

		/// opening a file
	auto sFD = foelsche::linux_ns::open(argv[1], O_RDONLY);
		/// checking for failure
	if (!sFD.has_value())
	{	std::cout << sFD.error().what() << std::endl;
		return 1;
	}
		/// creating the memory mapping
	auto sMap = foelsche::linux_ns::mmap(
		nullptr,
		[&](void)
		{	struct stat st;
			fstat(sFD.value().m_i, &st);
			return st.st_size;
		}(),
		PROT_READ,
		MAP_SHARED,
		sFD.value().m_i,
		0
	);
		/// checking for success
	if (sMap.has_value())
	{	//std::cout << "successful mmap!" << std::endl;
			/// moving into some common object for RAII purposes
		const auto sFdAndMap = std::make_pair(std::move(sFD.value()), std::move(sMap.value()));
			/// and printing the file contents
		std::cout << std::string_view(
			static_cast<const char*>(sFdAndMap.second.m_p),
			sFdAndMap.second.m_iL
		) << std::endl;
	}
	else
	{	std::cout << sMap.error().what() << std::endl;
		return 1;
	}
}
