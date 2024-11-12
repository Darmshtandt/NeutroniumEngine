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

	void Initialize(const Settings& settings, const Nt::String& rootPath, Selector* selectorPtr, Scene* pScence) {
		if (!std::exists(std::path(rootPath)))
			Raise("Root folder path is not valid.");
		else if (selectorPtr == nullptr)
			Raise("Selector pointer is null.");
		else if (pScence == nullptr)
			Raise("Scence pointer is null.");
		
		Create(settings.PropertyWindowRect, settings.CurrentLanguage.Window.Texts[Language::_WindowNames::TEXT_PROPERTYWINDOW]);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_DLGFRAME);
		SetBackgroundColor(settings.Styles.Property.BackgroundColor);

		m_Components.push_back(new PropertyTransform);
		m_Components.push_back(new PropertyTexture);
		m_Components.push_back(new PropertyScript(pScence));
		m_Components.push_back(new PropertyRigidBody);
		m_Components.push_back(new PropertyPrimitive(pScence));
		m_Components.push_back(new PropertySound);
		m_Components.push_back(new PropertyModel);

		m_SelectorPtr = selectorPtr;
		m_RootPath = rootPath;

		for (PropertyComponent* pComponent : m_Components) {
			pComponent->Initialize(settings);
			pComponent->SetParent(*this);
			pComponent->SetRootPath(m_RootPath);
			pComponent->SetSelector(m_SelectorPtr);
			pComponent->Show();
		}

		m_IsInitialized = true;
	}

	Bool PeekMessages(Nt::Event* pEvent) {
		if (!Window::PeekMessages(pEvent)) {
			for (PropertyComponent* pComponent : m_Components) {
				if (pComponent->PeekMessages(pEvent))
					return true;
			}

			return false;
		}

		return true;
	}

	void EnableCompotent(const Components& component, const Bool& isEnabled) {
		if (!m_IsInitialized) {
			Raise("PropertyWindow is not initialized");
			return;
		}

		if (m_Components[component]->IsEnabled() != isEnabled) {
			if (isEnabled)
				m_Components[component]->Enable();
			else
				m_Components[component]->Disable();
		}
	}

	void Update() {
		if (m_SelectorPtr == nullptr) {
			Raise("Selection pointer is null.");
			return;
		}

		if (!m_SelectorPtr->IsChanged())
			return;

		if (m_SelectorPtr->IsEmpty()) {
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

		for (const Object* pObject : m_SelectorPtr->GetObjectContaiter()) {
			if (pObject == nullptr) {
				Raise("Null object selected");
				return;
			}

			switch (pObject->ObjectType) {
			case ObjectTypes::PRIMITIVE:
				break;

			case ObjectTypes::ENTITY: {
				const Entity* pEntity = UpcastObjectToEntity(const_cast<Object*>(pObject));

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

			freeY += pComponent->GetWindowRect().Bottom;
		}

		m_SelectorPtr->UnmarkChanged();
	}

	void SetTheme(const Style& style) {
		if (!m_IsInitialized) {
			Raise("PropertyWindow is not initialized");
			return;
		}

		SetBackgroundColor(style.Property.BackgroundColor);

		for (PropertyComponent* pComponent : m_Components)
			pComponent->SetTheme(style);
	}
	void SetLanguage(const Language& language) {
		if (!m_IsInitialized) {
			Raise("PropertyWindow is not initialized");
			return;
		}

		SetName(language.Window.Texts[Language::_WindowNames::TEXT_PROPERTYWINDOW]);

		for (PropertyComponent* pComponent : m_Components) {
			pComponent->SetLanguage(language);
			pComponent->InvalidateRect(nullptr, true);
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
};