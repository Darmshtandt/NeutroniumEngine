#pragma once

struct PropertyComponent : protected Nt::Window {
	PropertyComponent() noexcept :
		m_SelectorPtr(nullptr),
		m_IsEnabled(true)
	{ 
	}
	__inline virtual ~PropertyComponent()
	{
	}

	virtual void Initialize(const Settings& settings) = 0;
	virtual void Update() = 0;

	virtual void SetTheme(const Style& style) = 0;
	virtual void SetLanguage(const Language& language) = 0;

	void SetSelector(Selector* selectorPtr) noexcept {
		if (selectorPtr == nullptr) {
			Raise("Selector pointer is null");
			return;
		}

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

	_NODISCARD
	Nt::String GetRootPath() const noexcept {
		return m_RootPath;
	}
	_NODISCARD
	Bool IsEnabled() const noexcept {
		return m_IsEnabled;
	}

	void SetRootPath(const Nt::String& rootPath) {
		m_RootPath = rootPath;
	}

public:
	using Window::Show;
	using Window::Hide;
	using Window::PeekMessages;
	using Window::InvalidateRect;
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

protected:
	_NODISCARD
	std::string _Browse(const std::wstring& filter) const noexcept {
		if (m_SelectorPtr == nullptr || m_SelectorPtr->IsEmpty())
			return "";

		Nt::String filePath = Nt::OpenFileDialog(GetRootPath().wstr().c_str(), filter.c_str());
		if (filePath.empty())
			return "";

		const std::string rootPath = GetRootPath();
		if (!IsValidPath(rootPath, filePath)) {
			WarningBox(L"To add a file, place it in the project's root folder", L"Warning");
			return "";
		}

		filePath.erase(filePath.begin(), filePath.begin() + rootPath.length() + 1);

		return filePath;
	}
};