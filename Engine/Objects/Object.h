#pragma once

#include <Core/Icon3D.h>
#include <Script/Script.h>

#include <Nt/Graphics/Ex/ClassIdentifier.h>
#include <Nt/Core/WeakSafePtr.h>
#include <Nt/Physics/RigidBody.h>
#include <Nt/Collider.h>

#include <TinyXML.h>

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
	virtual void Render(NotNull<Nt::Renderer*> pRenderer) const override;
	void RenderOutline(NotNull<Nt::Renderer*> pRenderer) const;

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

	template <typename _Ty, uInt size>
	[[nodiscard]] TiXmlElement* VectorToXML(const Nt::String& name, const Nt::Vector<_Ty, size>& vector) const noexcept {
		TiXmlElement* element = new TiXmlElement("name");
		for (uInt i = 0; i < size; ++i) {
			Char literal[1];
			if (i < 3)
				literal[0] = Char(uInt('x') + i);
			else
				literal[0] = Char(uInt('z') - i);

			element->SetAttribute(literal, vector.Array[i]);
		}

		return element;
	}

	[[nodiscard]] virtual TiXmlElement* ToXML() const;

	Nt::Renderer::DrawingMode GetDrawingMode() const noexcept;
	const Nt::Model& GetModel() const noexcept;
	const Nt::Collider* GetCollider() const noexcept;
	Script* GetScript() const noexcept;
	const std::vector<Script::Data>& GetScriptData() const noexcept;
	Nt::String GetLayerName() const noexcept;
	Nt::String GetName() const noexcept;
	Object* GetParentPtr() const noexcept;
	Nt::Texture* GetTexture() const noexcept;
	Nt::Mesh* GetMesh() const noexcept;
	Bool IsSelected() const noexcept;
	Bool IsInvisible() const noexcept;
	Bool IsStarted() const noexcept;
	Bool IsActivePhysics() const noexcept;

	void SetDrawingMode(Nt::Renderer::DrawingMode mode) noexcept;
	void SetName(const Nt::String& newName);
	void SetLayerName(const Nt::String& name);
	void SetParentPtr(Object* pNewParent) noexcept;
	void SetModel(const Nt::Model& newModel);
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
	std::vector<Script::Data> m_ScriptData;
	std::string m_LayerName = "Main";
	std::string m_Name;

	Nt::Collider* m_pCollider = new Nt::Collider;
	Nt::Model m_Model;

	Object* m_ParentPtr = nullptr;
	Script* m_pScript = nullptr;

	Bool m_IsSelected = false;
	Bool m_IsInvisible = false;
	Bool m_IsStarted = false;

protected:
	Object& _Clone(const Object& object);
	Object& _Move(Object&& object);

	void _SetParameters(const Object& object) noexcept;
	void _UpdateCollider();

	void Render(NotNull<Nt::Renderer*> pRenderer, const uInt& offset, const uInt& verticesCount) const override;
};

using ObjectContainer = std::vector<Object*>;