#pragma once

class GridLayout : public Nt::Window {
public:
	struct Cell {
		Nt::Window* WindowPtr = nullptr;
		Nt::Double2D SizeOnGrid = { 1.0, 1.0 };
	};

public:
	explicit GridLayout() = default;
	explicit GridLayout(const Nt::uInt2D& size) {
		Resize(size);
	}
	~GridLayout() {
		Clear();
	}

	void Resize(const Nt::uInt2D& newSize) {
		if (newSize == m_GridSize)
			return;

		if (newSize.x == 0 || newSize.y == 0) {
			Clear();
			return;
		}

		const Nt::Int2D ratio = Nt::Int2D(newSize) - Nt::Int2D(m_GridSize);
		for (uInt i = 0; i < ratio.x; ++i) {
			if (ratio.x < 0) {
				auto endWindow = (m_Cells.end() - 1);
				for (uInt j = 0; j < m_GridSize.y; ++j)
					delete((*endWindow)[j]);
				m_Cells.erase(endWindow);
			}
			else {
				std::vector<Cell*> cells;
				for (uInt i = 0; i < newSize.y; ++i)
					cells.push_back(new Cell);
				m_Cells.push_back(cells);
			}
		}

		if (ratio.y < 0) {
			for (uInt i = 0; i < m_GridSize.x; ++i)
				m_Cells[i].erase(m_Cells[i].end() + ratio.x, m_Cells[i].end());
		}
		else {
			for (uInt i = 0; i < m_GridSize.x; ++i)
				for (uInt i = 0; i < ratio.y; ++i)
					m_Cells[i].push_back(new Cell);
		}

		m_GridSize = newSize;
	}

	void ResizeCell(const uInt& x, const uInt& y, const Nt::Double2D& cellSize) {
		if (m_Cells[x][y]->WindowPtr) {
			Nt::uInt2D newWindowSize = m_Cells[x][y]->WindowPtr->GetRect().RightBottom;
			newWindowSize *= cellSize / m_Cells[x][y]->SizeOnGrid;
			m_Cells[x][y]->WindowPtr->SetSize(newWindowSize);
		}

		const Nt::Double2D different = cellSize - m_Cells[x][y]->SizeOnGrid;
		if ()

		m_Cells[x][y]->SizeOnGrid = cellSize;
	}

	void Merge(Nt::uInt2D start, Nt::uInt2D end) {
		if (start >= m_GridSize || end >= m_GridSize)
			Raise("Out of range");

		if (start == end)
			return;

		if (start.x > end.x)
			std::swap(start.x, end.x);
		if (start.y > end.y)
			std::swap(start.y, end.y);

		Cell* pStartCell = m_Cells[start.x][start.y];
		for (uInt x = start.x; x <= end.x; ++x) {
			for (uInt y = start.y; y <= end.y; ++y) {
				if (Nt::uInt2D(x, y) == start)
					continue;

				if (x == start.x)
					pStartCell->SizeOnGrid.y += m_Cells[x][y]->SizeOnGrid.y;
				if (y == start.y)
					pStartCell->SizeOnGrid.x += m_Cells[x][y]->SizeOnGrid.x;

				if (m_Cells[x][y]->WindowPtr)
					Nt::Log::Warning("Window was lost when merging cells");

				delete(m_Cells[x][y]);
				m_Cells[x][y] = pStartCell;
			}
		}

		if (pStartCell->WindowPtr) {
			const Nt::Double2D cellSize = Nt::Double2D(GetRect().RightBottom) / m_GridSize;
			pStartCell->WindowPtr->SetSize(cellSize * pStartCell->SizeOnGrid);
		}
	}

	void Clear() {
		m_GridSize = { 0, 0 };
		for (uInt i = 0; i < m_GridSize.x; ++i)
			for (uInt j = 0; j < m_GridSize.y; ++j)
				delete(m_Cells[i][j]);
		m_Cells.clear();
	}

	void SetWindow(const Nt::uInt2D& index, Nt::Window* pWindow) {
		if (index >= m_GridSize)
			Raise("Out of range");

		pWindow->SetParent(GetHandle());
		pWindow->AddStyles(WS_CHILD);
		pWindow->RemoveStyles(WS_OVERLAPPEDWINDOW);
		pWindow->SetBackgroundColor((Nt::uInt4D)Nt::Colors::Red * 255);
		m_Cells[index.x][index.y]->WindowPtr = pWindow;

		const Nt::Double2D cellRect = 
			Nt::Double2D(GetClientRect().RightBottom) / m_GridSize;

		Nt::uIntRect newWindowRect;
		newWindowRect.LeftTop = 
			cellRect * index + m_Gap / 2;
		newWindowRect.RightBottom = 
			cellRect * m_Cells[index.x][index.y]->SizeOnGrid - m_Gap;

		newWindowRect.LeftTop += cellRect - newWindowRect.RightBottom;
		pWindow->SetRect(newWindowRect);
	}

	std::vector<Cell*>& operator [] (const uInt& index) {
		return m_Cells[index];
	}

	void SetGap(const Nt::uInt2D& gap) noexcept {
		m_Gap = gap;
	}

	Nt::uInt2D GetGridSize() const noexcept {
		return m_GridSize;
	}

private:
	std::vector<std::vector<Cell*>> m_Cells;
	Nt::uInt2D m_GridSize;
	Nt::uInt2D m_Gap;
};