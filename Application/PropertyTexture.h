#pragma once

class PropertyTexture : public PropertyComponent {
private:
	enum Buttons {
		BUTTON_SELECT_TEXTURE = 9999,
		BUTTON_CLAMP_U,
		BUTTON_CLAMP_V,
	};
	enum TextEdits {
		TEXTEDIT_OFFSET_X, TEXTEDIT_OFFSET_Y, 
		TEXTEDIT_SCALE_X, TEXTEDIT_SCALE_Y, 
		TEXTEDIT_ROTATE_X, TEXTEDIT_ROTATE_Y,
		TEXTEDIT_COUNT
	};

public:
	struct LanguageData : Language::_PropertyWindow::_Texture {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertyTexture() noexcept : 
		m_ContentLayout(2),
		m_ParametersLayout({ 3, 5 }),
		m_Texts(LanguageData::TEXT_COUNT),
		m_TextEdits(TEXTEDIT_COUNT),
		m_IsWindowEnabled(false)
	{ 
	}

	void Initialize(const Settings& settings) {
		SetLanguage(settings.CurrentLanguage);
		m_Style = settings.Styles;

		constexpr Nt::IntRect contentPadding(10, 10, 20, 10);
		constexpr Nt::IntRect textureButtonRect(0, 0, 100, 100);

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right - GetSystemMetrics(SM_CXDLGFRAME) * 2;
		windowRect.Bottom = 
			(textureButtonRect.Top + textureButtonRect.Bottom) + (contentPadding.Top + contentPadding.Bottom);

		Create(windowRect, m_LanguageData.WindowName);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		const Nt::IntRect contentLayoutRect(windowRect.LeftTop, windowRect.RightBottom - 1);

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_ContentLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(contentLayoutRect, "Texture-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetGap(20, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(0, 0.66f, Nt::UnitType::UNIT_PERCENTAGE);
		m_ContentLayout.Show();

		m_ContentLayout.Insert(0, &m_TextureButton);
		m_TextureButton.SetID(BUTTON_SELECT_TEXTURE);
		m_TextureButton.AddStyles(BS_CENTER | BS_VCENTER | WS_VISIBLE);
		m_TextureButton.Create("No texture");
		m_TextureButton.DisableWindow();

		m_ContentLayout.Insert(1, &m_ParametersLayout);
		m_ParametersLayout.SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_ParametersLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ParametersLayout.Create("Texture-Parameters-layout");
		m_ParametersLayout.Show();

		for (uInt i = 0; i < LanguageData::TEXT_COUNT; ++i) {
			m_Texts[i].SetText(m_LanguageData.Texts[i]);
			m_Texts[i].SetColor(m_Style.Property.Texts.Color);
			m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);

			m_ParametersLayout.Insert(Nt::uInt2D(0, i), &m_Texts[i]);
		}

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			if (i % 2 == 0)
				m_ParametersLayout.Insert(Nt::uInt2D(2, i / 2), &m_TextEdits[i]);
			else
				m_ParametersLayout.Insert(Nt::uInt2D(1, i / 2), &m_TextEdits[i]);

			m_TextEdits[i].SetID(i);
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Create("0.0", true);
			m_TextEdits[i].DisableWindow();
			m_TextEdits[i].Show();
		}

		m_ParametersLayout[1][3].ToggleClampToEdge(false);
		m_ParametersLayout.Insert(Nt::uInt2D(1, 3), &m_CheckboxClampU);
		m_CheckboxClampU.SetID(BUTTON_CLAMP_U);
		m_CheckboxClampU.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
		m_CheckboxClampU.Create("");
		m_CheckboxClampU.DisableWindow();

		m_ParametersLayout[1][4].ToggleClampToEdge(false);
		m_ParametersLayout.Insert(Nt::uInt2D(1, 4), &m_CheckboxClampV);
		m_CheckboxClampV.SetID(BUTTON_CLAMP_V);
		m_CheckboxClampV.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
		m_CheckboxClampV.Create("");
		m_CheckboxClampV.DisableWindow();
	}
	void Update() {
		if (!IsEnabled())
			return;
		if (!m_SelectorPtr)
			Raise("Selector pointer is nullptr");

		if (m_SelectorPtr->GetObjects().size() == 1) {
			_EnableWindow();
			if (m_SelectorPtr->IsChanged()) {
				const Object* pObject = m_SelectorPtr->GetObjects()[0];
				if (pObject->ObjectType == ObjectTypes::PRIMITIVE) {
					_SetButtonTexture(pObject->GetTexturePtr());

					const Primitive* pPrimitive = dynamic_cast<const Primitive*>(pObject);
					m_TextEdits[TEXTEDIT_OFFSET_X].SetText(pPrimitive->GetTextureOffset().x);
					m_TextEdits[TEXTEDIT_OFFSET_Y].SetText(pPrimitive->GetTextureOffset().y);
					m_TextEdits[TEXTEDIT_SCALE_X].SetText(pPrimitive->GetTextureScale().x);
					m_TextEdits[TEXTEDIT_SCALE_Y].SetText(pPrimitive->GetTextureScale().y);
				}
			}
		}
		else {
			_DisableWindow();
		}
	}

	void BrowseTexture() {
		if (m_SelectorPtr) {
			const cwString filter = L"Targa (*.tga)\0*.tga\0All (*.*)\0*.*";

			Nt::String filePath = Nt::OpenFileDialog(GetRootPath().wstr().c_str(), filter);
			if (filePath.size() == 0)
				return;

			if (!IsValidPath(GetRootPath(), filePath)) {
				WarningBox(L"To add a file, place it in the project's root folder.", L"Warning");
				return;
			}
			filePath.erase(filePath.begin(), filePath.begin() + GetRootPath().length() + 1);

			Nt::Texture texture(filePath);
			for (Object* pObject : m_SelectorPtr->GetObjects())
				pObject->SetTexture(texture);

			m_Bitmap.Delete();
			m_Bitmap.Create(texture.GetSize(), texture.GetData());

			m_TextureButton.AddStyles(BS_BITMAP);
			m_TextureButton.SetImage(m_Bitmap);
		}
	}

	void SetTheme(const Style& style) {
		m_Style = style;

		SetBackgroundColor(m_Style.Property.BackgroundColor);
		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);

			if (i < LanguageData::TEXT_COUNT) {
				m_Texts[i].SetColor(m_Style.Property.Texts.Color);
				m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);
			}
		}
	}
	void SetLanguage(const Language& language) {
		m_LanguageData = (LanguageData)language.PropertyWindow.Texture;
		m_LanguageData.WindowName =
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_TEXTUREWINDOW];
		SetName(m_LanguageData.WindowName);

		for (uInt i = 0; i < LanguageData::TEXT_COUNT; ++i)
			m_Texts[i].SetText(m_LanguageData.Texts[i]);
	}

private:
	Nt::BoxLayout m_ContentLayout;
	Nt::GridLayout m_ParametersLayout;
	LanguageData m_LanguageData;
	Nt::GDI::Bitmap m_Bitmap;
	Nt::Button m_TextureButton;
	Nt::Button m_CheckboxClampU;
	Nt::Button m_CheckboxClampV;
	std::vector<Nt::Text> m_Texts;
	std::vector<Nt::TextEdit> m_TextEdits;
	Style m_Style;
	Bool m_IsWindowEnabled;

private:
	void _EnableWindow() {
		if (!m_IsWindowEnabled) {
			m_TextureButton.EnableWindow();
			m_CheckboxClampU.EnableWindow();
			m_CheckboxClampV.EnableWindow();
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.EnableWindow();
			m_IsWindowEnabled = true;
		}
	}
	void _DisableWindow() {
		if (m_IsWindowEnabled) {
			m_TextureButton.DisableWindow();
			m_CheckboxClampU.DisableWindow();
			m_CheckboxClampV.DisableWindow();
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.DisableWindow();

			_RemoveButtonTexture();
			m_IsWindowEnabled = false;
		}
	}
	void _SetButtonTexture(const Nt::Texture* pTexture) {
		if (pTexture) {
			m_Bitmap.Delete();
			m_Bitmap.Create(pTexture->GetSize(), pTexture->GetData());
			m_TextureButton.AddStyles(BS_BITMAP);
			m_TextureButton.SetImage(m_Bitmap);
		}
		else {
			_RemoveButtonTexture();
		}
	}
	void _RemoveButtonTexture() {
		m_TextureButton.RemoveImage();
		m_TextureButton.RemoveStyles(BS_BITMAP);
	}

	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		if (!m_IsWindowEnabled)
			return;

		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		switch (command) {
		case BN_CLICKED:
			if (m_SelectorPtr != nullptr) {
				Bool isClampingU = false;
				switch (id) {
				case BUTTON_SELECT_TEXTURE:
					BrowseTexture();
					break;
				case BUTTON_CLAMP_U:
					isClampingU = true;
				case BUTTON_CLAMP_V:
					const Bool isChecked = IsDlgButtonChecked(m_hwnd, id);
					const Nt::Texture::Wrap newWrapState =
						(isChecked)
						? Nt::Texture::Wrap::WRAP_CLAMP
						: Nt::Texture::Wrap::WRAP_REPEAT;

					for (Object* pObject : m_SelectorPtr->GetObjects()) {
						if (pObject == nullptr)
							Raise("nullptr in Selector");

						Nt::Texture* pTexture = pObject->GetTexturePtr();
						if (isClampingU)
							pTexture->SetWrapS(newWrapState);
						else
							pTexture->SetWrapT(newWrapState);
					}
					break;
				}
			}
			break;
		case EN_UPDATE:
			try {
				if (m_SelectorPtr == nullptr)
					break;

				for (Object* pObject : m_SelectorPtr->GetObjects()) {
					if (pObject == nullptr)
						Raise("nullptr in Selector");
					if (pObject->ObjectType != ObjectTypes::PRIMITIVE)
						continue;

					Primitive* pPrimitive = dynamic_cast<Primitive*>(pObject);
					const Nt::Float2D textureOffset = pPrimitive->GetTextureOffset();
					const Nt::Float2D textureScale = pPrimitive->GetTextureScale();

					switch (id) {
					case TEXTEDIT_OFFSET_X:
						pPrimitive->SetTextureOffset({
							m_TextEdits[id].GetText(),
							textureOffset.y });
						break;
					case TEXTEDIT_OFFSET_Y:
						pPrimitive->SetTextureOffset({
							textureOffset.x,
							m_TextEdits[id].GetText() });
						break;
					case TEXTEDIT_SCALE_X:
						pPrimitive->SetTextureScale({
							m_TextEdits[id].GetText(),
							textureScale.y });
						break;
					case TEXTEDIT_SCALE_Y:
						pPrimitive->SetTextureScale({
							textureScale.x,
							m_TextEdits[id].GetText() });
						break;
					case TEXTEDIT_ROTATE_X:
						break;
					case TEXTEDIT_ROTATE_Y:
						break;
					}
				}
			}
			catch (...) 
			{
			}
			break;
		}
	}
};