// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Object.h>
#include <ResourceManager.h>

Object::Object(std::string name, const ClassID id) :
	NtEx::Object(id),
	m_Name(std::move(name)),
	m_Collider(new Nt::Collider)
{
	m_RigidBody = AddComponent<NtEx::RigidBody>();
}
Object::Object(const Object& object) :
	IObject(object),
	NtEx::Object(object),
	m_Collider(new Nt::Collider)
{
	_Clone(object);
}
Object::Object(Object&& object) noexcept :
	IObject(std::move(object)),
	NtEx::Object(std::move(object)),
	m_Collider(new Nt::Collider)
{
	_Move(std::move(object));
}
Object::~Object() {
	RemoveScript();
}

void Object::Start() {
	if (m_pScript != nullptr) {
		m_ScriptData = m_pScript->GetScriptData();
		m_pScript->Start();
	}

	m_IsStarted = true;
}

void Object::Stop() {
	if (m_pScript != nullptr)
		m_pScript->Stop();

	m_IsStarted = false;
}

void Object::StaticUpdate() {
	if (IsDirty())
		m_Collider->SetLocalWorld(LocalToWorld());
}

void Object::Update(Float deltaTime) {
	if (m_IsStarted) {
		if (m_pScript != nullptr)
			m_pScript->Update(deltaTime);

		m_DeltaTime = deltaTime;
		if (m_EnabledGravitation)
			m_RigidBody->Body.AddForce(m_GravityDirection);

		if (m_RigidBody->Body.HasForce()) {
			m_RigidBody->Body.ApplyForces(m_DeltaTime);

			Translate(m_RigidBody->Body.GetLinearVelocity() * m_DeltaTime);

			m_RigidBody->Body.SetLinearVelocity({ });
		}
	}

	m_Collider->SetLocalWorld(LocalToWorld());
}

void Object::AddForce(Nt::Float3D force) noexcept {
	m_RigidBody->Body.AddForce(force);
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

	m_DrawingMode = object.m_DrawingMode;
	m_ParentPtr = object.m_ParentPtr;
	m_Mesh = object.m_Mesh;
	m_Texture = object.m_Texture;
	m_LayerName = object.m_LayerName;
	m_Name = object.m_Name;

	m_TextureLocalWorld = object.m_TextureLocalWorld;
	m_TextureOffset = object.m_TextureOffset;
	m_TextureScale = object.m_TextureScale;
	m_TextureRotation = object.m_TextureRotation;

	m_IsTexChanged = object.m_IsTexChanged;
	m_IsSelected = object.m_IsSelected;
	m_IsInvisible = object.m_IsInvisible;
	m_IsStarted = object.m_IsStarted;

	m_Collider->ToggleVisible(object.m_Collider->IsVisible());
}

void Object::_UpdateCollider() {
	if (m_Mesh.IsValid())
		m_Collider->SetShape(m_Mesh.Get()->GetShape());
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
	m_Collider->Show();
}

void Object::HidingCollider() {
	m_Collider->Hide();
}

void Object::AttachScript(NotNull<Lua*> pLua, const Nt::String& filePath, const std::vector<Script::Data>& data) {
	if (m_pScript != nullptr)
		delete(m_pScript);

	m_pScript = new Script(pLua, filePath, this);
	m_pScript->SetScriptData(data);
}

void Object::RemoveScript() {
	m_ScriptData.clear();
	SAFE_DELETE(&m_pScript);
}

void Object::Collision(NotNull<Object*> pObject) {
	if (m_EnabledCollider && pObject->m_EnabledCollider)
		return;

	const std::pair<Bool, Nt::Simplex> gjk = m_Collider->GJK(*pObject->m_Collider);
	if (gjk.first) {
		const Nt::CollisionPoint point = m_Collider->EPA(gjk.second, *pObject->m_Collider);
		Translate(-point.normal * point.depth);

		m_RigidBody->Body.SetLinearVelocity({ });
	}
}

Bool Object::CheckCollision(const Object* pObject) const {
	if (!m_EnabledCollider)
		return false;
	return m_Collider->GJK(*pObject->m_Collider).first;
}

Int Object::RayCastTest(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint) const {
	return m_Collider->RayCastTest(ray, pResultIntersectionPoint);
}

Object& Object::operator = (const Object& object) {
	if (this == &object)
		return *this;

	IObject::operator=(object);
	return _Clone(object);
}
Object& Object::operator = (Object&& object) noexcept {
	if (this == &object)
		return *this;

	IObject::operator=(std::move(object));
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

Nt::Collider* Object::GetCollider() const noexcept {
	return m_Collider.get();
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

NtEx::RigidBody* Object::GetRigidBody() const noexcept {
	return m_RigidBody;
}

Nt::Matrix3x3 Object::TextureLocalWorld() const noexcept {
	if (m_IsTexChanged) {
		m_TextureLocalWorld =
			Nt::Matrix3x3::GetTranslate({ m_TextureOffset, 1.f }) *
			Nt::Matrix3x3::GetRotateZ(m_TextureRotation) *
			Nt::Matrix3x3::GetScale({ m_TextureScale, 1.f });

		m_IsTexChanged = false;
	}

	return m_TextureLocalWorld;
}

Nt::Float2D Object::GetTextureOffset() const noexcept {
	return m_TextureOffset;
}

Nt::Float2D Object::GetTextureScale() const noexcept {
	return m_TextureScale;
}

Float Object::GetTextureRotation() const noexcept {
	return m_TextureRotation;
}
Bool Object::EnabledGravitation() const noexcept {
	return m_EnabledGravitation;
}

Bool Object::EnabledCollider() const noexcept {
	return m_EnabledCollider;
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

void Object::ToggleGravitation(Bool enabled) noexcept {
	m_EnabledGravitation = enabled;
}

void Object::ToggleCollider(Bool enabled) noexcept {
	m_EnabledCollider = enabled;
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
	assert(0);
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

void Object::SetTextureOffset(const Nt::Float2D& textureOffset) noexcept {
	if (m_TextureOffset == textureOffset)
		return;

	m_TextureOffset = textureOffset;
	m_IsTexChanged = true;
}

void Object::SetTextureScale(const Nt::Float2D& textureScale) noexcept {
	if (m_TextureScale == textureScale)
		return;

	m_TextureScale = textureScale;
	m_IsTexChanged = true;
}

void Object::SetTextureRotation(Float angle) noexcept {
	if (m_TextureRotation == angle)
		return;

	m_TextureRotation = angle;
	m_IsTexChanged = true;
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

ObjectContainer::const_iterator FindObject(const ObjectContainer& objects, NotNull<Object*> pObject) noexcept {
	return std::find_if(objects.begin(), objects.end(), [&] (const ObjectPtr& object) {
		return object.get() == pObject;
		});
}

ObjectContainer::const_iterator FindObject(const ObjectContainer& objects, NotNull<const Object*> pObject) noexcept {
	return std::find_if(objects.begin(), objects.end(), [&] (const ObjectPtr& object) {
		return object.get() == pObject;
		});
}

WeakObjectContainer::const_iterator FindObject(const WeakObjectContainer& objects, WeakObjectPtr weakObject) noexcept {
	const auto object = weakObject.lock();
	if (!object)
		return objects.cend();
	return std::find_if(objects.begin(), objects.end(), [&] (const WeakObjectPtr& other) {
		return other.lock() == object;
		});
}
