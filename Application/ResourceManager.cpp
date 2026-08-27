#include <ResourceManager.h>
#include <Nt/Graphics/Resources/Mesh.h>
#include <filesystem>

ResourceManager::ResourceData::ResourceData(const std::string& token, const LoadFunction& Function):
	Token(token),
	Function(Function)
{
}

ResourceManager::ResourceManager() :
	m_InitialPath(std::filesystem::current_path().string())
{
}

ResourceManager& ResourceManager::Instance() noexcept {
	static ResourceManager resourceManager;
	return resourceManager;
}

void ResourceManager::Clear() {
	m_ResourceMap.clear();
	Nt::ResourceManager::Instance().Clear();
}

void ResourceManager::AddToQueue(const std::string& token, const Nt::Shape& shape) {
	m_Queue.emplace(token, [shape] () -> std::unique_ptr<Nt::IResource> {
		return std::make_unique<Nt::Mesh>(shape);
		});
}

void ResourceManager::LoadAllFromQueue() {
	while (!m_Queue.empty())
		LoadFirstFromQueue();
}

void ResourceManager::LoadFirstFromQueue() {
	Assert(!m_Queue.empty(), "Empty queue");

	const ResourceData& data = m_Queue.front();
	Add(data.Token, data.Function());
	m_Queue.pop();
}

uInt ResourceManager::Add(const std::string& token, std::unique_ptr<Nt::IResource>&& pResource) {
	if (!m_ResourceMap.contains(token))
		m_ResourceMap[token] = Nt::ResourceManager::Instance().Add(std::move(pResource));
	return m_ResourceMap[token];
}

uInt ResourceManager::GetIndex(const std::string& token) const {
	if (!m_ResourceMap.contains(token))
		Raise(std::string("Token not exists: " + token));
	return m_ResourceMap.at(token);
}

uInt ResourceManager::GetQueueSize() const noexcept {
	return m_Queue.size();
}

void ResourceManager::SetInitialPath(const std::string& initialPath) {
	m_InitialPath = initialPath;
}