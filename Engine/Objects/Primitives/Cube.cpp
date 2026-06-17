#include <Objects/Primitives/Cube.h>
#include <ResourceLoader.h>
#include <ResourceTokens.h>
#include <Nt/Graphics/Geometry/Primitives.h>

static ResourceLoader<Nt::Mesh> g_MeshCube = { RToken::g_PrimitiveCube, Nt::Primitive::Cube({ 1.f, 1.f, 1.f }) };

Cube::Cube(const std::string& name) : Primitive(name, Class<Cube>::ID()) {
	SetPrimitiveMesh(Nt::Primitive::Cube({ 1.f, 1.f, 1.f }, Nt::Colors::White));
}

Cube* Cube::GetCopy() const {
	return new Cube(*this);
}

std::string Cube::GetClassToken() noexcept {
	return "Cube";
}
std::string Cube::GetToken() const noexcept {
	return GetClassToken();
}
