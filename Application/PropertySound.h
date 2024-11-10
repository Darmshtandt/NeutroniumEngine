#pragma once

class PropertySound : public PropertyComponent {
public:
	enum Buttons {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
		BUTTON_PLAY_AT_START,
		BUTTON_TOGGLE_LOOPINT,
	};
	enum TextEdits {
		TEXTEDIT_FIRST = 5000,
		TEXTEDIT_SOUND_PATH = TEXTEDIT_FIRST,
		TEXTEDIT_ROLLOFF_FACTOR,
		TEXTEDIT_REFERENCE_DISNANCE,
		TEXTEDIT_MAX_DISNANCE,
		TEXTEDIT_GAIN,
		TEXTEDIT_COUNT,
	};

public:
	struct LanguageData : Language::_PropertyWindow::_Sound {
		using ComponentName = Language::_PropertyWindow::_Component;

		Nt::String WindowName;
	};

public:
	PropertySound() :
		m_ContentLayout(3),
		m_ParametersLayout({ 2, 5 }),
		m_SoundPathLayout(2),
		m_SoundPlayerLayout(2),
		m_Texts(LanguageData::TEXT_COUNT),
		m_TextEdits(TEXTEDIT_COUNT - TEXTEDIT_FIRST)
	{
	}

	void Initialize(const Settings& settings) {
		SetLanguage(settings.CurrentLanguage);
		m_Style = settings.Styles;

		constexpr Nt::IntRect contentPadding = { 10, 10, 10, 10 };
		constexpr Nt::IntRect parametersPadding = { 0, 0, 0, 10 };
		const Int cellsHeight = Nt::TextEdit::DefaultSize.y;

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = (contentPadding.Top + contentPadding.Bottom) + (parametersPadding.Top + parametersPadding.Bottom);
		windowRect.Bottom += cellsHeight * (LanguageData::TEXT_COUNT + 2);

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
		m_ContentLayout.Create(contentLayoutRect, "Sound-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(2, Float(cellsHeight), Nt::UnitType::UNIT_PIXEL);
		{
			const Nt::Int firstCellSize = m_ContentLayout.GetCellSize(0) + m_ContentLayout.GetCellSize(1) - cellsHeight;
			m_ContentLayout.SetCellSize(0, Float(firstCellSize), Nt::UnitType::UNIT_PIXEL);
		}
		m_ContentLayout.Show();

		m_ContentLayout.Insert(0, &m_ParametersLayout);
		m_ParametersLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_ParametersLayout.SetBackgroundColor(settings.Styles.Property.BackgroundColor);
		m_ParametersLayout.Create("Sound-Parameters-layout");
		m_ParametersLayout.SetPadding(parametersPadding, Nt::UnitType::UNIT_PIXEL);
		m_ParametersLayout.Show();

		m_ContentLayout.Insert(1, &m_DeleteButton);
		m_DeleteButton.SetID(BUTTON_REMOVE);
		m_DeleteButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_DeleteButton.DisableWindow();
		m_DeleteButton.Create("Remove sound");
		m_DeleteButton.Show();

		m_ParametersLayout.Insert({ 1, 0 }, &m_SoundPathLayout);
		m_SoundPathLayout.SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_SoundPathLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_SoundPathLayout.Create("Sound-path-layout");

		constexpr Int browseButtonSize = 30;
		m_SoundPathLayout.SetCellSize(1, browseButtonSize, Nt::UnitType::UNIT_PIXEL);
		m_SoundPathLayout.Show();

		m_SoundPathLayout.Insert(1, &m_BrowseButton);
		m_BrowseButton.SetID(BUTTON_BROWSE);
		m_BrowseButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_BrowseButton.Create("...");
		m_BrowseButton.Show();

		for (uInt i = 0; i < TEXTEDIT_COUNT - TEXTEDIT_FIRST; ++i) {
			m_ParametersLayout.Insert({ 0, i }, &m_Texts[i]);
			m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
			m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);
			m_Texts[i].SetText(m_LanguageData.Texts[i]);

			if (TEXTEDIT_FIRST + i == TEXTEDIT_SOUND_PATH) {
				m_TextEdits[i].AddStyles(ES_READONLY);
				m_SoundPathLayout.Insert(0, &m_TextEdits[i]);
			}
			else {
				m_ParametersLayout.Insert({ 1, i }, &m_TextEdits[i]);
			}

			m_TextEdits[i].SetID(i + TEXTEDIT_FIRST);
			m_TextEdits[i].Create("", true);
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Show();
		}

		m_ContentLayout.Insert(2, &m_SoundPlayerLayout);
		m_SoundPlayerLayout.SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_SoundPlayerLayout.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_SoundPlayerLayout.Create("Sound-player-layout");
		m_SoundPlayerLayout.Show();

		m_SoundPlayerLayout.Insert(0, &m_PlayAtStartButton);
		m_PlayAtStartButton.SetID(BUTTON_PLAY_AT_START);
		m_PlayAtStartButton.AddStyles(BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX);
		m_PlayAtStartButton.DisableWindow();
		m_PlayAtStartButton.Create("Play at start");
		m_PlayAtStartButton.Show();

		m_SoundPlayerLayout.Insert(1, &m_ToggleLoopingButton);
		m_ToggleLoopingButton.SetID(BUTTON_TOGGLE_LOOPINT);
		m_ToggleLoopingButton.AddStyles(BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX);
		m_ToggleLoopingButton.DisableWindow();
		m_ToggleLoopingButton.Create("Loop");
		m_ToggleLoopingButton.Show();
	}
	void Update() {
		if (!IsEnabled())
			return;
		if (!m_SelectorPtr)
			Raise("Selector pointer is nullptr");
		if (!m_SelectorPtr->IsChanged())
			return;

		const uInt selectedObjectCount = m_SelectorPtr->GetObjects().size();
		if (selectedObjectCount == 1) {
			Object* pObject = m_SelectorPtr->GetObjects()[0];
			if (pObject->ObjectType == ObjectTypes::ENTITY) {
				Entity* pEntity = UpcastObjectToEntity(pObject);
				if (pEntity->GetEntityType() == EntityTypes::SOUND) {
					m_SelectedSoundPtr = UpcastEntityToGameSound(pEntity);

					m_TextEdits[TEXTEDIT_SOUND_PATH - TEXTEDIT_FIRST].SetText(m_SelectedSoundPtr->GetFilePath());
					m_TextEdits[TEXTEDIT_ROLLOFF_FACTOR - TEXTEDIT_FIRST].SetText(m_SelectedSoundPtr->GetRolloffFactor());
					m_TextEdits[TEXTEDIT_REFERENCE_DISNANCE - TEXTEDIT_FIRST].SetText(m_SelectedSoundPtr->GetReferenceDistance());
					m_TextEdits[TEXTEDIT_MAX_DISNANCE - TEXTEDIT_FIRST].SetText(m_SelectedSoundPtr->GetMaxDistance());
					m_TextEdits[TEXTEDIT_GAIN - TEXTEDIT_FIRST].SetText(m_SelectedSoundPtr->GetGain());

					m_DeleteButton.EnableWindow();
					m_PlayAtStartButton.EnableWindow();
					m_PlayAtStartButton.SetCheck(m_SelectedSoundPtr->IsPlayingAtStart());
					m_ToggleLoopingButton.EnableWindow();
					m_ToggleLoopingButton.SetCheck(m_SelectedSoundPtr->IsLooping());
					return;
				}
			}
		}

		m_SelectedSoundPtr = nullptr;
		m_TextEdits[TEXTEDIT_SOUND_PATH - TEXTEDIT_FIRST].SetText(
			(selectedObjectCount > 1) ? "..." : "No selected");

		m_DeleteButton.DisableWindow();
		m_PlayAtStartButton.DisableWindow();
	}

	void BrowseSound() {
		const uInt selectedObjectCount = m_SelectorPtr->GetObjects().size();
		if (m_SelectorPtr && selectedObjectCount > 0) {
			const cwString filter = L"Sound (*.wav)\0*.wav\0All (*.*)\0*.*";

			Nt::String filePath = Nt::OpenFileDialog(GetRootPath().wstr().c_str(), filter);
			if (filePath.size() == 0)
				return;

			if (!IsValidPath(GetRootPath(), filePath)) {
				WarningBox(L"To add a file, place it in the project's root folder.", L"Warning");
				return;
			}
			filePath.erase(filePath.begin(), filePath.begin() + GetRootPath().length() + 1);

			if (m_SelectedSoundPtr)
				m_SelectedSoundPtr->Load(filePath);

			m_TextEdits[TEXTEDIT_SOUND_PATH - TEXTEDIT_FIRST].SetText(filePath);
			m_DeleteButton.EnableWindow();
		}
	}

	void SetTheme(const Style& style) {
		m_Style = style;

		SetBackgroundColor(m_Style.Property.BackgroundColor);
		for (uInt i = 0; i < TEXTEDIT_COUNT - TEXTEDIT_FIRST; ++i) {
			m_Texts[i].SetColor(m_Style.Property.Texts.Color);
			m_Texts[i].SetWeight(m_Style.Property.Texts.Weight);

			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
		}
	}
	void SetLanguage(const Language& language) {
		m_LanguageData = (LanguageData)language.PropertyWindow.Sound;
		m_LanguageData.WindowName =
			language.PropertyWindow.Component.Texts[LanguageData::ComponentName::TEXT_SOUNDWINDOW];
		SetName(m_LanguageData.WindowName);

		for (uInt i = 0; i < LanguageData::TEXT_COUNT; ++i)
			m_Texts[i].SetText(m_LanguageData.Texts[i]);

	}
	void SetScence(Scence* pScence) {
		if (pScence == nullptr)
			Raise("Scence pointer is null.");
		m_pScence = pScence;
	}

private:
	Nt::BoxLayout m_ContentLayout;
	Nt::BoxLayout m_SoundPathLayout;
	Nt::BoxLayout m_SoundPlayerLayout;
	Nt::GridLayout m_ParametersLayout;
	GameSound* m_SelectedSoundPtr = nullptr;
	Scence* m_pScence;
	std::vector<Nt::Text> m_Texts;
	std::vector<Nt::TextEdit> m_TextEdits;
	Nt::Button m_BrowseButton;
	Nt::Button m_DeleteButton;
	Nt::Button m_PlayAtStartButton;
	Nt::Button m_ToggleLoopingButton;
	LanguageData m_LanguageData;
	Style m_Style;

private:
	void _WMPaint([[maybe_unused]] HDC& hdc, [[maybe_unused]] PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		switch (command) {
		case BN_CLICKED:
			switch (id) {
			case BUTTON_BROWSE:
				BrowseSound();
				break;
			case BUTTON_REMOVE:
				if (m_SelectedSoundPtr) {
					m_SelectedSoundPtr->Unload();
					m_TextEdits[TEXTEDIT_SOUND_PATH].SetText("No selected");
				}
				m_DeleteButton.DisableWindow();
				break;
			case BUTTON_PLAY_AT_START:
				if (m_SelectedSoundPtr) {
					if (m_PlayAtStartButton.IsChecked())
						m_SelectedSoundPtr->EnablePlaingAtStart();
					else
						m_SelectedSoundPtr->DisablePlaingAtStart();
				}
				break;
			case BUTTON_TOGGLE_LOOPINT:
				m_SelectedSoundPtr->ToggleLooping(m_ToggleLoopingButton.IsChecked());
				break;
			}
			break;
		case EN_UPDATE:
			if (m_SelectedSoundPtr) {
				switch (id) {
				case TEXTEDIT_SOUND_PATH:
					break;
				case TEXTEDIT_ROLLOFF_FACTOR:
					m_SelectedSoundPtr->SetRolloffFactor(m_TextEdits[id - TEXTEDIT_FIRST].GetText());
					break;
				case TEXTEDIT_REFERENCE_DISNANCE:
					m_SelectedSoundPtr->SetReferenceDistance(m_TextEdits[id - TEXTEDIT_FIRST].GetText());
					break;
				case TEXTEDIT_MAX_DISNANCE:
					m_SelectedSoundPtr->SetMaxDistance(m_TextEdits[id - TEXTEDIT_FIRST].GetText());
					break;
				case TEXTEDIT_GAIN:
					m_SelectedSoundPtr->SetGain(m_TextEdits[id - TEXTEDIT_FIRST].GetText());
					break;
				}
			}
			break;
		}
	}
};