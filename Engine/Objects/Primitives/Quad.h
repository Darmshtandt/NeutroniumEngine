#pragma once

#include <Objects/ObjectRegistrar.h>

class Quad : public Primitive {
public:
	Quad(const std::string& name);
	Quad(const Quad& quad) = default;
	Quad(Quad&& quad) noexcept = default;

	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Quad> m_Registrar;
};