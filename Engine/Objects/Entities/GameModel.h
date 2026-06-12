#pragma once

#include <Objects/ObjectRegistrar.h>

class GameModel : public Entity {
public:
	GameModel(const std::string& name);

	void LoadMesh(const Nt::String& filePath);

	void SetShape(const Nt::Shape& shape);

	_NODISCARD virtual GameModel* GetCopy() const override;
	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

private:
	Nt::Mesh m_pMesh;
};