#include <Language.h>

#include <Nt/Core/MessageWindow.h>
#include <Nt/Core/Log.h>

#include <fstream>

void Language::LoadFromFile(const std::string& fileName) {
	try {
		JSON.clear();

		const std::string fullPath = InitialPath + "\\Lang" + '\\' + fileName;

		std::ifstream file(fullPath);
		Assert(file.is_open(), "Failed to open: " + fullPath);

		file >> JSON;
		file.close();
	}
	catch (const json::parse_error& error) {
		Nt::MessageWindow(error.what(), "Error").ShowError();
	}
	catch (const Nt::Error& error) {
		error.Show();
	}
}

std::string Language::operator[](const std::string& key) const noexcept {
	if (JSON.contains(key))
		return JSON[key];

	Nt::Log::Instance().Warning("Not found  " + key);
	return key;
}
