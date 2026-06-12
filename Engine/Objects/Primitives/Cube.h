#pragma once

#include <Objects/ObjectRegistrar.h>

class Cube : public Primitive {
public:
	Cube(const std::string& name);
	Cube(const Cube& cube) = default;
	Cube(Cube&& cube) noexcept = default;

	void SetSize(const Nt::Float3D& size) override;

	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Cube> m_Registrar;
};