#include <Editor/Camera3D.h>

namespace NtEx {
	Camera3D::Camera3D(Object* pParent) : Object(Class<Camera3D>::ID(), pParent) {
		m_Transform = AddComponent<TransformFloat3D>();
	}

	void Camera3D::Translate(Float3D offset) noexcept {
		m_Transform->Translate(offset);
	}
	void Camera3D::Rotate(Float3D offset) noexcept {
		m_Transform->Rotate(offset);
	}
	void Camera3D::Scale(Float3D offset) noexcept {
		m_Transform->Resize(offset);
	}

	Matrix4x4 Camera3D::View() const noexcept {
		return m_Transform->WorldToLocal();

		Matrix4x4 mat = m_Transform->WorldToLocal() * -1.f;
		mat._44 = 1.f;

		return mat;

		const Float3D euler = m_Transform->Rotation();
		const Matrix4x4 rotation =
			Matrix4x4::GetRotateX(euler.x)
		*
			Matrix4x4::GetRotateY(euler.y)
		*
			Matrix4x4::GetRotateZ(euler.z)
		;
		return
			Matrix4x4::GetTranslate(-m_Transform->Position()).GetInverse() *
			m_Transform->LocalRotation().ToMatrix4x4().GetInverse() *
			Matrix4x4::GetScale(m_Transform->Size()).GetInverse();

		return
			Matrix4x4::GetScale(m_Transform->Size()) *
			rotation *
			Matrix4x4::GetTranslate(m_Transform->Position());
	}

	TransformFloat3D* Camera3D::Transform() const noexcept {
		return m_Transform;
	}
	Float3D Camera3D::Position() const noexcept {
		return m_Transform->Position();
	}
	Float3D Camera3D::RotationEuler() const noexcept {
		return m_Transform->Rotation();
	}
	Float3D Camera3D::Size() const noexcept {
		return m_Transform->Size();
	}

	void Camera3D::Position(Float3D position) noexcept {
		m_Transform->LocalPosition(position);
	}
	void Camera3D::Rotation(Quaternion rotation) noexcept {
		m_Transform->LocalRotation(rotation);
	}
	void Camera3D::RotationEuler(Float3D rotation) noexcept {
		m_Transform->LocalRotationEuler(rotation);
	}
	void Camera3D::Size(Float3D size) noexcept {
		m_Transform->Size(size);
	}
}