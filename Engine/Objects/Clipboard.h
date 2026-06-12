#pragma once

#include <Nt/Core/Utilities.h>
#include <memory>
#include <vector>

class Scene;
class Object;
class Selector;

class Clipboard {
	using ObjectPtr = std::unique_ptr<Object>;
	using ObjectContainer = std::vector<ObjectPtr>;

public:
	explicit Clipboard(NotNull<Scene*> pScene, NotNull<Selector*> pSelector) noexcept;

	void Copy();
	void Cut();
	void Paste();
	void Clear();

private:
	ObjectContainer m_Clipboard;
	Selector* m_pSelector;
	Scene* m_pScene;
};