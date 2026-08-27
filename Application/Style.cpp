#include <Style.h>

#include <Nt/Core/MessageWindow.h>

#include <fstream>

namespace fs = std::filesystem;

Style::Value::Value(const std::variant<Int, std::string>& data) noexcept :
	Data(data) 
{
}

Style::Value::operator Int() const {
	return std::get<Int>(Data);
}
Style::Value::operator uInt() const {
	return std::get<Int>(Data);
}
Style::Value::operator Nt::Byte3D() const {
	return Nt::Colors::Color4DFromHex(std::get<std::string>(Data)).xyz;
}
Style::Value::operator Nt::Float3D() const {
	return Nt::Colors::Byte4DToColor(operator Nt::Byte4D()).xyz;
}
Style::Value::operator Nt::Byte4D() const {
	return Nt::Colors::Color4DFromHex(std::get<std::string>(Data));
}
Style::Value::operator Nt::Float4D() const {
	return Nt::Colors::Byte4DToColor(operator Nt::Byte4D());
}

void Style::LoadFromFile(const std::string& fileName) {
	try {
		JSON.clear();

		const std::string fullPath = InitialPath + "\\Themes" + '\\' + fileName;

		std::ifstream file(fullPath);
		Assert(file.is_open(), "Failed to open: " + fullPath);

		file >> JSON;
		file.close();

		MessageBus::Instance().Publish(TOPIC_STYLE_CHANGE, this);
	}
	catch (const json::parse_error& error) {
		Nt::MessageWindow(error.what(), "Error").ShowError();
	}
	catch (const Nt::Error& error) {
		error.Show();
	}
}

void Style::UpdateThemeList() {
	Themes.clear();

	fs::path path(InitialPath + "Themes" + "\\");
	for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json")
			Themes.push_back(entry.path().stem().string());
	}
}

Style::Value Style::operator [] (const std::string& key) const {
	if (!JSON.contains(key))
		Raise(std::string("Not found style: ") + key);

	const auto& value = JSON[key];

	if (value.is_number_integer())
		return Value(value.get<Int>());
	if (value.is_string())
		return Value(value.get<std::string>());

	Raise("Type error");
}
