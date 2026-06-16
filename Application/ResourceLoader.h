#pragma once

#include <ResourceManager.h>
#include <Nt/Graphics/Resources/Mesh.h>

template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
class ResourceLoader_Impl {
public:
	explicit ResourceLoader_Impl(std::string token) :
		m_Token(std::move(token))
	{
	}

	[[nodiscard]] _Ty* Get() const {
		return ResourceManager::Instance().Get<_Ty>(m_Token);
	}
	[[nodiscard]] uInt GetIndex() const {
		return ResourceManager::Instance().GetIndex(m_Token);
	}

protected:
	std::string m_Token;
};

template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
class ResourceLoader : public ResourceLoader_Impl<_Ty> {
public:
	ResourceLoader(std::string token, const std::string& filePath) :
		ResourceLoader_Impl<_Ty>(std::move(token))
	{
		ResourceManager::Instance().AddToQueue<_Ty>(this->m_Token, filePath);
	}
};

template <>
class ResourceLoader<Nt::Mesh> : public ResourceLoader_Impl<Nt::Mesh> {
public:
	ResourceLoader(std::string token, const std::string& filePath) :
		ResourceLoader_Impl(std::move(token))
	{
		ResourceManager::Instance().AddToQueue<Nt::Mesh>(m_Token, filePath);
	}
	ResourceLoader(std::string token, const Nt::Shape& shape) :
		ResourceLoader_Impl(std::move(token))
	{
		ResourceManager::Instance().AddToQueue(m_Token, shape);
	}
};