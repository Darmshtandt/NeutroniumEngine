#pragma once

class Scene;

class Selector {
public:
	enum class SelectedAxis {
		NONE, X, Y, Z
	};
	enum TransformationMode {
		MODE_TRANSLATE,
		MODE_SCALE,
	};

public:
	Selector() : 
		m_AxisObject(ObjectTypes::NONE, "Axis"),
		m_SelectedAxis(SelectedAxis::NONE),
		m_TransformationMode(MODE_TRANSLATE),
		m_GridCellSize(1.f),
		m_ScencePtr(nullptr),
		m_IsSnapToGrid(false),
		m_IsChanged(true)
	{ 
	}

	void Initialize(Scene* pScence, const Nt::String& defaultInitialPath);

	void Control(const Nt::RenderWindow* pWindow, const Nt::Float3D& cameraPosition, const Nt::Float3D& cameraAngle, Nt::Keyboard& keyboard, Nt::Mouse& mouse);

	void Update(const Nt::Float3D& cameraPosition);
	void Render(Nt::RenderWindow* pWindow);

	void AddSelect(Object* pObject);
	void Select(Object* pObject);
	void Deselect(Object* pObject);
	void AllDeselect();

	void ToggleSnapToGrid(const Bool& isSnapToGrid) noexcept;
	void ToggleSnapToGrid() noexcept;

	void UnmarkChanged() noexcept;

	const ObjectContainer& GetObjectContaiter() const noexcept;
	Object* GetObjectPtr(const uInt& index) const noexcept;

	uInt GetObjectCount() const noexcept;
	Bool IsContained(const Object* pObject) const;
	Bool IsChanged() const noexcept;
	Bool IsEmpty() const noexcept;

	void SetGridCellSize(const Float& gridCellSize) noexcept;
	void SetTransformationMode(const TransformationMode& mode) noexcept;

private:
	ObjectContainer m_SelectedObjects;
	Scene* m_ScencePtr;
	Object m_AxisObject;

	Nt::Model m_AxisModel;
	Nt::Model m_AxisScaleModel;
	Nt::Float2D m_StartMovingCursor;
	Nt::Float3D m_TotalMovingOffset;

	TransformationMode m_TransformationMode;
	SelectedAxis m_SelectedAxis;

	Float m_GridCellSize;
	Bool m_IsSnapToGrid;
	Bool m_IsChanged;
};