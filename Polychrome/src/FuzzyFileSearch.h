#pragma once
#include <string>
#include <filesystem>

namespace Polychrome
{
	class FuzzyFileSearch
	{
	public:
		static std::vector<std::filesystem::path>Search(const std::string& search_term, const std::filesystem::path& root, size_t threshold = 8);
	};
}


