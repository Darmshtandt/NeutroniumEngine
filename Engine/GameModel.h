#pragma once

class GameModel : public Entity {
public:
	GameModel(const std::string& name) :
		Entity(name, EntityTypes::MODEL)
	{
		m_Model.SetMesh(Nt::Geometry::Cube({ 1.f, 1.f, 1.f }, Nt::Colors::White));
		_UpdateColliders();
	}

	void LoadMesh(const Nt::String& filePath) {
		Nt::Mesh mesh;
		mesh.LoadFromFile(filePath.c_str());
		m_Model.SetMesh(mesh);
		_UpdateColliders();
	}

	virtual Object* GetCopy() const override {
		return new GameModel(*this);
	}
};

__inline GameModel* UpcastEntityToGameModel(Entity* pEntity) {
	if (pEntity == nullptr)
		Raise("Entity pointer is null");
	if (pEntity->GetEntityType() != EntityTypes::SOUND)
		Raise("Entity pointer is not GameModel");

	GameModel* pGameModel = dynamic_cast<GameModel*>(pEntity);
	if (pGameModel == nullptr)
		Raise("Failed to upcast Entity to GameModel.");
	return pGameModel;
}