#pragma once

class Project : public Nt::ISerialization {
public:
	Project() = default;

	void Write(std::ostream& Stream) const override {
		Nt::Serialization::WriteAll(Stream, m_Name, m_Path, m_RootPath);
	}
	void Read(std::istream& Stream) override {
		Nt::Serialization::ReadAll(Stream, m_Name, m_Path, m_RootPath);
		m_IsLoaded = false;
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}
	constexpr uInt ClassType() const noexcept override {
		return 0;
	}
	static ISerialization* New(const uInt& ClassType) {
		return new Project;
	}

	void Create(const Nt::String& name, const Nt::String& path) {
		if (name.length() == 0)
			Raise("Invalid name entered.");

		m_Name = name;
		m_RootPath = path;
		if (*(m_RootPath.end() - 1) != '\\')
			m_RootPath += '\\';
		m_RootPath += name;

		if (GetFileAttributes(m_RootPath.wstr().c_str()) != INVALID_FILE_ATTRIBUTES) {
			uInt i = 1;
			while (GetFileAttributes((m_RootPath + Nt::String(i)).wstr().c_str()) != INVALID_FILE_ATTRIBUTES)
				++i;
			m_RootPath += i;
		}

		if (!std::create_directory(std::path(m_RootPath)))
			Raise("Filed to create project folder");

		std::create_directories(std::path(m_RootPath + "\\Assets"));
		std::create_directories(std::path(m_RootPath + "\\Levels"));

		m_Path = m_RootPath + '\\' + m_Name + ".nteproj";

		std::ofstream file(m_Path);
		if (!file.is_open())
			Raise("Failed to create project.");
		Nt::Serialization::WriteAll(file, m_Name, m_RootPath);
		file.close();

		m_IsLoaded = true;
	}

	void Load(const Nt::String& filePath) {
		m_Path = filePath;
		Load();
	}
	void Load() {
		if (!m_IsLoaded) {
			std::ifstream file(m_Path);
			if (!file.is_open())
				Raise("Failed to load project.");
			Nt::Serialization::ReadAll(file, m_Name);
			m_RootPath = m_Path;
			for (uInt i = m_RootPath.length() - 1; i >= 0; --i) {
				if (m_RootPath[i] == '\\') {
					m_RootPath.erase(m_RootPath.begin() + i, m_RootPath.end());
					break;
				}
			}
			file.close();
			m_IsLoaded = true;
		}
	}
	void Unload() {
		m_Path = m_Name = "";
		m_IsLoaded = false;
	}

	Nt::String GetName() const noexcept {
		return m_Name;
	}
	Nt::String GetPath() const noexcept {
		return m_Path;
	}
	Nt::String GetRootPath() const noexcept {
		return m_RootPath;
	}
	Bool IsLoaded() const noexcept {
		return m_IsLoaded;
	}

private:
	std::string m_Name;
	Nt::String m_Path;
	Nt::String m_RootPath;
	Bool m_IsLoaded = false;
};