#pragma once

#include <Objects/Object.h>
#include <Nt/Graphics/Objects/Camera.h>
#include <Nt/Graphics/Geometry/Primitives.h>
#include <Nt/Graphics/RenderWindow.h>

class Manipulator {
public:
	enum State {
		TRANSLATE,
		SCALE,
	};
	enum Axis {
		NONE = -1,
		X, Y, Z
	};

	class Arrow : public Object {
	public:
		explicit Arrow(const Axis& axis);
		~Arrow() noexcept;

		void Select() noexcept;
		void Deselect() noexcept;

		[[nodiscard]] const Nt::Mesh* GetLineMesh() const noexcept;
		[[nodiscard]] Bool IsShowedLine() const noexcept;

	private:
		std::unique_ptr<Nt::Mesh> m_LineMesh;
		Bool m_ShowingLine = false;
	};

public:
	Manipulator();

	[[deprecated]] void Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse);

	void Update(const Nt::Ray& ray);

	void Translate(const Nt::Float3D& offset) noexcept;

	void Show() noexcept;
	void Hide() noexcept;

	void BeginEditing(const Nt::Float2D& cursorPosition) noexcept;
	Nt::Float3D CalcMoveDelta(const Nt::Ray& ray);
	void EndEditing() noexcept;

	void ResetLocalMoveDelta() noexcept;

	[[nodiscard]] Axis RayCastTest(const Nt::Ray& ray);

	[[nodiscard]] const Arrow* GetArrow(uInt axis) const noexcept;
	[[nodiscard]] Nt::Float3D GetPosition() const noexcept;
	[[nodiscard]] Nt::Float3D GetLocalMoveDelta() const noexcept;
	[[nodiscard]] Nt::Float3D GetMoveDelta() const noexcept;
	[[nodiscard]] Nt::Float3D GetStartPoint() const noexcept;
	[[nodiscard]] State GetState() const noexcept;
	[[nodiscard]] Bool IsVisible() const noexcept;
	[[nodiscard]] Bool StartedEditing() const noexcept;

	void SetState(const State& state) noexcept;
	void SetPosition(const Nt::Float3D& position) noexcept;

private:
	std::array<std::unique_ptr<Arrow>, 3> m_AxisArrows;
	std::vector<Nt::Mesh*> m_StateMeshes;

	Nt::Float3D m_StartPoint;
	Nt::Float3D m_RayCastPoint;
	Nt::Float3D m_Position;
	Nt::Float2D m_StartCursorPosition;
	Nt::Float3D m_LocalMoveDelta;
	Nt::Float3D m_MoveDelta;

	Axis m_SelectedAxis = NONE;
	State m_State = TRANSLATE;

	Bool m_IsVisible = false;
	Bool m_StartedEditing = false;
};