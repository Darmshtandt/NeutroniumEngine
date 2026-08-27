#pragma once

#include <Nt/Graphics/Ex/Components/Transform.h>

namespace NtEx {
	class Camera3D : public Object {
	public:
		explicit Camera3D(Object* pParent = nullptr);

		void Translate(Float3D offset) noexcept;
		void Rotate(Float3D offset) noexcept;
		void Scale(Float3D offset) noexcept;

		NT_NODISCARD Matrix4x4 View() const noexcept;
		NT_NODISCARD TransformFloat3D* Transform() const noexcept;

		NT_NODISCARD Float3D Position() const noexcept;
		NT_NODISCARD Float3D RotationEuler() const noexcept;
		NT_NODISCARD Float3D Size() const noexcept;

		void Position(Float3D position) noexcept;
		void Rotation(Quaternion rotation) noexcept;
		void RotationEuler(Float3D rotation) noexcept;
		void Size(Float3D size) noexcept;

	private:
		TransformFloat3D* m_Transform;
	};
}