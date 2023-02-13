#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>

/* 
* Code editor to copy text into another file 
*
* you need to specify the area your text is copyed into comments
*
* Example for Autohotkey code
* ;myCopyPaste
* ...
* ;myCopyPaste
*
* ... stands for there could be multiple lines of the copied code.
* 
* there is 2 important parts
* 
* -make sure your areaComment are unique from your other comments used outside this
* -make sure to not have only the areaComment in the line
* 
*/


namespace tl {
	namespace bce {
		std::string TextBetweenAreaComments(const std::string& areaComment, std::fstream& fs) {
			std::string text = "";
			for (std::string line; std::getline(fs, line);) {
				if (line == areaComment) break;
			}
			for (std::string line; std::getline(fs, line);) {
				if (line == areaComment) break;
				text += line + '\n';
			}
			return text;
		}
		std::string FindCopyText(const std::string& areaComment, const std::string& filePath) {
			std::fstream fs(filePath);
			std::string copyText = TextBetweenAreaComments(areaComment, fs);
			fs.close();
			return copyText;
		}
		std::string PlainFullText(std::fstream& fs) {
			std::string fullText = "";
			for (std::string line; std::getline(fs, line);) {
				fullText += line + "\n";
			}
			fullText.pop_back(); //removes last linechange
			return fullText;
		}
		std::string FindFullText(const std::string& areaComment, const std::string& text, std::fstream& fs) {
			std::string fullText;
			//text before areaComment
			for (std::string line; std::getline(fs, line);) {
				fullText += line + "\n";
				if (line == areaComment) break;
			}
			//text between areaComments
			for (std::string line; std::getline(fs, line);) {
				if (line == areaComment) {
					fullText += text; //already got \n in it
					fullText += line + '\n';
					break;
				}
			}
			//text after areaComment
			for (std::string line; std::getline(fs, line);) {
				fullText += line + '\n';
			}
			fullText.pop_back(); //removes unneccesary linechange
			return fullText;
		}
		void ReplaceWithText(const std::string& areaComment, const std::string& text, const std::string& filePath) {
			std::fstream fs(filePath);
			std::string fullText = FindFullText(areaComment, text, fs);
			fs.close();
			fs.open(filePath, std::ios::out);
			fs << fullText;
			fs.close();
		}
		void validateFile(
			const std::string& areaComment,
			const std::string& filePath
		) {
			std::string fullError = "";
			//check for correct parameters
			if (!std::filesystem::exists(filePath))
				fullError += "filePath not found " + filePath + "\n";
			if (areaComment.size() == 0)
				fullError += "areaComment size 0 \n";

			//start looking for correct areaComments in both files
			if (std::filesystem::exists(filePath)) {
				std::fstream fs(filePath);
				int areaCommentCount = 0;
				for (std::string line; std::getline(fs, line);) {
					if (line == areaComment) areaCommentCount++;
				}
				if (areaCommentCount != 2)
					fullError += "baseFile wrong count of areaComments the count is " + std::to_string(areaCommentCount) + "\n";
			}

			if (fullError.size() != 0) {
				throw std::runtime_error(fullError);
			}
		}
		//will throw on error
		void validateFiles(const std::string& areaComment,
			const std::string& baseFilePath,
			const std::string& updatingFilePath
		) {
			std::string fullError = "";
			//check for correct parameters
			if (!std::filesystem::exists(baseFilePath))
				fullError += "baseFilePath not found " + baseFilePath + "\n";
			if (!std::filesystem::exists(updatingFilePath))
				fullError += "updatingFilePath not found " + updatingFilePath + "\n";
			if (areaComment.size() == 0)
				fullError += "areaComment size 0 \n";

			//start looking for correct areaComments in both files
			if (std::filesystem::exists(baseFilePath)) {
				std::fstream fs(baseFilePath);
				int areaCommentCount = 0;
				for (std::string line; std::getline(fs, line);) {
					if (line == areaComment) areaCommentCount++;
				}
				if (areaCommentCount != 2)
					fullError += "baseFile wrong count of areaComments the count is " + std::to_string(areaCommentCount) + "\n";
			}
			if (std::filesystem::exists(updatingFilePath)) {
				std::fstream fs(updatingFilePath);
				int areaCommentCount = 0;
				for (std::string line; std::getline(fs, line);) {
					if (line == areaComment) areaCommentCount++;
				}
				if (areaCommentCount != 2)
					fullError += "updatingFile wrong count of areaComments the count is " + std::to_string(areaCommentCount) + "\n";
			}

			if (fullError.size() != 0) {
				throw std::runtime_error(fullError);
			}
		}
		void RunEditor(
			const std::string& areaComment,
			const std::string& baseFilePath,
			const std::string& updatingFilePath
		) {
			try 
			{
				validateFiles(areaComment, baseFilePath, updatingFilePath);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
				return;
			}
			std::string copyText = FindCopyText(areaComment, baseFilePath);
			ReplaceWithText(areaComment, copyText, updatingFilePath);
			
		}

		std::string RelativeToDirectPath(const std::string& file) {
			return std::filesystem::current_path().string() + '\\' + file;
		}

		void RunEditorRelativeToExePath(
			const std::string& areaComment,
			const std::string& baseFile,
			const std::string& updatingFile
		) {
			RunEditor(areaComment, RelativeToDirectPath(baseFile), RelativeToDirectPath(updatingFile));
		}
	}
}