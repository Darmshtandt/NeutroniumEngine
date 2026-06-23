#pragma once

#include <Tools/PropertyComponent.h>

#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameLight.h>
#include <Objects/Entities/GameSound.h>
#include <Nt/Graphics/Layouts.h>

class PropertyRigidBody : public PropertyComponent {
public:
	enum Texts {
		TEXT_MASS,
		TEXT_GRAVITY_DIRECTION,
		TEXT_FRICTION,
		TEXT_COUNT,
	};
	
	enum Buttons {
		BUTTON_ACTIVE,
		BUTTON_COLLISION,
		BUTTON_GRAVITATION,
		BUTTON_SHOW_COLLIDER,
		BUTTON_COUNT,
	};

	enum TextEdits {
		TEXTEDIT_MASS,
		TEXTEDIT_GRAVITY_DIRECTION,
		TEXTEDIT_FRICTION,
		TEXTEDIT_COUNT,
	};

public:
	PropertyRigidBody(NotNull<Selector*> pSelector, Scene*) noexcept :
		PropertyComponent(pSelector, TEXT_COUNT, TEXTEDIT_COUNT, BUTTON_COUNT),
		m_ContentLayout(2),
		m_ButtonsLayout({ 2, 2 }),
		m_ParametersLayout({ 2, 3 })
	{
		m_RejectedObjects = {
			GameCamera::GetClassToken(),
			GameSound::GetClassToken(),
			GameLight::GetClassToken(),
		};
	}

	void Initialize(const Settings& settings) override {
		constexpr Nt::IntRect contentPadding = { 10, 10, 10, 10 };
		constexpr Int contentGap = 10;

		const Int cellsHeight = Nt::TextEdit::DefaultSize.y;

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right;
		windowRect.Bottom = (contentPadding.Top + contentPadding.Bottom);
		windowRect.Bottom += (cellsHeight * Int(std::roundf(Float(BUTTON_COUNT) / 2.f)));
		windowRect.Bottom += contentGap + (cellsHeight * TEXTEDIT_COUNT);

		_Create(settings, settings.Language["Window.Property.RigidBody"], windowRect);

		const Nt::IntRect contentLayoutRect = {
			windowRect.Left, 
			windowRect.Top,
			windowRect.Right - GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXDLGFRAME) * 2,
			windowRect.Bottom - GetSystemMetrics(SM_CYBORDER) * 2
		};

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.TogleVertical(true);
		m_ContentLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(contentLayoutRect, "RigidBody-content-layout");
		m_ContentLayout.SetPadding(contentPadding, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetGap(contentGap, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.SetCellSize(0, 0.5f, Nt::UnitType::UNIT_PERCENTAGE);
		m_ContentLayout.Show();

		m_ContentLayout.Insert(0, &m_ButtonsLayout);
		m_ButtonsLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ButtonsLayout.Create("RigidBody-Buttons-layout");
		m_ButtonsLayout.Show();

		m_ContentLayout.Insert(1, &m_ParametersLayout);
		m_ParametersLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ParametersLayout.Create("RigidBody-Parameters-layout");
		m_ParametersLayout.Show();

		const std::string buttonTexts[BUTTON_COUNT] = {
			settings.Language["Window.Property.RigidBody.Active"],
			settings.Language["Window.Property.RigidBody.CollisionDetect"],
			settings.Language["Window.Property.RigidBody.Gravity"],
			settings.Language["Window.Property.RigidBody.ShowCollider"]
		};

		for (uInt i = 0; i < BUTTON_COUNT; ++i) {
			m_ButtonsLayout.Insert({ i % 2, i / 2 }, &m_Buttons[i]);

			m_Buttons[i].AddStyles(BS_CHECKBOX | BS_AUTOCHECKBOX | STYLE_VISIBLE);
			m_Buttons[i].Create(buttonTexts[i]);
		}

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_ParametersLayout.Insert({ 0, i }, &m_Texts[i + TEXT_MASS]);
			m_ParametersLayout.Insert({ 1, i }, &m_TextEdits[i]);

			m_TextEdits[i].Create("0.0", true);
			m_TextEdits[i].Show();
		}
	}
	void Update() override {
		if (!IsEnabled())
			return;

		if (m_SelectorPtr->GetObjectCount() != 1) {
			_DisableWindow();
			return;
		}

		_EnableWindow();

		if (!m_SelectorPtr->IsChanged())
			return;

		const auto& object = m_SelectorPtr->GetObjectPtr(0).lock();

		m_TextEdits[TEXTEDIT_MASS].SetText(object->GetRigidBody()->Body.GetMass());
		//m_TextEdits[TEXTEDIT_FRICTION].SetText(object->GetFriction());

		//m_Buttons[BUTTON_ACTIVE].SetCheck(object->IsPhysicsEnabled());
		m_Buttons[BUTTON_COLLISION].SetCheck(object->EnabledCollider());
		m_Buttons[BUTTON_GRAVITATION].SetCheck(object->EnabledGravitation());
		m_Buttons[BUTTON_SHOW_COLLIDER].SetCheck(object->GetCollider()->IsVisible());
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_ContentLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_ButtonsLayout.SetBackgroundColor(style["Property.BackgroundColor"]);
		m_ParametersLayout.SetBackgroundColor(style["Property.BackgroundColor"]);

		for (uInt i = 0; i < TEXTEDIT_COUNT; ++i) {
			m_TextEdits[i].SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);
			m_TextEdits[i].SetTextColor(style["Property.TextEdits.Text.Color"]);
			m_TextEdits[i].SetTextWeight(style["Property.TextEdits.Text.Weight"]);
		}

		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			m_Texts[i].SetColor(style["Property.Texts.Color"]);
			m_Texts[i].SetWeight(style["Property.Texts.Weight"]);
		}
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.RigidBody"]);

		m_Buttons[BUTTON_ACTIVE].SetName(language["Window.Property.RigidBody.Active"]);
		m_Buttons[BUTTON_COLLISION].SetName(language["Window.Property.RigidBody.CollisionDetect"]);
		m_Buttons[BUTTON_GRAVITATION].SetName(language["Window.Property.RigidBody.Gravity"]);
		m_Buttons[BUTTON_SHOW_COLLIDER].SetName(language["Window.Property.RigidBody.ShowCollider"]);

		m_Texts[TEXT_MASS].SetText(language["Window.Property.RigidBody.Mass"]);
		m_Texts[TEXT_GRAVITY_DIRECTION].SetText(language["Window.Property.RigidBody.GravityDirection"]);
		m_Texts[TEXT_FRICTION].SetText(language["Window.Property.RigidBody.Friction"]);
	}

private:
	Nt::BoxLayout m_ContentLayout;
	Nt::GridLayout m_ButtonsLayout;
	Nt::GridLayout m_ParametersLayout;
	inline static PropertyRegistrar<PropertyRigidBody> m_Registrar { "RigidBody" };

private:
	void _AddSelection(Object* pObject) override {
		UpdateUI(pObject, true);
		PropertyComponent::_AddSelection(pObject);
	}

	void _Paint(HDC& hdc, PAINTSTRUCT& paint) override {
		(void)hdc;
		(void)paint;

		for (Nt::Text& text : m_Texts)
			text.Draw(*this);
	}

	void _HandleNotification(const uInt& command, const uInt& id, const HWND& handle) override {
		(void)handle;

		try {
			for (const WeakObjectPtr& weakObject : m_SelectorPtr->GetObjectContainer()) {
				const auto object = weakObject.lock();
				if (object == nullptr)
					continue;

				switch (command) {
				case BN_CLICKED:
					_ButtonsNotification_OnClick(id, reinterpret_cast<HWND>(object.get()));
					break;

				case EN_UPDATE:
					_TextEditsNotification_Update(id, reinterpret_cast<HWND>(object.get()));
					break;
				}
			}
		}
		catch (...)
		{
		}
	}

	void _ButtonsNotification_OnClick(const uInt& id, const HWND& param) override {
		Object* pObject = RequireNotNull(reinterpret_cast<Object*>(param));

		switch (id) {
		case BUTTON_ACTIVE:
			assert(0);
			break;

		case BUTTON_COLLISION:
			pObject->ToggleCollider(m_Buttons[BUTTON_COLLISION].IsChecked());
			break;

		case BUTTON_GRAVITATION:
			pObject->ToggleGravitation(m_Buttons[BUTTON_GRAVITATION].IsChecked());
			break;

		case BUTTON_SHOW_COLLIDER:
			if (m_Buttons[BUTTON_SHOW_COLLIDER].IsChecked())
				pObject->ShowingCollider();
			else
				pObject->HidingCollider();
			break;
		}
	}

	void _TextEditsNotification_Update(const uInt& id, const HWND& param) override {
		Object* pObject = RequireNotNull(reinterpret_cast<Object*>(param));

		switch (id) {
		case TEXTEDIT_MASS:
			pObject->GetRigidBody()->Body.SetMass(m_TextEdits[id].GetText());
			break;

		case TEXTEDIT_GRAVITY_DIRECTION:
			assert(0);
			break;

		case TEXTEDIT_FRICTION:
			assert(0);
			//pObject->SetFriction(m_TextEdits[id].GetText());
			break;
		}
	}

	void UpdateUI(Object* pObject, Bool fForce) {
		if (!pObject->IsDirty() && !fForce)
			return;

		//m_Buttons[BUTTON_ACTIVE].SetCheck(pObject->IsActivePhysics());
		m_Buttons[BUTTON_COLLISION].SetCheck(pObject->EnabledCollider());
		m_Buttons[BUTTON_GRAVITATION].SetCheck(pObject->EnabledGravitation());
		m_Buttons[BUTTON_SHOW_COLLIDER].SetCheck(pObject->GetCollider()->IsVisible());

		m_TextEdits[TEXTEDIT_MASS].SetText(pObject->GetRigidBody()->Body.GetMass());
		//m_TextEdits[TEXTEDIT_FRICTION].SetText(pObject->GetFriction());
	}
};