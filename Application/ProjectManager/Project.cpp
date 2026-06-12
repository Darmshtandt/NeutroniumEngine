// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ProjectManager/Project.h>

void Project::Write(std::ostream& Stream) const {
	Nt::Serialization::WriteAll(Stream, m_Name, m_Path, m_RootPath);
}

void Project::Read(std::istream& Stream) {
	Nt::Serialization::ReadAll(Stream, m_Name, m_Path, m_RootPath);
	m_IsLoaded = false;
}

Nt::ISerialization* Project::New([[maybe_unused]] const uInt& ClassType) {
	return new Project;
}

void Project::Create(const Nt::String& name, const Nt::String& path) {
	if (name.empty())
		Raise("Invalid name entered.");

	m_Name = name;
	m_RootPath = path;
	if (m_RootPath.back() != '\\')
		m_RootPath += '\\';

	m_RootPath += name;

	std::wstring wPath = m_RootPath;
	if (GetFileAttributes(wPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
		uInt i = 1;
		wPath = Nt::StringToWString(m_RootPath + Nt::String(i));
		while (GetFileAttributes(wPath.c_str()) != INVALID_FILE_ATTRIBUTES)
			++i;

		m_RootPath += i;
	}

	if (!std::create_directory(std::path(m_RootPath)))
		Raise("Filed to create project folder");

	std::create_directories(std::path(m_RootPath + "\\Assets"));
	std::create_directories(std::path(m_RootPath + "\\Levels"));

	m_Path = m_RootPath + "\\" + m_Name + ".nteproj";

	std::ofstream file(m_Path);
	if (!file.is_open())
		Raise("Failed to create project.");

	Nt::Serialization::WriteAll(file, m_Name, m_RootPath);
	file.close();

	m_IsLoaded = true;
}

void Project::Load(const Nt::String& filePath) {
	m_Path = filePath;
	Load();
}

void Project::Load() {
	if (!m_IsLoaded) {
		std::ifstream file(m_Path);
		if (!file.is_open())
			Raise("Failed to load project.");

		Nt::Serialization::ReadAll(file, m_Name);

		m_RootPath = m_Path;
		for (Nt::String::reverse_iterator iterator = m_RootPath.rbegin(); iterator != m_RootPath.rend(); ++iterator) {
			if ((*iterator) == '\\') {
				m_RootPath.erase(iterator.base(), m_RootPath.end());
				break;
			}
		}

		file.close();
		m_IsLoaded = true;
	}
}

void Project::Unload() {
	m_Path = m_Name = "";
	m_IsLoaded = false;
}

Nt::String Project::GetName() const noexcept {
	return m_Name;
}

Nt::String Project::GetPath() const noexcept {
	return m_Path;
}

Nt::String Project::GetRootPath() const noexcept {
	return m_RootPath;
}

Bool Project::IsLoaded() const noexcept {
	return m_IsLoaded;
}
