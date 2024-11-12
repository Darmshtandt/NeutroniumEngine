#pragma once

class PropertyScript : public PropertyComponent {
public:
	class Control {
	public:
		enum Types {
			TEXTEDIT,
			CHECKBOX
		};

	public:
		Control(const Control& control) = delete;

		Control(const Types& type, const std::string& name, const std::string& variableName) :
			m_Type(type),
			m_Name(name),
			m_VariableName(variableName)
		{
		}

		~Control() {
			delete(m_pHandle);
		}

		void Create(const Style& style, const uInt& id, const Nt::IntRect& rect, HandleWindow* pParent, Object* pObject) {
			if (m_IsCreated) {
				Raise("Control already created.");
				return;
			}

			if (pParent == nullptr) {
				Raise("Parent pointer is null.");
				return;
			}

			if (pObject == nullptr) {
				Raise("Object pointer is null.");
				return;
			}

			m_ObjectPtr = pObject;

			switch (m_Type) {
			case TEXTEDIT: {
				Nt::IntRect textRect = rect;
				textRect.Right /= 2;

				m_Text.SetColor(style.Property.Texts.Color);
				m_Text.SetWeight(style.Property.Texts.Weight);
				m_Text.SetText(m_Name);
				m_Text.SetPosition(textRect.LeftTop);

				Nt::IntRect textEditRect = rect;
				textEditRect.Left = textRect.Left + textRect.Right;
				textEditRect.Right -= textEditRect.Left;

				Nt::TextEdit* pTextEdit = new Nt::TextEdit;
				pTextEdit->SetID(id);
				pTextEdit->SetParent(*pParent);
				pTextEdit->Create(textEditRect, "", true);
				pTextEdit->SetBackgroundColor(style.Property.TextEdits.BackgroundColor);
				pTextEdit->SetTextColor(style.Property.TextEdits.Text.Color);
				pTextEdit->SetTextWeight(style.Property.TextEdits.Text.Weight);
				pTextEdit->Show();

				m_pHandle = pTextEdit;
			}
				break;

			case CHECKBOX:
				m_pHandle = new Nt::Button(*pParent, rect, id, m_Name);
				break;

			default:
				Raise("Failed control type.");
				return;
			}

			m_IsCreated = true;
		}

		void Update() {
			if (m_ObjectPtr == nullptr) {
				Raise("Object pointer is null");
				return;
			}

			Script* pScript = m_ObjectPtr->GetScript();
			if (pScript == nullptr) {
				Raise("Script pointer is null");
				return;
			}

			switch (m_Type) {
			case TEXTEDIT: {
				Nt::TextEdit* pTextEdit = dynamic_cast<Nt::TextEdit*>(m_pHandle);
				if (pTextEdit == nullptr) {
					Raise("Failed to upcast HandleWindow to TextEdit");
					return;
				}
				
				pScript->SetDataValue(m_VariableName, pTextEdit->GetText());
			}
				break;

			case CHECKBOX: {
				Nt::Button* pCheckbox = dynamic_cast<Nt::Button*>(m_pHandle);
				if (pCheckbox == nullptr) {
					Raise("Failed to upcast HandleWindow to Button");
					return;
				}

				pScript->SetDataValue(m_VariableName, pCheckbox->IsChecked());
			}
				break;

			default:
				Raise("Failed control type.");
				return;
			}
		}

		void RenderText(const Nt::HandleWindow& handle) {
			m_Text.Draw(handle);
		}

		void SetValue(const Nt::String& variableValue) {
			switch (m_Type) {
			case Control::TEXTEDIT:
				dynamic_cast<Nt::TextEdit*>(m_pHandle)->SetText(variableValue);
				break;

			case Control::CHECKBOX:
				dynamic_cast<Nt::Button*>(m_pHandle)->SetCheck(variableValue);
				break;
			}
		}

		Nt::IntRect GetRect() const {
			if (!m_IsCreated) {
				Raise("Control is not created.");
				return Nt::IntRect();
			}

			return m_pHandle->GetClientRect();
		}
		const Nt::Text& GetText() noexcept {
			return m_Text;
		}
		uInt GetType() const noexcept {
			return m_Type;
		}

	private:
		Nt::HandleWindow* m_pHandle;
		Nt::Text m_Text;

		std::string m_VariableName;
		std::string m_Name;

		Object* m_ObjectPtr;
		Types m_Type;
		uInt m_ID;
		Bool m_IsCreated = false;
	};

	enum TextEdits {
		TEXTEDIT_SCRIPT_PATH,
	};

	enum Buttons {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
	};

public:
	struct LanguageData : Language::_PropertyWindow::_Script {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertyScript(Scene* pScence) {
		SetScence(pScence);
	}
	~PropertyScript() override {
		ClearControls();
	}

	void Initialize(const Settings& settings) {
		SetLanguage(settings.CurrentLanguage);
		SetTheme(settings.Styles);

		m_Padding = { 10, 10, 20, 10 };

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;

		Create(windowRect, m_LanguageData.WindowName);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		m_ScriptText.SetPosition(m_Padding.LeftTop);
		m_ScriptText.SetColor(settings.Styles.Property.Texts.Color);
		m_ScriptText.SetWeight(settings.Styles.Property.Texts.Weight);
		m_ScriptText.SetText(m_LanguageData.Texts[LanguageData::TEXT_SCRIPT]);

		Nt::IntRect buttonRect = { };
		buttonRect.Top = m_Padding.Top;
		buttonRect.RightBottom = { 48, 25 };

		Nt::IntRect textEditRect = { };
		textEditRect.Left = 70;
		textEditRect.Top = buttonRect.Top;
		textEditRect.Right = m_ClientRect.Right - textEditRect.Left - buttonRect.Right - m_Padding.Right;
		textEditRect.Bottom = buttonRect.Bottom;

		m_ScriptPathTextEdit.SetParent(*this);
		m_ScriptPathTextEdit.SetID(TEXTEDIT_SCRIPT_PATH);
		m_ScriptPathTextEdit.AddStyles(ES_READONLY);
		m_ScriptPathTextEdit.Create(textEditRect, "No selected", true);
		m_ScriptPathTextEdit.SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
		m_ScriptPathTextEdit.SetTextColor(m_Style.Property.TextEdits.Text.Color);
		m_ScriptPathTextEdit.SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
		m_ScriptPathTextEdit.Show();

		buttonRect.Left = textEditRect.Left + textEditRect.Right;
		m_BrowseButton.SetParent(*this);
		m_BrowseButton.SetID(BUTTON_BROWSE);
		m_BrowseButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_BrowseButton.Create(buttonRect, "...");
		m_BrowseButton.Show();

		buttonRect.Left = m_Padding.Left;
		buttonRect.Top += buttonRect.Bottom + 10;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - m_Padding.Right;
		buttonRect.Bottom = 24;
		m_DeleteButton.SetParent(*this);
		m_DeleteButton.SetID(BUTTON_REMOVE);
		m_DeleteButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_DeleteButton.DisableWindow();
		m_DeleteButton.Create(buttonRect, "Remove script");
		m_DeleteButton.Show();

		windowRect.Bottom = buttonRect.Top + buttonRect.Bottom;
		windowRect.Bottom += m_Padding.Bottom;

		SetWindowRect(windowRect);
	}
	void Update() {
		if (!IsEnabled())
			return;

		if (m_SelectorPtr == nullptr) {
			Raise("Selector pointer is null");
			return;
		}

		if (!m_SelectorPtr->IsChanged())
			return;

		const uInt selectedObjectCount = m_SelectorPtr->GetObjectCount();
		if (selectedObjectCount == 1) {
			Object* pObject = m_SelectorPtr->GetObjectPtr(0);
			if (pObject == nullptr) {
				Raise("Null object selected");
				return;
			}

			const Script* pScript = pObject->GetScript();
			if (pScript != nullptr) {
				m_ScriptPathTextEdit.SetText(pScript->GetFilePath());
				m_DeleteButton.EnableWindow();

				_UpdateControls();
				return;
			}
		}

		ClearControls();

		m_ScriptPathTextEdit.SetText((selectedObjectCount > 1) ? "..." : "No selected");
		m_DeleteButton.DisableWindow();
	}

	void AddField(const Control::Types& type, const Nt::String& name, const Nt::String& variableName, const Nt::String& variableValue) {
		if (m_SelectorPtr->GetObjectCount() != 1)
			return;

		Nt::IntRect controlRect;
		controlRect.Left = m_Padding.Left;
		controlRect.Top = m_ClientRect.Bottom;
		controlRect.Right = m_ClientRect.Right - m_Padding.Right;

		Control* pControl = new Control(type, name, variableName);

		switch (type) {
		case Control::TEXTEDIT:
			controlRect.Bottom = 24;
			break;

		case Control::CHECKBOX:
			controlRect.Bottom = GetSystemMetrics(SM_CYMENUCHECK);
			break;
		}

		SetSize({ GetClientRect().Right, controlRect.Top + controlRect.Bottom + m_Padding.Bottom });;

		pControl->Create(m_Style, m_Controls.size(), controlRect, this, m_SelectorPtr->GetObjectContaiter()[0]);
		pControl->SetValue(variableValue);

		m_Controls.push_back(pControl);
	}
	void ClearControls() {
		for (Control* pControl : m_Controls)
			delete(pControl);

		m_Controls.clear();

		const Nt::IntRect buttonRect = m_DeleteButton.GetWindowRect();
		SetSize({ GetClientRect().Right, buttonRect.Top + m_Padding.Bottom });
	}

	void BrowseScript() {
		const std::string filePath = _Browse(L"Script (*.lua)\0*.lua\0All (*.*)\0*.*");

		if (filePath.empty())
			return;

		for (Object* pObject : m_SelectorPtr->GetObjectContaiter())
			pObject->AttachScript(m_pScence->GetLua(), m_pScence, filePath, { });

		_UpdateControls();

		m_ScriptPathTextEdit.SetText(filePath);
		m_DeleteButton.EnableWindow();
	}

	void SetTheme(const Style& style) {
		m_Style = style;

		SetBackgroundColor(m_Style.Property.BackgroundColor);

		m_ScriptText.SetColor(m_Style.Property.Texts.Color);
		m_ScriptText.SetWeight(m_Style.Property.Texts.Weight);

		m_ScriptPathTextEdit.SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
		m_ScriptPathTextEdit.SetTextColor(m_Style.Property.TextEdits.Text.Color);
		m_ScriptPathTextEdit.SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
	}
	void SetLanguage(const Language& language) {
		m_LanguageData = (LanguageData)language.PropertyWindow.Script;
		m_LanguageData.WindowName =
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_SCRIPTWINDOW];

		SetName(m_LanguageData.WindowName);

		m_ScriptText.SetText(m_LanguageData.Texts[LanguageData::TEXT_SCRIPT]);
	}
	void SetScence(Scene* pScence) {
		if (pScence == nullptr)
			Raise("Scence pointer is null.");
		m_pScence = pScence;
	}

private:
	std::vector<Control*> m_Controls;

	LanguageData m_LanguageData;
	Style m_Style;
	Scene* m_pScence;

	Nt::Text m_ScriptText;
	Nt::TextEdit m_ScriptPathTextEdit;
	Nt::Button m_BrowseButton;
	Nt::Button m_DeleteButton;
	Nt::IntRect m_Padding;

private:
	void _UpdateControls() {
		ClearControls();

		if (m_SelectorPtr != nullptr && m_SelectorPtr->GetObjectCount() == 1) {
			Object* pObject = m_SelectorPtr->GetObjectPtr(0);
			if (pObject == nullptr) {
				Raise("Null object selected");
				return;
			}

			Script* pScript = pObject->GetScript();
			if (pScript == nullptr)
				return;

			try {
				luabridge::LuaRef propertyField = pScript->GetGlobal("PropertyField");

				if (!propertyField.isNil() && propertyField.isFunction()) {
					const luabridge::LuaResult result = propertyField();

					if (result.hasFailed())
						throw std::exception(result.errorMessage().c_str());

					for (Script::Data& data : pScript->GetScriptData()) {
						const Control::Types type =
							(data.Type == Script::Data::BOOL) ? Control::CHECKBOX : Control::TEXTEDIT;

						AddField(type, data.FieldName, data.Name, data.Value);
					}
				}
			}
			catch (const std::exception& except) {
				ErrorBoxA(except.what(), "Error");
			}
		}
	}

	void _WMPaint([[maybe_unused]] HDC& hdc, [[maybe_unused]] PAINTSTRUCT& paint) override {
		m_ScriptText.Draw(*this);

		for (Control* pControl : m_Controls) {
			if (pControl->GetType() == Control::TEXTEDIT)
				pControl->RenderText(*this);
		}
	}
	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		if (m_SelectorPtr == nullptr)
			return;

		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		switch (command) {
		case BN_CLICKED:
			switch (id) {
			case BUTTON_BROWSE:
				BrowseScript();
				break;

			case BUTTON_REMOVE:
				m_ScriptPathTextEdit.SetText("No selected");

				for (Object* pObject : m_SelectorPtr->GetObjectContaiter()) {
					if (pObject == nullptr) {
						Raise("Null object selected");
						return;
					}

					pObject->RemoveScript();
				}

				m_DeleteButton.DisableWindow();
				break;

			default:
				m_Controls[id]->Update();
				break;
			}

			break;

		case EN_UPDATE:
			if (id != TEXTEDIT_SCRIPT_PATH)
				m_Controls[id]->Update();

			break;
		}
	}
};