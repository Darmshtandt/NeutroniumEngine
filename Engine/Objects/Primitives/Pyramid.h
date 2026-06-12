#pragma once

#include <Objects/ObjectRegistrar.h>

class Pyramid : public Primitive {
public:
	Pyramid(const std::string& name);
	Pyramid(const Pyramid& pyramid) = default;
	Pyramid(Pyramid&& pyramid) noexcept = default;

	void SetSize(const Nt::Float3D& size) override;

	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Pyramid> m_Registrar;
};