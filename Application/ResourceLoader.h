#pragma once

#include <ResourceManager.h>

template <class _Ty> requires std::is_base_of_v<Nt::IResource, _Ty>
class ResourceLoader {
public:
	ResourceLoader(const std::string& token, const std::string& filePath) :
		m_Token(token)
	{
		ResourceManager::Instance().AddToQueue<_Ty>(m_Token, filePath);
	}

	_Ty* Get() const {
		return ResourceManager::Instance().Get<_Ty>(m_Token);
	}

private:
	std::string m_Token;
};