#pragma once

#include <Editor/Commands/IEditCommand.h>
#include <Objects/Object.h>

namespace Nt {
	class EventBus;
}

namespace Edit {
	class AddObjectCommand final : public IEditCommand {
	public:
		AddObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectPtr& object);
		~AddObjectCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		ObjectPtr m_pObject;
		std::weak_ptr<Nt::EventBus> m_pEventBus;
	};

	class RemoveObjectCommand final : public IEditCommand {
	public:
		RemoveObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectPtr& object);
		~RemoveObjectCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		ObjectPtr m_pObject;
		std::weak_ptr<Nt::EventBus> m_pEventBus;
	};
	
	class MultiAddObjectCommand final : public IEditCommand {
	public:
		MultiAddObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectContainer& objects);
		~MultiAddObjectCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		ObjectContainer m_Objects;
		std::weak_ptr<Nt::EventBus> m_pEventBus;
	};

	class MultiRemoveObjectCommand final : public IEditCommand {
	public:
		MultiRemoveObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectContainer& objects);
		~MultiRemoveObjectCommand() noexcept override = default;

		void Execute() override;
		void Undo() override;

	private:
		ObjectContainer m_Objects;
		std::weak_ptr<Nt::EventBus> m_pEventBus;
	};
}