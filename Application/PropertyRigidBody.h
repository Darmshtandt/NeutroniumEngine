#pragma once

class PropertyRigidBody : public PropertyComponent {
public:
	enum Buttons {
		BUTTON_ACTIVE,
		BUTTON_COLLISION,
		BUTTON_GRAVITATION,
		BUTTON_SHOW_COLLIDER,
		BUTTON_COUNT,
	};
	enum TextEdits {
		TEXTEDIT_MASS,
		//TEXTEDIT_GRAVITY_DIRECTION,
		TEXTEDIT_FRICTION,
		TEXTEDIT_COUNT,
	};
	enum Texts {
		TEXT_MASS,
		//TEXT_GRAVITY_DIRECTION,
		TEXT_FRICTION,
		TEXT_COUNT,
	};

	const uInt ID = 5000;

public:
	PropertyRigidBody() :
		m_Buttons(BUTTON_COUNT),
		m_TextEdits(TEXTEDIT_COUNT),
		m_Texts(TEXT_COUNT)
	{
	}

	void Initialize(const Settings& settings) override {
		m_Language = settings.CurrentLanguage;
		m_Style = settings.Styles;
		m_PaddingRect = { 5, 5, 10, 10 };

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = 200;

		Create(windowRect, m_Language.Window.PropertyComponent.RigidBodyWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		const std::wstring buttonTexts[] = {
			m_Language.Window.PropertyRigidBody.Active,
			m_Language.Window.PropertyRigidBody.EnableCollision,
			m_Language.Window.PropertyRigidBody.EnableGravitation,
			m_Language.Window.PropertyRigidBody.ShowCollider,
		};

		Nt::IntRect buttonRect;
		buttonRect.LeftTop = m_PaddingRect.LeftTop;
		buttonRect.Right = (m_ClientRect.Right - m_PaddingRect.Right) / 2;
		buttonRect.Bottom = Float(GetSystemMetrics(SM_CYMENUCHECK)) * 1.5f;

		for (uInt i = 0; i < BUTTON_COUNT; ++i) {
			m_Buttons[i].SetID(ID + BUTTON_ACTIVE + i);
			m_Buttons[i].SetParent(*this);
			m_Buttons[i].AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
			m_Buttons[i].Create(buttonRect, buttonTexts[i]);

			SetBkMode(m_Buttons[i].GetDC(), TRANSPARENT);

			if (i % 2 == 1) {
				buttonRect.Left = m_PaddingRect.Left;
				buttonRect.Top += buttonRect.Bottom;
			}
			else {
				buttonRect.Left = buttonRect.Right;
			}
		}
		
		const std::wstring texts[] = {
			m_Language.Window.PropertyRigidBody.Mass,
			//m_Language.Window.PropertyRigidBody.GravityDirection,
			m_Language.Window.PropertyRigidBody.Friction
		};

		Nt::IntRect textEditRect;
		textEditRect.Left = buttonRect.Right;
		textEditRect.Top = buttonRect.Top + buttonRect.Bottom + 10;
		textEditRect.Right = buttonRect.Right;
		textEditRect.Bottom = 24;

		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			m_TextEdits[i].SetParent(*this);
			m_TextEdits[i].SetID(ID + TEXTEDIT_MASS + i);
			m_TextEdits[i].SetBackgroundColor(settings.Styles.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(settings.Styles.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(settings.Styles.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Create(textEditRect, "0.0", true);
			m_TextEdits[i].Show();

			m_Texts[i].SetText(texts[i]);
			m_Texts[i].SetPosition({ m_PaddingRect.Left, textEditRect.Top });
			m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
			m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);

			textEditRect.Top += textEditRect.Bottom;
		}

		windowRect.Bottom = textEditRect.Top;
		windowRect.Bottom += m_PaddingRect.Bottom;
		SetSize(windowRect.RightBottom);
	}
	void Update() override {
		if (!IsEnabled())
			return;
		if (!m_SelectorPtr)
			Raise("Selector pointer is nullptr");

		if (m_SelectorPtr->GetObjects().size() == 1) {
			_EnableWindow();
			if (m_SelectorPtr->IsChanged()) {
				const Object* pObject = m_SelectorPtr->GetObjects()[0];
				m_TextEdits[TEXTEDIT_MASS].SetText(pObject->GetMass());
				m_TextEdits[TEXTEDIT_FRICTION].SetText(pObject->GetFriction());

				Button_SetCheck(m_Buttons[BUTTON_ACTIVE].GetHandle(), pObject->IsPhysicsEnabled());
				Button_SetCheck(m_Buttons[BUTTON_COLLISION].GetHandle(), pObject->IsEnabledCollision());
				Button_SetCheck(m_Buttons[BUTTON_GRAVITATION].GetHandle(), pObject->IsEnabledGravitation());
				Button_SetCheck(m_Buttons[BUTTON_SHOW_COLLIDER].GetHandle(), pObject->IsShowingCollider());
			}
		}
		else {
			_DisableWindow();
		}
	}

	void SetTheme(const Style& style) override {
		m_Style = style;

		SetBackgroundColor(m_Style.Property.BackgroundColor);
		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);

			m_Texts[i].SetColor(m_Style.Property.Texts.Color);
			m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);
		}
	}
	void SetLanguage(const Language& language) override {
		m_Language = language;

		m_Buttons[BUTTON_ACTIVE].SetName(m_Language.Window.PropertyRigidBody.Active);
		m_Buttons[BUTTON_COLLISION].SetName(m_Language.Window.PropertyRigidBody.EnableCollision);
		m_Buttons[BUTTON_GRAVITATION].SetName(m_Language.Window.PropertyRigidBody.EnableGravitation);
		m_Buttons[BUTTON_SHOW_COLLIDER].SetName(m_Language.Window.PropertyRigidBody.ShowCollider);

		m_Texts[TEXT_FRICTION].SetText(m_Language.Window.PropertyRigidBody.Friction);
		m_Texts[TEXT_MASS].SetText(m_Language.Window.PropertyRigidBody.Mass);
	}

private:
	std::vector<Nt::Button> m_Buttons;
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Text> m_Texts;
	Nt::IntRect m_PaddingRect;
	Language m_Language;
	Style m_Style;

private:
	void _EnableWindow() {
		if (!m_IsWindowEnabled) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.EnableWindow();
			for (Nt::Button& Button : m_Buttons)
				Button.EnableWindow();
			m_IsWindowEnabled = true;
		}
	}
	void _DisableWindow() {
		if (m_IsWindowEnabled) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.DisableWindow();
			for (Nt::Button& Button : m_Buttons)
				Button.DisableWindow();
			m_IsWindowEnabled = false;
		}
	}

	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		try {
			if (m_SelectorPtr == nullptr)
				return;

			for (Object* pObject : m_SelectorPtr->GetObjects()) {
				if (pObject == nullptr)
					Raise("Object pointer is null.");
				if (pObject->ObjectType != ObjectTypes::PRIMITIVE)
					continue;

				switch (command) {
				case BN_CLICKED:
					if (m_SelectorPtr != nullptr) {
						const Bool isChecked = IsDlgButtonChecked(m_hwnd, id);
						switch (id - ID) {
						case BUTTON_ACTIVE:
							if (isChecked)
								pObject->EnablePhysics();
							else
								pObject->DisablePhysics();
							break;
						case BUTTON_COLLISION:
							if (isChecked)
								pObject->EnableCollider();
							else
								pObject->DisableCollider();
							break;
						case BUTTON_GRAVITATION:
							if (isChecked)
								pObject->EnableGravitation();
							else
								pObject->DisableGravitation();
							break;
						case BUTTON_SHOW_COLLIDER:
							if (isChecked)
								pObject->ShowingCollider();
							else
								pObject->HidingCollider();
							break;
						}
					}
					break;
				case EN_UPDATE:
					switch (id - ID) {
					case TEXTEDIT_MASS:
						pObject->SetMass(m_TextEdits[id - ID].GetText());
						break;
					//case TEXTEDIT_GRAVITY_DIRECTION:
						break;
					case TEXTEDIT_FRICTION:
						pObject->SetFriction(m_TextEdits[id - ID].GetText());
						break;
					}
					break;
				}
			}
		}
		catch (...)
		{
		}
	}
};