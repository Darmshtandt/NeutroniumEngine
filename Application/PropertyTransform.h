#pragma once

class PropertyTransform : public PropertyComponent {
private:
	enum TextEditIDs {
		TEXTEDIT_X, TEXTEDIT_Y, TEXTEDIT_Z,
		TEXTEDIT_WIDTH, TEXTEDIT_HEIGHT, TEXTEDIT_LENGTH,
		TEXTEDIT_ROLL, TEXTEDIT_PITCH, TEXTEDIT_YAW,
		TEXTEDIT_COUNT
	};

public:
	struct LanguageData : Language::_PropertyWindow::_Transform {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertyTransform() noexcept :
		m_ContentLayout({ 2, 12 }),
		m_Texts(LanguageData::TEXT_COUNT),
		m_TextEdits(TEXTEDIT_COUNT)
	{
	}

	void Initialize(const Settings& settings) override {
		SetLanguage(settings.CurrentLanguage);
		SetTheme(settings.Styles);

		constexpr Nt::FloatRect headerTextPadding(0.f, 5.f, 0.f, 0.f);
		constexpr Nt::FloatRect nonheaderTextPadding(10.f, 0.f, 0.f, 0.f);

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right - GetSystemMetrics(SM_CXDLGFRAME) * 2;
		windowRect.Bottom = Nt::TextEdit::DefaultSize.y + Int(headerTextPadding.Top + headerTextPadding.Bottom) / 3;
		windowRect.Bottom *= LanguageData::TEXT_COUNT;

		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		Create(windowRect, m_LanguageData.WindowName);
		SetBackgroundColor(settings.Styles.Property.BackgroundColor);

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ContentLayout.SetBackgroundColor(settings.Styles.Property.BackgroundColor);
		m_ContentLayout.Create(m_ClientRect, "Transform-layout");
		m_ContentLayout.SetPadding({ 10, 5, 10, 10 }, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.Show();

		uInt textEditID = 0;
		for (uInt i = 0; i < LanguageData::TEXT_COUNT; ++i) {
			m_Texts[i].DisableDefaultRectSize();
			m_Texts[i].SetText(m_LanguageData.Texts[i]);

			const Bool isHeader = (i % 4 == 0);

			if (isHeader) {
				m_Texts[i].SetColor(settings.Styles.Property.HeaderTexts.Color);
				m_Texts[i].SetWeight(settings.Styles.Property.HeaderTexts.Weight);

				m_ContentLayout[0][i].SetPadding(headerTextPadding, Nt::UnitType::UNIT_PIXEL);
			}
			else {
				m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
				m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);

				Nt::TextEdit* textEditPtr = &m_TextEdits[textEditID];

				m_ContentLayout[0][i].SetPadding(nonheaderTextPadding, Nt::UnitType::UNIT_PIXEL);
				m_ContentLayout.Insert(Nt::uInt2D(1, i), textEditPtr);

				textEditPtr->SetID(textEditID);
				textEditPtr->Create("0.0", true);
				textEditPtr->SetBackgroundColor(settings.Styles.Property.TextEdits.BackgroundColor);
				textEditPtr->SetTextColor(settings.Styles.Property.TextEdits.Text.Color);
				textEditPtr->SetTextWeight(settings.Styles.Property.TextEdits.Text.Weight);
				textEditPtr->DisableWindow();
				textEditPtr->Show();

				++textEditID;
			}

			m_ContentLayout.Insert({ 0, i }, &m_Texts[i]);
		}
	}

	void Update() override {
		static Bool prevActiveState = false;
		const Bool isSelectedOnlyOne = (m_SelectorPtr->GetObjectCount() == 1);

		if ((!prevActiveState) && isSelectedOnlyOne) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.EnableWindow();

			prevActiveState = true;
		}
		else if (prevActiveState && (!isSelectedOnlyOne)) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.DisableWindow();

			prevActiveState = false;
		}

		_UpdateTextEdits();
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style.Property.BackgroundColor);
		m_ContentLayout.SetBackgroundColor(style.Property.BackgroundColor);

		for (uInt i = 0; i < m_Texts.size(); ++i) {
			const Bool isHeader = (i % 4 == 0);

			if (isHeader) {
				m_Texts[i].SetColor(style.Property.HeaderTexts.Color);
				m_Texts[i].SetWeight(style.Property.HeaderTexts.Weight);
			}
			else {
				m_Texts[i].SetColor(style.Property.Texts.Color);
				m_Texts[i].SetWeight(style.Property.Texts.Weight);
			}
		}

		for (uInt i = 0; i < m_TextEdits.size(); ++i) {
			m_TextEdits[i].SetBackgroundColor(style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(style.Property.TextEdits.Text.Weight);
		}
	}
	void SetLanguage(const Language& language) override {
		m_LanguageData = (LanguageData)language.PropertyWindow.Transform;
		m_LanguageData.WindowName = 
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_TRANSFORMWINDOW];

		SetName(m_LanguageData.WindowName);

		for (uInt i = 0; i < LanguageData::TEXT_COUNT; ++i)
			m_Texts[i].SetText(m_LanguageData.Texts[i]);
	}

private:
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Text> m_Texts;
	Nt::GridLayout m_ContentLayout;
	LanguageData m_LanguageData;

private:
	void _WMCommand(const Long& param_1, const Long& param_2) override {
		const uInt command = param_1;
		const HWND hwnd = reinterpret_cast<HWND>(param_2);

		if (command == EN_UPDATE && hwnd != nullptr && m_SelectorPtr->GetObjectCount() == 1) {
			std::wstring wText(GetWindowTextLength(hwnd), L'\0');
			GetWindowText(hwnd, wText.data(), wText.length() + 1);

			const uInt id = GetWindowLongPtr(hwnd, GWLP_ID);

			const Bool isPosition = (id / 3 == 0);
			const Bool isSize = (id / 3 == 1);
			const Bool isAngle = (id / 3 == 2);

			const uInt coordinate = (id % 3);

			try {
				const Float value = Nt::String(wText);

				Object* pObject = m_SelectorPtr->GetObjectPtr(0);

				if (pObject == nullptr) {
					Raise("Null object selected");
					return;
				}

				if (isPosition) {
					Nt::Float3D position = pObject->GetPosition();
					position[coordinate] = value;
					pObject->SetPosition(position);
				}
				else if (isSize) {
					Nt::Float3D size = pObject->GetSize();
					size[coordinate] = value;
					pObject->SetSize(size);
				}
				else if (isAngle) {
					Nt::Float3D angle = pObject->GetAngle();
					angle[coordinate] = value;
					pObject->SetAngle(angle);
				}

				pObject->UnmarkChanged();
			}
			catch (...)
			{
			}
		}
	}

	void _UpdateTextEdits() {
		if (m_SelectorPtr->GetObjectCount() != 1)
			return;

		Object* pObject = m_SelectorPtr->GetObjectPtr(0);
		if (pObject == nullptr) {
			Raise("Null object selected");
			return;
		}

		if ((!m_SelectorPtr->IsChanged()) && (!pObject->IsChanged()))
			return;

		for (uInt i = 0; i < m_TextEdits.size(); ++i) {
			const Bool isPosition = (i / 3 == 0);
			const Bool isSize = (i / 3 == 1);
			const Bool isAngle = (i / 3 == 2);

			const uInt coordinate = (i % 3);

			Float value = 0.f;

			if (isPosition)
				value = pObject->GetPosition()[coordinate];
			else if (isSize)
				value = pObject->GetSize()[coordinate];
			else if (isAngle)
				value = pObject->GetAngle()[coordinate];

			m_TextEdits[i].SetText(value);
		}

		pObject->UnmarkChanged();
	}
};