#pragma once

#include <Objects/Entities/GameModel.h>
#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameLight.h>
#include <Objects/Entities/GameSound.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Plane.h>
#include <Objects/Primitives/Quad.h>
#include <Objects/Primitives/Pyramid.h>

class PropertySound : public PropertyComponent {
private:
	enum Texts {
		TEXT_SOUND_PATH,
		TEXT_ROLLOFF_FACTOR,
		TEXT_REFERENCE_DISNANCE,
		TEXT_MAX_DISNANCE,
		TEXT_GAIN,
		TEXT_COUNT,
	};

	enum Buttons {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
		BUTTON_PLAY_AT_START,
		BUTTON_TOGGLE_LOOPINT,
		BUTTON_COUNT,
	};

	enum TextEdits {
		TEXTEDIT_SOUND_PATH,
		TEXTEDIT_ROLLOFF_FACTOR,
		TEXTEDIT_REFERENCE_DISNANCE,
		TEXTEDIT_MAX_DISNANCE,
		TEXTEDIT_GAIN,
		TEXTEDIT_COUNT,
	};

public:
	PropertySound(NotNull<Selector*> pSelector, NotNull<Scene*> pScene) noexcept :
		PropertyComponent(pSelector, Texts::TEXT_COUNT, TEXTEDIT_COUNT, BUTTON_COUNT),
		m_pScene(pScene),
		m_ContentLayout(3),
		m_ParametersLayout({ 2, 5 }),
		m_SoundPathLayout(2),
		m_SoundPlayerLayout(2)
	{
		m_RejectedObjects = {
			GameCamera::GetClassToken(),
			GameModel::GetClassToken(),
			GameLight::GetClassToken(),

			Cube::GetClassToken(),
			Plane::GetClassToken(),
			Quad::GetClassToken(),
			Pyramid::GetClassToken(),
			Primitive::GetClassToken(),
		};
	}

	void Initialize(const Settings& settings) {
		constexpr Nt::IntRect contentPadding = { 10, 10, 10, 10 };
		constexpr Nt::IntRect parametersPadding = { 0, 0, 0, 10 };

		const Int cellsHeight = Nt::TextEdit::DefaultSize.y;

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = (contentPadding.Top + contentPadding.Bottom) + (parametersPadding.Top + parametersPadding.Bottom);
		windowRect.Bottom += cellsHeight * (Texts::TEXT_COUNT + 2);

		_Create(settings, settings.Language["Window.Property.Sound"], windowRect);

		const Nt::IntRect contentLayoutRect = {
			windowRect.Left,
			windowRect.Top,
			windowRect.Right - GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXDLGFRAME) * 2,
			windowRect.Bottom - GetSystemMetrics(SM_CYBORDER) * 2
		};

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.TogleVertical(true);
		m_ContentLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(contentLayoutRect, "Sound-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(2, Float(cellsHeight), Nt::UnitType::UNIT_PIXEL);

		{
			const Int firstCellSize = m_ContentLayout.GetCellSize(0) + m_ContentLayout.GetCellSize(1) - cellsHeight;
			m_ContentLayout.SetCellSize(0, Float(firstCellSize), Nt::UnitType::UNIT_PIXEL);
		}

		m_ContentLayout.Show();

		m_ContentLayout.Insert(0, &m_ParametersLayout);
		m_ParametersLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ParametersLayout.Create("Sound-Parameters-layout");
		m_ParametersLayout.SetPadding(parametersPadding, Nt::UnitType::UNIT_PIXEL);
		m_ParametersLayout.Show();

		auto setupButton = [&](Nt::Button& button, const uInt& styles, const cString& name) {
			button.AddStyles(styles | STYLE_VISIBLE);
			button.Create(name);
		};

		m_ContentLayout.Insert(1, &m_Buttons[BUTTON_REMOVE]);
		setupButton(m_Buttons[BUTTON_REMOVE], BS_CENTER | BS_VCENTER, "Remove sound");

		m_ParametersLayout.Insert({ 1, 0 }, &m_SoundPathLayout);
		m_SoundPathLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_SoundPathLayout.Create("Sound-path-layout");

		constexpr Int browseButtonSize = 30;

		m_SoundPathLayout.SetCellSize(1, browseButtonSize, Nt::UnitType::UNIT_PIXEL);
		m_SoundPathLayout.Show();

		m_SoundPathLayout.Insert(1, &m_Buttons[BUTTON_BROWSE]);
		setupButton(m_Buttons[BUTTON_BROWSE], BS_CENTER | BS_VCENTER, "...");

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_ParametersLayout.Insert({ 0, i }, &m_Texts[i]);

			if (i == TEXTEDIT_SOUND_PATH) {
				m_TextEdits[i].AddStyles(ES_READONLY);
				m_SoundPathLayout.Insert(0, &m_TextEdits[i]);
			}
			else {
				m_ParametersLayout.Insert({ 1, i }, &m_TextEdits[i]);
			}

			m_TextEdits[i].Create("", true);
			m_TextEdits[i].Show();
		}

		m_ContentLayout.Insert(2, &m_SoundPlayerLayout);
		m_SoundPlayerLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_SoundPlayerLayout.Create("Sound-player-layout");
		m_SoundPlayerLayout.Show();

		m_SoundPlayerLayout.Insert(0, &m_Buttons[BUTTON_PLAY_AT_START]);
		setupButton(m_Buttons[BUTTON_PLAY_AT_START], BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX, "Play at star");

		m_SoundPlayerLayout.Insert(1, &m_Buttons[BUTTON_TOGGLE_LOOPINT]);
		setupButton(m_Buttons[BUTTON_TOGGLE_LOOPINT], BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX, "Loop");

		_DisableWindow();
	}
	void Update() override {
		if (!IsEnabled())
			return;

		const uInt selectedObjectCount = m_SelectorPtr->GetObjectCount();
		if (selectedObjectCount == 1) {
			const auto object = m_SelectorPtr->GetObjectPtr(0).lock();
			if (object->GetToken() == GameSound::GetClassToken()) {
				m_SelectedSoundPtr = static_cast<GameSound*>(object.get());

				m_TextEdits[TEXTEDIT_SOUND_PATH].SetText(m_SelectedSoundPtr->GetFilePath());
				m_TextEdits[TEXTEDIT_ROLLOFF_FACTOR].SetText(m_SelectedSoundPtr->GetRolloffFactor());
				m_TextEdits[TEXTEDIT_REFERENCE_DISNANCE].SetText(m_SelectedSoundPtr->GetReferenceDistance());
				m_TextEdits[TEXTEDIT_MAX_DISNANCE].SetText(m_SelectedSoundPtr->GetMaxDistance());
				m_TextEdits[TEXTEDIT_GAIN].SetText(m_SelectedSoundPtr->GetGain());

				_EnableWindow();

				m_Buttons[BUTTON_PLAY_AT_START].EnableWindow();
				m_Buttons[BUTTON_PLAY_AT_START].SetCheck(m_SelectedSoundPtr->IsPlayingAtStart());

				m_Buttons[BUTTON_TOGGLE_LOOPINT].EnableWindow();
				m_Buttons[BUTTON_TOGGLE_LOOPINT].SetCheck(m_SelectedSoundPtr->IsLooping());
				return;
			}
		}

		m_SelectedSoundPtr = nullptr;

		m_TextEdits[TEXTEDIT_SOUND_PATH].SetText((selectedObjectCount > 1) ? "..." : "No selected");

		m_Buttons[BUTTON_PLAY_AT_START].DisableWindow();
		m_Buttons[BUTTON_TOGGLE_LOOPINT].DisableWindow();
	}

	void BrowseSound() {
		if (m_SelectedSoundPtr == nullptr)
			return;

		const std::string filePath = _Browse(L"Sound (*.wav)\0*.wav\0All (*.*)\0*.*");

		if (filePath.empty())
			return;

		m_SelectedSoundPtr->Load(filePath);

		m_TextEdits[TEXTEDIT_SOUND_PATH].SetText(filePath);

		m_Buttons[BUTTON_REMOVE].EnableWindow();
	}
	void SetScene(NotNull<Scene*> pScene) {
		m_pScene = pScene;
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_ContentLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_ParametersLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_SoundPathLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_SoundPlayerLayout.SetBackgroundColor(style["Property.BackgroundColor"]);

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_Texts[i].SetColor(style["Property.Texts.Color"]);
			m_Texts[i].SetWeight(style["Property.Texts.Weight"]);

			m_TextEdits[i].SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);
			m_TextEdits[i].SetTextColor(style["Property.TextEdits.Text.Color"]);
			m_TextEdits[i].SetTextWeight(style["Property.TextEdits.Text.Weight"]);
		}
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.Sound"]);

		const std::string texts[TEXT_COUNT] = {
			language["FilePath"],
			language["Window.Property.Sound.RolloffFactor"],
			language["Window.Property.Sound.ReferenceDistance"],
			language["Window.Property.Sound.MaxDistance"],
			language["Window.Property.Sound.Gain"]
		};

		for (uInt i = 0; i < Texts::TEXT_COUNT; ++i)
			m_Texts[i].SetText(texts[i]);
	}

private:
	Nt::GridLayout m_ParametersLayout;
	Nt::BoxLayout m_ContentLayout;
	Nt::BoxLayout m_SoundPathLayout;
	Nt::BoxLayout m_SoundPlayerLayout;

	GameSound* m_SelectedSoundPtr = nullptr;
	Scene* m_pScene = nullptr;
	inline static PropertyRegistrar<PropertySound> m_Registrar { "Sound" };

private:
	void _AddSelection(Object* pObject) override {
		PropertyComponent::_AddSelection(pObject);
	}
	void _Deselect(Object* pObject) override {
		PropertyComponent::_Deselect(pObject);
	}

	void _Paint(HDC& hdc, PAINTSTRUCT& paint) override {
		(void)hdc;
		(void)paint;

		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}

	void _ButtonsNotification_OnClick(const uInt& id, const HWND& handle) override {
		(void)handle;

		switch (id) {
		case BUTTON_BROWSE:
			BrowseSound();
			break;

		case BUTTON_REMOVE:
			m_SelectedSoundPtr->Unload();

			m_TextEdits[TEXTEDIT_SOUND_PATH].SetText("No selected");

			m_Buttons[id].DisableWindow();
			break;

		case BUTTON_PLAY_AT_START:
			if (m_Buttons[id].IsChecked())
				m_SelectedSoundPtr->EnablePlayingAtStart();
			else
				m_SelectedSoundPtr->DisablePlayingAtStart();
			break;

		case BUTTON_TOGGLE_LOOPINT:
			m_SelectedSoundPtr->ToggleLooping(m_Buttons[id].IsChecked());
			break;
		}
	}

	void _TextEditsNotification_Update(const uInt& id, const HWND& handle) override {
		(void)handle;

		switch (id) {
		case TEXTEDIT_SOUND_PATH:
			break;

		case TEXTEDIT_ROLLOFF_FACTOR:
			m_SelectedSoundPtr->SetRolloffFactor(m_TextEdits[id].GetText());
			break;

		case TEXTEDIT_REFERENCE_DISNANCE:
			m_SelectedSoundPtr->SetReferenceDistance(m_TextEdits[id].GetText());
			break;

		case TEXTEDIT_MAX_DISNANCE:
			m_SelectedSoundPtr->SetMaxDistance(m_TextEdits[id].GetText());
			break;

		case TEXTEDIT_GAIN:
			m_SelectedSoundPtr->SetGain(m_TextEdits[id].GetText());
			break;
		}
	}
};