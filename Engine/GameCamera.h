#pragma once

class GameCamera : public Entity {
public:
	GameCamera(const Nt::String& name) : Entity(name, EntityTypes::CAMERA) { 
		Nt::Shape doubleSideQuad = Nt::Geometry::Quad({ 1.f, 1.f }, Nt::Colors::White);
		for (uInt i = 0; i < 6; ++i)
			doubleSideQuad.Indices.push_back(doubleSideQuad.Indices[i]);

		std::swap(doubleSideQuad.Indices[1], doubleSideQuad.Indices[4]);
		for (Nt::Vertex& vertex : doubleSideQuad.Vertices)
			std::swap(vertex.Position.x, vertex.Position.z);

		m_Model.SetMesh(doubleSideQuad);

		if (g_TextureCameraIndex != uInt(-1))
			m_Model.SetTexture(g_TextureCameraIndex);
		else
			Nt::Log::Warning("Texture camera index is -1.");

		m_Camera.SetPosition({ 0.5f, -0.5f, 0.5f });

		_UpdateColliders();
		DisableCollider();
	}
	GameCamera(const GameCamera& camera) :
		Entity(camera),
		m_Camera(camera.m_Camera)
	{
	}

	void Write(std::ostream& stream) const override {
		Entity::Write(stream);
	}
	void Read(std::istream& stream) override {
		Entity::Read(stream);
		m_Camera = Nt::Camera(-m_Position + Nt::Float3D(0.5f, -0.5f, 0.5f), m_Angle);
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}

	void Render(Nt::Renderer* pRenderer) const override {
		if (!m_IsStarted)
			Entity::Render(pRenderer);
	}

	void Set(Nt::RenderWindow* windowPtr) {
		if (windowPtr == nullptr)
			Raise("Window pointer is nullptr");
		windowPtr->SetCurrentCamera(&m_Camera);
	}

	virtual Object* GetCopy() const override {
		return new GameCamera(*this);
	}
	void SetOrigin(const Nt::Float3D& origin) override {
		Entity::SetOrigin(origin);
		m_Camera.SetOrigin(-origin);
	}
	void SetPosition(const Nt::Float3D& position) override {
		Entity::SetPosition(position);
		m_Camera.SetPosition(-position + Nt::Float3D(0.5f, -0.5f, 0.5f));
	}
	void SetAngle(const Nt::Float3D& angle) override {
		Entity::SetAngle(angle);
		m_Camera.SetAngle(angle);
	}

private:
	using Entity::SetModel;

private:
	Nt::Camera m_Camera;
};

__inline GameCamera* UpcastEntityToGameCamera(Entity* pEntity) {
	if (pEntity == nullptr)
		Raise("Entity pointer is null");
	if (pEntity->GetEntityType() != EntityTypes::CAMERA)
		Raise("Entity pointer is not GameCamera");

	GameCamera* pGameCamera = dynamic_cast<GameCamera*>(pEntity);
	if (pGameCamera == nullptr)
		Raise("Failed to upcast Object to GameCamera.");
	return pGameCamera;
}