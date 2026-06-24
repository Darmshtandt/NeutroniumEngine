// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Primitives/Primitive.h>

Primitive::Primitive(std::string name, const ClassID id) :
	Object(std::move(name), id),
	m_Mesh(new Nt::Mesh)
{
	SetMesh(m_Mesh.get());
}

Primitive::Primitive(const Primitive& primitive) :
	Object(primitive),
	m_Mesh(primitive.m_Mesh)
{
	if (m_Mesh->GetVerticesCount() > 0)
		SetMesh(m_Mesh.get());
}

Primitive::Primitive(Primitive&& primitive) noexcept :
	Object(primitive),
	m_Mesh(std::move(primitive.m_Mesh))
{
	SetMesh(m_Mesh.get());
}


std::string Primitive::GetClassToken() noexcept {
	return "Primitive";
}
std::string Primitive::GetClassTypeToken() noexcept {
	return GetClassToken();
}
std::string Primitive::GetTypeToken() const noexcept {
	return GetClassToken();
}
std::string Primitive::GetToken() const noexcept {
	return GetClassToken();
}

Primitive* Primitive::GetCopy() const {
	return new Primitive(*this);
}

void Primitive::SetPrimitiveMesh(const Nt::Mesh& mesh) {
	m_Mesh.reset(new Nt::Mesh(mesh));
	SetMesh(m_Mesh.get());
}

const Nt::Shape& Primitive::GetShape() const noexcept {
	return m_Mesh->GetShape();
}