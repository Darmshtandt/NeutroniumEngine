// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Nt/Core/Colors.h>
#include <Nt/Graphics/Geometry/Primitives.h>

#include <Core/Icon3D.h>

Icon3D::Icon3D() {
	Nt::Shape doubleSideQuad = Nt::Primitive::Quad({ 1.f, 1.f }, Nt::Colors::White, { 1, 0, 0, 1 });
	for (uInt i = 0; i < 6; ++i)
		doubleSideQuad.Indices.push_back(doubleSideQuad.Indices[i]);

	std::swap(doubleSideQuad.Indices[1], doubleSideQuad.Indices[4]);
	for (Nt::Vertex& vertex : doubleSideQuad.Vertices)
		std::swap(vertex.Position.x, vertex.Position.z);

	m_Mesh.SetShape(doubleSideQuad);
	SetMeshByPtr(&m_Mesh);
}