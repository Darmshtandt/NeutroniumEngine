#pragma once

#include <Nt/Core/Colors.h>
#include <MessageBus.h>

#include <filesystem>
#include <variant>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr const Char* TOPIC_STYLE_CHANGE = "Style: Change";

struct Style {
public:
	using MessageBus = MessageBus<Style*>;

	struct Value {
		Value(const std::variant<Int, std::string>& data) noexcept;

		[[nodiscard]] operator Int() const;
		[[nodiscard]] operator uInt() const;
		[[nodiscard]] operator Nt::Byte3D() const;
		[[nodiscard]] operator Nt::Float3D() const;
		[[nodiscard]] operator Nt::Byte4D() const;
		[[nodiscard]] operator Nt::Float4D() const;

		std::variant<Int, std::string> Data;
	};

public:
	Style() noexcept = default;

	void LoadFromFile(const std::string& fileName);
	void UpdateThemeList();

	[[nodiscard]] Value operator [] (const std::string& key) const;

	json JSON;
	std::vector<std::string> Themes;
	std::string InitialPath = std::filesystem::current_path().string() + "/..";
};