#include <Editor/Commands/TransformCommands.h>

namespace Edit {
	TransformCommands::TransformCommands(const WeakObjectPtr& object, const Nt::Float3D& newPosition) :
		m_pObject(object),
		m_NewValue(newPosition)
	{
		Initialize();
	}

	void TransformCommands::Execute() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			SetToObject(object, m_NewValue);
			object->StaticUpdate();
		}
	}
	void TransformCommands::Undo() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			SetToObject(object, m_OldValue);
			object->StaticUpdate();
		}
	}

	void TransformCommands::Initialize() noexcept {
		assert(!m_pObject.expired());
		m_OldValue = GetFromObject(m_pObject.lock());
	}


	TransMoveCommand::TransMoveCommand(const WeakObjectPtr& object, const Nt::Float3D& newPosition) :
		TransformCommands(object, newPosition)
	{
	}
	Nt::Float3D TransMoveCommand::GetFromObject(const ObjectPtr& object) const noexcept {
		return object->GetPosition();
	}
	void TransMoveCommand::SetToObject(const ObjectPtr& object, const Nt::Float3D& value) const noexcept {
		object->SetPosition(value);
	}


	MoveCommand::MoveCommand(const WeakObjectPtr& object, const Nt::Float3D& newPosition) :
		m_pObject(object),
		m_NewPosition(newPosition)
	{
		assert(!object.expired());
		m_OldPosition = object.lock()->GetPosition();
	}
	MoveCommand::MoveCommand(const WeakObjectPtr& object, const Nt::Float3D& newPosition, const Nt::Float3D& oldPosition) :
		m_pObject(object),
		m_NewPosition(newPosition),
		m_OldPosition(oldPosition)
	{
		assert(!object.expired());
	}

	void MoveCommand::Execute() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetPosition(m_NewPosition);
			object->StaticUpdate();
		}
	}
	void MoveCommand::Undo() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetPosition(m_OldPosition);
			object->StaticUpdate();
		}
	}


	RotationCommand::RotationCommand(const WeakObjectPtr& object, const Nt::Float3D& newRotation) :
		m_pObject(object),
		m_NewRotation(newRotation)
	{
		assert(!object.expired());
		m_OldRotation = object.lock()->GetAngle();
	}
	RotationCommand::RotationCommand(const WeakObjectPtr& object, const Nt::Float3D& newRotation, const Nt::Float3D& oldRotation) :
		m_pObject(object),
		m_NewRotation(newRotation),
		m_OldRotation(oldRotation)
	{
		assert(!m_pObject.expired());
	}

	void RotationCommand::Execute() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetAngle(m_NewRotation);
			object->StaticUpdate();
		}
	}
	void RotationCommand::Undo() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetAngle(m_OldRotation);
			object->StaticUpdate();
		}
	}


	SizeCommand::SizeCommand(const WeakObjectPtr& object, const Nt::Float3D& newScale) :
		m_pObject(object),
		m_NewScale(newScale)
	{
		assert(!object.expired());
		m_OldScale = object.lock()->GetSize();
	}
	SizeCommand::SizeCommand(const WeakObjectPtr& object, const Nt::Float3D& newScale, const Nt::Float3D& oldScale) :
		m_pObject(object),
		m_NewScale(newScale),
		m_OldScale(oldScale)
	{
		assert(!m_pObject.expired());
	}

	void SizeCommand::Execute() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetSize(m_NewScale);
			object->StaticUpdate();
		}
	}
	void SizeCommand::Undo() {
		assert(!m_pObject.expired());

		const auto object = m_pObject.lock();
		if (object) {
			object->SetSize(m_OldScale);
			object->StaticUpdate();
		}
	}
}