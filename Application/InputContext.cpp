#include <InputContext.h>
#include <Nt/Core/Input.h>

void InputContext::AddActiveKey(const Nt::Key& key) {
	m_ActiveKeys.insert(key);

	for (const HotKey& hotKey : m_HotKeys) {
		if (IsPressedHotKey(hotKey))
			hotKey.Func();
	}
}

void InputContext::RemoveActiveKey(const Nt::Key& key) {
	if (m_ActiveKeys.contains(key))
		m_ActiveKeys.erase(key);
}

void InputContext::ClearActiveKeys() {
	m_ActiveKeys.clear();
}

void InputContext::AddHotKey(KetSet&& keys, Action&& func) {
	m_HotKeys.emplace_back(std::move(keys), std::move(func));
}

Bool InputContext::IsPressedHotKey(const HotKey& hotKey) const noexcept {
	for (const Nt::Key& key : hotKey.Keys) {
		if (!m_ActiveKeys.contains(key))
			return false;
	}
	return true;
}