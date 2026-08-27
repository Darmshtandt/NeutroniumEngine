#include <Objects/Primitives/Pyramid.h>
#include <Nt/Graphics/Geometry/Primitives.h>

Pyramid::Pyramid(const std::string& name) : Primitive(name, Class<Pyramid>::ID()) {
	SetShape(Nt::Primitive::Pyramid({ 1.f, 1.f, 1.f }, Nt::Colors::White));
}

Pyramid* Pyramid::GetCopy() const {
	return new Pyramid(*this);
}

std::string Pyramid::GetClassToken() noexcept {
	return "Pyramid";
}
std::string Pyramid::GetToken() const noexcept {
	return GetClassToken();
}