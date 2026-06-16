#pragma once

#include <Core/Manipulator.h>
#include <Nt/Graphics/RenderWindow.h>

constexpr const Char* TOPIC_SELECTOR_ADD_SELECTION = "Selector: Add selection";
constexpr const Char* TOPIC_SELECTOR_DESELECT = "Selector: Deselect";
constexpr const Char* TOPIC_SELECTOR_ALL_DESELECT = "Selector: All deselect";

class EditingHistory;
class Grid;
class Scene;

class Selector {
public:
	using Axis = Manipulator::Axis;

public:
	Selector(const std::weak_ptr<Nt::EventBus>& pBus, NotNull<Scene*> pScene, NotNull<Grid*> pGrid, NotNull<EditingHistory*> pEditingHistory);

	void Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Keyboard& keyboard, Nt::Mouse& mouse);

	void Update();
	void RayCastTest(const Nt::Ray& ray, const Nt::Float2D& cursorPosition, const Bool isMulti);

	void AddSelect(const WeakObjectPtr& weakObject);
	void Select(const WeakObjectPtr& weakObject);
	void Deselect(const WeakObjectPtr& weakObject);
	void Deselect(const WeakObjectContainer::const_iterator& iterator);
	void AllDeselect();
	[[deprecated]] void RemoveSelected();

	void ToggleSnapToGrid(const Bool& isSnapToGrid) noexcept;
	void ToggleSnapToGrid() noexcept;

	void UnmarkChanged() noexcept;

	const WeakObjectContainer& GetObjectContainer() const noexcept;
	const WeakObjectPtr& GetObjectPtr(const uInt& index) const;

	const Manipulator* GetManipulator() const noexcept;
	const Nt::Mesh* GetRayMesh() const noexcept;
	uInt GetObjectCount() const noexcept;
	Bool IsContained(const WeakObjectPtr& pObject) const;
	Bool EnabledDebug() const noexcept;
	Bool IsChanged() const noexcept;
	Bool IsEmpty() const noexcept;

	void SetTransformMode(const Manipulator::State& state) noexcept;

private:
	std::unique_ptr<Manipulator> m_pManipulator;
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	WeakObjectContainer m_SelectedObjects;
	EditingHistory* m_pEditingHistory;
	Grid* m_pGrid;
	Axis m_SelectedAxis = Axis::NONE;
	Scene* m_pScene = nullptr;

	std::unique_ptr<Nt::Mesh> m_pRayMesh;
	Nt::Ray m_Ray;

	Bool m_IsChanged = true;
	Bool m_EnabledDebug = false;
	Bool m_IsSnapToGrid = false;

private:
	[[deprecated]] void _MouseControl(NotNull<const Nt::RenderWindow*> pWindow, Nt::Keyboard& keyboard, Nt::Mouse& mouse);
	void _AxisControl(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse);
};