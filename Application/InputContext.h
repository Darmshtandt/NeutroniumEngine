#pragma once

#include <Nt/Core/NtTypes.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>

namespace Nt {
	enum Key : Byte;
}

class InputContext {
public:
	using Action = std::function<void()>;
	using KetSet = std::unordered_set<Nt::Key>;

	struct HotKey final {
		KetSet Keys;
		Action Func;
	};

public:
	InputContext() = default;

	void AddActiveKey(const Nt::Key& key);
	void RemoveActiveKey(const Nt::Key& key);
	void ClearActiveKeys();

	void AddHotKey(KetSet&& keys, Action&& func);

private:
	std::vector<HotKey> m_HotKeys;
	KetSet m_ActiveKeys;

private:
	[[nodiscard]] Bool IsPressedHotKey(const HotKey& hotKey) const noexcept;
};

struct AddInputContextEvent final {
	std::weak_ptr<InputContext> Context;
	std::string Name;
};