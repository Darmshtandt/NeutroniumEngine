#pragma once

#include <Function.h>

#include <algorithm>
#include <string>
#include <unordered_map>

template <class _Ty>
class MessageBus {
public:
	using Func = Function<void(_Ty)>;
	using FuncContainer = std::vector<Func>;

public:
	[[nodiscard]]
	static MessageBus& Instance() noexcept {
		static MessageBus bus;
		return bus;
	}

	void Lock() noexcept {
		m_IsLocked = true;
	}
	void Unlock() noexcept {
		m_IsLocked = false;
	}

	void Publish(const std::string& message, _Ty data) {
		if (m_IsLocked || !m_Bus.contains(message))
			return;

		const FuncContainer& functions = m_Bus[message];
		for (const Func& func : functions)
			func(data);
	}

	void Subscribe(const std::string& message, const Func& function) {
		m_Bus[message].push_back(function);
	}
	void Unsubscribe(const std::string& message, const Func& function) {
		if (!m_Bus.contains(message))
			return;

		FuncContainer& functions = m_Bus[message];
		functions.erase(
			std::remove(functions.begin(), functions.end(), function),
			functions.end());

		if (functions.empty())
			m_Bus.erase(message);
	}

private:
	std::unordered_map<std::string, FuncContainer> m_Bus;
	Bool m_IsLocked = false;
};