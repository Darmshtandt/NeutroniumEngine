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
		TEXTEDIT_GRAVITY_DIRECTION,
		TEXTEDIT_FRICTION,
		TEXTEDIT_COUNT,
	};

	const uInt ID = 5000;

public:
	struct LanguageData : Language::_PropertyWindow::_RigidBody {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertyRigidBody() :
		m_ContentLayout(2),
		m_ButtonsLayout({ 2, 2 }),
		m_ParametersLayout({ 2, 3 }),
		m_Buttons(BUTTON_COUNT),
		m_TextEdits(TEXTEDIT_COUNT),
		m_Texts(Int(LanguageData::TEXT_COUNT) - Int(BUTTON_COUNT))
	{
	}

	void Initialize(const Settings& settings) override {
		SetLanguage(settings.CurrentLanguage);
		m_Style = settings.Styles;

		constexpr Nt::IntRect contentPadding = { 10, 10, 10, 10 };
		constexpr Nt::Int contentGap = 10;
		const Int cellsHeight = Nt::TextEdit::DefaultSize.y;

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = (contentPadding.Top + contentPadding.Bottom);
		windowRect.Bottom += (cellsHeight * Int(std::roundf(Float(BUTTON_COUNT) / 2.f)));
		windowRect.Bottom += contentGap + (cellsHeight * TEXTEDIT_COUNT);

		Create(windowRect, m_LanguageData.WindowName);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		const Nt::IntRect contentLayoutRect = {
			windowRect.Left, 
			windowRect.Top,
			windowRect.Right - GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXDLGFRAME) * 2,
			windowRect.Bottom - GetSystemMetrics(SM_CYBORDER) * 2
		};

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_ContentLayout.TogleVertical(true);
		m_ContentLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(contentLayoutRect, "RigidBody-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetGap(contentGap, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(0, 0.5f, Nt::UnitType::UNIT_PERCENTAGE);
		m_ContentLayout.Show();

		m_ContentLayout.Insert(0, &m_ButtonsLayout);
		m_ButtonsLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ButtonsLayout.SetBackgroundColor(settings.Styles.Property.BackgroundColor);
		m_ButtonsLayout.Create("RigidBody-Buttons-layout");
		m_ButtonsLayout.Show();

		m_ContentLayout.Insert(1, &m_ParametersLayout);
		m_ParametersLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ParametersLayout.SetBackgroundColor(settings.Styles.Property.BackgroundColor);
		m_ParametersLayout.Create("RigidBody-Parameters-layout");
		m_ParametersLayout.Show();

		for (uInt i = 0; i < BUTTON_COUNT; ++i) {
			m_ButtonsLayout.Insert({ i % 2, i / 2 }, &m_Buttons[i]);
			m_Buttons[i].SetID(ID + BUTTON_ACTIVE + i);
			m_Buttons[i].AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
			m_Buttons[i].Create(m_LanguageData.Texts[i]);
		}

		for (uInt i = 0; i < Int(LanguageData::TEXT_COUNT) - Int(BUTTON_COUNT); ++i) {
			m_ParametersLayout.Insert({ 0, i }, &m_Texts[i]);
			m_Texts[i].SetText(m_LanguageData.Texts[i + BUTTON_COUNT]);
			m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
			m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);

			m_ParametersLayout.Insert({ 1, i }, &m_TextEdits[i]);
			m_TextEdits[i].SetID(ID + TEXTEDIT_MASS + i);
			m_TextEdits[i].SetBackgroundColor(settings.Styles.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(settings.Styles.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(settings.Styles.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Create("0.0", true);
			m_TextEdits[i].Show();
		}
	}
	void Update() override {
		if (!IsEnabled())
			return;

		if (m_SelectorPtr == nullptr) {
			Raise("Selector pointer is nullptr");
			return;
		}

		if (m_SelectorPtr->GetObjectCount() != 1) {
			_DisableWindow();
			return;
		}

		_EnableWindow();

		if (m_SelectorPtr->IsChanged()) {
			const Object* pObject = m_SelectorPtr->GetObjectPtr(0);

			m_TextEdits[TEXTEDIT_MASS].SetText(pObject->GetMass());
			m_TextEdits[TEXTEDIT_FRICTION].SetText(pObject->GetFriction());

			m_Buttons[BUTTON_ACTIVE].SetCheck(pObject->IsPhysicsEnabled());
			m_Buttons[BUTTON_COLLISION].SetCheck(pObject->IsEnabledCollision());
			m_Buttons[BUTTON_GRAVITATION].SetCheck(pObject->IsEnabledGravitation());
			m_Buttons[BUTTON_SHOW_COLLIDER].SetCheck(pObject->IsShowingCollider());
		}
	}

	void SetTheme(const Style& style) override {
		m_Style = style;
		
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		for (uInt i = 0; i < Int(LanguageData::TEXT_COUNT) - Int(BUTTON_COUNT); ++i) {
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);

			m_Texts[i].SetColor(m_Style.Property.Texts.Color);
			m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);
		}
	}
	void SetLanguage(const Language& language) override {
		m_LanguageData = (LanguageData)language.PropertyWindow.RigidBody;
		m_LanguageData.WindowName =
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_RIGIDBODYWINDOW];

		SetName(m_LanguageData.WindowName);

		m_Buttons[BUTTON_ACTIVE].SetName(m_LanguageData.Texts[LanguageData::TEXT_ACTIVE]);
		m_Buttons[BUTTON_COLLISION].SetName(m_LanguageData.Texts[LanguageData::TEXT_ENABLECOLLISION]);
		m_Buttons[BUTTON_GRAVITATION].SetName(m_LanguageData.Texts[LanguageData::TEXT_ENABLEGRAVITATION]);
		m_Buttons[BUTTON_SHOW_COLLIDER].SetName(m_LanguageData.Texts[LanguageData::TEXT_SHOWCOLLIDER]);

		m_Texts[Int(LanguageData::TEXT_FRICTION) - Int(BUTTON_COUNT)].SetText(m_LanguageData.Texts[Int(LanguageData::TEXT_FRICTION) - Int(BUTTON_COUNT)]);
		m_Texts[Int(LanguageData::TEXT_MASS) - Int(BUTTON_COUNT)].SetText(m_LanguageData.Texts[Int(LanguageData::TEXT_MASS) - Int(BUTTON_COUNT)]);
	}

private:
	Nt::BoxLayout m_ContentLayout;
	Nt::GridLayout m_ButtonsLayout;
	Nt::GridLayout m_ParametersLayout;
	std::vector<Nt::Button> m_Buttons;
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Text> m_Texts;
	LanguageData m_LanguageData;
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

	void _WMPaint([[maybe_unused]] HDC& hdc, [[maybe_unused]] PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		if (m_SelectorPtr == nullptr)
			return;

		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		try {
			for (Object* pObject : m_SelectorPtr->GetObjectContaiter()) {
				if (pObject == nullptr) {
					Raise("Object pointer is null.");
					return;
				}

				switch (command) {
				case BN_CLICKED:
					switch (id - ID) {
					case BUTTON_ACTIVE:
						if (m_Buttons[BUTTON_ACTIVE].IsChecked())
							pObject->EnablePhysics();
						else
							pObject->DisablePhysics();
						break;

					case BUTTON_COLLISION:
						if (m_Buttons[BUTTON_COLLISION].IsChecked())
							pObject->EnableCollider();
						else
							pObject->DisableCollider();
						break;

					case BUTTON_GRAVITATION:
						if (m_Buttons[BUTTON_GRAVITATION].IsChecked())
							pObject->EnableGravitation();
						else
							pObject->DisableGravitation();
						break;

					case BUTTON_SHOW_COLLIDER:
						if (m_Buttons[BUTTON_SHOW_COLLIDER].IsChecked())
							pObject->ShowingCollider();
						else
							pObject->HidingCollider();
						break;
					}

					break;

				case EN_UPDATE:
					switch (id - ID) {
					case TEXTEDIT_MASS:
						pObject->SetMass(m_TextEdits[id - ID].GetText());
						break;

					case TEXTEDIT_GRAVITY_DIRECTION:
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