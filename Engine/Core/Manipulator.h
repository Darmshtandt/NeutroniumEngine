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
		Arrow(const Axis& axis);
		~Arrow() noexcept;

		void Render(NotNull<Nt::Renderer*> pRenderer) const noexcept override;

		void Select() noexcept;
		void Deselect() noexcept;

		void SetPosition(const Nt::Float3D& position);
		void SetColor(const Nt::Float4D& color);
		void SetAngle(const Nt::Float3D& angle);

	private:
		Nt::Model m_AxisLine;
		Nt::Mesh m_AxisLineMesh = Nt::Primitive::Line(10000.f, Nt::Colors::White);
		Bool m_ShowingLine = false;
	};

public:
	Manipulator();

	[[deprecated]] void Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse);

	void Update(const Nt::Ray& ray);
	void Render(NotNull<Nt::Renderer*> pRenderer) const noexcept;

	void Translate(const Nt::Float3D& offset) noexcept;

	void Show() noexcept;
	void Hide() noexcept;

	void BeginEditing(const Nt::Float2D& cursorPosition) noexcept;
	Nt::Float3D CalcMoveDelta(const Nt::Ray& ray);
	void EndEditing() noexcept;

	void ResetMoveDelta() noexcept;

	[[nodiscard]] Axis RayCastTest(const Nt::Ray& ray);

	[[nodiscard]] Nt::Float3D GetPosition() const noexcept;
	[[nodiscard]] Nt::Float3D GetMoveDelta() const noexcept;
	[[nodiscard]] Nt::Float3D GetStartPoint() const noexcept;
	[[nodiscard]] State GetState() const noexcept;
	[[nodiscard]] Bool IsVisible() const noexcept;
	[[nodiscard]] Bool StartedEditing() const noexcept;

	void SetState(const State& state) noexcept;
	void SetPosition(const Nt::Float3D& position) noexcept;

private:
	std::vector<Nt::Mesh*> m_StateMeshes;

	Nt::Float3D m_StartPoint;
	Nt::Float3D m_RayCastPoint;
	Nt::Float3D m_Position;
	Nt::Float2D m_StartCursorPosition;
	Nt::Float3D m_MoveDelta;

	Axis m_SelectedAxis = NONE;
	State m_State = TRANSLATE;

	Arrow m_AxisArrows[3];

	Bool m_IsVisible = false;
	Bool m_StartedEditing = false;
};