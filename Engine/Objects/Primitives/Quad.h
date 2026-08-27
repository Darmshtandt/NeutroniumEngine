#pragma once

#include <ResourceLoader.h>
#include <Objects/ObjectRegistrar.h>

class Quad : public Primitive {
public:
	Quad(const std::string& name);
	Quad(const Quad& quad) = default;
	Quad(Quad&& quad) noexcept = default;

	[[nodiscard]] Quad* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

private:
	inline static PrimitiveRegistrar<Quad> m_Registrar;
};