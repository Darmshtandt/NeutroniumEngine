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

void Plane::SetSize(const Nt::Float3D& size) {
	if (m_Size == size)
		return;

	Nt::Float3D correctSize = size;
	for (Float& scalar : correctSize.Array) {
		if (scalar == 0.f)
			scalar = FLT_EPSILON;
	}

	const Nt::Float3D scale = correctSize / m_Size;

	Nt::Shape shape = GetShape();
	for (uInt i = 0; i < shape.Vertices.size(); ++i) {
		shape.Vertices[i].Position *= Nt::Float4D(scale, 0.f, 1.f);
		shape.Vertices[i].TexCoord.x *= scale.x;
		shape.Vertices[i].TexCoord.y *= scale.z;
	}

	SetShape(shape);
	Primitive::SetSize(correctSize);
}

_NODISCARD std::string Plane::GetClassToken() noexcept {
	return "Plane";
}

std::string Plane::GetToken() const noexcept {
	return GetClassToken();
}
