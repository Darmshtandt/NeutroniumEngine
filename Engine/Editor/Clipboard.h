#pragma once

#include <Nt/Core/Utilities.h>
#include <vector>
#include <Objects/Object.h>

class Scene;
class Selector;

class Clipboard {
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