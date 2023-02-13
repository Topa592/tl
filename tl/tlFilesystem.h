#pragma once
#include <filesystem>
#include <string>

namespace tl {
	namespace filesystem {
		std::string GetFolderPath(const std::string& directFilePath) {
			size_t start = directFilePath.rfind('\\');
			return directFilePath.substr(0, start+1);
		}
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
		std::string GetFileNameFromPath(const std::string& directFilePath) {
			size_t start = directFilePath.rfind('\\');
			return directFilePath.substr(start + 1);
		}
	}
}