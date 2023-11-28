#pragma once

struct PropertyComponent : protected Nt::Window {
	PropertyComponent() noexcept :
		m_SelectorPtr(nullptr),
		m_IsEnabled(true)
	{ }

	virtual void Initialize(const Settings& settings) = 0;
	virtual void Update() = 0;

	virtual void SetTheme(const Style& style) = 0;
	virtual void SetLanguage(const Language& language) = 0;

	void SetSelector(Selector* selectorPtr) noexcept {
		m_SelectorPtr = selectorPtr;
	}

	void Enable() {
		if (!m_IsEnabled) {
			Show();
			m_IsEnabled = true;
		}
	}
	void Disable() {
		if (m_IsEnabled) {
			Hide();
			m_IsEnabled = false;
		}
	}

	Nt::String GetRootPath() const {
		return m_RootPath;
	}
	Bool IsEnabled() const noexcept {
		return m_IsEnabled;
	}

	void SetRootPath(const Nt::String& rootPath) {
		m_RootPath = rootPath;
	}

public:
	using Window::Show;
	using Window::Hide;
	using Window::PopEvent;
	using Window::GetHandle;
	using Window::GetParent;
	using Window::GetClientRect;
	using Window::GetWindowRect;
	using Window::SetParent;
	using Window::SetSize;
	using Window::SetPosition;
	using Window::SetWindowRect;

protected:
	Selector* m_SelectorPtr;

private:
	Nt::String m_RootPath;
	Bool m_IsEnabled;
};