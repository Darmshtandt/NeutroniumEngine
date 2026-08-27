#pragma once

#include <Nt/Core/Colors.h>
#include <Nt/Core/Math/Vector.h>
#include <Nt/Graphics/Resources/ResourceHandle.h>

namespace NtEx {
	class Camera3D;
}

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
	RenderEngine(NotNull<Nt::Renderer*> pRenderer, NotNull<Nt::Shader*> pShader);
	~RenderEngine() noexcept;

	void Render() const;
	void RenderObject(const Object* pObject) const;
	void RenderOutline(const Object* pObject) const;

	[[nodiscard]] NtEx::Camera3D* GetCamera() const noexcept;
	[[nodiscard]] Nt::Shader* GetShader() const noexcept;

	void SetScene(Scene* pScene) noexcept;
	void SetCamera(NtEx::Camera3D* pCamera) noexcept;

private:
	std::unique_ptr<Nt::Shader> m_Shader;
	NtEx::Camera3D* m_pCamera = nullptr;
	Nt::Renderer* m_pRenderer;
	Scene* m_pScene = nullptr;
	Nt::Float4D m_SelectionColor = Nt::Colors::Orange;
};