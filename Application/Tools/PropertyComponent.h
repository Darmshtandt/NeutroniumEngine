#pragma once

#include <Main.h>
#include <Selector.h>

#include <MessageBus.h>
#include <ranges>

struct PropertyComponent : protected Nt::Window {
protected:
	using MessageBus = MessageBus<Object*>;
	using BusFunc = MessageBus::Func;
	using Subscription = std::pair<std::string, BusFunc>;

public:
	PropertyComponent(NotNull<Selector*> pSelector, const uInt& textsCount, const uInt& textEditsCount, const uInt& buttonsCount) noexcept :
		m_Texts(textsCount),
		m_TextEdits(textEditsCount),
		m_Buttons(buttonsCount),
		m_SelectorPtr(pSelector),
		m_IsEnabled(true)
	{
		const uInt maxCount = std::max(textEditsCount, buttonsCount);
		for (uInt i = 0; i < maxCount; ++i) {
			if (i < textEditsCount)
				m_TextEdits[i].SetID(i);

			if (i < buttonsCount)
				m_Buttons[i].SetID(i);
		}

		m_Subscriptions = {
			{ TOPIC_SELECTOR_ADD_SELECTION, BusFunc(this, &PropertyComponent::_AddSelection) },
			{ TOPIC_SELECTOR_DESELECT, BusFunc(this, &PropertyComponent::_Deselect) },
			{ TOPIC_SELECTOR_ALL_DESELECT, BusFunc(this, &PropertyComponent::_AllDeselect) }
		};

		for (const Subscription& subscription : m_Subscriptions)
			MessageBus::Instance().Subscribe(subscription.first, subscription.second);
	}
	virtual ~PropertyComponent() {
		for (const Subscription& subscription : m_Subscriptions)
			MessageBus::Instance().Unsubscribe(subscription.first, subscription.second);
	}

	virtual void Initialize(const Settings& settings) = 0;
	virtual void Update() = 0;

	virtual void SetLanguage(const Language& language) = 0;
	virtual void SetTheme(const Style& style) = 0;

	virtual void SetEventBus(const std::weak_ptr<Nt::EventBus>& pBus) {
		(void)pBus;
	}
	void SetSelector(NotNull<Selector*> pSelector) {
		m_SelectorPtr = pSelector;
	}
	void SetRootPath(const Nt::String& rootPath) {
		m_RootPath = rootPath;
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

	[[nodiscard]] Nt::String GetRootPath() const noexcept {
		return m_RootPath;
	}
	[[nodiscard]] Bool IsEnabled() const noexcept {
		return m_IsEnabled;
	}

public:
	using Window::Show;
	using Window::Hide;
	using Window::PeekMessages;
	using Window::InvalidateRect;
	using Window::GetHandle;
	using Window::GetParent;
	using Window::GetPosition;
	using Window::GetSize;
	using Window::GetClientRect;
	using Window::GetWindowRect;
	using Window::IsShowed;
	using Window::SetParent;
	using Window::SetSize;
	using Window::SetPosition;
	using Window::SetWindowRect;

protected:
	std::vector<std::string> m_RejectedObjects;
	std::vector<Nt::Text> m_Texts;
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Button> m_Buttons;
	Selector* m_SelectorPtr;

private:
	std::vector<Subscription> m_Subscriptions;
	std::map<std::string, uInt> m_RejectedObjectsMap;
	Nt::String m_RootPath;
	Bool m_IsEnabled;

protected:
	virtual void _AddSelection(Object* pObject) {
		const std::string& objectToken = pObject->GetToken();
		if (_IsRejectedObjects(objectToken))
			++m_RejectedObjectsMap[objectToken];

		if (m_RejectedObjectsMap.empty()) {
			_EnableWindow();
			Show();
		}
		else {
			_DisableWindow();
			Hide();
		}
	}
	virtual void _Deselect(Object* pObject) {
		const std::string& objectToken = pObject->GetToken();
		if (!_IsRejectedObjects(objectToken))
			return;

		--m_RejectedObjectsMap[objectToken];
		if (m_RejectedObjectsMap[objectToken] <= 0)
			m_RejectedObjectsMap.erase(objectToken);

		if (m_SelectorPtr->GetObjectCount() > 1 && m_RejectedObjectsMap.empty()) {
			_EnableWindow();
			Show();
		}
	}
	virtual void _AllDeselect(Object* pObject) {
		(void)pObject;

		m_RejectedObjectsMap.clear();
		Hide();
	}

	[[nodiscard]] Bool _IsRejectedObjects(const std::string& objectToken) {
		const std::vector<std::string>::const_iterator iterator =
			std::ranges::find(m_RejectedObjects, objectToken);
		return (iterator != m_RejectedObjects.cend());
	}

	void _EnableWindow() {
		if (m_IsWindowEnabled)
			return;

		for (Nt::Button& button : m_Buttons)
			button.EnableWindow();
		for (Nt::TextEdit& textEdit : m_TextEdits)
			textEdit.EnableWindow();

		m_IsWindowEnabled = true;
	}
	void _DisableWindow() {
		if (!m_IsWindowEnabled)
			return;

		for (Nt::Button& button : m_Buttons)
			button.DisableWindow();
		for (Nt::TextEdit& textEdit : m_TextEdits)
			textEdit.DisableWindow();

		m_IsWindowEnabled = false;
	}

	[[nodiscard]] std::string _Browse(cwString filter) const {
		const std::wstring wRootPath(GetRootPath());
		Nt::String filePath = Nt::OpenFileDialog(wRootPath.c_str(), filter);
		if (filePath.empty())
			return "";

		const std::string rootPath = GetRootPath();
		if (!IsValidPath(rootPath, filePath)) {
			Nt::MessageWindow("To add a file, place it in the project's root folder", "Warning").ShowWarning();
			return "";
		}

		filePath.erase(filePath.begin(), filePath.begin() + rootPath.length());

		return filePath;
	}

	void _Create(const Settings& settings, const std::string& name, const Nt::IntRect& windowRect) {
		SetLanguage(settings.Language);
		SetTheme(settings.Style);

		Create(windowRect, name);
		RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		AddStyles(STYLE_BORDER);
	}

private:
	void _Command(const Long& param_1, const Long& param_2) final {
		if (m_IsWindowEnabled)
			_HandleNotification(HIWORD(param_1), LOWORD(param_1), reinterpret_cast<HWND>(param_2));
	}

	virtual void _HandleNotification(const uInt& command, const uInt& elementID, const HWND& handle) {
		switch (command) {
		case BN_CLICKED:
			_ButtonsNotification_OnClick(elementID, handle);
			break;

		case EN_UPDATE:
			_TextEditsNotification_Update(elementID, handle);
			break;
		}
	}

	virtual void _ButtonsNotification_OnClick(const uInt& id, const HWND& handle) {
		(void)handle;

		Assert(id < m_Buttons.size(), "Out of range");
		m_Buttons[id].OnClick();
	}

	virtual void _TextEditsNotification_Update(const uInt& id, const HWND& handle) {
		(void)handle;

		Assert(id < m_Buttons.size(), "Out of range");
		m_TextEdits[id].OnUpdate();
	}
};