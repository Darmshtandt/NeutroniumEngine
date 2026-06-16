#pragma once

#include <Nt/Core/EventBus.h>
#include <Nt/Graphics/Objects/Camera.h>

class EditingHistory;
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
	std::shared_ptr<Scene> m_Scene;
	std::unique_ptr<Grid> m_Grid;
	std::unique_ptr<WorldDocument> m_Document;
	std::unique_ptr<EditingHistory> m_EditingHistory;
	std::unique_ptr<Selector> m_Selector;
	Bool m_IsChanged = false;
};