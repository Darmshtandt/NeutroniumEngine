#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Language {
public:
	Language() noexcept = default;

	void LoadFromFile(const std::string& fileName);

	std::string operator [] (const std::string& key) const noexcept;

	json JSON;
	std::string InitialPath = std::filesystem::current_path().string() + "/..";
};