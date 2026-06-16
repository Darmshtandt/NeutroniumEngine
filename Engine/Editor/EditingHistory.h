#pragma once

#include <stack>
#include <Nt/Core/Utilities.h>

struct IEditCommand;

class EditingHistory {
	using CommandPtr = std::unique_ptr<IEditCommand>;
	using StackCommands = std::stack<CommandPtr>;

public:
	void AddEndExecute(NotNull<IEditCommand*> command);

	void Undo();
	void Redo();

private:
	StackCommands m_UndoStack;
	StackCommands m_RedoStack;
};