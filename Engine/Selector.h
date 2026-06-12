#pragma once

#include <Core/Manipulator.h>

#include <Nt/Graphics/RenderWindow.h>

constexpr const Char* TOPIC_SELECTOR_ADD_SELECTION = "Selector: Add selection";
constexpr const Char* TOPIC_SELECTOR_DESELECT = "Selector: Deselect";
constexpr const Char* TOPIC_SELECTOR_ALL_DESELECT = "Selector: All deselect";

class Grid;
class Scene;

class Selector {
public:
	using Axis = Manipulator::Axis;

public:
	Selector(const std::weak_ptr<Nt::EventBus>& pBus, NotNull<Scene*> pScene, NotNull<Grid*> pGrid);

	void Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Keyboard& keyboard, Nt::Mouse& mouse);

	void Update();
	void Render(NotNull<Nt::Renderer*> pRenderer);

	void RayCastTest(const Nt::Ray& ray, const Nt::Float2D& cursorPosition, const Bool isMulti);

	void AddSelect(NotNull<Object*> pObject);
	void Select(NotNull<Object*> pObject);
	void Deselect(NotNull<Object*> pObject);
	void Deselect(const ObjectContainer::const_iterator& iterator);
	void AllDeselect();
	[[deprecated]] void RemoveSelected();

	void ToggleSnapToGrid(const Bool& isSnapToGrid) noexcept;
	void ToggleSnapToGrid() noexcept;

	void UnmarkChanged() noexcept;

	const ObjectContainer& GetObjectContainer() const noexcept;
	Object* GetObjectPtr(const uInt& index) const;

	uInt GetObjectCount() const noexcept;
	Bool IsContained(const NotNull<Object*> pObject) const;
	Bool IsChanged() const noexcept;
	Bool IsEmpty() const noexcept;

	void SetTransformMode(const Manipulator::State& state) noexcept;

private:
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	ObjectContainer m_SelectedObjects;
	Manipulator m_Manipulator;
	Grid* m_pGrid;
	Axis m_SelectedAxis = Axis::NONE;
	Scene* m_pScene = nullptr;

	Nt::Ray m_Ray;
	Nt::Mesh m_RayMesh;
	Nt::Model m_RayModel;

	Bool m_IsChanged = true;
	Bool m_EnabledDebug = false;
	Bool m_IsSnapToGrid = false;

private:
	[[deprecated]] void _MouseControl(NotNull<const Nt::RenderWindow*> pWindow, Nt::Keyboard& keyboard, Nt::Mouse& mouse);
	void _AxisControl(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse);
};