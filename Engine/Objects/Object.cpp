// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Object.h>
#include <ResourceManager.h>

Object::Object(std::string name, const ClassID id) :
	Identifier(id),
	m_Name(std::move(name))
{
}

Object::Object(const Object& object) :
	RigidBody(object),
	Identifier(object.GetID())
{
	_Clone(object);
}

Object::Object(Object&& object) noexcept :
	RigidBody(std::move(object)),
	Identifier(object.GetID())
{
	_Move(std::move(object));
}

Object::~Object() {
	RemoveScript();
	delete(m_pCollider);
}

void Object::Start() {
	if (m_pScript != nullptr) {
		m_ScriptData = m_pScript->GetScriptData();
		m_pScript->Start();
	}

	RigidBody::Activate();

	m_IsStarted = true;
}

void Object::Stop() {
	if (m_pScript != nullptr)
		m_pScript->Stop();

	RigidBody::Deactivate();

	m_IsStarted = false;
}

void Object::StaticUpdate() {
	if (!IsDirty())
		return;

	RigidBody::StaticUpdate();
	m_pCollider->SetLocalWorld(LocalToWorld());
}

void Object::Update(const Float& time) {
	if (m_IsStarted) {
		if (m_pScript != nullptr)
			m_pScript->Update(time);
		RigidBody::Update(time);
	}

	m_pCollider->SetLocalWorld(LocalToWorld());
}

Object& Object::_Clone(const Object& object) {
	if (this == &object)
		return *this;

	_SetParameters(object);
	_UpdateCollider();

	return *this;
}

Object& Object::_Move(Object&& object) {
	assert(0);
	if (this == &object)
		return *this;

	RemoveScript();

	if (object.m_pScript != nullptr) {
		m_pScript = std::move(object.m_pScript);
		object.m_pScript = nullptr;
	}

	_SetParameters(object);
	_UpdateCollider();

	return *this;
}

void Object::_SetParameters(const Object& object) noexcept {
	if (this == &object)
		return;

	m_LayerName = object.m_LayerName;
	m_Name = object.m_Name;
	m_ParentPtr = object.m_ParentPtr;
	m_IsSelected = object.m_IsSelected;
	m_IsInvisible = object.m_IsInvisible;
	m_IsStarted = object.m_IsStarted;
	m_Mesh = object.m_Mesh;
	m_Texture = object.m_Texture;

	m_pCollider->ToggleVisible(object.m_pCollider->IsVisible());
}

void Object::_UpdateCollider() {
	if (m_Mesh.IsValid())
		m_pCollider->SetShape(m_Mesh.Get()->GetShape());
}

void Object::Render(NotNull<Nt::Renderer*> pRenderer) const
{
}
void Object::Render(NotNull<Nt::Renderer*> pRenderer, const uInt& offset, const uInt& verticesCount) const
{
}

void Object::EnableOutline() noexcept {
	m_IsSelected = true;
}

void Object::DisableOutline() noexcept {
	m_IsSelected = false;
}

void Object::EnableInvisible() noexcept {
	m_IsInvisible = true;
}

void Object::DisableInvisible() noexcept {
	m_IsInvisible = false;
}

void Object::ShowingCollider() {
	m_pCollider->Show();
}

void Object::HidingCollider() {
	m_pCollider->Hide();
}

void Object::AttachScript(NotNull<Lua*> pLua, const Nt::String& filePath, const std::vector<Script::Data>& data) {
	if (m_pScript != nullptr)
		delete(m_pScript);

	m_pScript = new Script(pLua);
	m_pScript->Load(filePath, this);
	m_pScript->SetScriptData(data);
}

void Object::RemoveScript() {
	m_ScriptData.clear();
	SAFE_DELETE(&m_pScript);
}

void Object::Collision(NotNull<Object*> pObject) {
	if (!(IsActive() && IsEnabledCollision() && pObject->IsEnabledCollision()))
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

Bool Object::CheckCollision(const Object* pObject) const {
	if (!IsActive())
		return false;
	return m_pCollider->GJK(*pObject->m_pCollider).first;
}

Int Object::RayCastTest(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint) const {
	return m_pCollider->RayCastTest(ray, pResultIntersectionPoint);
}

Object& Object::operator = (const Object& object) {
	if (this == &object)
		return *this;

	RigidBody::operator=(object);
	return _Clone(object);
}
Object& Object::operator = (Object&& object) noexcept {
	if (this == &object)
		return *this;

	RigidBody::operator=(std::move(object));
	return _Move(std::move(object));
}

Object* Object::GetCopy() const {
	return new Object(*this);
}

std::string Object::GetTypeToken() const noexcept {
	return "Object";
}
std::string Object::GetToken() const noexcept {
	return GetTypeToken();
}

Nt::Renderer::DrawingMode Object::GetDrawingMode() const noexcept {
	return m_DrawingMode;
}

const Nt::Collider* Object::GetCollider() const noexcept {
	return m_pCollider;
}

Script* Object::GetScript() const noexcept {
	return m_pScript;
}

const std::vector<Script::Data>& Object::GetScriptData() const noexcept {
	return m_ScriptData;
}

Nt::String Object::GetLayerName() const noexcept {
	return m_LayerName;
}

Nt::String Object::GetName() const noexcept {
	return m_Name;
}

Object* Object::GetParentPtr() const noexcept {
	return m_ParentPtr;
}

Nt::ResourceHandle<Nt::Mesh> Object::GetMesh() const noexcept {
	return m_Mesh;
}
Nt::ResourceHandle<Nt::Texture> Object::GetTexture() const noexcept {
	return m_Texture;
}

Bool Object::IsSelected() const noexcept {
	return m_IsSelected;
}
Bool Object::IsInvisible() const noexcept {
	return m_IsInvisible;
}
Bool Object::IsStarted() const noexcept {
	return m_IsStarted;
}
Bool Object::IsActivePhysics() const noexcept {
	return RigidBody::IsActive();
}

void Object::SetDrawingMode(Nt::Renderer::DrawingMode mode) noexcept {
	m_DrawingMode = mode;
}

void Object::SetName(const Nt::String& newName) {
	m_Name = newName;
}

void Object::SetLayerName(const Nt::String& name) {
	m_LayerName = name;
}

void Object::SetParentPtr(Object* pNewParent) noexcept {
	m_ParentPtr = pNewParent;
}

void Object::SetShape(const Nt::Shape& newShape) {
	if (!m_Mesh.IsValid())
		return;

	m_Mesh.Get()->SetShape(newShape);
	_UpdateCollider();
}

void Object::SetTexture(Nt::Texture* pTexture) noexcept {
	m_Texture = pTexture;
}

void Object::SetMesh(Nt::Mesh* pMesh) {
	m_Mesh = pMesh;
	_UpdateCollider();
}

void Object::SetTexture(const uInt& index) {
	m_Texture.Set(index);
}
void Object::SetTexture(const std::string& token) {
	m_Texture.Set(ResourceManager::Instance().GetIndex(token));
}

void Object::SetMesh(const std::string& token) {
	m_Mesh.Set(ResourceManager::Instance().GetIndex(token));
	_UpdateCollider();
}
void Object::SetMesh(const uInt& index) {
	m_Mesh.Set(index);
	_UpdateCollider();
}

void Object::SetPosition(const Nt::Float3D& position) {
	IObject::SetPosition(position);
}
void Object::SetSize(const Nt::Float3D& size) {
	IObject::SetSize(size);
}
void Object::SetAngle(const Nt::Float3D& angle) {
	IObject::SetAngle(angle);
}
void Object::SetAngleOrigin(const Nt::Float3D& angleOrigin) {
	IObject::SetAngleOrigin(angleOrigin);
}
void Object::SetOrigin(const Nt::Float3D& origin) {
	IObject::SetOrigin(origin);
}
void Object::SetColor(const Nt::Float4D& color) {
	IObject::SetColor(color);
}