#pragma once

#include <Nt/Core/Utilities.h>
#include <memory>

template <typename _Ty>
class Function;

template <typename _Ret, typename... _Args>
class Function<_Ret(_Args...)> {
public:
	struct ICallable {
		virtual ~ICallable() = default;
		virtual _Ret Invoke(_Args... args) = 0;
		virtual ICallable* Clone() const = 0;
	};

	template <typename _Func>
	struct CallableImpl : ICallable {
		CallableImpl(_Func func) noexcept : 
			Func(std::move(func)) 
		{
		}

		_Ret Invoke(_Args... args) override {
			return Func(std::forward<_Args>(args)...);
		}

		_NODISCARD
		ICallable* Clone() const override {
			return new CallableImpl<_Func>(Func);
		}

		_Func Func;
	};

public:
	Function() = default;
	Function(Function&& function) noexcept :
		m_pCallable(function.m_pCallable),
		m_ID(function.m_ID)
	{
		function.m_pCallable = nullptr;
	}
	Function(const Function& other) {
		if (other.m_pCallable != nullptr) {
			m_pCallable = other.m_pCallable->Clone();
			m_ID = other.m_ID;
		}
	}

	template <typename _Func>
	Function(_Func function) :
		m_pCallable(new CallableImpl<_Func>(std::move(function))),
		m_ID(_GetNewID())
	{
	}

	template <typename _Class>
	Function(_Class* pInstance, _Ret(_Class::* method)(_Args...)) :
		m_ID(_GetNewID())
	{
		auto wrapper = [=] (_Args&&... args) {
			return (pInstance->*method)(std::forward<_Args>(args)...);
			};

		m_pCallable = new CallableImpl<decltype(wrapper)>(std::move(wrapper));
	}
	~Function() {
		if (m_pCallable != nullptr)
			delete(m_pCallable);
	}

	void Swap(Function& other) noexcept {
		std::swap(m_pCallable, other.m_pCallable);
	}
	void Reset() {
		if (m_pCallable != nullptr) {
			delete(m_pCallable);
			m_pCallable = nullptr;
		}
	}

	_Ret operator () (_Args... args) const {
		if (m_pCallable != nullptr)
			return m_pCallable->Invoke(std::forward<_Args>(args)...);

		Raise("Bad alloc");
	}

	Function& operator = (const Function& other) {
		if (this == &other)
			return *this;

		if (m_pCallable != nullptr)
			delete(m_pCallable);

		m_pCallable = (other.m_pCallable != nullptr) ? other.m_pCallable->Clone() : nullptr;
		m_ID = _GetNewID();

		return *this;
	}

	Function& operator = (Function&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_pCallable != nullptr)
			delete(m_pCallable);

		m_pCallable = other.m_pCallable;
		m_ID = other.m_ID;
		other.m_pCallable = nullptr;

		return *this;
	}

	Bool operator == (const Function& other) const noexcept {
		if (m_pCallable == nullptr || other.m_pCallable == nullptr)
			return (m_pCallable == other.m_pCallable);

		return (m_ID == other.m_ID);
	}

	explicit operator Bool() const noexcept {
		return (m_pCallable != nullptr);
	}

private:
	ICallable* m_pCallable = nullptr;
	uInt m_ID = 0;

private:
	_NODISCARD uInt _GetNewID() noexcept {
		static uInt counter = 0;
		return ++counter;
	}
};