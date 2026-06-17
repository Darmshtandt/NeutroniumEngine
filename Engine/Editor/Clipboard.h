#pragma once

#include <Nt/Core/Utilities.h>
#include <Objects/Object.h>

namespace Nt {
	class EventBus;
}

class Selector;

class Clipboard {
public:
	explicit Clipboard(const std::weak_ptr<Nt::EventBus>& pBus, NotNull<Selector*> pSelector) noexcept;

	void Copy();
	void Cut();
	void Paste();
	void Clear();

private:
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	ObjectContainer m_Clipboard;
	Selector* m_pSelector;
};