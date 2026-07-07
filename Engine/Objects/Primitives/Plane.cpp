#include <Objects/Primitives/Plane.h>
#include <Nt/Graphics/Geometry/Primitives.h>

Plane::Plane(const std::string& name) : Primitive(name, Class<Plane>::ID()) {
	Nt::Shape plane = Nt::Primitive::Quad({ 1.f, 1.f }, Nt::Colors::White);
	plane.Indices.push_back(0);
	plane.Indices.push_back(1);
	plane.Indices.push_back(2);
	plane.Indices.push_back(2);
	plane.Indices.push_back(3);
	plane.Indices.push_back(0);

	for (Nt::Vertex& vertex : plane.Vertices)
		std::swap(vertex.Position.y, vertex.Position.z);

	SetShape(plane);
}

Plane* Plane::GetCopy() const {
	return new Plane(*this);
}
std::string Plane::GetClassToken() noexcept {
	return "Plane";
}
std::string Plane::GetToken() const noexcept {
	return GetClassToken();
}