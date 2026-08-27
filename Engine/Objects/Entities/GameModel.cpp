// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Entities/GameModel.h>

#include <ResourceLoader.h>
#include <ResourceTokens.h>
#include <Nt/Graphics/Geometry/Primitives.h>

static EntityRegistrar<GameModel> g_Registrar;
static ResourceLoader<Nt::Mesh> g_MeshCube = { RToken::g_PrimitiveCube, Nt::Primitive::Cube({ 1.f, 1.f, 1.f }) };

GameModel::GameModel(const std::string& name) : Entity(name, Class<GameModel>::ID()) {
	SetMesh(g_MeshCube.GetIndex());
}

GameModel* GameModel::GetCopy() const {
	return new GameModel(*this);
}
std::string GameModel::GetClassToken() noexcept {
	return "Model";
}
std::string GameModel::GetToken() const noexcept {
	return GetClassToken();
}