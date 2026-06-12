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

	m_Model.SetPosition(m_Position);
	m_Model.SetAngle(m_Angle);
	m_Model.SetSize(m_Size);

	m_pCollider->SetLocalWorld(LocalToWorld());
}

void Object::Update(const Float& time) {
	if (m_pScript != nullptr)
		m_pScript->Update(time);

	RigidBody::Update(time);

	m_Model.SetPosition(m_Position);
	m_Model.SetAngle(m_Angle);
	m_Model.SetSize(m_Size);

	m_pCollider->SetLocalWorld(LocalToWorld());
}

Object& Object::_Clone(const Object& object) {
	if (this == &object)
		return *this;

	RemoveScript();

	if (object.m_pScript != nullptr) {
		Lua* pLua = object.m_pScript->GetLuaPtr();
		std::string filePath = object.m_pScript->GetFilePath();
		AttachScript(pLua, filePath, object.m_pScript->GetScriptData());
	}

	_SetParameters(object);
	_UpdateCollider();

	return *this;
}

Object& Object::_Move(Object&& object) {
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
	m_Model = object.m_Model;
	m_ParentPtr = object.m_ParentPtr;
	m_IsSelected = object.m_IsSelected;
	m_IsInvisible = object.m_IsInvisible;
	m_IsStarted = object.m_IsStarted;

	m_pCollider->ToggleVisible(object.m_pCollider->IsVisible());
}

void Object::_UpdateCollider() {
	if (m_Model.GetMesh().IsValid())
		m_pCollider->SetShape(m_Model.GetMesh().Get()->GetShape());
}

void Object::Render(NotNull<Nt::Renderer*> pRenderer) const {
	if ((!IsVisible()) || (m_IsStarted && m_IsInvisible))
		return;

	m_pCollider->Render(pRenderer);

	if (m_IsInvisible)
		pRenderer->GetShaderPtr()->SetUniform<Bool>("IsObjectInvisible", true);

	if (m_IsSelected)
		RenderOutline(pRenderer);

	m_Model.Render(pRenderer);

	if (m_IsSelected)
		pRenderer->GetShaderPtr()->SetUniform<Bool>("IsObjectSelected", false);
}

void Object::Render(NotNull<Nt::Renderer*> pRenderer, const uInt& offset, const uInt& verticesCount) const {
}

void Object::RenderOutline(NotNull<Nt::Renderer*> pRenderer) const {
	const Nt::Float4D color = pRenderer->GetColor();
	const Nt::CullFace cullFace = pRenderer->GetCullFace();
	const Float scaleValue = 1.05f;

	pRenderer->MatrixWorldPush();
	pRenderer->MatrixViewPush();
	pRenderer->SetView(pRenderer->GetView().Scale({ scaleValue, scaleValue, scaleValue }));
	pRenderer->SetColor(Nt::Colors::Orange);
	pRenderer->SetCullFace(Nt::CullFace::FRONT);
	pRenderer->DisableDepthMask();
	
	pRenderer->Transform(m_Position / scaleValue, m_Origin / scaleValue, m_Angle, m_AngleOrigin);
	pRenderer->UnbindTexture();

	if (m_Model.GetMesh().IsValid())
		pRenderer->Render(m_Model.GetMesh().Get());

	pRenderer->EnableDepthMask();
	pRenderer->SetCullFace(cullFace);
	pRenderer->SetColor(color);
	pRenderer->MatrixViewPop();
	pRenderer->MatrixWorldPop();
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
	return "Object";
}

TiXmlElement* Object::ToXML() const {
	TiXmlElement* elementIObject = new TiXmlElement("IObject");
	elementIObject->SetAttribute("IsVisible", IsVisible());
	elementIObject->LinkEndChild(VectorToXML("Position", m_Position));
	elementIObject->LinkEndChild(VectorToXML("Origin", m_Origin));
	elementIObject->LinkEndChild(VectorToXML("Angle", m_Angle));
	elementIObject->LinkEndChild(VectorToXML("AngleOrigin", m_AngleOrigin));
	elementIObject->LinkEndChild(VectorToXML("Size", m_Size));
	elementIObject->LinkEndChild(VectorToXML("Color", m_Color));

	TiXmlElement* elementRigidBody = new TiXmlElement("RigidBody");
	elementRigidBody->SetAttribute("g", m_G);
	elementRigidBody->SetAttribute("Mass", m_Mass);
	elementRigidBody->SetAttribute("Friction", m_Friction);
	elementRigidBody->SetAttribute("FrictionStatic", m_FrictionStatic);
	elementRigidBody->SetAttribute("FrictionAir", m_FrictionAir);
	elementRigidBody->SetAttribute("IsObjectCollided", m_IsObjectCollided);
	elementRigidBody->SetAttribute("IsActive", IsActive());
	elementRigidBody->SetAttribute("Enabled", IsPhysicsEnabled());
	elementRigidBody->SetAttribute("EnabledCollision", IsEnabledCollision());
	elementRigidBody->SetAttribute("EnabledGravitation", IsEnabledGravitation());

	elementRigidBody->LinkEndChild(elementIObject);
	elementRigidBody->LinkEndChild(VectorToXML("GravityDirection", m_GravityDirection));
	elementRigidBody->LinkEndChild(VectorToXML("Force", m_Force));
	elementRigidBody->LinkEndChild(VectorToXML("LinearAcceleration", m_LinearAcceleration));
	elementRigidBody->LinkEndChild(VectorToXML("LinearVelocity", m_LinearVelocity));
	elementRigidBody->LinkEndChild(VectorToXML("PrevPosition", m_PrevPosition));

	TiXmlElement* elementScript = new TiXmlElement("Script");
	if (m_pScript != nullptr)
		elementScript->SetAttribute("FilePath", m_pScript->GetFilePath().c_str());

	for (const Script::Data& data : m_ScriptData) {
		TiXmlElement* elementData = new TiXmlElement("Data");
		elementData->SetAttribute("Type", data.Type);
		elementData->SetAttribute("FieldName", data.FieldName.c_str());
		elementData->SetAttribute("Name", data.Name.c_str());
		elementData->SetAttribute("Value", data.Value.c_str());
		elementScript->LinkEndChild(elementData);
	}

	TiXmlElement* elementModel = new TiXmlElement("Model");
	const auto& meshHandle = m_Model.GetMesh();
	if (meshHandle.IsValid() && (!meshHandle.Get()->GetFilePath().empty()))
		elementModel->SetAttribute("MeshFilePath", meshHandle.Get()->GetFilePath().c_str());

	const auto& textureHandle = m_Model.GetTexture();
	if (meshHandle.IsValid() && (!textureHandle.Get()->GetFilePath().empty()))
		elementModel->SetAttribute("TextureFilePath", textureHandle.Get()->GetFilePath().c_str());

	TiXmlElement* elementThis = new TiXmlElement("Object");
	elementThis->SetAttribute("IsSelected", m_IsSelected);
	elementThis->SetAttribute("IsInvisible", m_IsInvisible);

	elementThis->LinkEndChild(elementRigidBody);
	elementThis->LinkEndChild(elementScript);
	elementThis->LinkEndChild(elementModel);

	return elementThis;
}

Nt::Renderer::DrawingMode Object::GetDrawingMode() const noexcept {
	return m_DrawingMode;
}

const Nt::Model& Object::GetModel() const noexcept {
	return m_Model;
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

Nt::Mesh* Object::GetMesh() const noexcept {
	return m_Model.GetMesh().Get();
}

Nt::Texture* Object::GetTexture() const noexcept {
	return m_Model.GetTexture().Get();
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
	return RigidBody::IsActive();;
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

void Object::SetModel(const Nt::Model& newModel) {
	m_Model = newModel;
	_UpdateCollider();
}

void Object::SetShape(const Nt::Shape& newShape) {
	Assert(m_Model.GetMesh().IsValid(), "Mesh pointer is null");
	m_Model.GetMesh().Get()->SetShape(newShape);
	_UpdateCollider();
}

void Object::SetTexture(Nt::Texture* pTexture) noexcept {
	m_Model.SetTextureByPtr(pTexture);
}

void Object::SetMesh(Nt::Mesh* pMesh) {
	m_Model.SetMeshByPtr(pMesh);
	_UpdateCollider();
}

void Object::SetTexture(const uInt& index) {
	m_Model.SetTexture(index);
}
void Object::SetTexture(const std::string& token) {
	m_Model.SetTexture(ResourceManager::Instance().GetIndex(token));
}

void Object::SetMesh(const std::string& token) {
	m_Model.SetMesh(ResourceManager::Instance().GetIndex(token));
	_UpdateCollider();
}
void Object::SetMesh(const uInt& index) {
	m_Model.SetMesh(index);
	_UpdateCollider();
}

void Object::SetPosition(const Nt::Float3D& position) {
	m_Model.SetPosition(position);
	IObject::SetPosition(position);
	//_UpdateCollider();
}

void Object::SetSize(const Nt::Float3D& size) {
	if (!m_Model.GetMesh().IsValid())
		m_Model.GetMesh().Get()->SetScale(size);

	m_Model.SetSize(size);
	IObject::SetSize(size);
	//_UpdateCollider();
}

void Object::SetAngle(const Nt::Float3D& angle) {
	m_Model.SetAngle(angle);
	IObject::SetAngle(angle);
}

void Object::SetAngleOrigin(const Nt::Float3D& angleOrigin) {
	m_Model.SetAngleOrigin(angleOrigin);
	IObject::SetAngleOrigin(angleOrigin);
}

void Object::SetOrigin(const Nt::Float3D& origin) {
	m_Model.SetOrigin(origin);
	IObject::SetOrigin(origin);
}

void Object::SetColor(const Nt::Float4D& color) {
	m_Model.SetColor(color);
	IObject::SetColor(color);
}