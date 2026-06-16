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
	for (const ObjectPtr& object : allObjects)
		RenderObject(object.get());
}

void RenderEngine::SetScene(Scene* pScene) noexcept {
	m_pScene = pScene;
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

void RenderEngine::RenderModel(const Nt::Model& model) const {
	assert(0);
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
	assert(0);
	if (!pCollider->IsVisible())
		return;

	m_pRenderer->SetDepthMode(Nt::DepthMode::ALWAYS);
	m_pRenderer->SetLineWidth(10);
	m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::LINE_STRIP);
	m_pRenderer->UnbindTexture();

	m_pRenderer->MatrixWorldPush();
	m_pRenderer->SetWorld(pCollider->LocalToWorld());
	m_pRenderer->Render(pCollider->GetMesh());
	m_pRenderer->MatrixWorldPop();

	m_pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::TRIANGLES);
	m_pRenderer->SetLineWidth(1);
	m_pRenderer->SetDepthMode(Nt::DepthMode::LESS);
}