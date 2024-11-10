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

		constexpr Nt::FloatRect HeaderTextPadding(0.f, 5.f, 0.f, 0.f);
		constexpr Nt::FloatRect nonHeaderTextPadding(10.f, 0.f, 0.f, 0.f);

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right - GetSystemMetrics(SM_CXDLGFRAME) * 2;
		windowRect.Bottom = Nt::TextEdit::DefaultSize.y + Int(HeaderTextPadding.Top + HeaderTextPadding.Bottom) / 3;
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

				m_ContentLayout[0][i].SetPadding(HeaderTextPadding, Nt::UnitType::UNIT_PIXEL);
			}
			else {
				m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
				m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);

				Nt::TextEdit* textEditPtr = &m_TextEdits[textEditID];

				m_ContentLayout[0][i].SetPadding(nonHeaderTextPadding, Nt::UnitType::UNIT_PIXEL);
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
		const Bool isOneSelect = (m_SelectorPtr->GetObjects().size() == 1);
		if ((!prevActiveState) && isOneSelect) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.EnableWindow();
			prevActiveState = true;
		}
		else if (prevActiveState && (!isOneSelect)) {
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
	LanguageData m_LanguageData;
	Nt::GridLayout m_ContentLayout;
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Text> m_Texts;

private:
	void _WMCommand(const Long& param_1, const Long& param_2) override {
		if (HIWORD(param_1) == EN_UPDATE && param_2 != 0 && m_SelectorPtr->GetObjects().size() == 1) {
			const HWND hwnd = (HWND)param_2;

			std::wstring wText(GetWindowTextLength(hwnd), L'\0');
			GetWindowText(hwnd, wText.data(), wText.length() + 1);

			const uInt id = GetWindowLongPtr(hwnd, GWLP_ID);
			const Bool isPosition = (id / 3 == 0);
			const Bool isSize = (id / 3 == 1);
			const Bool isAngle = (id / 3 == 2);

			const uInt coordinate = id % 3;
			try {
				const Float value = Nt::String(wText);

				Object* objectPtr = m_SelectorPtr->GetObjects()[0];
				if (!objectPtr)
					Raise("Selected object is nullptr");

				if (isPosition) {
					Nt::Float3D position = objectPtr->GetPosition();
					position[coordinate] = value;
					objectPtr->SetPosition(position);
				}
				else if (isSize) {
					Nt::Float3D size = objectPtr->GetSize();
					size[coordinate] = value;
					objectPtr->SetSize(size);
				}
				else if (isAngle) {
					Nt::Float3D angle = objectPtr->GetAngle();
					angle[coordinate] = value;
					objectPtr->SetAngle(angle);
				}
				objectPtr->UnmarkChanged();
			}
			catch (...)
			{
			}
		}
	}

	void _UpdateTextEdits() {
		Object* pSelectedObjectPtr = nullptr;
		if (m_SelectorPtr->GetObjects().size() == 1)
			pSelectedObjectPtr = m_SelectorPtr->GetObjects()[0];

		if (pSelectedObjectPtr && (m_SelectorPtr->IsChanged() || pSelectedObjectPtr->IsChanged())) {
			pSelectedObjectPtr = m_SelectorPtr->GetObjects()[0];
			for (uInt i = 0; i < m_TextEdits.size(); ++i) {
				const Bool isPosition = (i / 3 == 0);
				const Bool isSize = (i / 3 == 1);
				const Bool isAngle = (i / 3 == 2);

				const uInt coordinate = (i % 3);
				Float value;
				if (isPosition)
					value = pSelectedObjectPtr->GetPosition()[coordinate];
				else if (isSize)
					value = pSelectedObjectPtr->GetSize()[coordinate];
				else if (isAngle)
					value = pSelectedObjectPtr->GetAngle()[coordinate];

				m_TextEdits[i].SetText(value);
			}
			pSelectedObjectPtr->UnmarkChanged();
		}
	}
};