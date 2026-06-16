#pragma once

#include <Objects/Object.h>

class Primitive : public Object {
protected:
	Primitive(std::string name, const ClassID id);

public:
	Primitive(const Primitive& primitive);
	Primitive(Primitive&& primitive) noexcept;
	~Primitive() noexcept override = default;

	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] static std::string GetClassTypeToken() noexcept;
	[[nodiscard]] std::string GetTypeToken() const noexcept override;
	[[nodiscard]] std::string GetToken() const noexcept override;

	[[nodiscard]] Primitive* GetCopy() const override;

	void SetPrimitiveMesh(const Nt::Mesh& mesh);

protected:
	const Nt::Shape& GetShape() const noexcept;

private:
	std::shared_ptr<Nt::Mesh> m_Mesh;
};