#pragma once

#include <Editor/Commands/IEditCommand.h>
#include <Nt/Core/Utilities.h>
#include <stack>

struct AddToHistoryCommand final {
	NotNull<IEditCommand*> Command;
};

class EditingHistory {
	using CommandPtr = std::unique_ptr<IEditCommand>;
	using StackCommands = std::stack<CommandPtr>;

public:
	~EditingHistory() noexcept;

	void AddEndExecute(NotNull<IEditCommand*> command);

	void Undo();
	void Redo();

private:
	StackCommands m_UndoStack;
	StackCommands m_RedoStack;
};