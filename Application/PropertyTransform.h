#pragma once

class PropertyTransform : public PropertyComponent {
private:
	enum TextIDs {
		TEXT_POSITION, TEXT_X, TEXT_Y, TEXT_Z,
		TEXT_SIZE, TEXT_WIDTH, TEXT_HEIGHT, TEXT_LENGTH,
		TEXT_ANGLE, TEXT_ROLL, TEXT_PITCH, TEXT_YAW,
		TEXT_COUNT
	};
	enum TextEditIDs {
		TEXTEDIT_X, TEXTEDIT_Y, TEXTEDIT_Z,
		TEXTEDIT_WIDTH, TEXTEDIT_HEIGHT, TEXTEDIT_LENGTH,
		TEXTEDIT_ROLL, TEXTEDIT_PITCH, TEXTEDIT_YAW,
		TEXTEDIT_COUNT
	};

public:
	PropertyTransform() noexcept :
		m_MainLayout({ 2, 3 }),
		m_LayoutPosition(3),
		m_LayoutSize(3),
		m_LayoutRotation(3)
	{
	}

	void Initialize(const Settings& settings) override {
		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = 500;

		Create(windowRect, settings.CurrentLanguage.Window.PropertyComponent.TransformWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		//SetBackgroundColor(settings.Styles.Property.BackgroundColor);

		m_MainLayout.SetBackgroundColor(Nt::Float3D(Nt::Colors::Black));
		m_MainLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_MainLayout.Create(windowRect, "");
		m_MainLayout.SetParent(*this);
		m_MainLayout.SetGap({ 0, 10 }, Nt::UnitType::UNIT_PIXEL);
		m_MainLayout.Show();

		const std::wstring texts[] = {
			settings.CurrentLanguage.PropertyWindow.Position.wstr().c_str(),
			L"X", L"Y", L"Z",
			settings.CurrentLanguage.PropertyWindow.Size.wstr().c_str(),
			settings.CurrentLanguage.PropertyWindow.Size_Width.wstr().c_str(),
			settings.CurrentLanguage.PropertyWindow.Size_Height.wstr().c_str(),
			settings.CurrentLanguage.PropertyWindow.Size_Length.wstr().c_str(),
			settings.CurrentLanguage.PropertyWindow.Angle.wstr().c_str(),
			L"R", L"P", L"Y"
		};

		m_Texts.resize(TEXT_COUNT);
		for (uInt i = 0; i < m_Texts.size(); ++i)
			m_Texts[i].SetText(texts[i]);

		m_LayoutPosition.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_LayoutSize.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_LayoutRotation.RemoveStyles(WS_OVERLAPPEDWINDOW);

		m_LayoutPosition.Create("");
		m_LayoutSize.Create("");
		m_LayoutRotation.Create("");

		m_MainLayout.Insert({ 1, 0 }, &m_LayoutPosition);
		m_MainLayout.Insert({ 1, 1 }, &m_LayoutSize);
		m_MainLayout.Insert({ 1, 2 }, &m_LayoutRotation);

		m_LayoutPosition.UpdateContent();
		m_LayoutSize.UpdateContent();
		m_LayoutRotation.UpdateContent();

		m_LayoutPosition.TogleVertical(true);
		m_LayoutSize.TogleVertical(true);
		m_LayoutRotation.TogleVertical(true);

		m_TextEdits.resize(TEXTEDIT_COUNT);
		for (uInt i = 0; i < 3; ++i) {
			for (uInt j = 0; j < 3; ++j) {
				const uInt ttextEditIndex = (i * 3 + j);

				Nt::TextEdit& textEdit = m_TextEdits[ttextEditIndex];
				textEdit.SetID(i - i / 4);
				textEdit.Create("0.0", true);
				textEdit.SetBackgroundColor(settings.Styles.Property.TextEdits.BackgroundColor);
				textEdit.SetTextColor(settings.Styles.Property.TextEdits.Text.Color);
				textEdit.SetTextWeight(settings.Styles.Property.TextEdits.Text.Weight);
				textEdit.DisableWindow();

				const uInt cellIndex = (i + j) % 3;

				switch (i) {
				case 0:
					m_LayoutPosition.Insert(cellIndex, &textEdit);
					break;
				case 1:
					m_LayoutSize.Insert(cellIndex, &textEdit);
					break;
				case 2:
					m_LayoutRotation.Insert(cellIndex, &textEdit);
					break;
				}

				textEdit.Show();
			}
		}

		m_LayoutPosition.Show();
		m_LayoutSize.Show();
		m_LayoutRotation.Show();
		return;


		//const std::wstring texts[] = {
		//	settings.CurrentLanguage.PropertyWindow.Position.wstr().c_str(),
		//	L"X", L"Y", L"Z",
		//	settings.CurrentLanguage.PropertyWindow.Size.wstr().c_str(),
		//	settings.CurrentLanguage.PropertyWindow.Size_Width.wstr().c_str(),
		//	settings.CurrentLanguage.PropertyWindow.Size_Height.wstr().c_str(),
		//	settings.CurrentLanguage.PropertyWindow.Size_Length.wstr().c_str(),
		//	settings.CurrentLanguage.PropertyWindow.Angle.wstr().c_str(),
		//	L"R", L"P", L"Y"
		//};

		Nt::Int2D textPosition = { 10, 5 };
		Nt::IntRect textEditRect = {
			70,
			0,
			m_ClientRect.Right - 70 * 2,
			25
		};
		const Int lineIndent = 25;

		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			const Bool isHeader = (i % 4 == 0);
			if (i > 0) {
				const Int mult = (isHeader) ? 2 : 1;

				textEditRect.Top += lineIndent * mult;
				textPosition.y += lineIndent * mult;
			}

			Nt::Text text(textPosition, texts[i]);
			text.DisableBackground();

			if (isHeader) {
				text.SetColor(settings.Styles.Property.HeaderTexts.Color);
				text.SetWeight(settings.Styles.Property.HeaderTexts.Weight);
			}
			else {
				text.SetColor(settings.Styles.Property.Texts.Color);
				text.SetWeight(settings.Styles.Property.Texts.Weight);

				Nt::TextEdit& textEdit = m_TextEdits[i - i / 4 - 1];
				textEdit.SetParent(*this);
				textEdit.SetID(i - i / 4);
				textEdit.Create(textEditRect, "0.0", true);
				textEdit.SetBackgroundColor(settings.Styles.Property.TextEdits.BackgroundColor);
				textEdit.SetTextColor(settings.Styles.Property.TextEdits.Text.Color);
				textEdit.SetTextWeight(settings.Styles.Property.TextEdits.Text.Weight);
				textEdit.DisableWindow();
				textEdit.Show();
			}
			m_Texts[i] = text;
		}

		windowRect.Bottom = textPosition.y + 20 + lineIndent / 2;
		SetWindowRect(windowRect);
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
		SetName(language.Window.PropertyWindow);

		const std::wstring texts[] = {
			language.PropertyWindow.Position.wstr().c_str(),
			L"X", L"Y", L"Z",
			language.PropertyWindow.Size.wstr().c_str(),
			language.PropertyWindow.Size_Width.wstr().c_str(),
			language.PropertyWindow.Size_Height.wstr().c_str(),
			language.PropertyWindow.Size_Length.wstr().c_str(),
			language.PropertyWindow.Angle.wstr().c_str(),
			L"R", L"P", L"Y"
		};
		for (uInt i = 0; i < TEXT_COUNT; ++i)
			m_Texts[i].SetText(texts[i]);
	}

private:
	Nt::BoxLayout m_LayoutPosition;
	Nt::BoxLayout m_LayoutSize;
	Nt::BoxLayout m_LayoutRotation;
	Nt::GridLayout m_MainLayout;
	std::vector<Nt::TextEdit> m_TextEdits;
	std::vector<Nt::Text> m_Texts;

private:
	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
		if (HIWORD(param_1) == EN_UPDATE && param_2 != 0 && m_SelectorPtr->GetObjects().size() == 1) {
			const HWND hwnd = (HWND)param_2;

			std::wstring wText(GetWindowTextLength(hwnd), L'\0');
			GetWindowText(hwnd, wText.data(), wText.length() + 1);

			const uInt id = GetWindowLongPtr(hwnd, GWLP_ID);
			const Bool isPosition = ((id - 1) / 3 == 0);
			const Bool isSize = ((id - 1) / 3 == 1);
			const Bool isAngle = ((id - 1) / 3 == 2);

			const uInt coordinate = (id - 1) % 3;
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