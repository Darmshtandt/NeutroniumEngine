#pragma once

class PropertySound : public PropertyComponent {
public:
	enum {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
		BUTTON_PLAY_AT_START,
		BUTTON_TOGGLE_LOOPINT,
	};
	enum {
		TEXTEDIT_FIRST = 5000,
		TEXTEDIT_SOUND_PATH = TEXTEDIT_FIRST,
		TEXTEDIT_ROLLOFF_FACTOR,
		TEXTEDIT_REFERENCE_DISNANCE,
		TEXTEDIT_MAX_DISNANCE,
		TEXTEDIT_GAIN,
		TEXTEDIT_COUNT,
	};

public:
	PropertySound() = default;

	void Initialize(const Settings& settings) {
		m_Language = settings.CurrentLanguage;
		m_Style = settings.Styles;
		m_Padding = { 10, 10, 20, 10 };

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;

		Create(windowRect, m_Language.Window.PropertyComponent.SoundWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		m_Texts.resize(TEXTEDIT_COUNT - TEXTEDIT_FIRST);
		m_TextEdits.resize(TEXTEDIT_COUNT - TEXTEDIT_FIRST);

		std::wstring texts[] = {
			m_Language.Window.PropertySound.Sound,
			m_Language.Window.PropertySound.RolloffFactor,
			m_Language.Window.PropertySound.ReferenceDistance,
			m_Language.Window.PropertySound.MaxDistance,
			m_Language.Window.PropertySound.Gain,
		};

		for (uInt i = 0; i < TEXTEDIT_COUNT - TEXTEDIT_FIRST; ++i) {
			m_Texts[i].SetColor(settings.Styles.Property.Texts.Color);
			m_Texts[i].SetWeight(settings.Styles.Property.Texts.Weight);
			m_Texts[i].SetText(texts[i]);

			m_TextEdits[i].SetParent(*this);
			m_TextEdits[i].SetID(i + TEXTEDIT_FIRST);
			if (TEXTEDIT_FIRST + i == TEXTEDIT_SOUND_PATH)
				m_TextEdits[i].AddStyles(ES_READONLY);
			m_TextEdits[i].Create({ 0, 0, 1, 1 }, "", true);
			m_TextEdits[i].SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
			m_TextEdits[i].SetTextColor(m_Style.Property.TextEdits.Text.Color);
			m_TextEdits[i].SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
			m_TextEdits[i].Show();
		}

		Nt::IntRect buttonRect = { };
		buttonRect.Top = m_Padding.Top;
		buttonRect.RightBottom = { 24, 24 };

		Nt::IntRect textEditRect = { };
		textEditRect.Left = 150;
		textEditRect.Top = buttonRect.Top;
		textEditRect.Right = m_ClientRect.Right - textEditRect.Left - buttonRect.Right - m_Padding.Right;
		textEditRect.Bottom = buttonRect.Bottom;

		Nt::Float2D textPosition = m_Padding.LeftTop;
		m_Texts[TEXTEDIT_SOUND_PATH - TEXTEDIT_FIRST].SetPosition(textPosition);
		m_TextEdits[TEXTEDIT_SOUND_PATH - TEXTEDIT_FIRST].SetWindowRect(textEditRect);

		buttonRect.Left = textEditRect.Left + textEditRect.Right;
		m_BrowseButton.SetParent(*this);
		m_BrowseButton.SetID(BUTTON_BROWSE);
		m_BrowseButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_BrowseButton.Create(buttonRect, "...");
		m_BrowseButton.Show();

		textEditRect.Right += buttonRect.Right;
		for (uInt i = 1; i < TEXTEDIT_COUNT - TEXTEDIT_FIRST; ++i) {
			textPosition.y += textEditRect.Bottom + 10;
			textEditRect.Top += textEditRect.Bottom + 10;
			m_Texts[i].SetPosition(textPosition);
			m_TextEdits[i].SetWindowRect(textEditRect);
		}

		buttonRect.Left = m_Padding.Left;
		buttonRect.Top += textEditRect.Top + textEditRect.Bottom + 10;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - m_Padding.Right;
		buttonRect.Bottom = 24;
		m_DeleteButton.SetParent(*this);
		m_DeleteButton.SetID(BUTTON_REMOVE);
		m_DeleteButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_DeleteButton.DisableWindow();
		m_DeleteButton.Create(buttonRect, "Remove sound");
		m_DeleteButton.Show();

		buttonRect.Top += buttonRect.Bottom + 10;
		buttonRect.Right = (m_ClientRect.Right - buttonRect.Left - m_Padding.Right) / 2;
		m_PlayAtStartButton.SetParent(*this);
		m_PlayAtStartButton.SetID(BUTTON_PLAY_AT_START);
		m_PlayAtStartButton.AddStyles(BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX);
		m_PlayAtStartButton.DisableWindow();
		m_PlayAtStartButton.Create(buttonRect, "Play at start");
		m_PlayAtStartButton.Show();

		buttonRect.Left = buttonRect.Right;
		m_ToggleLoopingButton.SetParent(*this);
		m_ToggleLoopingButton.SetID(BUTTON_TOGGLE_LOOPINT);
		m_ToggleLoopingButton.AddStyles(BS_CENTER | BS_CHECKBOX | BS_AUTOCHECKBOX);
		m_ToggleLoopingButton.DisableWindow();
		m_ToggleLoopingButton.Create(buttonRect, "Loop");
		m_ToggleLoopingButton.Show();

		windowRect.Bottom = buttonRect.Top + buttonRect.Bottom;
		windowRect.Bottom += m_Padding.Bottom;
		SetWindowRect(windowRect);
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
				WarningBox(m_Language.Messages.AddingFile.wstr().c_str(), L"Warning");
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
		m_Language = language;
		std::wstring texts[] = {
			m_Language.Window.PropertySound.Sound,
			m_Language.Window.PropertySound.RolloffFactor,
			m_Language.Window.PropertySound.ReferenceDistance,
			m_Language.Window.PropertySound.MaxDistance,
			m_Language.Window.PropertySound.Gain,
		};

		for (uInt i = 0; i < TEXTEDIT_COUNT - TEXTEDIT_FIRST; ++i)
			m_Texts[i].SetText(texts[i]);

	}
	void SetScence(Scence* pScence) {
		if (pScence == nullptr)
			Raise("Scence pointer is null.");
		m_pScence = pScence;
	}

private:
	GameSound* m_SelectedSoundPtr = nullptr;
	Scence* m_pScence;
	std::vector<Nt::Text> m_Texts;
	std::vector<Nt::TextEdit> m_TextEdits;
	Nt::Button m_BrowseButton;
	Nt::Button m_DeleteButton;
	Nt::Button m_PlayAtStartButton;
	Nt::Button m_ToggleLoopingButton;
	Nt::IntRect m_Padding;
	Language m_Language;
	Style m_Style;

private:
	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
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