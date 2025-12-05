#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace rlx {
	namespace File
	{
		inline bool Exists(const std::filesystem::path& filePath) {
			return std::filesystem::exists(filePath);
		}

		inline bool Delete(const std::filesystem::path& filePath) {
			return std::filesystem::remove(filePath);
		}

		inline bool Copy(const std::filesystem::path& sourcePath, const std::filesystem::path& destPath, bool overwrite = false) {
			std::filesystem::copy_options options = std::filesystem::copy_options::none;
			if (overwrite) {
				options |= std::filesystem::copy_options::overwrite_existing;
			}

			return std::filesystem::copy_file(sourcePath, destPath, options);
		}

		inline void Move(const std::filesystem::path& sourcePath, const std::filesystem::path& destPath, bool overwrite = false) {
			if (overwrite && std::filesystem::exists(destPath)) {
				std::filesystem::remove(destPath);
			}

			std::filesystem::rename(sourcePath, destPath);
		}

		inline std::string ReadAllText(const std::filesystem::path& filePath) {
			std::ifstream file(filePath, std::ios::in | std::ios::binary);
			if (!file) {
				throw std::runtime_error("Failed to open file for reading: " + filePath.string());
			}
			std::ostringstream contents;
			contents << file.rdbuf();
			return contents.str();
		}
	}

	namespace Directory
	{
		inline bool Exists(const std::filesystem::path& dirPath) {
			return std::filesystem::exists(dirPath);
		}

		inline bool Create(const std::filesystem::path& dirPath) {
			return std::filesystem::create_directory(dirPath);
		}

		inline bool Delete(const std::filesystem::path& dirPath) {
			return std::filesystem::remove(dirPath);
		}

		inline std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& dirPath) {
			std::vector<std::filesystem::path> files;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_regular_file()) {
					files.push_back(entry.path());
				}
			}
			return files;
		}

		inline std::vector<std::filesystem::path> GetDirectories(const std::filesystem::path& dirPath) {
			std::vector<std::filesystem::path> directories;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_directory()) {
					directories.push_back(entry.path());
				}
			}
			return directories;
		}

		inline std::vector<std::filesystem::path> GetFilesWithExtension(const std::filesystem::path& dirPath, const std::string& extension) {
			std::vector<std::filesystem::path> files;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_regular_file() && entry.path().extension() == extension) {
					files.push_back(entry.path());
				}
			}
			return files;
		}
	}
}
