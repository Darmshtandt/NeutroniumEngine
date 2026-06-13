#pragma once

#include <Nt/Graphics/Objects/Model.h>
#include <Nt/Graphics/Resources/Texture.h>
#include <Nt/Graphics/Resources/Mesh.h>
#include <Nt/Graphics/Resources/ResourceHandle.h>
#include <Script/Script.h>

#include <Nt/Graphics/Ex/ClassIdentifier.h>
#include <Nt/Physics/RigidBody.h>
#include <Nt/Collider.h>

using NtEx::ClassID;
using NtEx::Class;

class Lua;
class Script;
class Scene;

class Object : public Nt::RigidBody, public NtEx::Identifier {
protected:
	Object(std::string name, const ClassID id);

public:
	Object(const Object& object);
	Object(Object&& object) noexcept;
	~Object() override;

	virtual void Start();
	virtual void Stop();

	void StaticUpdate() override;
	virtual void Update(const Float& time) override;

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
	const Nt::Collider* GetCollider() const noexcept;
	Script* GetScript() const noexcept;
	const std::vector<Script::Data>& GetScriptData() const noexcept;
	Nt::String GetLayerName() const noexcept;
	Nt::String GetName() const noexcept;
	Object* GetParentPtr() const noexcept;
	Nt::ResourceHandle<Nt::Texture> GetTexture() const noexcept;
	Nt::ResourceHandle<Nt::Mesh> GetMesh() const noexcept;
	Bool IsSelected() const noexcept;
	Bool IsInvisible() const noexcept;
	Bool IsStarted() const noexcept;
	Bool IsActivePhysics() const noexcept;

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

	virtual void SetPosition(const Nt::Float3D& position);
	virtual void SetSize(const Nt::Float3D& size);
	virtual void SetAngle(const Nt::Float3D& angle);
	virtual void SetAngleOrigin(const Nt::Float3D& angleOrigin);
	virtual void SetOrigin(const Nt::Float3D& origin);
	virtual void SetColor(const Nt::Float4D& color);

private:
	using RigidBody::IsActive;

protected:
	Nt::Renderer::DrawingMode m_DrawingMode = Nt::Renderer::DrawingMode::TRIANGLES;
	Nt::ResourceHandle<Nt::Mesh> m_Mesh;
	Nt::ResourceHandle<Nt::Texture> m_Texture;
	std::vector<Script::Data> m_ScriptData;
	std::string m_LayerName = "Main";
	std::string m_Name;

	Nt::Collider* m_pCollider = new Nt::Collider;

	Object* m_ParentPtr = nullptr;
	Script* m_pScript = nullptr;

	Bool m_IsSelected = false;
	Bool m_IsInvisible = false;
	Bool m_IsStarted = false;

private:
	Object& _Clone(const Object& object);
	Object& _Move(Object&& object);

	void _SetParameters(const Object& object) noexcept;
	void _UpdateCollider();

	void Render(NotNull<Nt::Renderer*> pRenderer, const uInt& offset, const uInt& verticesCount) const override;
	void Render(NotNull<Nt::Renderer*> pRenderer) const override;
};

using ObjectContainer = std::vector<Object*>;