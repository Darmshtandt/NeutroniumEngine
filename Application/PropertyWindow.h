#pragma once

#include <PropertyComponent.h>
#include <PropertyTransform.h>
#include <PropertyTexture.h>
#include <PropertyScript.h>
#include <PropertyRigidBody.h>
#include <PropertyPrimitive.h>
#include <PropertySound.h>
#include <PropertyModel.h>

class PropertyWindow : private Nt::Window {
public:
	enum Components {
		COMPONENT_TRANSFORM,
		COMPONENT_TEXTURE,
		COMPONENT_SCRIPT,
		COMPONENT_RIGIDBODY,
		COMPONENT_PRIMITIVE,
		COMPONENT_SOUND,
		COMPONENT_MODEL,
	};

public:
	PropertyWindow() noexcept : 
		m_SelectorPtr(nullptr),
		m_IsInitialized(false)
	{ 
	}
	~PropertyWindow() {
		for (PropertyComponent* pComponent : m_Components)
			SAFE_DELETE(&pComponent);
	}

	void Initialize(const Settings& settings, const Nt::String& rootPath, Selector* selectorPtr, Scence* pScence) {
		if (!std::exists(std::path(rootPath)))
			Raise("Root folder path is not valid.");
		if (selectorPtr == nullptr)
			Raise("Selector pointer is null.");
		if (pScence == nullptr)
			Raise("Scence pointer is null.");
		
		Create(settings.PropertyWindowRect, settings.CurrentLanguage.Window.PropertyWindow);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_DLGFRAME);
		SetBackgroundColor(settings.Styles.Property.BackgroundColor);

		PropertyScript* pPropertyScript = new PropertyScript;
		pPropertyScript->SetScence(pScence);

		PropertyPrimitive* pPropertyPrimitive = new PropertyPrimitive;
		pPropertyPrimitive->SetScence(pScence);

		m_Components.push_back(new PropertyTransform);
		m_Components.push_back(new PropertyTexture);
		m_Components.push_back(pPropertyScript);
		m_Components.push_back(new PropertyRigidBody);
		m_Components.push_back(pPropertyPrimitive);
		m_Components.push_back(new PropertySound);
		m_Components.push_back(new PropertyModel);

		m_SelectorPtr = selectorPtr;
		m_RootPath = rootPath;

		Int freeY = 0;
		for (PropertyComponent* pComponent : m_Components) {
			pComponent->SetPosition({ 0, freeY });
			pComponent->Initialize(settings);
			pComponent->SetParent(*this);
			pComponent->SetRootPath(m_RootPath);
			pComponent->SetSelector(m_SelectorPtr);
			pComponent->Show();

			const Nt::IntRect rect = pComponent->GetClientRect();
			freeY = rect.Top + rect.Bottom;
		}

		m_IsInitialized = true;
	}

	Bool PopEvent(Nt::Event* pEvent) {
		if (!Window::PopEvent(pEvent)) {
			for (PropertyComponent* pComponent : m_Components)
				if (pComponent->PopEvent(pEvent))
					return true;
			return false;
		}
		return true;
	}

	void EnableCompotent(const Components& component, const Bool& isEnabled) {
		if (!m_IsInitialized)
			Raise("PropertyWindow is not initialized");

		if (m_Components[component]->IsEnabled() != isEnabled) {
			if (isEnabled)
				m_Components[component]->Enable();
			else
				m_Components[component]->Disable();
		}
	}

	void Update() {
		if (m_SelectorPtr == nullptr)
			Raise("Selection pointer is null.");

		if (m_SelectorPtr->IsChanged()) {
			if (m_SelectorPtr->GetObjects().size() == 0) {
				EnableCompotent(COMPONENT_TEXTURE, false);
				EnableCompotent(COMPONENT_SCRIPT, false);
				EnableCompotent(COMPONENT_RIGIDBODY, false);
				EnableCompotent(COMPONENT_PRIMITIVE, false);
				EnableCompotent(COMPONENT_SOUND, false);
				EnableCompotent(COMPONENT_MODEL, false);
				return;
			}

			Bool modelPropEnabled = false;
			Bool soundPropEnabled = false;
			Bool primitivePropEnabled = true;
			Bool texturePropEnabled = true;
			for (const Object* pObject : m_SelectorPtr->GetObjects()) {
				if (pObject == nullptr)
					Raise("Object pointer is null.");

				switch (pObject->ObjectType) {
				case ObjectTypes::PRIMITIVE:
					break;
				case ObjectTypes::ENTITY:
				{
					const Entity* pEntity = dynamic_cast<const Entity*>(pObject);
					if (pEntity == nullptr)
						Raise("Failed to upcast entity.");

					soundPropEnabled = (pEntity->GetEntityType() == EntityTypes::SOUND);
					modelPropEnabled = (pEntity->GetEntityType() == EntityTypes::MODEL);
					primitivePropEnabled = texturePropEnabled = modelPropEnabled;
				}
					break;
				default:
					Raise("Non-existent object type specified");
				}
			}

			EnableCompotent(COMPONENT_TEXTURE, texturePropEnabled);
			EnableCompotent(COMPONENT_SCRIPT, true);
			EnableCompotent(COMPONENT_RIGIDBODY, true);
			EnableCompotent(COMPONENT_PRIMITIVE, primitivePropEnabled);
			EnableCompotent(COMPONENT_SOUND, soundPropEnabled);
			EnableCompotent(COMPONENT_MODEL, modelPropEnabled);

			Int freeY = 0;
			for (PropertyComponent* pComponent : m_Components) {
				if (!pComponent->IsEnabled())
					continue;

				pComponent->SetPosition({ 0, freeY });
				pComponent->Update();

				const Nt::IntRect rect = pComponent->GetClientRect();
				freeY += rect.Bottom;
			}
		}
		m_SelectorPtr->UnmarkChanged();
	}

	void SetTheme(const Style& style) {
		if (!m_IsInitialized)
			Raise("PropertyWindow is not initialized");

		SetBackgroundColor(style.Property.BackgroundColor);
		for (PropertyComponent* pComponent : m_Components)
			pComponent->SetTheme(style);
	}
	void SetLanguage(const Language& language) {
		if (!m_IsInitialized)
			Raise("PropertyWindow is not initialized");

		for (PropertyComponent* pComponent : m_Components) {
			pComponent->SetLanguage(language);
			InvalidateRect(pComponent->GetHandle(), nullptr, TRUE);
		}
	}

public:
	using Window::Show;
	using Window::SetParent;
	using Window::SetWindowRect;

private:
	std::vector<PropertyComponent*> m_Components;
	Nt::String m_RootPath;
	Selector* m_SelectorPtr;
	Bool m_IsInitialized;

private:
	void _WMPaint(HDC& hdc, PAINTSTRUCT& paint) override {
	}
	void _WMCommand(const Long& param_1, const Long& param_2) override {
	}
};