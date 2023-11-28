#pragma once

enum class PrimitiveTypes {
	CUBE,
	QUAD,
	PLANE,
	PYRAMID,
	MIX,
};

class Primitive : public Object {
public:
	Primitive(const std::string& name) :
		Object(ObjectTypes::PRIMITIVE, name),
		m_TextureScale({ 1.f, 1.f })
	{ }
	Primitive(const std::string& name, const PrimitiveTypes& type, const Nt::Float3D& size) :
		Object(ObjectTypes::PRIMITIVE, name),
		m_TextureScale({ 1.f, 1.f })
	{ 
		Create(type, size);
	}
	Primitive(const Primitive& primitive) :
		Object(primitive),
		m_TextureOffset(primitive.m_TextureOffset),
		m_TextureScale(primitive.m_TextureScale),
		m_Type(primitive.m_Type)
	{
	}

	void Write(std::ostream& stream) const override {
		Object::Write(stream);
		Nt::Serialization::WriteAll(stream, m_TextureOffset, m_TextureScale, m_Type);
	}
	void Read(std::istream& stream) override {
		Object::Read(stream);
		Nt::Serialization::ReadAll(stream, m_TextureOffset, m_TextureScale, m_Type);
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}
	constexpr uInt ClassType() const noexcept {
		return uInt(ObjectTypes::PRIMITIVE);
	}

	void Create(const PrimitiveTypes& type, const Nt::Float3D& size) {
		m_Size = size;
		m_Type = type;
		switch (m_Type) {
		case PrimitiveTypes::CUBE:
			m_Model.SetMesh(Nt::Geometry::Cube(size, Nt::Colors::White));
			break;
		case PrimitiveTypes::QUAD:
			m_Model.SetMesh(_CreateQuadShape(size, Nt::Colors::White));
			break;
		case PrimitiveTypes::PLANE:
		{
			Nt::Shape plane = _CreateQuadShape(size, Nt::Colors::White);
			for (Nt::Vertex& vertex : plane.Vertices)
				std::swap(vertex.Position.y, vertex.Position.z);
			m_Model.SetMesh(plane);
		}
			break;
		case PrimitiveTypes::PYRAMID:
			m_Model.SetMesh(Nt::Geometry::Pyramid(size, Nt::Colors::White));
			break;
		case PrimitiveTypes::MIX:
			Raise("Cannot create primitive with type mix");
		}
		_UpdateColliders();
	}


	virtual Object* GetCopy() const override {
		return new Primitive(*this);
	}
	Nt::Float2D GetTextureOffset() const noexcept {
		return m_TextureOffset;
	}
	Nt::Float2D GetTextureScale() const noexcept {
		return m_TextureScale;
	}
	PrimitiveTypes GetPrimitiveType() const noexcept {
		return m_Type;
	}

	void SetSize(const Nt::Float3D& size) override {
		Nt::Mesh* pMesh = m_Model.GetMeshPtr();
		if (pMesh == nullptr || m_Size == size)
			return;

		Nt::Float3D correctSize = size;
		if (size.x == 0.f)
			correctSize.x = 0.001f;
		if (size.y == 0.f)
			correctSize.y = 0.001f;
		if (size.z == 0.f)
			correctSize.z = 0.001f;

		Nt::Shape shape = pMesh->GetShape();
		switch (m_Type) {
		case PrimitiveTypes::CUBE:
			_ScaleCube(shape, correctSize / m_Size);
			break;
		case PrimitiveTypes::QUAD:
			_ScaleQuad(shape, correctSize / m_Size);
			break;
		case PrimitiveTypes::PLANE:
			_ScalePlane(shape, correctSize / m_Size);
			break;
		case PrimitiveTypes::PYRAMID:
			_ScalePyramid(shape, correctSize / m_Size);
			break;
		}

		pMesh->SetShape(shape);
		IObject::SetSize(correctSize);
		_UpdateColliders();
	}
	void SetTextureOffset(const Nt::Float2D& textureOffset) {
		if (m_TextureOffset == textureOffset)
			return;

		Nt::Mesh* pMesh = m_Model.GetMeshPtr();
		if (pMesh) {
			Nt::Shape shape = pMesh->GetShape();
			const Nt::Float2D offsetValue = textureOffset - m_TextureOffset;
			for (Nt::Vertex& vertex : shape.Vertices)
				vertex.TexCoord += offsetValue;
			pMesh->SetShape(shape);
		}
		m_TextureOffset = textureOffset;
	}
	void SetTextureScale(Nt::Float2D textureScale) {
		if (m_TextureScale == textureScale)
			return;

		if (textureScale.x == 0.f)
			textureScale.x = 0.000001f;
		if (textureScale.y == 0.f)
			textureScale.y = 0.000001f;

		Nt::Mesh* pMesh = m_Model.GetMeshPtr();
		if (pMesh) {
			Nt::Shape shape = pMesh->GetShape();
			for (uInt i = 0; i < shape.Vertices.size(); ++i)
				shape.Vertices[i].TexCoord *= textureScale / m_TextureScale;
			pMesh->SetShape(shape);
		}
		m_TextureScale = textureScale;
	}

private:
	Nt::Float2D m_TextureOffset;
	Nt::Float2D m_TextureScale;
	PrimitiveTypes m_Type;

private:
	void _ScaleCube(Nt::Shape& shape, const Nt::Float3D& scale) {
		for (uInt i = 0; i < shape.Vertices.size(); ++i) {
			shape.Vertices[i].Position *= scale;
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
	}
	void _ScaleQuad(Nt::Shape& shape, const Nt::Float3D& scale) {
		for (uInt i = 0; i < shape.Vertices.size(); ++i) {
			shape.Vertices[i].Position *= scale;
			shape.Vertices[i].TexCoord *= scale;
		}
	}
	void _ScalePlane(Nt::Shape& shape, const Nt::Float3D& scale) {
		for (uInt i = 0; i < shape.Vertices.size(); ++i) {
			shape.Vertices[i].Position *= scale;
			shape.Vertices[i].TexCoord.x *= scale.x;
			shape.Vertices[i].TexCoord.y *= scale.z;
		}
	}
	void _ScalePyramid(Nt::Shape& shape, const Nt::Float3D& scale) {
		for (uInt i = 0; i < shape.Vertices.size(); ++i) {
			shape.Vertices[i].Position *= scale;
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
	}

	Nt::Shape _CreateQuadShape(const Nt::Float2D& size, const Nt::Float4D& color) const {
		Nt::Shape quad = Nt::Geometry::Quad(size, Nt::Colors::White);
		quad.Indices.push_back(0);
		quad.Indices.push_back(1);
		quad.Indices.push_back(2);
		quad.Indices.push_back(2);
		quad.Indices.push_back(3);
		quad.Indices.push_back(0);
		return quad;
	}
};

__inline Primitive* UpcastObjectToPrimitive(Object* pObject) {
	if (pObject == nullptr)
		Raise("Object pointer is null");
	if (pObject->ObjectType != ObjectTypes::PRIMITIVE)
		Raise("Object pointer is not Primitive");

	Primitive* pPrimitive = dynamic_cast<Primitive*>(pObject);
	if (pPrimitive == nullptr)
		Raise("Failed to upcast Object to Primitive.");
	return pPrimitive;
}