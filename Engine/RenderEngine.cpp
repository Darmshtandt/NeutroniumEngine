#include <RenderEngine.h>

#include <Scene.h>
#include <Nt/Graphics/Objects/Camera.h>
#include <Objects/Object.h>

RenderEngine::RenderEngine(NotNull<Nt::Renderer*> pRenderer, NotNull<Nt::Shader*> pShader) :
	m_pRenderer(pRenderer),
	m_Shader(pShader)
{
}
RenderEngine::~RenderEngine() noexcept = default;

void RenderEngine::Render() const {
	if (m_pScene == nullptr)
		return;

	m_pRenderer->SetShader(m_Shader.get());
	if (m_pCamera)
		m_pRenderer->SetView(m_pCamera->GetView());

	const ObjectContainer& allObjects = m_pScene->GetObjects();
	for (const ObjectPtr& object : allObjects)
		RenderObject(object.get());
}

void RenderEngine::SetScene(Scene* pScene) noexcept {
	m_pScene = pScene;
}

void RenderEngine::SetCamera(Nt::Camera* pCamera) noexcept {
	m_pCamera = pCamera;
}

void RenderEngine::RenderObject(const Object* pObject) const {
	if (!pObject->IsVisible())
		return;
	if (pObject->IsStarted() && pObject->IsInvisible())
		return;

	m_pRenderer->MatrixWorldPush();
	m_pRenderer->SetWorld(pObject->LocalToWorld());

	const auto collider = pObject->GetCollider();
	if (collider->IsVisible()) {
		m_pRenderer->SetDepthMode(Nt::DepthMode::ALWAYS);
		m_pRenderer->SetLineWidth(10);
		m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::LINE_STRIP);
		m_pRenderer->UnbindTexture();

		m_pRenderer->Render(collider->GetMesh());

		m_pRenderer->SetLineWidth(1);
		m_pRenderer->SetDepthMode(Nt::DepthMode::LESS);
	}

	const auto& mesh = pObject->GetMesh();
	if (!mesh.IsValid()) {
		m_pRenderer->MatrixWorldPop();
		return;
	}

	m_pRenderer->GetShaderPtr()->SetUniform("IsObjectInvisible", pObject->IsInvisible());
	m_pRenderer->GetShaderPtr()->SetUniform("IsObjectSelected", pObject->IsSelected());
	m_pRenderer->GetShaderPtr()->SetUniform("SelectionColor", m_SelectionColor);

	if (pObject->IsSelected())
		RenderOutline(pObject);

	const Nt::Float4D color = m_pRenderer->GetColor();
	const auto texture = pObject->GetTexture();
	if (texture.IsValid()) {
		m_pRenderer->BindTexture(pObject->GetTexture().Get());
		m_pRenderer->GetShaderPtr()->SetUniformMatrix3x3("TexWorld", Nt::UNIFORM_FLOAT, pObject->TextureLocalWorld());
	}
	else {
		m_pRenderer->UnbindTexture();
	}

	m_pRenderer->SetColor(pObject->GetColor());
	m_pRenderer->SetDrawingMode(pObject->GetDrawingMode());
	m_pRenderer->Render(mesh.Get());

	m_pRenderer->MatrixWorldPop();
	m_pRenderer->SetColor(color);

	if (pObject->IsSelected())
		m_pRenderer->GetShaderPtr()->SetUniform("IsObjectSelected", false);
}

void RenderEngine::RenderOutline(const Object* pObject) const {
	const auto& mesh = pObject->GetMesh();
	if (!mesh.IsValid())
		return;

	const Nt::Float4D color = m_pRenderer->GetColor();
	const Nt::CullFace cullFace = m_pRenderer->GetCullFace();
	const Nt::DepthMode depthMode = m_pRenderer->GetDepthMode();
	const Float scaleValue = 1.05f;

	m_pRenderer->MatrixWorldPush();
	m_pRenderer->SetColor(m_SelectionColor);
	m_pRenderer->SetCullFace(Nt::CullFace::FRONT);
	m_pRenderer->DisableDepthMask();
	m_pRenderer->SetDepthMode(Nt::DepthMode::LEQUAL);
	m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::TRIANGLES);

	m_pRenderer->SetWorld(pObject->LocalToWorld());
	m_pRenderer->Scale({ scaleValue, scaleValue, scaleValue });
	m_pRenderer->UnbindTexture();

	m_pRenderer->Render(mesh.Get());

	m_pRenderer->SetDepthMode(depthMode);
	m_pRenderer->EnableDepthMask();
	m_pRenderer->SetCullFace(cullFace);
	m_pRenderer->SetColor(color);
	m_pRenderer->MatrixWorldPop();
}

Nt::Camera* RenderEngine::GetCamera() const noexcept {
	return m_pCamera;
}

Nt::Shader* RenderEngine::GetShader() const noexcept {
	return m_Shader.get();
}