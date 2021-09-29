#include "FuzzyFileSearch.h"
#include "thid_party/edlib/edlib.h"
#include <map>
#include <Chroma/Core/Log.h>
#include <Chroma/Math/Math.h>


namespace Polychrome
{
	std::vector<std::filesystem::path> FuzzyFileSearch::Search(const std::string& search_term, const std::filesystem::path& root, size_t threshold)
	{
		std::map<size_t, std::vector<std::filesystem::path>> paths;
		std::string term(search_term);
		std::transform(term.begin(), term.end(), term.begin(),
			[](unsigned char c) { return std::tolower(c); });

		for (auto &p : std::filesystem::recursive_directory_iterator(root))
		{
			if (p.is_regular_file())
			{
				std::string filename(p.path().filename().string());
				std::transform(filename.begin(), filename.end(), filename.begin(),
					[](unsigned char c) { return std::tolower(c); });
				
				size_t distance = 0;

				auto result = edlibAlign(term.c_str(), term.length(), filename.c_str(), filename.length(), edlibDefaultAlignConfig());
				if (result.status == EDLIB_STATUS_OK)
				{
					if (result.editDistance != -1)
						distance = result.editDistance;
				}
				edlibFreeAlignResult(result);

				int i = 0;
				for (char c : term)
				{
					if (filename.length() >= i || c != filename[i])
						break;
				}

				distance -= i;

				distance += Math::abs(term.length() - filename.length());


				if (distance <= threshold)
				{
					if (paths.find(distance) == paths.end())
						paths.emplace(distance, std::vector<std::filesystem::path>());
					paths[distance].push_back(p);
				}
			}	
		}

		std::vector<std::filesystem::path> sorted;
		for (auto& [distance, path] : paths)
		{
			sorted.insert(sorted.end(), path.begin(), path.end());
		}
		
		return sorted;
		
	}
}