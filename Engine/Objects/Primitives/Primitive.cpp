// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Primitives/Primitive.h>

#include <Nt/Graphics/Geometry/Primitives.h>

Primitive::Primitive(std::string name, const ClassID id) :
	Object(std::move(name), id),
	m_TextureScale({ 1.f, 1.f }) 
{
	SetMesh(&m_Mesh);
}

Primitive::Primitive(const Primitive& primitive) :
	Object(primitive),
	m_TextureOffset(primitive.m_TextureOffset),
	m_TextureScale(primitive.m_TextureScale),
	m_Mesh(primitive.m_Mesh)
{
	SetMesh(&m_Mesh);
}

Primitive::Primitive(Primitive&& primitive) noexcept :
	Object(primitive),
	m_TextureOffset(primitive.m_TextureOffset),
	m_TextureScale(primitive.m_TextureScale),
	m_Mesh(std::move(primitive.m_Mesh))
{
	SetMesh(&m_Mesh);
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

Nt::Float2D Primitive::GetTextureOffset() const noexcept {
	return m_TextureOffset;
}

Nt::Float2D Primitive::GetTextureScale() const noexcept {
	return m_TextureScale;
}

void Primitive::SetPrimitiveMesh(const Nt::Mesh& mesh) {
	m_Mesh = mesh;
	SetMesh(&m_Mesh);
}

void Primitive::SetTextureOffset(const Nt::Float2D& textureOffset) {
	if (m_TextureOffset == textureOffset)
		return;

	Nt::Shape shape = m_Mesh.GetShape();

	const Nt::Float2D offsetValue = textureOffset - m_TextureOffset;
	for (Nt::Vertex& vertex : shape.Vertices)
		vertex.TexCoord.xy += offsetValue;

	m_Mesh.SetShape(shape);

	m_TextureOffset = textureOffset;
}

void Primitive::SetTextureScale(Nt::Float2D textureScale) {
	if (m_TextureScale == textureScale)
		return;

	if (textureScale.x == 0.f)
		textureScale.x = std::numeric_limits<Float>::epsilon();
	if (textureScale.y == 0.f)
		textureScale.y = std::numeric_limits<Float>::epsilon();

	Nt::Shape shape = m_Mesh.GetShape();
	for (uInt i = 0; i < shape.Vertices.size(); ++i)
		shape.Vertices[i].TexCoord.xy *= textureScale / m_TextureScale;
	m_Mesh.SetShape(shape);

	m_TextureScale = textureScale;
}

const Nt::Shape& Primitive::GetShape() const noexcept {
	return m_Mesh.GetShape();
}