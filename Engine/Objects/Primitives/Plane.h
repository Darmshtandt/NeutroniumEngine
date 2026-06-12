#pragma once

#include <Objects/ObjectRegistrar.h>
#include <Nt/Graphics/Geometry/Primitives.h>

class Plane : public Primitive {
public:
	Plane(const std::string& name);
	Plane(const Plane& plane) = default;
	Plane(Plane&& plane) noexcept = default;

	void SetSize(const Nt::Float3D& size) override;

	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Plane> m_Registrar;
};