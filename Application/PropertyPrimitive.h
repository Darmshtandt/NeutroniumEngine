#pragma once

class PropertyPrimitive : public PropertyComponent {
public:
	enum Buttons {
		BUTTON_JOIN,
		BUTTON_SPLIT,
		BUTTON_INVISIBLE,
	};

	const uInt ID = 7000;

public:
	struct LanguageData : Language::_PropertyWindow::_Primitive {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertyPrimitive(Scence* pScence) {
		SetScence(pScence);
	}

	void Initialize(const Settings& settings) override {
		SetLanguage(settings.CurrentLanguage);

		m_Style = settings.Styles;
		m_PaddingRect = { 5, 5, 10, 10 };

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = 200;

		Create(windowRect, m_LanguageData.WindowName);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(settings.Styles.Property.BackgroundColor);

		Nt::IntRect buttonRect;
		buttonRect.LeftTop = m_PaddingRect.LeftTop;
		buttonRect.Right = windowRect.Right;
		buttonRect.Bottom = 24;

		//m_ButtonAdd.SetID(ID + BUTTON_JOIN);
		//m_ButtonAdd.SetParent(*this);
		//m_ButtonAdd.AddStyles(BS_AUTOCHECKBOX);
		//m_ButtonAdd.Create(buttonRect, m_Language.Window.PropertyPrimitive.Join);

		//buttonRect.Top += buttonRect.Bottom + 1;
		//m_ButtonAdd.SetID(ID + BUTTON_SPLIT);
		//m_ButtonAdd.SetParent(*this);
		//m_ButtonAdd.AddStyles(BS_AUTOCHECKBOX);
		//m_ButtonAdd.Create(buttonRect, m_Language.Window.PropertyPrimitive.Split);

		//buttonRect.Top += buttonRect.Bottom + 1;
		m_ButtonAdd.SetID(ID + BUTTON_INVISIBLE);
		m_ButtonAdd.SetParent(*this);
		m_ButtonAdd.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
		m_ButtonAdd.Create(buttonRect, m_LanguageData.Texts[LanguageData::TEXT_INVISIBLE]);

		windowRect.Bottom = buttonRect.Top;
		windowRect.Bottom += m_PaddingRect.Bottom;
		SetSize(windowRect.RightBottom);
	}
	void Update() override {
		if (!IsEnabled())
			return;

		if (!m_SelectorPtr)
			Raise("Selector pointer is nullptr");

		if (m_SelectorPtr->GetObjects().size() == 1 && m_SelectorPtr->IsChanged())
			Button_SetCheck(m_ButtonInivisible.GetHandle(), m_SelectorPtr->GetObjects()[0]->IsInvisible());
	}

	void SetTheme(const Style& style) override {
		m_Style = style;
		SetBackgroundColor(m_Style.Property.BackgroundColor);
	}
	void SetLanguage(const Language& language) override {
		m_LanguageData = (LanguageData)language.PropertyWindow.Primitive;
		m_LanguageData.WindowName =
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_PRIMITIVEWINDOW];
		SetName(m_LanguageData.WindowName);

		m_ButtonAdd.SetName(m_LanguageData.Texts[LanguageData::TEXT_INVISIBLE]);
	}
	void SetScence(Scence* pScence) {
		if (pScence == nullptr)
			Raise("Scence pointer is nullptr.");
		m_ScencePtr = pScence;
	}

private:
	LanguageData m_LanguageData;
	Scence* m_ScencePtr = nullptr;
	Nt::Button m_ButtonAdd;
	Nt::Button m_ButtonSplit;
	Nt::Button m_ButtonInivisible;
	Nt::IntRect m_PaddingRect;
	Style m_Style;

private:
	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		try {
			if (m_SelectorPtr == nullptr)
				return;

			switch (command) {
			case BN_CLICKED:
				if (m_SelectorPtr != nullptr) {
					switch (id - ID) {
					case BUTTON_JOIN:
						break;
					case BUTTON_SPLIT:
						break;
					case BUTTON_INVISIBLE:
						for (Object* pObject : m_SelectorPtr->GetObjects()) {
							if (IsDlgButtonChecked(m_hwnd, id))
								pObject->EnableInvisible();
							else
								pObject->DisableInvisible();
						}
						break;
					}
				}
				break;
			}
		}
		catch (...)
		{
		}
	}
};