#pragma once

#include <Nt/Core/Utilities.h>

class Lua;
class TiXmlElement;
class Scene;

class WorldDocument {
public:
	explicit WorldDocument(NotNull<Lua*> pLua) noexcept;

	[[nodiscard]] Bool Open(NotNull<Scene*> pScene);
	[[nodiscard]] Bool Save(NotNull<Scene*> pScene);
	[[nodiscard]] Bool SaveAs(NotNull<Scene*> pScene);

	void SetDefaultPath(const Nt::String& defaultPath) noexcept;

private:
	Lua* m_pLua;
	Nt::String m_FilePath;
	std::wstring m_DefaultPath;

private:
	[[nodiscard]] Bool SaveToFile(NotNull<TiXmlElement*> pRoot) const;
};