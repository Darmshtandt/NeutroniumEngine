#pragma once

#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameLight.h>
#include <Objects/Entities/GameSound.h>

class PropertyTexture : public PropertyComponent {
private:
	enum Texts {
		TEXT_OFFSET,
		TEXT_SCALE,
		TEXT_ROTATE,
		TEXT_COUNT
	};

	enum Buttons {
		BUTTON_SELECT_TEXTURE,
		BUTTON_CLAMP_U,
		BUTTON_CLAMP_V,
		BUTTON_COUNT,
	};

	enum TextEdits {
		TEXTEDIT_OFFSET_X, TEXTEDIT_OFFSET_Y, 
		TEXTEDIT_SCALE_X, TEXTEDIT_SCALE_Y, 
		TEXTEDIT_ROTATION,
		TEXTEDIT_COUNT
	};

public:
	PropertyTexture(Selector* pSelector, Scene*) noexcept :
		PropertyComponent(pSelector, TEXT_COUNT, TEXTEDIT_COUNT, BUTTON_COUNT),
		m_ContentLayout(2),
		m_ParametersLayout({ 3, 5 })
	{
		m_RejectedObjects = {
			GameCamera::GetClassToken(),
			GameLight::GetClassToken(),
			GameSound::GetClassToken()
		};
	}

	void Initialize(const Settings& settings) override {
		constexpr Nt::IntRect contentPadding(10, 10, 20, 10);
		constexpr Nt::IntRect textureButtonRect(0, 0, 100, 100);
		
		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right - GetSystemMetrics(SM_CXDLGFRAME) * 2;
		windowRect.Bottom =
			(textureButtonRect.Top + textureButtonRect.Bottom) + (contentPadding.Top + contentPadding.Bottom);

		_Create(settings, settings.Language["Window.Property.Texture"], windowRect);

		const Nt::IntRect contentLayoutRect(windowRect.LeftTop, windowRect.RightBottom - 1);

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(contentLayoutRect, "Texture-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetGap(20, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(0, 0.66f, Nt::UnitType::UNIT_PERCENTAGE);
		m_ContentLayout.Show();

		Nt::Button* pTextureButton = &m_Buttons[BUTTON_SELECT_TEXTURE];

		m_ContentLayout.Insert(0, pTextureButton);
		pTextureButton->AddStyles(BS_CENTER | BS_VCENTER | WS_VISIBLE);
		pTextureButton->Create("No texture");

		m_ContentLayout.Insert(1, &m_ParametersLayout);
		m_ParametersLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ParametersLayout.Create("Texture-Parameters-layout");
		m_ParametersLayout.Show();

		for (uInt i = 0; i < Texts::TEXT_COUNT; ++i)
			m_ParametersLayout.Insert(Nt::uInt2D(0, i), &m_Texts[i]);

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			Nt::uInt2D indexOnLayout = Nt::uInt2D(2, i / 2);
			if (i % 2 != 0)
				indexOnLayout.x = 1;

			m_ParametersLayout.Insert(indexOnLayout, &m_TextEdits[i]);

			m_TextEdits[i].Create("0.0", true);
			m_TextEdits[i].Show();
		}

		const uInt checkButtons[] = { BUTTON_CLAMP_U, BUTTON_CLAMP_V };
		for (const uInt& id : checkButtons) {
			const Nt::uInt2D positionOnLayout = (id == BUTTON_CLAMP_U) ? Nt::uInt2D(1, 3) : Nt::uInt2D(1, 4);

			m_ParametersLayout[positionOnLayout.x][positionOnLayout.y].ToggleClampToEdge(false);
			m_ParametersLayout.Insert(positionOnLayout, &m_Buttons[id]);

			m_Buttons[id].AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | WS_VISIBLE);
			m_Buttons[id].Create("");
		}

		m_Buttons[BUTTON_SELECT_TEXTURE].SetOnClick([&]() {
			BrowseTexture(); 
			});

		m_Buttons[BUTTON_CLAMP_U].SetOnClick([&]() {
			_ToggleClamp(m_Buttons[BUTTON_CLAMP_U].IsChecked(), true, false); 
			});

		m_Buttons[BUTTON_CLAMP_V].SetOnClick([&]() {
			_ToggleClamp(m_Buttons[BUTTON_CLAMP_V].IsChecked(), false, true);
			});

		_DisableWindow();
	}
	void Update() override {
		if (!IsEnabled())
			return;

		if (m_SelectorPtr->GetObjectCount() != 1) {
			_DisableWindow();
			return;
		}

		_EnableWindow();
	}

	void BrowseTexture() {
		const std::string filePath = _Browse(L"Targa (*.tga)\0*.tga\0All (*.*)\0*.*\0\0");
		if (filePath.empty())
			return;

		const uInt textureIndex = Nt::ResourceManager::Instance().Add<Nt::Texture>(filePath);
		for (const WeakObjectPtr& weakObject : m_SelectorPtr->GetObjectContainer()) {
			const auto object = weakObject.lock();
			if (object == nullptr)
				continue;
			object->SetTexture(textureIndex);
		}

		_SetButtonTexture(Nt::ResourceManager::Instance().Get<Nt::Texture>(textureIndex));
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_ContentLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_ParametersLayout.SetBackgroundColor(style["Property.BackgroundColor"]);

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_TextEdits[i].SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);
			m_TextEdits[i].SetTextColor(style["Property.TextEdits.Text.Color"]);
			m_TextEdits[i].SetTextWeight(style["Property.TextEdits.Text.Weight"]);
		}

		for (uInt i = 0; i < Texts::TEXT_COUNT; ++i) {
			m_Texts[i].SetColor(style["Property.Texts.Color"]);
			m_Texts[i].SetWeight(style["Property.Texts.Weight"]);
		}
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.Texture"]);

		const std::string texts[TEXT_COUNT] = {
			language["Window.Property.Texture.Offset"],
			language["Window.Property.Texture.Scale"],
			language["Window.Property.Texture.Rotate"]
		};

		for (uInt i = 0; i < Texts::TEXT_COUNT; ++i)
			m_Texts[i].SetText(texts[i]);
	}

private:
	Nt::GDI::Bitmap m_Bitmap;
	Nt::BoxLayout m_ContentLayout;
	Nt::GridLayout m_ParametersLayout;
	inline static PropertyRegistrar<PropertyTexture> m_Registrar { "Texture" };

private:
	void _AddSelection(Object* pObject) override {
		assert(pObject);
		if (pObject->GetTypeToken() == Primitive::GetClassTypeToken()) {
			_SetButtonTexture(pObject->GetTexture().Get());

			m_TextEdits[TEXTEDIT_OFFSET_X].SetText(pObject->GetTextureOffset().x);
			m_TextEdits[TEXTEDIT_OFFSET_Y].SetText(pObject->GetTextureOffset().y);
			m_TextEdits[TEXTEDIT_SCALE_X].SetText(pObject->GetTextureScale().x);
			m_TextEdits[TEXTEDIT_SCALE_Y].SetText(pObject->GetTextureScale().y);
			m_TextEdits[TEXTEDIT_ROTATION].SetText(pObject->GetTextureRotation() / RADf);
		}

		PropertyComponent::_AddSelection(pObject);
	}

	void _SetButtonTexture(const Nt::Texture* pTexture) {
		if (pTexture == nullptr) {
			_RemoveButtonTexture();
			return;
		}

		m_Bitmap.Delete();
		m_Bitmap.Create(pTexture->GetSize(), pTexture->GetData());

		Nt::Button* pTextureButton = &m_Buttons[BUTTON_SELECT_TEXTURE];

		pTextureButton->AddStyles(BS_BITMAP);
		pTextureButton->SetImage(m_Bitmap);
	}
	void _RemoveButtonTexture() {
		Nt::Button* pTextureButton = &m_Buttons[BUTTON_SELECT_TEXTURE];

		pTextureButton->RemoveImage();
		pTextureButton->RemoveStyles(BS_BITMAP);
	}

	void _ToggleClamp(const Bool& isEnabled, const Bool& isClampingU, const Bool& isClampingV) {
		const Nt::Wrap newWrapState =
			(isEnabled) ? Nt::Wrap::WRAP_CLAMP : Nt::Wrap::WRAP_REPEAT;

		for (const WeakObjectPtr& weakObject : m_SelectorPtr->GetObjectContainer()) {
			const auto object = weakObject.lock();
			if (object == nullptr)
				continue;

			auto texture = object->GetTexture();
			if (!texture.IsValid())
				continue;

			if (isClampingU)
				texture.Get()->SetWrapS(newWrapState);
			if (isClampingV)
				texture.Get()->SetWrapT(newWrapState);
		}
	}

	void _TextEditsNotification_Update(const uInt& id, const HWND& handle) override {
		(void)handle;

		const WeakObjectContainer& selectedObjects = m_SelectorPtr->GetObjectContainer();
		auto iterator = selectedObjects.cbegin();

		try {
			for (; iterator != selectedObjects.end(); ++iterator) {
				const auto object = iterator->lock();
				if (object == nullptr || object->GetTypeToken() != Primitive::GetClassTypeToken())
					continue;

				const Nt::Float2D textureOffset = object->GetTextureOffset();
				const Nt::Float2D textureScale = object->GetTextureScale();
				const Float value = m_TextEdits[id].GetText();

				switch (id) {
				case TEXTEDIT_OFFSET_X:
					object->SetTextureOffset({ value, textureOffset.y });
					break;

				case TEXTEDIT_OFFSET_Y:
					object->SetTextureOffset({ textureOffset.x, value });
					break;

				case TEXTEDIT_SCALE_X:
					object->SetTextureScale({ value, textureScale.y });
					break;

				case TEXTEDIT_SCALE_Y:
					object->SetTextureScale({ textureScale.x, value });
					break;

				case TEXTEDIT_ROTATION:
					object->SetTextureRotation(value * RADf);
					break;
				}
			}
		}
		catch (const Nt::Error& error) {
			m_SelectorPtr->Deselect(iterator);
			Nt::MessageWindow(error.what(), "Warning").ShowWarning();
		}
		catch (...)
		{
		}
	}
};