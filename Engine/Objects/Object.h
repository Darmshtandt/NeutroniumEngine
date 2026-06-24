#pragma once

#include <Nt/Graphics/Objects/Model.h>
#include <Nt/Graphics/Resources/Texture.h>
#include <Nt/Graphics/Resources/Mesh.h>
#include <Nt/Graphics/Resources/ResourceHandle.h>
#include <Script/Script.h>

#include <Nt/Graphics/Ex/Components/Physics.h>
#include <Nt/Graphics/Ex/Object.h>
#include <Nt/Collider.h>

using NtEx::ClassID;
using NtEx::Class;

class Lua;
class Script;
class Scene;

class Object : public Nt::IObject, public NtEx::Object {
protected:
	Object(std::string name, const ClassID id);

public:
	Object(const Object& object);
	Object(Object&& object) noexcept;
	~Object() override;

	virtual void Start();
	virtual void Stop();

	void StaticUpdate() override;
	void Update(Float deltaTime);

	void AddForce(Nt::Float3D force) noexcept;

	void EnableOutline() noexcept;
	void DisableOutline() noexcept;
	void EnableInvisible() noexcept;
	void DisableInvisible() noexcept;
	void ShowingCollider();
	void HidingCollider();

	void AttachScript(NotNull<Lua*> pLua, const Nt::String& filePath, const std::vector<Script::Data>& data);
	void RemoveScript();

	void Collision(NotNull<Object*> pObject);
	Bool CheckCollision(const Object* pObject) const;
	Int RayCastTest(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint = nullptr) const;

	Object& operator = (const Object& object);
	Object& operator = (Object&& object) noexcept;

	[[nodiscard]] virtual Object* GetCopy() const;
	[[nodiscard]] virtual std::string GetTypeToken() const noexcept;
	[[nodiscard]] virtual std::string GetToken() const noexcept;

	Nt::Renderer::DrawingMode GetDrawingMode() const noexcept;
	Nt::Collider* GetCollider() const noexcept;
	Script* GetScript() const noexcept;
	const std::vector<Script::Data>& GetScriptData() const noexcept;
	Nt::String GetLayerName() const noexcept;
	Nt::String GetName() const noexcept;
	Object* GetParentPtr() const noexcept;
	Nt::ResourceHandle<Nt::Texture> GetTexture() const noexcept;
	Nt::ResourceHandle<Nt::Mesh> GetMesh() const noexcept;
	NtEx::RigidBody* GetRigidBody() const noexcept;

	Nt::Matrix3x3 TextureLocalWorld() const noexcept;
	Nt::Float2D GetTextureOffset() const noexcept;
	Nt::Float2D GetTextureScale() const noexcept;
	Float GetTextureRotation() const noexcept;

	Bool EnabledGravitation() const noexcept;
	Bool EnabledCollider() const noexcept;
	Bool IsSelected() const noexcept;
	Bool IsInvisible() const noexcept;
	Bool IsStarted() const noexcept;

	void ToggleGravitation(Bool enabled) noexcept;
	void ToggleCollider(Bool enabled) noexcept;
	void ToggleVisible(Bool enabled) noexcept;

	void SetDrawingMode(Nt::Renderer::DrawingMode mode) noexcept;
	void SetName(const Nt::String& newName);
	void SetLayerName(const Nt::String& name);
	void SetParentPtr(Object* pNewParent) noexcept;
	void SetShape(const Nt::Shape& newShape);

	void SetTexture(Nt::Texture* pTexture) noexcept;
	void SetTexture(const uInt& index);
	void SetTexture(const std::string& token);
	void SetMesh(Nt::Mesh* pMesh);
	void SetMesh(const std::string& token);
	void SetMesh(const uInt& index);

	void SetTextureOffset(const Nt::Float2D& textureOffset) noexcept;
	void SetTextureScale(const Nt::Float2D& textureScale) noexcept;
	void SetTextureRotation(Float angle) noexcept;

	virtual void SetPosition(const Nt::Float3D& position);
	virtual void SetSize(const Nt::Float3D& size);
	virtual void SetAngle(const Nt::Float3D& angle);
	virtual void SetAngleOrigin(const Nt::Float3D& angleOrigin);
	virtual void SetOrigin(const Nt::Float3D& origin);
	virtual void SetColor(const Nt::Float4D& color);

protected:
	NtEx::RigidBody* m_RigidBody;

	Nt::Renderer::DrawingMode m_DrawingMode = Nt::Renderer::DrawingMode::TRIANGLES;
	Nt::ResourceHandle<Nt::Mesh> m_Mesh;
	Nt::ResourceHandle<Nt::Texture> m_Texture;
	std::unique_ptr<Nt::Collider> m_Collider;
	std::vector<Script::Data> m_ScriptData;
	std::string m_LayerName = "Main";
	std::string m_Name;

	Nt::Float3D m_GravityDirection = { 0.f, -1.f, 0.f };
	Float m_DeltaTime = 0.f;
	Bool m_EnabledGravitation = false;
	Bool m_EnabledCollider = false;

	mutable Nt::Matrix3x3 m_TextureLocalWorld;
	Nt::Float2D m_TextureOffset;
	Nt::Float2D m_TextureScale = { 1.f, 1.f };
	Float m_TextureRotation = 0.f;

	Object* m_ParentPtr = nullptr;
	Script* m_pScript = nullptr;

	mutable Bool m_IsTexChanged = false;
	Bool m_IsSelected = false;
	Bool m_IsInvisible = false;
	Bool m_IsStarted = false;
	Bool m_IsVisible = false;

private:
	Object& _Clone(const Object& object);
	Object& _Move(Object&& object);

	void _SetParameters(const Object& object) noexcept;
	void _UpdateCollider();

	void Render(NotNull<Nt::Renderer*> pRenderer, const uInt& offset, const uInt& verticesCount) const override;
	void Render(NotNull<Nt::Renderer*> pRenderer) const override;
};

using ObjectPtr = std::shared_ptr<Object>;
using ObjectContainer = std::vector<ObjectPtr>;
using WeakObjectPtr = std::weak_ptr<Object>;
using WeakObjectContainer = std::vector<WeakObjectPtr>;

[[nodiscard]] ObjectContainer::const_iterator FindObject(const ObjectContainer& objects, NotNull<Object*> pObject) noexcept;
[[nodiscard]] ObjectContainer::const_iterator FindObject(const ObjectContainer& objects, NotNull<const Object*> pObject) noexcept;
[[nodiscard]] WeakObjectContainer::const_iterator FindObject(const WeakObjectContainer& objects, WeakObjectPtr weakObject) noexcept;