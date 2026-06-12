#pragma once

#include <Tools/PropertyComponent.h>
#include <Tools/PropertyTransform.h>
#include <Tools/PropertyTexture.h>
#include <Tools/PropertyScript.h>
#include <Tools/PropertyRigidBody.h>
#include <Tools/PropertyPrimitive.h>
#include <Tools/PropertySound.h>
#include <Tools/PropertyModel.h>

#include <Tools/PropertyFactory.h>

class PropertyWindow : private Nt::Window {
public:
	using MessageBus = MessageBus<Object*>;
	using BusFunc = MessageBus::Func;
	using Subscription = std::pair<std::string, BusFunc>;

public:
	PropertyWindow(NotNull<Selector*> selectorPtr, NotNull<Scene*> pScene) noexcept :
		m_SelectorPtr(selectorPtr),
		m_ScenePtr(pScene),
		m_IsInitialized(false)
	{
	}
	~PropertyWindow() {
		for (const Subscription& subscription : m_Subscriptions)
			MessageBus::Instance().Unsubscribe(subscription.first, subscription.second);

		for (PropertyComponent* pComponent : m_Components)
			SAFE_DELETE(&pComponent);
	}

	void Initialize(const std::weak_ptr<Nt::EventBus>& pBus, const Settings& settings, const Nt::String& rootPath) {
		if (!std::exists(std::path(rootPath)))
			Raise("Invalid root folder path");
		
		Create(settings.PropertyWindowRect, settings.Language["Window.Property"]);
		RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		AddStyles(STYLE_BORDER);
		SetBackgroundColor(settings.Style["Property.BackgroundColor"]);

		m_RootPath = rootPath;

		const std::vector<std::string>& componentNames = PropertyFactory::Instance().GetAllNames();
		for (const std::string& name : componentNames) {
			PropertyComponent* pComponent =
				PropertyFactory::Instance().CreateComponent(name, m_SelectorPtr, m_ScenePtr);

			pComponent->Initialize(settings);
			pComponent->SetParent(*this);
			pComponent->SetRootPath(m_RootPath);
			pComponent->SetEventBus(pBus);

			m_Components.push_back(pComponent);
		}

		m_Subscriptions = {
			{ TOPIC_SELECTOR_ADD_SELECTION, BusFunc(this, &PropertyWindow::_SortProperties) },
			{ TOPIC_SELECTOR_DESELECT, BusFunc(this, &PropertyWindow::_SortProperties) }
		};

		for (const Subscription& subscription : m_Subscriptions)
			MessageBus::Instance().Subscribe(subscription.first, subscription.second);

		m_IsInitialized = true;
	}

	void Update() {
		if (!m_SelectorPtr->IsChanged())
			return;

		m_SelectorPtr->UnmarkChanged();
	}

	void SetTheme(const Style& style) {
		if (!m_IsInitialized)
			Raise("PropertyWindow not initialized");

		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		for (PropertyComponent* pComponent : m_Components)
			pComponent->SetTheme(style);
	}
	void SetLanguage(const Language& language) {
		if (!m_IsInitialized)
			Raise("PropertyWindow not initialized");

		SetName(language["Window.Property"]);

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
	std::vector<Subscription> m_Subscriptions;
	Nt::String m_RootPath;
	Selector* m_SelectorPtr;
	Scene* m_ScenePtr;
	Bool m_IsInitialized;

private:
	void _SortProperties(Object* pObject) {
		(void)pObject;

		if (m_Components.empty())
			return;

		Int positionY = m_Components.front()->GetPosition().y;
		for (PropertyComponent* pComponent : m_Components) {
			if (!pComponent->IsShowed())
				continue;

			const Int positionX = pComponent->GetPosition().x;
			pComponent->SetPosition({ positionX, positionY });

			positionY += pComponent->GetSize().y;
		}
	}
};