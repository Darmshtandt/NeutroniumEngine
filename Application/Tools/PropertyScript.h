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

		Control(const Types& type, const std::string& name, const std::string& variableName, NotNull<Object*> pObject) :
			m_Type(type),
			m_Name(name),
			m_VariableName(variableName),
			m_ObjectPtr(pObject)
		{
		}

		~Control() {
			if (m_pHandle != nullptr)
				delete(m_pHandle);
		}

		void Create(const Style& style, const uInt& id, const Nt::IntRect& rect, NotNull<HandleWindow*> pParent) {
			if (m_IsCreated)
				Raise("Control already created.");

			switch (m_Type) {
			case TEXTEDIT:
				_CreateTextEdit(style, id, rect, pParent);
				break;

			case CHECKBOX:
				_CreateCheckBox(id, rect, pParent);
				break;

			default:
				Raise("Failed control type.");
			}

			m_pHandle->SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);

			m_IsCreated = true;
		}

		void Update() {
			NotNull<Script*> pScript = m_ObjectPtr->GetScript();

			switch (m_Type) {
			case TEXTEDIT: 
			{
				const NotNull<Nt::TextEdit*> pTextEdit = dynamic_cast<Nt::TextEdit*>(m_pHandle);
				pScript->SetDataValue(m_VariableName, pTextEdit->GetText());
			}
				break;

			case CHECKBOX:
			{
				const NotNull<Nt::Button*> pCheckbox = dynamic_cast<Nt::Button*>(m_pHandle);
				pScript->SetDataValue(m_VariableName, pCheckbox->IsChecked());
			}
				break;

			default:
				Raise("Failed control type.");
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

		_NODISCARD Nt::IntRect GetRect() const {
			if (!m_IsCreated)
				Raise("Control not created.");

			return m_pHandle->GetClientRect();
		}
		_NODISCARD const Nt::Text& GetText() noexcept {
			return m_Text;
		}
		_NODISCARD uInt GetType() const noexcept {
			return m_Type;
		}

	private:
		const Types m_Type;

		Nt::HandleWindow* m_pHandle = nullptr;
		Nt::Text m_Text;

		std::string m_VariableName;
		std::string m_Name;

		Object* m_ObjectPtr;
		uInt m_ID = 0;
		Bool m_IsCreated = false;

	private:
		void _CreateTextEdit(const Style& style, const uInt& id, const Nt::IntRect& rect, NotNull<HandleWindow*> pParent) {
			Nt::IntRect textRect = rect;
			textRect.Right /= 2;

			m_Text.SetColor(style["Property.Texts.Color"]);
			m_Text.SetWeight(style["Property.Texts.Weight"]);
			m_Text.SetText(m_Name);
			m_Text.SetPosition(textRect.LeftTop);

			Nt::IntRect textEditRect = rect;
			textEditRect.Left = textRect.Left + textRect.Right;
			textEditRect.Right -= textEditRect.Left;

			Nt::TextEdit* pTextEdit = new Nt::TextEdit;
			pTextEdit->SetID(id);
			pTextEdit->SetParent(*pParent);
			pTextEdit->Create(textEditRect, "", true);
			pTextEdit->SetTextColor(style["Property.TextEdits.Text.Color"]);
			pTextEdit->SetTextWeight(style["Property.TextEdits.Text.Weight"]);
			pTextEdit->Show();

			m_pHandle = pTextEdit;
		}

		void _CreateCheckBox(const uInt& id, const Nt::IntRect& rect, NotNull<HandleWindow*> pParent) {
			m_pHandle = new Nt::Button(*pParent, rect, id, m_Name);
		}
	};

private:
	enum Texts {
		TEXT_FILE_PATH,
		TEXT_COUNT
	};

	enum TextEdits {
		TEXTEDIT_SCRIPT_PATH,
		TEXTEDIT_COUNT
	};

	enum Buttons {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
		BUTTON_COUNT
	};

public:
	PropertyScript(NotNull<Selector*> pSelector, NotNull<Scene*> pScene) :
		PropertyComponent(pSelector, Texts::TEXT_COUNT, TEXTEDIT_COUNT, BUTTON_COUNT),
		m_pScene(pScene)
	{
	}

	~PropertyScript() override {
		ClearControls();
	}

	void Initialize(const Settings& settings) {
		m_Padding = { 10, 10, 20, 10 };
		
		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;

		_Create(settings, settings.Language["Window.Property.Script"], windowRect);

		Nt::IntRect buttonRect = { };
		buttonRect.Top = m_Padding.Top;
		buttonRect.RightBottom = { 48, 25 };

		Nt::IntRect textEditRect = { };
		textEditRect.Left = 70;
		textEditRect.Top = buttonRect.Top;
		textEditRect.Right = m_ClientRect.Right - textEditRect.Left - buttonRect.Right - m_Padding.Right;
		textEditRect.Bottom = buttonRect.Bottom;

		m_Texts[TEXT_FILE_PATH].SetPosition(m_Padding.LeftTop);

		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetParent(*this);
		m_TextEdits[TEXTEDIT_SCRIPT_PATH].AddStyles(ES_READONLY | STYLE_VISIBLE);
		m_TextEdits[TEXTEDIT_SCRIPT_PATH].Create(textEditRect, "No selected", true);

		auto setupButton = [&](Nt::Button& button, const Nt::IntRect& rect, const cString& text) {
			button.SetParent(*this);
			button.AddStyles(BS_CENTER | BS_VCENTER | STYLE_VISIBLE);
			button.Create(rect, text);
		};

		buttonRect.Left = textEditRect.Left + textEditRect.Right;

		setupButton(m_Buttons[BUTTON_BROWSE], buttonRect, "...");

		buttonRect.Left = m_Padding.Left;
		buttonRect.Top += buttonRect.Bottom + 10;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - m_Padding.Right;
		buttonRect.Bottom = 24;

		setupButton(m_Buttons[BUTTON_REMOVE], buttonRect, "Remove script");

		windowRect.Bottom = buttonRect.Top + buttonRect.Bottom;
		windowRect.Bottom += m_Padding.Bottom;

		SetWindowRect(windowRect);

		_DisableWindow();
	}
	void Update() {
		if (!IsEnabled())
			return;

		if (!m_SelectorPtr->IsChanged())
			return;

		const uInt selectedObjectCount = m_SelectorPtr->GetObjectCount();
		if (selectedObjectCount == 1) {
			const Script* pScript = m_SelectorPtr->GetObjectPtr(0)->GetScript();

			if (pScript != nullptr) {
				m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetText(pScript->GetFilePath());
				m_Buttons[BUTTON_REMOVE].EnableWindow();

				_UpdateControls();
				return;
			}
		}

		ClearControls();

		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetText((selectedObjectCount > 1) ? "..." : "No selected");
		m_Buttons[BUTTON_REMOVE].DisableWindow();
	}

	void AddField(const Control::Types& type, const Nt::String& name, const Nt::String& variableName, const Nt::String& variableValue) {
		if (m_SelectorPtr->GetObjectCount() != 1)
			return;

		Nt::IntRect controlRect;
		controlRect.Left = m_Padding.Left;
		controlRect.Top = m_ClientRect.Bottom;
		controlRect.Right = m_ClientRect.Right - m_Padding.Right;

		Object* pObject = m_SelectorPtr->GetObjectPtr(0);
		Control* pControl = new Control(type, name, variableName, pObject);

		switch (type) {
		case Control::TEXTEDIT:
			controlRect.Bottom = 24;
			break;

		case Control::CHECKBOX:
			controlRect.Bottom = GetSystemMetrics(SM_CYMENUCHECK);
			break;
		}

		SetSize({ GetClientRect().Right, controlRect.Top + controlRect.Bottom + m_Padding.Bottom });;


		const uInt id = (m_Controls.size() + 10);

		pControl->Create(m_Style, id, controlRect, this);
		pControl->SetValue(variableValue);

		m_Controls.push_back(pControl);
	}
	void ClearControls() {
		for (Control* pControl : m_Controls)
			delete(pControl);

		m_Controls.clear();

		const Nt::IntRect buttonRect = m_Buttons[BUTTON_REMOVE].GetWindowRect();
		SetSize({ GetClientRect().Right, buttonRect.Top + m_Padding.Bottom });
	}

	void BrowseScript() {
		const std::string filePath = _Browse(L"Script (*.lua)\0*.lua\0All (*.*)\0*.*");

		if (filePath.empty())
			return;

		for (Object* pObject : m_SelectorPtr->GetObjectContainer())
			pObject->AttachScript(m_pScene->GetLua(), filePath, { });

		_UpdateControls();

		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetText(filePath);
		m_Buttons[BUTTON_REMOVE].EnableWindow();
	}

	void SetScene(NotNull<Scene*> pScene) {
		m_pScene = pScene;
	}
	void SetTheme(const Style& style) override {
		m_Style = style;

		SetBackgroundColor(m_Style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_Texts[TEXT_FILE_PATH].SetColor(m_Style["Property.Texts.Color"]);
		m_Texts[TEXT_FILE_PATH].SetWeight(m_Style["Property.Texts.Weight"]);

		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetBackgroundColor(m_Style["Property.TextEdits.BackgroundColor"]);
		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetTextColor(m_Style["Property.TextEdits.Text.Color"]);
		m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetTextWeight(m_Style["Property.TextEdits.Text.Weight"]);
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.Script"]);

		m_Texts[TEXT_FILE_PATH].SetText(language["FilePath"]);
	}

private:
	std::vector<Control*> m_Controls;
	Nt::IntRect m_Padding;
	Style m_Style;
	Scene* m_pScene;
	inline static PropertyRegistrar<PropertyScript> m_Registrar { "Script" };

private:
	void _UpdateControls() {
		ClearControls();

		if (m_SelectorPtr->GetObjectCount() != 1)
			return;

		Script* pScript = m_SelectorPtr->GetObjectPtr(0)->GetScript();
		if (pScript == nullptr)
			return;

		try {
			luabridge::LuaRef propertyField = pScript->GetGlobal("PropertyField");

			if (propertyField.isNil() && (!propertyField.isFunction()))
				return;

			const luabridge::LuaResult result = propertyField();
			if (result.hasFailed())
				Raise(result.errorMessage());

			for (Script::Data& data : pScript->GetScriptData()) {
				const Control::Types type =
					(data.Type == Script::Data::BOOL) ? Control::CHECKBOX : Control::TEXTEDIT;

				AddField(type, data.FieldName, data.Name, data.Value);
			}
		}
		catch (const Nt::Error& error) {
			error.Show();
		}
	}

	void _Paint([[maybe_unused]] HDC& hdc, [[maybe_unused]] PAINTSTRUCT& paint) override {
		m_Texts[TEXT_FILE_PATH].Draw(*this);

		for (Control* pControl : m_Controls) {
			if (pControl->GetType() == Control::TEXTEDIT)
				pControl->RenderText(*this);
		}
	}

	void _ButtonsNotification_OnClick(const uInt& id, [[maybe_unused]] const HWND& handle) override {
		switch (id) {
		case BUTTON_BROWSE:
			BrowseScript();
			break;

		case BUTTON_REMOVE:
			m_TextEdits[TEXTEDIT_SCRIPT_PATH].SetText("No selected");

			for (Object* pObject : m_SelectorPtr->GetObjectContainer())
				pObject->RemoveScript();

			m_Buttons[BUTTON_REMOVE].DisableWindow();
			break;

		default:
			m_Controls[id]->Update();
			break;
		}
	}

	void _TextEditsNotification_Update(const uInt& id, [[maybe_unused]] const HWND& handle) override {
		if (id != TEXTEDIT_SCRIPT_PATH)
			m_Controls[id]->Update();
	}
};