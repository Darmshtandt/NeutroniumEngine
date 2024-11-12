#pragma once

enum class ObjectTypes {
	NONE,
	PRIMITIVE,
	ENTITY,
};

class Object : public Nt::RigidBody, public Nt::ISerialization {
public:
	const ObjectTypes ObjectType;

public:
	Object(const ObjectTypes& type, const std::string& name) :
		ObjectType(type),
		m_Name(name)
	{
	}
	Object(const ObjectTypes& type, const std::string& name, const Nt::Model& model) :
		m_Model(model),
		ObjectType(type),
		m_Name(name)
	{
		_RecreateColliders();
	}
	Object(const ObjectTypes& type, const std::string& name, const Nt::Mesh& mesh) :
		m_Model(mesh),
		ObjectType(type),
		m_Name(name)
	{
		_RecreateColliders();
	}
	Object(const Object& object) :
		RigidBody(object),
		ObjectType(object.ObjectType),
		m_Model(object.m_Model),
		m_Name(object.m_Name),
		m_LayerName(object.m_LayerName),
		m_ParentPtr(object.m_ParentPtr),
		m_pScript(nullptr),
		m_IsColliderShowed(object.m_IsColliderShowed),
		m_IsInvisible(object.m_IsInvisible)
	{
		_RecreateColliders();
	}
	~Object() {
		RemoveScript();

		SAFE_DELETE(&m_pCollider);
	}

	void Write(std::ostream& stream) const override {
		m_Model.Write(stream);

		Nt::Serialization::WriteAll(stream, 
			m_GravityDirection, m_G, m_Friction, 
			IsPhysicsEnabled(), IsEnabledCollision(), IsEnabledGravitation(),
			m_Name, m_Position, m_Origin, m_Size, m_Angle, m_AngleOrigin, m_IsInvisible);
	}
	void Read(std::istream& stream) override {
		m_LinearVelocity = { };
		m_IsSelected = false;

		Bool isPhysicsEnabled = false;
		Bool isEnabledCollision = false;
		Bool isEnabledGravitation = false;

		m_Model.Read(stream);

		Nt::Serialization::ReadAll(stream, 
			m_GravityDirection, m_G, m_Friction,
			isPhysicsEnabled, isEnabledCollision, isEnabledGravitation,
			m_Name, m_Position, m_Origin, m_Size, m_Angle, m_AngleOrigin, m_IsInvisible);

		if (isPhysicsEnabled)
			EnablePhysics();
		else
			DisablePhysics();

		if (isEnabledCollision)
			EnableCollider();
		else
			DisableCollider();

		if (isEnabledGravitation)
			EnableGravitation();
		else
			DisableGravitation();

		_RecreateColliders();
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(Object);
	}
	constexpr uInt ClassType() const noexcept {
		return uInt(ObjectTypes::NONE);
	}
	static ISerialization* New(const uInt& classType);

	virtual void Start() {
		if (m_pScript != nullptr) {
			m_ScriptData = m_pScript->GetScriptData();
			m_pScript->Start();
		}

		RigidBody::Activate();

		m_IsStarted = true;
	}
	virtual void Stop() {
		if (m_pScript != nullptr)
			m_pScript->Stop();

		RigidBody::Deactivate();

		m_IsStarted = false;
	}

	virtual void Update(const Float& time) {
		if (m_pScript != nullptr)
			m_pScript->Update(time);

		RigidBody::Update(time);

		m_Model.SetPosition(m_Position);
		m_Model.SetAngle(m_Angle);

		if (m_pCollider != nullptr)
			m_pCollider->SetLocalWorld(LocalToWorld());
	}
	virtual void Render(Nt::Renderer* pRenderer) const override {
		if (!IsRenderEnabled())
			return;

		if (m_IsStarted && m_IsInvisible)
			return;

		if (m_IsColliderShowed && m_pCollider)
			m_pCollider->Render(pRenderer);
		
		if (m_IsInvisible)
			pRenderer->GetShaderPtr()->SetUniform("IsObjectInvisible", GL_INT, true);

		if (m_IsSelected) {
			pRenderer->GetShaderPtr()->SetUniform("IsObjectSelected", GL_INT, true);
			m_Model.Render(pRenderer);
			pRenderer->GetShaderPtr()->SetUniform("IsObjectSelected", GL_INT, false);
		}
		else {
			m_Model.Render(pRenderer);
		}

		if (m_IsInvisible)
			pRenderer->GetShaderPtr()->SetUniform("IsObjectInvisible", GL_INT, false);
	}

	void EnableSelectionColor() noexcept {
		m_IsSelected = true;
	}
	void DisableSelectionColor() noexcept {
		m_IsSelected = false;
	}
	void EnableInvisible() noexcept {
		m_IsInvisible = true;
	}
	void DisableInvisible() noexcept {
		m_IsInvisible = false;
	}
	void ShowingCollider() {
		m_IsColliderShowed = true;
	}
	void HidingCollider() {
		m_IsColliderShowed = false;
	}

	void AttachScript(Lua* pLua, Scene* pScence, const Nt::String& filePath, const std::vector<Script::Data>& data) {
		if (m_pScript)
			delete(m_pScript);

		m_pScript = new Script(pLua);
		m_pScript->Initialize(pScence);
		m_pScript->Load(filePath, this);
		m_pScript->SetScriptData(data);
	}
	void RemoveScript() {
		SAFE_DELETE(&m_pScript);
	}

	void Collision(Object* pObject) {
		if ((!IsActive()) || (!IsEnabledCollision()) || (!pObject->IsEnabledCollision()))
			return;

		const std::pair<Bool, Nt::Simplex> gjk = m_pCollider->GJK(*pObject->m_pCollider);
		m_IsObjectCollided = gjk.first;
		if (m_IsObjectCollided) {
			const Nt::CollisionPoint point = m_pCollider->EPA(gjk.second, *pObject->m_pCollider);
			Translate(-point.normal * point.depth);

			SetLinearVelocity({ });
			SetFrictionStatic(pObject->GetFriction());
		}
		else {
			SetFrictionStatic(pObject->GetFrictionAir());
		}
	}
	Bool CheckCollision(const Object* pObject) const {
		if (!IsActive())
			return false;
		return m_pCollider->GJK(*pObject->m_pCollider).first;
	}
	Int RayCastTest(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint = nullptr) const {
		return m_pCollider->RayCastTest(ray, pResultIntersectionPoint);
	}

	virtual Object* GetCopy() const {
		return new Object(*this);
	}
	const Nt::Model& GetModel() const {
		return m_Model;
	}
	Nt::Collider::PointContainer GetColliderPointContainer() const {
		if (m_pCollider == nullptr) {
			Raise("Collider pointer is null");
			return { };
		}

		return m_pCollider->GetPointContainer();
	}
	Script* GetScript() const noexcept {
		return m_pScript;
	}
	Nt::String GetLayerName() const noexcept {
		return m_LayerName;
	}
	Nt::String GetName() const noexcept {
		return m_Name;
	}
	Object* GetParentPtr() const noexcept {
		return m_ParentPtr;
	}
	Nt::Texture* GetTexturePtr() const noexcept {
		return m_Model.GetTexturePtr();
	}
	Bool IsSelected() const noexcept {
		return m_IsSelected;
	}
	Bool IsInvisible() const noexcept {
		return m_IsInvisible;
	}
	Bool IsShowingCollider() const noexcept {
		return m_IsColliderShowed;
	}
	Bool IsStarted() const noexcept {
		return m_IsStarted;
	}
	Bool IsActivePhycisc() const noexcept {
		return RigidBody::IsActive();;
	}

	void SetName(const Nt::String& newName) {
		m_Name = newName;
	}
	void SetLayerName(const Nt::String& name) {
		m_LayerName = name;
	}
	void SetParentPtr(Object* pNewParent) noexcept {
		m_ParentPtr = pNewParent;
	}
	void SetTexture(const uInt& textureIndex) {
		m_Model.SetTexture(textureIndex);
	}
	void SetTexture(const Nt::Texture& texture) {
		m_Model.SetTexture(texture);
	}
	void SetMesh(const uInt& meshIndex) {
		m_Model.SetMesh(meshIndex);
		_RecreateColliders();
	}
	void SetMesh(const Nt::Mesh& mesh) {
		m_Model.SetMesh(mesh);
		_RecreateColliders();
	}
	void SetModel(const Nt::Model& newModel) {
		m_Model = newModel;
		_RecreateColliders();
	}
	virtual void SetPosition(const Nt::Float3D& position) override {
		m_Model.SetPosition(position);
		IObject::SetPosition(position);
		_RecreateColliders();
	}
	virtual void SetSize(const Nt::Float3D& size) override {
		if (m_Model.GetMeshPtr())
			m_Model.GetMeshPtr()->SetScale(size);

		m_Model.SetSize(size);
		IObject::SetSize(size);
		_RecreateColliders();
	}
	virtual void SetAngle(const Nt::Float3D& angle) override {
		m_Model.SetAngle(angle);
		IObject::SetAngle(angle);
	}
	virtual void SetAngleOrigin(const Nt::Float3D& angleOrigin) override {
		m_Model.SetAngleOrigin(angleOrigin);
		IObject::SetAngleOrigin(angleOrigin);
	}
	virtual void SetOrigin(const Nt::Float3D& origin) override {
		m_Model.SetOrigin(origin);
		IObject::SetOrigin(origin);
	}
	virtual void SetColor(const Nt::Float4D& color) override {
		m_Model.SetColor(color);
		IObject::SetColor(color);
	}

private:
	using RigidBody::IsActive;

protected:
	std::vector<Script::Data> m_ScriptData;
	std::string m_LayerName = "Main";
	std::string m_Name;

	Nt::Collider* m_pCollider = nullptr;
	Nt::Model m_Model;

	Object* m_ParentPtr = nullptr;
	Script* m_pScript = nullptr;

	Bool m_IsSelected = false;
	Bool m_IsColliderShowed = false;
	Bool m_IsInvisible = false;
	Bool m_IsStarted = false;

protected:
	void _RecreateColliders() {
		if (m_pCollider != nullptr)
			delete(m_pCollider);

		m_pCollider = new Nt::Collider;
		m_pCollider->SetShape(m_Model.GetMeshPtr()->GetShape(), m_Size);
		m_pCollider->SetLocalWorld(LocalToWorld());
	}
};

using ObjectContainer = std::vector<Object*>;