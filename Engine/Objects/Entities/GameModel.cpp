// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Entities/GameModel.h>


static EntityRegistrar<GameModel> g_Registrar;

GameModel::GameModel(const std::string& name) : Entity(name, Class<GameModel>::ID()) {
	SetMesh(&m_pMesh);
}

void GameModel::LoadMesh(const Nt::String& filePath) {
	m_pMesh.LoadFromFile(filePath.c_str());
	SetMesh(&m_pMesh);
}

void GameModel::SetShape(const Nt::Shape& shape) {
	m_pMesh.SetShape(shape);
	SetMesh(&m_pMesh);
}

_NODISCARD GameModel* GameModel::GetCopy() const {
	return new GameModel(*this);
}

_NODISCARD std::string GameModel::GetClassToken() noexcept {
	return "Model";
}

std::string GameModel::GetToken() const noexcept {
	return GetClassToken();
}
