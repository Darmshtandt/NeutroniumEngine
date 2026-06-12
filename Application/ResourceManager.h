#pragma once

#include <Function.h>
#include <Nt/Graphics/Resources/ResourceManager.h>

#include <queue>
#include <filesystem>

class ResourceManager {
public:
	using LoadFunction = Function<std::unique_ptr<Nt::IResource>()>;

	struct ResourceData {
		ResourceData() = delete;
		ResourceData(const std::string& token, const LoadFunction& Function) :
			Token(token),
			Function(Function)
		{
		}

		std::string Token;
		LoadFunction Function;
	};

public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;

	[[nodiscard]] static ResourceManager& Instance() noexcept {
		static ResourceManager resourceManager;
		return resourceManager;
	}

	void Clear() {
		m_ResourceMap.clear();
		Nt::ResourceManager::Instance().Clear();
	}

	template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
	void AddToQueue(const std::string& token, const std::string& filePath) {
		m_Queue.emplace(token, [=] () -> std::unique_ptr<Nt::IResource> {
			return std::make_unique<_Ty>(m_InitialPath + filePath);
			});
	}
	void LoadAllFromQueue() {
		while (!m_Queue.empty())
			LoadFirstFromQueue();
	}
	void LoadFirstFromQueue() {
		Assert(!m_Queue.empty(), "Empty queue");

		const ResourceData& data = m_Queue.front();
		Add(data.Token, data.Function());
		m_Queue.pop();
	}

	template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
	void Load(const std::string& token, const std::string& filePath) {
		if (m_ResourceMap.contains(token))
			Raise(std::string("Token already exists: " + token));

		m_ResourceMap[token] = Nt::ResourceManager::Instance().Add<_Ty>(filePath);
	}
	void Add(const std::string& token, std::unique_ptr<Nt::IResource>&& pResource) {
		if (m_ResourceMap.contains(token))
			Raise(std::string("Token already exists: " + token));

		m_ResourceMap[token] = Nt::ResourceManager::Instance().Add(std::move(pResource));
	}

	template <class _Ty>
	void Remove(const std::string& token) {
		if (!m_ResourceMap.contains(token))
			Raise(std::string("Token not exists: " + token));

		Nt::ResourceManager::Instance().Remove<_Ty>(m_ResourceMap[token]);
		m_ResourceMap.erase(token);
	}

	template <typename _Ty>
	[[nodiscard]] _Ty* Get(const std::string& token) {
		if (!m_ResourceMap.contains(token))
			Raise(std::string("Token not exists: " + token));

		return Nt::ResourceManager::Instance().Get<_Ty>(m_ResourceMap[token]);
	}

	[[nodiscard]] uInt GetIndex(const std::string& token) const {
		if (!m_ResourceMap.contains(token))
			Raise(std::string("Token not exists: " + token));
		return m_ResourceMap.at(token);
	}

	[[nodiscard]] uInt GetQueueSize() const noexcept {
		return m_Queue.size();
	}

	void SetInitialPath(const std::string& initialPath) {
		m_InitialPath = initialPath;
	}

private:
	std::map<std::string, uInt> m_ResourceMap;
	std::queue<ResourceData> m_Queue;
	std::string m_InitialPath = std::filesystem::current_path().string();
};