#pragma once
#include <filesystem>
#include <string>
#include <vector>

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
		std::string ExeFolderPath() {
			return std::filesystem::current_path().string();
		}
		std::string RelativeToDirectPath(const std::string& file) {
			return ExeFolderPath() + '\\' + file;
		}
		struct FolderPaths {
			std::vector<std::filesystem::path> files;
			void sortByDate(bool oldestFirst = true) {
				std::vector<std::filesystem::path> sortedByDate;
				for (const auto& path : files) {
					bool notAdded = true;
					std::filesystem::file_time_type curPathTime = std::filesystem::last_write_time(path);
					for (int i = 0; i < sortedByDate.size(); i++) {
						std::filesystem::file_time_type time = std::filesystem::last_write_time(sortedByDate[i]);
						if (oldestFirst) {
							if (curPathTime < time) continue;
						}
						else {
							if (curPathTime > time) continue;
						}
						sortedByDate.insert(std::next(sortedByDate.begin(), i), path);
						notAdded = false;
						break;
					}
					if (notAdded) {
						sortedByDate.push_back(path);
					}
				}
				files = sortedByDate;
			}
			void addFolderFiles(std::filesystem::path folder, std::string optionalAddOnlyFileExtension = "") {
				for (auto& p : std::filesystem::directory_iterator(folder)) {
					auto extension = p.path().extension();
					std::string s = extension.generic_string();
					if (!optionalAddOnlyFileExtension.empty() && !s.ends_with(optionalAddOnlyFileExtension)) {
						continue;
					}
					files.push_back(p);
				}
			}
		};
	}
}