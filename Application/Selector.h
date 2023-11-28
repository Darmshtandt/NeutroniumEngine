#pragma once

class Selector {
public:
	Selector() : m_IsChanged(true)
	{ 
	}

	void AddSelect(Object* pObject) {
		if (pObject) {
			m_IsChanged = true;
			auto it = std::find(m_Objects.begin(), m_Objects.end(), pObject);
			if (it != m_Objects.end()) {
				pObject->DisableSelectionColor();
				m_Objects.erase(it);
			}
			else {
				pObject->EnableSelectionColor();
				m_Objects.push_back(pObject);
			}
		}
		else {
			ERROR_MSG(L"Selector::AddSelect: Object is nullptr.", L"Error");
		}
	} 
	void Select(Object* pObject) {
		const Bool isObjectSelected = pObject->IsSelected();
		const uInt objectCount = m_Objects.size();

		AllDeselect();
		if ((!isObjectSelected) || objectCount > 1)
			AddSelect(pObject);
	}
	void Deselect(Object* pObject) {
		auto objectIterator = 
			std::find(m_Objects.begin(), m_Objects.end(), pObject);
		if (objectIterator != m_Objects.end()) {
			m_IsChanged = true;
			(*objectIterator)->DisableSelectionColor();
			m_Objects.erase(objectIterator);
		}
		else {
			Nt::Log::Warning(L"This object is not selected.");
		}
	}
	void AllDeselect() {
		m_IsChanged = true;
		while (m_Objects.size() > 0) {
			m_Objects[0]->DisableSelectionColor();
			m_Objects.erase(m_Objects.begin());
		}
	}


	void UnmarkChanged() noexcept {
		m_IsChanged = false;
	}

	const ObjectContainer& GetObjects() const noexcept {
		return m_Objects;
	}
	Bool IsContained(const Object* pObject) const {
		auto iterator = std::find(m_Objects.begin(), m_Objects.end(), pObject);
		return (iterator != m_Objects.end());
	}
	Bool IsChanged() const noexcept {
		return m_IsChanged;
	}

private:
	ObjectContainer m_Objects;
	Bool m_IsChanged;
};