#pragma once

#include <Objects/Object.h>

class Grid : public Object {
public:
	Grid();

	void Update();

	void Show() noexcept;
	void Hide() noexcept;

	[[nodiscard]] Nt::Float3D Snap(const Nt::Float3D& position) const noexcept;
	[[nodiscard]] const Nt::Mesh* GetMesh() const noexcept;
	[[nodiscard]] Float GetCellSize() const noexcept;

	void SetTarget(NtEx::TransformFloat3D* pTarget) noexcept;
	void SetCellSize(const Float& cellSize);
	void SetSize(const Nt::Float2D& size);
	void SetPosition(Nt::Float3D position);

private:
	std::unique_ptr<Nt::Mesh> m_pMesh;
	NtEx::TransformFloat3D* m_pTarget = nullptr;
	Nt::Float2D m_Size = { 100.f, 100.f };
	Nt::Float3D m_Position;
	Float m_CellSize = 1.f;
	Bool m_IsVisible = true;

private:
	Nt::Shape Build(const Nt::Float2D& gridSize, Float cellSize) const;
};
