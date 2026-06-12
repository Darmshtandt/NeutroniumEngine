#pragma once

#include <Objects/Entities/GameModel.h>
#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameLight.h>
#include <Objects/Entities/GameSound.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Plane.h>
#include <Objects/Primitives/Quad.h>
#include <Objects/Primitives/Pyramid.h>

class PropertyModel : public PropertyComponent {
public:
	enum Texts {
		TEXT_FILE_PATH,
		TEXT_COUNT
	};

	enum TextEdits {
		TEXTEDIT_MODEL_PATH,
		TEXTEDIT_COUNT
	};
	enum Buttons {
		BUTTON_BROWSE,
		BUTTON_REMOVE,
		BUTTON_COUNT
	};

public:
	PropertyModel(NotNull<Selector*> pSelector, NotNull<Scene*> pScene) noexcept :
		PropertyComponent(pSelector, Texts::TEXT_COUNT, TEXTEDIT_COUNT, BUTTON_COUNT),
		m_pScene(pScene)
	{
		m_RejectedObjects = {
			GameCamera::GetClassToken(),
			GameSound::GetClassToken(),
			GameLight::GetClassToken(),

			Cube::GetClassToken(),
			Plane::GetClassToken(),
			Quad::GetClassToken(),
			Pyramid::GetClassToken(),
			Primitive::GetClassToken(),
		};
	}

	void Initialize(const Settings& settings) {
		m_Padding = { 10, 10, 20, 10 };

		Nt::IntRect windowRect;
		windowRect.LeftTop = m_ClientRect.LeftTop;
		windowRect.Right = settings.PropertyWindowRect.Right;

		_Create(settings, settings.Language["Window.Property.Model"], windowRect);

		m_Texts[TEXT_FILE_PATH].SetPosition(m_Padding.LeftTop);

		Nt::IntRect buttonRect = { };
		buttonRect.Top = m_Padding.Top;
		buttonRect.RightBottom = { 48, 25 };

		Nt::IntRect textEditRect = { };
		textEditRect.Left = 70;
		textEditRect.Top = buttonRect.Top;
		textEditRect.Right = m_ClientRect.Right - textEditRect.Left - buttonRect.Right - m_Padding.Right;
		textEditRect.Bottom = buttonRect.Bottom;

		m_TextEdits[TEXTEDIT_MODEL_PATH].SetParent(*this);
		m_TextEdits[TEXTEDIT_MODEL_PATH].AddStyles(ES_READONLY | STYLE_VISIBLE);
		m_TextEdits[TEXTEDIT_MODEL_PATH].Create(textEditRect, "No selected", true);

		buttonRect.Left = textEditRect.Left + textEditRect.Right;

		m_Buttons[BUTTON_BROWSE].SetParent(*this);
		m_Buttons[BUTTON_BROWSE].AddStyles(BS_CENTER | BS_VCENTER | STYLE_VISIBLE);
		m_Buttons[BUTTON_BROWSE].Create(buttonRect, "...");
		m_Buttons[BUTTON_BROWSE].SetOnClick([&]() { BrowseModel(); });

		buttonRect.Left = m_Padding.Left;
		buttonRect.Top += buttonRect.Bottom + 10;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - m_Padding.Right;
		buttonRect.Bottom = 24;

		m_Buttons[BUTTON_REMOVE].SetParent(*this);
		m_Buttons[BUTTON_REMOVE].AddStyles(BS_CENTER | BS_VCENTER | STYLE_VISIBLE);
		m_Buttons[BUTTON_REMOVE].Create(buttonRect, "Remove model");
		m_Buttons[BUTTON_REMOVE].SetOnClick([&]() { _RemoveModel(); });

		windowRect.Bottom = buttonRect.Top + buttonRect.Bottom;
		windowRect.Bottom += m_Padding.Bottom;

		SetWindowRect(windowRect);

		_DisableWindow();
	}

	void Update() {
		if ((!IsEnabled()) || (!m_SelectorPtr->IsChanged()))
			return;

		const uInt selectedObjectCount = m_SelectorPtr->GetObjectCount();
		if (selectedObjectCount != 1) {
			m_TextEdits[TEXTEDIT_MODEL_PATH].SetText((selectedObjectCount == 0) ? "No selected" : "...");
			m_Buttons[BUTTON_REMOVE].DisableWindow();
			return;
		}

		const NotNull<Nt::Mesh*> pMesh = m_SelectorPtr->GetObjectPtr(0)->GetMesh();

		m_TextEdits[TEXTEDIT_MODEL_PATH].SetText(pMesh->GetFilePath());
		m_Buttons[BUTTON_REMOVE].EnableWindow();
	}

	void BrowseModel() {
		const std::string filePath = _Browse(L"Model (*.obj)\0*.obj\0All (*.*)\0*.*");
		if (filePath.empty())
			return;

		const Nt::Shape shape = Nt::Mesh(filePath).GetShape();

		for (Object* pObject : m_SelectorPtr->GetObjectContainer()) {
			GameModel* pModel = dynamic_cast<GameModel*>(pObject);
			if (pModel != nullptr)
				pModel->SetShape(shape);
		}

		m_TextEdits[TEXTEDIT_MODEL_PATH].SetText(filePath);
		m_Buttons[BUTTON_REMOVE].EnableWindow();
	}

	void SetScene(NotNull<Scene*> pScene) {
		m_pScene = pScene;
	}

	void SetTheme(const Style& style) {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_Texts[TEXT_FILE_PATH].SetColor(style["Property.Texts.Color"]);
		m_Texts[TEXT_FILE_PATH].SetWeight(style["Property.Texts.Weight"]);

		m_TextEdits[TEXTEDIT_MODEL_PATH].SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);
		m_TextEdits[TEXTEDIT_MODEL_PATH].SetTextColor(style["Property.TextEdits.Text.Color"]);
		m_TextEdits[TEXTEDIT_MODEL_PATH].SetTextWeight(style["Property.TextEdits.Text.Weight"]);
	}
	void SetLanguage(const Language& language) {
		SetName(language["Window.Property.Model"]);

		m_Texts[TEXT_FILE_PATH].SetText(language["FilePath"]);
	}

private:
	Nt::IntRect m_Padding;
	Scene* m_pScene;
	inline static PropertyRegistrar<PropertyModel> m_Registrar { "Model" };

private:
	void _RemoveModel() {
		m_TextEdits[TEXTEDIT_MODEL_PATH].SetText("No selected");

		Nt::Float3D cubeSize = { 1.f, 1.f, 1.f };
		Nt::Shape cubeShape = Nt::Primitive::Cube(cubeSize, Nt::Colors::White);

		for (Object* pObject : m_SelectorPtr->GetObjectContainer()) {
			GameModel* pModel = dynamic_cast<GameModel*>(pObject);
			if (pModel == nullptr)
				continue;

			if (cubeSize != pModel->GetSize()) {
				cubeSize = pModel->GetSize();
				cubeShape = Nt::Primitive::Cube(cubeSize, Nt::Colors::White);
			}

			pModel->SetShape(cubeShape);
		}

		m_Buttons[BUTTON_REMOVE].DisableWindow();
	}

	void _Paint([[maybe_unused]] HDC& hdc, [[maybe_unused]] PAINTSTRUCT& paint) override {
		m_Texts[TEXT_FILE_PATH].Draw(*this);
	}
};