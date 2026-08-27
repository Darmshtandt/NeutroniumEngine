#include <Objects/Primitives/Quad.h>
#include <Nt/Graphics/Geometry/Primitives.h>

Nt::Shape TwoSizeQuad() {
	Nt::Shape quad = Nt::Primitive::Quad({ 1.f, 1.f });
	quad.Indices.insert(quad.Indices.end(), { 0, 1, 2, 2, 3, 0 });
	return quad;
}

static ResourceLoader<Nt::Mesh> g_MeshQuad = { "Primitive.Quad", TwoSizeQuad() };

Quad::Quad(const std::string& name) : Primitive(name, Class<Quad>::ID()) {
	SetMesh(g_MeshQuad.Get());
}

Quad* Quad::GetCopy() const {
	return new Quad(*this);
}

std::string Quad::GetClassToken() noexcept {
	return "Quad";
}
std::string Quad::GetToken() const noexcept {
	return GetClassToken();
}
