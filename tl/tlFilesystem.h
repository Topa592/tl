#pragma once
#include <filesystem>
#include <string>

namespace tl {
	namespace filesystem {
		class RelativePather {
			std::string path;
		public:
			RelativePather(const std::string& folderDirectPath)
			: path(folderDirectPath) 
			{}
			std::string operator()(const std::string& relativePath) {
				return path + '\\' + relativePath;
			}
		};
	}
}