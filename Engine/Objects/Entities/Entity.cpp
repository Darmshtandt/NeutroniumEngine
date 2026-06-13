// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Entities/Entity.h>


Entity::Entity(std::string name, const ClassID id) :
	Object(std::move(name), id)
{
}

Entity* Entity::GetCopy() const {
	return new Entity(*this);
}

std::string Entity::GetClassToken() noexcept {
	return "Entity";
}
std::string Entity::GetClassTypeToken() noexcept {
	return GetClassToken();
}
std::string Entity::GetTypeToken() const noexcept {
	return GetClassToken();
}
std::string Entity::GetToken() const noexcept {
	return GetClassToken();
}
