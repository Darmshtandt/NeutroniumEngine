#pragma once

#include <Objects/ObjectRegistrar.h>

class Cube : public Primitive {
public:
	Cube(const std::string& name);
	Cube(const Cube& cube) = default;
	Cube(Cube&& cube) noexcept = default;

	[[nodiscard]] Cube* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Cube> m_Registrar;
};