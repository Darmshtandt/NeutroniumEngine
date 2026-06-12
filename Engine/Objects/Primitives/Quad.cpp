#include <Objects/Primitives/Quad.h>
#include <Nt/Graphics/Geometry/Primitives.h>

Quad::Quad(const std::string& name) : Primitive(name, Class<Quad>::ID()) {
	Nt::Shape quad = Nt::Primitive::Quad({ 1.f, 1.f }, Nt::Colors::White);
	quad.Indices.push_back(0);
	quad.Indices.push_back(1);
	quad.Indices.push_back(2);
	quad.Indices.push_back(2);
	quad.Indices.push_back(3);
	quad.Indices.push_back(0);

	SetShape(quad);
}

_NODISCARD std::string Quad::GetClassToken() noexcept {
	return "Quad";
}

std::string Quad::GetToken() const noexcept {
	return GetClassToken();
}
