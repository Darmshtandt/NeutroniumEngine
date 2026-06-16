#pragma once

#include <Objects/Entities/GameModel.h>
#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameLight.h>
#include <Objects/Entities/GameSound.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Plane.h>
#include <Objects/Primitives/Quad.h>
#include <Objects/Primitives/Pyramid.h>

class PropertyPrimitive : public PropertyComponent {
public:
	enum Buttons {
		BUTTON_JOIN,
		BUTTON_SPLIT,
		BUTTON_INVISIBLE,
		BUTTON_COUNT,
	};

public:
	PropertyPrimitive(NotNull<Selector*> pSelector, NotNull<Scene*> pScene) noexcept :
		PropertyComponent(pSelector, 0, 0, BUTTON_COUNT),
		m_ScenePtr(pScene)
	{
		m_RejectedObjects = {
			GameCamera::GetClassToken(),
			GameSound::GetClassToken(),
			GameLight::GetClassToken()
		};
	}

	void Initialize(const Settings& settings) override {
		m_PaddingRect = { 5, 5, 10, 10 };

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = 200;

		_Create(settings, settings.Language["Window.Property.Primitive"], windowRect);
		SetBackgroundColor(settings.Style["Property.BackgroundColor"]);

		auto setupButtons = [&](Nt::Button& button, const Nt::IntRect& buttonRect, const std::string& name) {
			button.SetParent(*this);
			button.AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | STYLE_VISIBLE);
			button.Create(buttonRect, name);
		};

		Nt::IntRect buttonRect;
		buttonRect.LeftTop = m_PaddingRect.LeftTop;
		buttonRect.Right = windowRect.Right;
		buttonRect.Bottom = 24;

		setupButtons(m_Buttons[BUTTON_JOIN], buttonRect, settings.Language["Window.Property.Primitive.Join"]);

		buttonRect.Top += buttonRect.Bottom + 1;
		setupButtons(m_Buttons[BUTTON_SPLIT], buttonRect, settings.Language["Window.Property.Primitive.Split"]);

		buttonRect.Top += buttonRect.Bottom + 1;
		setupButtons(m_Buttons[BUTTON_INVISIBLE], buttonRect, settings.Language["Window.Property.Primitive.Invisible"]);


		windowRect.Bottom = buttonRect.Top + buttonRect.Bottom + 1;
		windowRect.Bottom += m_PaddingRect.Bottom;

		SetSize(windowRect.RightBottom);

		_DisableWindow();
	}

	void Update() override {
		if (!IsEnabled())
			return;

		if (!m_SelectorPtr->IsChanged())
			return;

		if (m_SelectorPtr->IsEmpty()) {
			_DisableWindow();
			return;
		}

		m_Buttons[BUTTON_INVISIBLE].SetCheck(m_SelectorPtr->GetObjectPtr(0).lock()->IsInvisible());
	}

	void SetScene(NotNull<Scene*> pScene) {
		m_ScenePtr = pScene;
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.Primitive"]);

		m_Buttons[BUTTON_JOIN].SetName(language["Window.Property.Primitive.Join"]);
		m_Buttons[BUTTON_SPLIT].SetName(language["Window.Property.Primitive.Split"]);
		m_Buttons[BUTTON_INVISIBLE].SetName(language["Window.Property.Primitive.Invisible"]);
	}

private:
	Nt::IntRect m_PaddingRect;
	Scene* m_ScenePtr = nullptr;
	inline static PropertyRegistrar<PropertyPrimitive> m_Registrar { "Primitive" };

private:
	void _ButtonsNotification_OnClick(const uInt& id, [[maybe_unused]] const HWND& handle) override {
		switch (id) {
		case BUTTON_JOIN:
			break;

		case BUTTON_SPLIT:
			break;

		case BUTTON_INVISIBLE:
			for (const WeakObjectPtr& weakObject : m_SelectorPtr->GetObjectContainer()) {
				const auto object = weakObject.lock();
				if (object == nullptr)
					continue;

				if (m_Buttons[id].IsChecked())
					object->EnableInvisible();
				else
					object->DisableInvisible();
			}

			break;
		}
	}
};