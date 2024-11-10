#pragma once

class Grid : public Nt::IObject {
public:
	Grid() {
		_Recreate();
	}

	void Update() {
		if (IsChanged()) {
			m_GridModel.SetPosition(m_Position);
			m_GridModel.SetAngle(m_Angle);
			UnmarkChanged();
		}
	}

	void Render(Nt::Renderer* pRenderer) const override {
		if (IsVisible()) {
			const Nt::Renderer::DrawingMode prevDrawingMode = pRenderer->GetDrawingMode();

			pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::LINES);
			m_GridModel.Render(pRenderer);
			pRenderer->SetDrawingMode(prevDrawingMode);
		}
	}

	void SetCellSize(const Float& cellSize) {
		if (m_CellSize != cellSize) {

			m_CellSize = cellSize;
		}
	}

private:
	Nt::Model m_GridModel;
	Float m_CellSize = 0.5f;

private:
	void _Recreate() {
		if (m_CellSize <= 0.f)
			Raise("Incorrect cell size");

		Nt::Shape gridShape;

		m_GridModel.SetMesh(gridShape);
	}
};