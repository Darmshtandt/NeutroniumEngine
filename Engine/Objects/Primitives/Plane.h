#pragma once

#include <Objects/ObjectRegistrar.h>

class Plane : public Primitive {
public:
	Plane(const std::string& name);
	Plane(const Plane& plane) = default;
	Plane(Plane&& plane) noexcept = default;

	[[nodiscard]] Plane* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Plane> m_Registrar;
};