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
		_UpdateColliders();
	}
	Object(const ObjectTypes& type, const std::string& name, const Nt::Mesh& mesh) :
		m_Model(mesh),
		ObjectType(type),
		m_Name(name)
	{
		_UpdateColliders();
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
		_UpdateColliders();
	}
	~Object() {
		RemoveScript();
		delete(m_pCollider);
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

		_UpdateColliders();
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}
	constexpr uInt ClassType() const noexcept {
		return uInt(ObjectTypes::NONE);
	}
	static ISerialization* New(const uInt& ClassType);

	virtual void Start() {
		if (m_pScript) {
			m_ScriptData = m_pScript->GetScriptData();
			m_pScript->Start();
		}
		Activate();
		m_IsStarted = true;
	}
	virtual void Stop() {
		if (m_pScript)
			m_pScript->Stop();
		Deactivate();
		m_IsStarted = false;
	}

	virtual void Update(const Float& time) {
		if (m_pScript)
			m_pScript->Update(time);

		RigidBody::Update(time);
		m_Model.SetPosition(m_Position);
		m_Model.SetAngle(m_Angle);

		if (m_pCollider)
			m_pCollider->SetLocalWorld(LocalToWorld());
	}
	virtual void Render(Nt::Renderer* pRenderer) const override {
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

	void AttachScript(Scence* pScence, const Nt::String& filePath) {
		if (m_pScript)
			delete(m_pScript);

		m_pScript = new Script;
		m_pScript->Initialize(pScence);
		m_pScript->Load(filePath, this);
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
	Bool RayCastTest(const Nt::Ray& ray) const {
		return m_pCollider->RayCastTest(ray);
	}

	virtual Object* GetCopy() const {
		return new Object(*this);
	}
	const Nt::Model& GetModel() const {
		return m_Model;
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

	void SetName(const Nt::String& newName) {
		m_Name = newName;
	}
	void SetLayerName(const Nt::String& name) {
		m_LayerName = name;
	}
	void SetParentPtr(Object* pNewParent) noexcept {
		m_ParentPtr = pNewParent;
	}
	void SetTexture(const Nt::Texture& texture) {
		m_Model.SetTexture(texture);
	}
	void SetModel(const Nt::Model& newModel) {
		m_Model = newModel;
		_UpdateColliders();
	}
	void SetMesh(const Nt::Mesh& mesh) {
		m_Model.SetMesh(mesh);
		_UpdateColliders();
	}
	virtual void SetPosition(const Nt::Float3D& position) override {
		m_Model.SetPosition(position);
		IObject::SetPosition(position);
	}
	virtual void SetSize(const Nt::Float3D& size) override {
		if (m_Model.GetMeshPtr())
			m_Model.GetMeshPtr()->SetScale(size);
		m_Model.SetSize(size);
		IObject::SetSize(size);
		_UpdateColliders();
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

protected:
	std::vector<Script::Data> m_ScriptData;
	Nt::Collider* m_pCollider = nullptr;
	Object* m_ParentPtr = nullptr;
	Script* m_pScript = nullptr;
	Nt::Model m_Model;
	std::string m_Name;
	std::string m_LayerName = "Main";
	Bool m_IsSelected = false;
	Bool m_IsColliderShowed = false;
	Bool m_IsInvisible = false;
	Bool m_IsStarted = false;

protected:
	void _UpdateColliders() {
		if (m_pCollider)
			delete(m_pCollider);

		m_pCollider = new Nt::Collider;
		m_pCollider->SetShape(m_Model.GetMeshPtr()->GetShape());
		m_pCollider->SetLocalWorld(LocalToWorld());
	}
};

using ObjectContainer = std::vector<Object*>;