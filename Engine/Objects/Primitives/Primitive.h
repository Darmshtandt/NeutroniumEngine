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
	[[nodiscard]] virtual std::string GetToken() const noexcept override;

	[[nodiscard]] Primitive* GetCopy() const override;
	Nt::Float2D GetTextureOffset() const noexcept;
	Nt::Float2D GetTextureScale() const noexcept;

	void SetPrimitiveMesh(const Nt::Mesh& mesh);

	void SetTextureOffset(const Nt::Float2D& textureOffset);
	void SetTextureScale(Nt::Float2D textureScale);

protected:
	const Nt::Shape& GetShape() const noexcept;

private:
	std::shared_ptr<Nt::Mesh> m_Mesh;
	Nt::Float2D m_TextureOffset;
	Nt::Float2D m_TextureScale;
};