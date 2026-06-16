#pragma once

#include <Editor/Commands/IEditCommand.h>
#include <Objects/Object.h>

namespace Edit {
	class TransformCommands : public IEditCommand {
	public:
		TransformCommands(const WeakObjectPtr& object, const Nt::Float3D& newPosition);
		~TransformCommands() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		WeakObjectPtr m_pObject;
		Nt::Float3D m_NewValue;
		Nt::Float3D m_OldValue;

		void Initialize() noexcept;

	protected:
		[[nodiscard]] virtual Nt::Float3D GetFromObject(const ObjectPtr& object) const noexcept = 0;
		virtual void SetToObject(const ObjectPtr& object, const Nt::Float3D& value) const noexcept = 0;
	};

	class TransMoveCommand final : public TransformCommands {
	public:
		TransMoveCommand(const WeakObjectPtr& object, const Nt::Float3D& newPosition);
		~TransMoveCommand() noexcept override = default;

	private:
		[[nodiscard]] Nt::Float3D GetFromObject(const ObjectPtr& object) const noexcept override;
		void SetToObject(const ObjectPtr& object, const Nt::Float3D& value) const noexcept override;
	};

	class MoveCommand final : public IEditCommand {
	public:
		MoveCommand(const WeakObjectPtr& object, const Nt::Float3D& position);
		~MoveCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		WeakObjectPtr m_pObject;
		Nt::Float3D m_NewPosition;
		Nt::Float3D m_OldPosition;
	};

	class RotationCommand final : public IEditCommand {
	public:
		RotationCommand(const WeakObjectPtr& object, const Nt::Float3D& rotation);
		~RotationCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		WeakObjectPtr m_pObject;
		Nt::Float3D m_NewRotation;
		Nt::Float3D m_OldRotation;
	};

	class SizeCommand final : public IEditCommand {
	public:
		SizeCommand(const WeakObjectPtr& object, const Nt::Float3D& scale);
		~SizeCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		WeakObjectPtr m_pObject;
		Nt::Float3D m_NewScale;
		Nt::Float3D m_OldScale;
	};
}