#include <Objects/Primitives/Pyramid.h>
#include <Nt/Graphics/Geometry/Primitives.h>

Pyramid::Pyramid(const std::string& name) : Primitive(name, Class<Pyramid>::ID()) {
	SetShape(Nt::Primitive::Pyramid({ 1.f, 1.f, 1.f }, Nt::Colors::White));
}

void Pyramid::SetSize(const Nt::Float3D& size) {
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

		if (i < 8) {
			shape.Vertices[i].TexCoord *= scale;
		}
		else if (i < 16) {
			shape.Vertices[i].TexCoord.x *= scale.z;
			shape.Vertices[i].TexCoord.y *= scale.y;
		}
		else {
			shape.Vertices[i].TexCoord.x *= scale.x;
			shape.Vertices[i].TexCoord.y *= scale.z;
		}
	}

	SetShape(shape);
	Primitive::SetSize(correctSize);
}

_NODISCARD std::string Pyramid::GetClassToken() noexcept {
	return "Pyramid";
}

std::string Pyramid::GetToken() const noexcept {
	return GetClassToken();
}
