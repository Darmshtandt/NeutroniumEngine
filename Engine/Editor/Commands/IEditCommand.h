#pragma once

struct IEditCommand {
	virtual ~IEditCommand() noexcept = default;
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};