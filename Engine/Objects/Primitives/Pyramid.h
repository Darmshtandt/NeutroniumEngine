#pragma once

#include <Objects/ObjectRegistrar.h>

class Pyramid : public Primitive {
public:
	Pyramid(const std::string& name);
	Pyramid(const Pyramid& pyramid) = default;
	Pyramid(Pyramid&& pyramid) noexcept = default;

	[[nodiscard]] Pyramid* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Pyramid> m_Registrar;
};