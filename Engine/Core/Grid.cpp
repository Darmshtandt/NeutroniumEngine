// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Core/Grid.h>
#include <Nt/Graphics/Renderer.h>

Grid::Grid() :
	Object("Engine::Grid", Class<Grid>::ID()),
	m_pMesh(new Nt::Mesh)
{
	m_DrawingMode = Nt::Renderer::DrawingMode::LINES;

	m_pMesh->SetShape(Build(m_Size, m_CellSize));
	SetMesh(m_pMesh.get());

	SetOrigin({ m_CellSize / 2.f, 0.f, m_CellSize / 2.f });
	Object::SetSize({ m_CellSize, 0.f, m_CellSize });
	SetColor(Nt::Colors::DarkGray);
}

void Grid::Update() {
	constexpr Nt::Float3D XZ(-1.f, 0.f, -1.f);
	if (m_pTarget != nullptr)
		SetPosition(m_pTarget->GetPosition() * XZ);
}

void Grid::Show() noexcept {
	m_IsVisible = true;
}
void Grid::Hide() noexcept {
	m_IsVisible = false;
}

Nt::Float3D Grid::Snap(const Nt::Float3D& position) const noexcept {
	return (position / m_CellSize).Round() * m_CellSize;
}

const Nt::Mesh* Grid::GetMesh() const noexcept {
	return m_pMesh.get();
}
Float Grid::GetCellSize() const noexcept {
	return m_CellSize;
}

void Grid::SetTarget(IObject* pTarget) noexcept {
	m_pTarget = pTarget;
}
void Grid::SetCellSize(const Float& cellSize) {
	if (m_CellSize == cellSize || cellSize <= 0.f)
		return;

	m_CellSize = cellSize;
	SetOrigin({ m_CellSize / 2.f, 0.f, m_CellSize / 2.f });
	Object::SetSize({ m_CellSize, 0.f, m_CellSize });
}
void Grid::SetSize(const Nt::Float2D& size) {
	if (m_Size == size || size <= 0.f)
		return;

	const Nt::Float2D scale = size / m_Size;
	Object::SetSize(Nt::Float3D(scale.x, 0.f, scale.y));
	m_Size = size;
}
void Grid::SetPosition(Nt::Float3D position) {
	position = Snap(position);
	if (m_Position != position) {
		m_Position = position;
		Object::SetPosition(position);
	}
}

Nt::Shape Grid::Build(const Nt::Float2D& gridSize, Float cellSize) const {
	Nt::Shape shape;

	const Nt::Float2D vertexDensity = gridSize / cellSize;
	const Nt::Float2D vertexDistance = gridSize / vertexDensity;
	const Nt::Float2D halsSize = gridSize * 0.5f;

	shape.Vertices.reserve(static_cast<uInt>(vertexDensity.x * vertexDensity.y));
	for (Float x = 0.f; x < vertexDensity.x; ++x) {
		for (Float z = 0.f; z < vertexDensity.y; ++z) {
			Nt::Vertex vertex = { };
			vertex.Position.x = vertexDistance.x * x - halsSize.x;
			vertex.Position.y = 0.f;
			vertex.Position.z = vertexDistance.y * z - halsSize.y;
			vertex.Position.w = 1.f;
			vertex.Color = Nt::Colors::White;

			shape.Vertices.emplace_back(vertex);
		}
	}

	const Nt::uInt2D squaresCount = Nt::uInt2D(vertexDensity) - 1;
	shape.Indices.reserve(squaresCount.x * squaresCount.y * 6);

	for (uInt x = 0; x < squaresCount.x; ++x) {
		for (uInt z = 0; z < squaresCount.y; ++z) {
			const Nt::uInt2D indices2D[6] = {
				{ x, z },         { x + 1, z }, { x + 1, z + 1 },
				{ x + 1, z + 1 }, { x, z + 1 }, { x, z }
			};

			for (uInt i = 0; i < 6; ++i) {
				const Nt::Index_t index = Nt::Index_t(vertexDensity.y * indices2D[i].y + indices2D[i].x);
				shape.Indices.push_back(index);
			}
		}
	}

	return shape;
}