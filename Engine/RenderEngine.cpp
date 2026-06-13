#include <RenderEngine.h>

#include <Scene.h>
#include <Objects/Object.h>

RenderEngine::RenderEngine(NotNull<Nt::Renderer*> pRenderer) :
	m_pRenderer(pRenderer)
{
}
RenderEngine::~RenderEngine() noexcept = default;

void RenderEngine::Render() const {
	if (m_pScene == nullptr)
		return;

	const ObjectContainer& allObjects = m_pScene->GetObjects();
	for (const Object* pObject : allObjects)
		RenderObject(pObject);
}

void RenderEngine::SetScene(Scene* pScene) noexcept {
	m_pScene = pScene;
}

void RenderEngine::RenderObject(const Object* pObject) const {
	if (!pObject->IsVisible())
		return;
	if (pObject->IsStarted() && pObject->IsInvisible())
		return;

	RenderCollider(pObject->GetCollider());

	const auto& mesh = pObject->GetMesh();
	if (!mesh.IsValid())
		return;

	m_pRenderer->GetShaderPtr()->SetUniform("IsObjectInvisible", pObject->IsInvisible());
	m_pRenderer->GetShaderPtr()->SetUniform("IsObjectSelected", pObject->IsSelected());
	m_pRenderer->GetShaderPtr()->SetUniform("SelectionColor", m_SelectionColor);

	if (pObject->IsSelected())
		RenderOutline(pObject);

	const Nt::Float4D color = m_pRenderer->GetColor();
	m_pRenderer->BindTexture(pObject->GetTexture().Get());
	m_pRenderer->SetColor(pObject->GetColor());
	m_pRenderer->MatrixWorldPush();
	m_pRenderer->SetWorld(pObject->LocalToWorld());
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

void RenderEngine::RenderModel(const Nt::Model& model) const {
	const Nt::ResourceHandle<Nt::Mesh> mesh = model.GetMesh();
	if (!mesh.IsValid() || !model.IsVisible())
		return;

	const Nt::Float4D color = m_pRenderer->GetColor();
	m_pRenderer->BindTexture(model.GetTexture().Get());
	m_pRenderer->SetColor(model.GetColor());
	m_pRenderer->MatrixWorldPush();
	m_pRenderer->Transform(model.GetPosition(), model.GetOrigin(), model.GetAngle(), model.GetAngleOrigin());
	m_pRenderer->Scale(model.GetSize());
	m_pRenderer->Render(mesh.Get());
	m_pRenderer->MatrixWorldPop();
	m_pRenderer->SetColor(color);
}

void RenderEngine::RenderCollider(const Nt::Collider* pCollider) const {
	if (!pCollider->IsVisible())
		return;

	m_pRenderer->SetDepthMode(Nt::DepthMode::ALWAYS);
	m_pRenderer->SetLineWidth(10);
	m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::LINE_STRIP);
	RenderModel(pCollider->GetModel());
	m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::TRIANGLES);
	m_pRenderer->SetLineWidth(1);
	m_pRenderer->SetDepthMode(Nt::DepthMode::LESS);
}