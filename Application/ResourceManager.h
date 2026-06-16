#pragma once

#include <Nt/Graphics/Resources/ResourceManager.h>
#include <Function.h>
#include <unordered_map>
#include <queue>

namespace Nt {
	struct Shape;
}

class ResourceManager {
private:
	ResourceManager();

public:
	using LoadFunction = Function<std::unique_ptr<Nt::IResource>()>;

	struct ResourceData {
		ResourceData() = delete;
		ResourceData(const std::string& token, const LoadFunction& Function);

		std::string Token;
		LoadFunction Function;
	};

public:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;

	[[nodiscard]] static ResourceManager& Instance() noexcept;

	void Clear();

	template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
	void AddToQueue(const std::string& token, const std::string& filePath) {
		m_Queue.emplace(token, [this, filePath] () -> std::unique_ptr<Nt::IResource> {
			return std::make_unique<_Ty>(m_InitialPath + filePath);
		});
	}
	void AddToQueue(const std::string& token, const Nt::Shape& shape);

	void LoadAllFromQueue();
	void LoadFirstFromQueue();

	template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
	uInt Load(const std::string& token, const std::string& filePath) {
		if (!m_ResourceMap.contains(token))
			m_ResourceMap[token] = Nt::ResourceManager::Instance().Add<_Ty>(filePath);
		return m_ResourceMap[token];
	}
	uInt Add(const std::string& token, std::unique_ptr<Nt::IResource>&& pResource);

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

	[[nodiscard]] uInt GetIndex(const std::string& token) const;
	[[nodiscard]] uInt GetQueueSize() const noexcept;

	void SetInitialPath(const std::string& initialPath);

private:
	std::unordered_map<std::string, uInt> m_ResourceMap;
	std::queue<ResourceData> m_Queue;
	std::string m_InitialPath;
};