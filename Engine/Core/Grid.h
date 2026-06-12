#pragma once

#include <Nt/Graphics/Objects/Model.h>

class Grid {
public:
	Grid();

	void Update();
	void Render(NotNull<Nt::Renderer*> pRenderer) const;

	void Show() noexcept;
	void Hide() noexcept;

	[[nodiscard]] Nt::Float3D Snap(const Nt::Float3D& position) const noexcept;
	[[nodiscard]] Float GetCellSize() const noexcept;

	void SetTarget(Nt::IObject* pTarget) noexcept;
	void SetCellSize(const Float& cellSize);
	void SetSize(const Nt::Float2D& size);
	void SetPosition(Nt::Float3D position);

private:
	Nt::Model m_Model;
	Nt::Mesh m_Mesh;
	Nt::IObject* m_pTarget = nullptr;
	Nt::Float2D m_Size = { 100.f, 100.f };
	Nt::Float3D m_Position;
	Float m_CellSize = 1.f;
	Bool m_IsVisible = true;

private:
	Nt::Shape Build(const Nt::Float2D& gridSize, Float cellSize) const;
};