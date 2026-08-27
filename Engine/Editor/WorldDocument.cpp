#include <Editor/WorldDocument.h>
#include <Scene.h>
#include <SerializerXML.h>
#include <Nt/Graphics/System/Window.h>
#include <TinyXML.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Plane.h>
#include <Objects/Primitives/Pyramid.h>
#include <Objects/Primitives/Quad.h>

WorldDocument::WorldDocument(NotNull<Lua*> pLua) noexcept :
	m_pLua(pLua)
{
}

Bool WorldDocument::Open(NotNull<Scene*> pScene) {
	const Nt::String path = Nt::OpenFileDialog(L"", L"Scene (*.xml)\0*.xml");
	if (path.empty())
		return false;

	m_FilePath = path;

	TiXmlDocument doc;
	doc.LoadFile(m_FilePath);

	SerializerXML::FromXML(doc.FirstChildElement(), pScene, m_pLua);

	return true;
}

Bool WorldDocument::Save(NotNull<Scene*> pScene) {
	if (m_FilePath.empty())
		return SaveAs(pScene);
	return SaveToFile(SerializerXML::ToXML(pScene));
}

Bool WorldDocument::SaveAs(NotNull<Scene*> pScene) {
	const Nt::String path = Nt::SaveAsFileDialog(L"", L"Scene (*.xml)\0*.xml");
	//const Nt::String path = Nt::SaveAsFileDialog(m_DefaultPath.c_str(), L"Scene (*.ntascn)\0*.ntascn");
	if (path.empty())
		return false;

	m_FilePath = path;
	return SaveToFile(SerializerXML::ToXML(pScene));
}

void WorldDocument::SetDefaultPath(const Nt::String& defaultPath) noexcept {
	m_DefaultPath = defaultPath;
}

Bool WorldDocument::SaveToFile(NotNull<TiXmlElement*> pRoot) const {
	TiXmlDocument doc;
	doc.LinkEndChild(pRoot);
	return doc.SaveFile(m_FilePath);
}