#pragma once

class PropertyTexture : public PropertyComponent {
public:
	enum Buttons {
		BUTTON_SELECT_TEXTURE = 9999,
		BUTTON_CLAMP_U,
		BUTTON_CLAMP_V,
	};
	enum Texts {
		TEXT_OFFSET, TEXT_SCALE, TEXT_ROTATE, 
		TEXT_CLAMP_U, TEXT_CLAMP_V,
		TEXT_COUNT
	};
	enum TextEdits {
		TEXTEDIT_OFFSET_X, TEXTEDIT_OFFSET_Y, 
		TEXTEDIT_SCALE_X, TEXTEDIT_SCALE_Y, 
		TEXTEDIT_ROTATE_X, TEXTEDIT_ROTATE_Y,
		TEXTEDIT_COUNT
	};

public:
	PropertyTexture() noexcept : 
		m_Texts(TEXT_COUNT),
		m_TextEdits(TEXTEDIT_COUNT),
		m_IsWindowEnabled(false)
	{ 
	}

	void Initialize(const Settings& settings) {
		m_Language = settings.CurrentLanguage;
		m_Style = settings.Styles;
		m_PaddingRect = { 10, 10, 10, 10 };

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;

		Create(windowRect, m_Language.Window.PropertyComponent.TextureWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		m_TextureButton.SetParent(*this);
		m_TextureButton.SetID(BUTTON_SELECT_TEXTURE);
		m_TextureButton.AddStyles(BS_CENTER | BS_VCENTER | WS_VISIBLE);
		m_TextureButton.Create({ m_PaddingRect.LeftTop, { 100, 100 } }, "No texture");
		m_TextureButton.DisableWindow();
		//m_TextureButton.SetOnClick(std::bind(&PropertyTexture::ChanGeTexture, this));

		const std::wstring texts[] = {
			m_Language.Window.PropertyTexture.Offset,
			m_Language.Window.PropertyTexture.Scale,
			m_Language.Window.PropertyTexture.Rotate,
			m_Language.Window.PropertyTexture.ClampU,
			m_Language.Window.PropertyTexture.ClampV,
		};

		Nt::uInt2D textPosition;
		textPosition = m_PaddingRect.LeftTop;
		textPosition.x += m_TextureButton.GetClientRect().Right + 10;
		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			m_Texts[i].SetText(texts[i]);
			m_Texts[i].SetPosition(textPosition);
			m_Texts[i].SetColor(m_Style.Property.Texts.Color);
			m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);
			textPosition.y += 20;
		}

		Nt::uIntRect textEditRect(textPosition, { 40, 20 });
		textEditRect.Left += 70;
		textEditRect.Top = m_PaddingRect.Top;
		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_TextEdits[i].SetParent(*this);
			m_TextEdits[i].SetID(i);
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Create(textEditRect, "0.0", true);
			m_TextEdits[i].DisableWindow();
			m_TextEdits[i].Show();

			if (i % 2 == 0) {
				textEditRect.Left += m_TextEdits[i].GetClientRect().Right;
			}
			else {
				textEditRect.Left -= m_TextEdits[i].GetClientRect().Right;
				textEditRect.Top += 20;
			}
		}

		Nt::uIntRect checkBoxRect = textEditRect;
		checkBoxRect.Right = GetSystemMetrics(SM_CXMENUCHECK);
		checkBoxRect.Bottom = GetSystemMetrics(SM_CYMENUCHECK);
		m_CheckboxClampU.SetParent(*this);
		m_CheckboxClampU.SetID(BUTTON_CLAMP_U);
		m_CheckboxClampU.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
		m_CheckboxClampU.Create(checkBoxRect, "");
		m_CheckboxClampU.DisableWindow();

		checkBoxRect.Top += 20;
		m_CheckboxClampV.SetParent(*this);
		m_CheckboxClampV.SetID(BUTTON_CLAMP_V);
		m_CheckboxClampV.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
		m_CheckboxClampV.Create(checkBoxRect, "");
		m_CheckboxClampV.DisableWindow();

		windowRect.Bottom = m_TextureButton.GetClientRect().Top;
		windowRect.Bottom += m_TextureButton.GetClientRect().Bottom;
		windowRect.Bottom += m_PaddingRect.Bottom;
		SetSize(windowRect.RightBottom);
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
				WarningBox(m_Language.Messages.AddingFile.wstr().c_str(), L"Warning");
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

			if (i < TEXT_COUNT) {
				m_Texts[i].SetColor(m_Style.Property.Texts.Color);
				m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);
			}
		}
	}
	void SetLanguage(const Language& language) {
		m_Language = language;
		const std::wstring texts[] = {
			m_Language.Window.PropertyTexture.Offset,
			m_Language.Window.PropertyTexture.Scale,
			m_Language.Window.PropertyTexture.Rotate,
			m_Language.Window.PropertyTexture.ClampU,
			m_Language.Window.PropertyTexture.ClampV,
		};

		for (uInt i = 0; i < TEXT_COUNT; ++i)
			m_Texts[i].SetText(texts[i]);
	}

private:
	Nt::IntRect m_PaddingRect;
	Nt::GDI::Bitmap m_Bitmap;
	Nt::Button m_TextureButton;
	Nt::Button m_CheckboxClampU;
	Nt::Button m_CheckboxClampV;
	std::vector<Nt::Text> m_Texts;
	std::vector<Nt::TextEdit> m_TextEdits;
	Language m_Language;
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

	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
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