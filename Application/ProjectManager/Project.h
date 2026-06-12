#pragma once

#include <Main.h>

class Project : public Nt::ISerialization {
public:
	Project() noexcept = default;

	void Write(std::ostream& Stream) const override;
	void Read(std::istream& Stream) override;

	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}
	constexpr uInt ClassType() const noexcept override {
		return 0;
	}
	static ISerialization* New([[maybe_unused]] const uInt& ClassType);

	void Create(const Nt::String& name, const Nt::String& path);

	void Load(const Nt::String& filePath);
	void Load();
	void Unload();

	Nt::String GetName() const noexcept;
	Nt::String GetPath() const noexcept;
	Nt::String GetRootPath() const noexcept;
	Bool IsLoaded() const noexcept;

private:
	std::string m_Name;
	Nt::String m_Path;
	Nt::String m_RootPath;
	Bool m_IsLoaded = false;
};