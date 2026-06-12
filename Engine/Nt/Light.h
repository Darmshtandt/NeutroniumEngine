#pragma once

#include <Nt/Graphics/Objects/IObject.h>
#include <Nt/Graphics/Renderer.h>

namespace Nt {
	struct Material {

	};

	struct LightData {
		Nt::Float4D Color;
		Nt::Float3D Attenuation;
		Nt::Float3D Position;
		Float Range;
		Bool Enabled;
	};

	class Light : public IObject {
	public:
		enum Type {
			POINT,
			DIRECTION,
			SPOT
		};

	public:
		Light(const Type& type) :
			m_Type(type)
		{
		}
		Light(const Type& type, const Nt::Float3D& position) :
			m_Type(type)
		{
			IObject::SetPosition(position);
		}
		Light(const Type& type, const Nt::Float3D& position, const Nt::Float3D& angle) :
			m_Type(type)
		{
			IObject::SetPosition(position);
			IObject::SetAngle(angle);
		}

		void Render(NotNull<Nt::Renderer*> pRenderer) const override
		{
		}

		[[nodiscard]]
		Type GetType() const noexcept {
			return m_Type;
		}
		[[nodiscard]] 
		LightData GetData() const noexcept {
			LightData data = { };
			data.Position = m_Position;
			data.Attenuation = { 1.f, 1.5f, 1.9f };
			data.Color = Nt::Colors::Yellow;
			data.Range = 5.f;
			data.Enabled = true;

			return data;
		}

		void SetType(const Type& type) noexcept {
			m_Type = type;
		}

	private:
		Type m_Type;

	private:
		void Render([[maybe_unused]] NotNull<Renderer*> pRenderer, [[maybe_unused]] const uInt& offset, [[maybe_unused]] const uInt& verticesCount) const override
		{
		}
	};
}