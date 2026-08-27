#pragma once

#include <Objects/Object.h>

class Entity : public Object {
protected:
	Entity(std::string name, const ClassID id);

public:
	[[nodiscard]] Entity* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] static std::string GetClassTypeToken() noexcept;
	[[nodiscard]] std::string GetTypeToken() const noexcept override;
	[[nodiscard]] virtual std::string GetToken() const noexcept override;
};