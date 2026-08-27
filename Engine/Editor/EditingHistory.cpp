#include <Editor/EditingHistory.h>

EditingHistory::~EditingHistory() noexcept = default;

void EditingHistory::AddEndExecute(NotNull<IEditCommand*> command) {
	command->Execute();
	m_UndoStack.emplace(command);
	m_RedoStack = StackCommands();
}

void EditingHistory::Undo() {
	if (m_UndoStack.empty())
		return;

	CommandPtr command = std::move(m_UndoStack.top());
	m_UndoStack.pop();

	command->Undo();
	m_RedoStack.push(std::move(command));
}

void EditingHistory::Redo() {
	if (m_RedoStack.empty())
		return;

	CommandPtr command = std::move(m_RedoStack.top());
	m_RedoStack.pop();

	command->Execute();
	m_UndoStack.push(std::move(command));
}