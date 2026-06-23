#pragma once

#include <Nt/Core/EventBus.h>
#include <Nt/Graphics/Objects/Camera.h>

class Clipboard;
class InputContext;
class EditingHistory;
class Grid;
class Selector;
class Scene;
class WorldDocument;

class WorldEditor {
public:
	WorldEditor(const std::weak_ptr<Nt::EventBus>& pBus, const Nt::String& defaultInitialPath);
	~WorldEditor() noexcept;

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
	[[nodiscard]] std::weak_ptr<Scene> GetScene() const noexcept;
	[[nodiscard]] Selector* GetSelector() const noexcept;
	[[nodiscard]] Bool IsChanged() const noexcept;

private:
	std::shared_ptr<InputContext> m_InputContext;
	std::unique_ptr<EditingHistory> m_EditingHistory;
	std::unique_ptr<Grid> m_Grid;
	std::shared_ptr<Scene> m_Scene;
	std::unique_ptr<WorldDocument> m_Document;
	std::unique_ptr<Selector> m_Selector;
	std::unique_ptr<Clipboard> m_Clipboard;
	Nt::Camera m_Camera;
	Bool m_IsChanged = false;
};