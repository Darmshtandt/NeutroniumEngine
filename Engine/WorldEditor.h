#pragma once

#include <Nt/Core/EventBus.h>
#include <Nt/Graphics/Objects/Camera.h>

class Grid;
class Selector;
class Scene;
class WorldDocument;

class WorldEditor {
public:
	WorldEditor(const std::weak_ptr<Nt::EventBus>& pBus, const Nt::String& defaultInitialPath);

	void CreatePrimitive(const std::string& className);
	void CreateEntity(const std::string& className);

	void ResetCamera() noexcept;

	void Update(Float deltaTime);

	void New();
	void Open();
	void Save();
	void SaveAs();

	[[nodiscard]] Nt::Camera& GetCamera() noexcept;
	[[nodiscard]] Grid* GetGrid() const noexcept;
	[[nodiscard]] Scene* GetScene() const noexcept;
	[[nodiscard]] Selector* GetSelector() const noexcept;
	[[nodiscard]] Bool IsChanged() const noexcept;

private:
	Nt::Camera m_Camera;
	std::unique_ptr<Grid> m_pGrid;
	std::unique_ptr<Scene> m_pScene;
	std::unique_ptr<WorldDocument> m_pDocument;
	std::unique_ptr<Selector> m_pSelector;
	Bool m_IsChanged = false;
};