#pragma once

#include <Nt/Core/Colors.h>
#include <Nt/Core/Math/Vector.h>
#include <Nt/Graphics/Resources/ResourceHandle.h>
#include <Nt/Graphics/Resources/Texture.h>

namespace Nt {
	class Renderer;
	class Collider;
}

namespace Nt {
	class Model;
	class Shader;
}

class Object;
class Scene;

class RenderEngine {
public:
	explicit RenderEngine(NotNull<Nt::Renderer*> pRenderer);
	~RenderEngine() noexcept;

	void Render() const;
	void RenderObject(const Object* pObject) const;
	void RenderOutline(const Object* pObject) const;
	void RenderModel(const Nt::Model& model) const;
	void RenderCollider(const Nt::Collider* pCollider) const;

	void SetScene(Scene* pScene) noexcept;

private:
	Nt::Renderer* m_pRenderer;
	Scene* m_pScene = nullptr;
	Nt::Float4D m_SelectionColor = Nt::Colors::Orange;
};