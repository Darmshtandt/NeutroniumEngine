#pragma once

class PropertyModel : public PropertyComponent {
public:
	enum {
		TEXTEDIT_MODEL_PATH = 5500,
		BUTTON_BROWSE,
		BUTTON_REMOVE,
	};

public:
	PropertyModel() = default;

	void Initialize(const Settings& settings) {
		m_Language = settings.CurrentLanguage;
		m_Style = settings.Styles;
		m_Padding = { 10, 10, 20, 10 };

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;

		Create(windowRect, m_Language.Window.PropertyComponent.ModelWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_BORDER);
		SetBackgroundColor(m_Style.Property.BackgroundColor);

		m_ModelText.SetPosition(m_Padding.LeftTop);
		m_ModelText.SetColor(settings.Styles.Property.Texts.Color);
		m_ModelText.SetWeight(settings.Styles.Property.Texts.Weight);
		m_ModelText.SetText(m_Language.Window.PropertyModel.Model);

		Nt::IntRect buttonRect = { };
		buttonRect.Top = m_Padding.Top;
		buttonRect.RightBottom = { 48, 25 };

		Nt::IntRect textEditRect = { };
		textEditRect.Left = 70;
		textEditRect.Top = buttonRect.Top;
		textEditRect.Right = m_ClientRect.Right - textEditRect.Left - buttonRect.Right - m_Padding.Right;
		textEditRect.Bottom = buttonRect.Bottom;

		m_ModelPathTextEdit.SetParent(*this);
		m_ModelPathTextEdit.SetID(TEXTEDIT_MODEL_PATH);
		m_ModelPathTextEdit.AddStyles(ES_READONLY);
		m_ModelPathTextEdit.Create(textEditRect, "No selected", true);
		m_ModelPathTextEdit.SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
		m_ModelPathTextEdit.SetTextColor(m_Style.Property.TextEdits.Text.Color);
		m_ModelPathTextEdit.SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
		m_ModelPathTextEdit.Show();

		buttonRect.Left = textEditRect.Left + textEditRect.Right;
		m_BrowseButton.SetParent(*this);
		m_BrowseButton.SetID(BUTTON_BROWSE);
		m_BrowseButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_BrowseButton.Create(buttonRect, "...");
		m_BrowseButton.Show();

		buttonRect.Left = m_Padding.Left;
		buttonRect.Top += buttonRect.Bottom + 10;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - m_Padding.Right;
		buttonRect.Bottom = 24;
		m_DeleteButton.SetParent(*this);
		m_DeleteButton.SetID(BUTTON_REMOVE);
		m_DeleteButton.AddStyles(BS_CENTER | BS_VCENTER);
		m_DeleteButton.DisableWindow();
		m_DeleteButton.Create(buttonRect, "Remove model");
		m_DeleteButton.Show();

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
			const Nt::Mesh* pMesh = pObject->GetModel().GetMeshPtr();
			if (pMesh != nullptr) {
				m_ModelPathTextEdit.SetText(pMesh->GetFilePath());
				m_DeleteButton.EnableWindow();
				return;
			}
		}

		m_ModelPathTextEdit.SetText((selectedObjectCount > 1) ? "..." : "No selected");
		m_DeleteButton.DisableWindow();
	}

	void BrowseModel() {
		const uInt selectedObjectCount = m_SelectorPtr->GetObjects().size();
		if (m_SelectorPtr && selectedObjectCount > 0) {
			const cwString filter = L"Model (*.obj)\0*.obj\0All (*.*)\0*.*";

			Nt::String filePath = Nt::OpenFileDialog(GetRootPath().wstr().c_str(), filter);
			if (filePath.size() == 0)
				return;

			if (!IsValidPath(GetRootPath(), filePath)) {
				WarningBox(m_Language.Messages.AddingFile.wstr().c_str(), L"Warning");
				return;
			}
			filePath.erase(filePath.begin(), filePath.begin() + GetRootPath().length() + 1);

			Nt::Mesh mesh;
			mesh.LoadFromFile(filePath);

			Nt::Model model(mesh);
			for (Object* pObject : m_SelectorPtr->GetObjects()) {
				Nt::Texture* pTexture = pObject->GetModel().GetTexturePtr();
				if (pTexture)
					model.SetTexture(*pTexture);
				pObject->SetModel(model);
			}

			m_ModelPathTextEdit.SetText(filePath);
			m_DeleteButton.EnableWindow();
		}
	}

	void SetTheme(const Style& style) {
		m_Style = style;

		SetBackgroundColor(m_Style.Property.BackgroundColor);
		m_ModelText.SetColor(m_Style.Property.Texts.Color);
		m_ModelText.SetWeight(m_Style.Property.Texts.Weight);

		m_ModelPathTextEdit.SetBackgroundColor(m_Style.Property.TextEdits.BackgroundColor);
		m_ModelPathTextEdit.SetTextColor(m_Style.Property.TextEdits.Text.Color);
		m_ModelPathTextEdit.SetTextWeight(m_Style.Property.TextEdits.Text.Weight);
	}
	void SetLanguage(const Language& language) {
		m_Language = language;
		m_ModelText.SetText(m_Language.Window.PropertyModel.Model);
	}
	void SetScence(Scence* pScence) {
		if (pScence == nullptr)
			Raise("Scence pointer is null.");
		m_pScence = pScence;
	}

private:
	Scence* m_pScence;
	Nt::Text m_ModelText;
	Nt::TextEdit m_ModelPathTextEdit;
	Nt::Button m_BrowseButton;
	Nt::Button m_DeleteButton;
	Nt::IntRect m_Padding;
	Language m_Language;
	Style m_Style;

private:
	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
		m_ModelText.Draw(*this);
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
		const uInt id = LOWORD(param_1);
		const uInt command = HIWORD(param_1);

		switch (command) {
		case BN_CLICKED:
			switch (id) {
			case BUTTON_BROWSE:
				BrowseModel();
				break;
			case BUTTON_REMOVE:
				if (m_SelectorPtr) {
					m_ModelPathTextEdit.SetText("No selected");
					Nt::Float3D cubeSize = { 1.f, 1.f, 1.f };
					Nt::Mesh cube = Nt::Geometry::Cube(cubeSize, Nt::Colors::White);
					for (Object* pObject : m_SelectorPtr->GetObjects()) {
						if (cubeSize != pObject->GetSize()) {
							cubeSize = pObject->GetSize();
							cube = Nt::Geometry::Cube(cubeSize, Nt::Colors::White);
						}
						const Nt::Texture* pTexture = pObject->GetModel().GetTexturePtr();
						if (pTexture)
							pObject->SetModel(Nt::Model(cube, *pTexture));
						else
							pObject->SetModel(Nt::Model(cube));
					}
				}
				m_DeleteButton.DisableWindow();
				break;
			}
			break;
		case EN_UPDATE:
			break;
		}
	}
};