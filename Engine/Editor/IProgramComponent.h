#pragma once

#include <Nt/Core/Utilities.h>

class InputContext;

struct IProgramComponent {
	virtual ~IProgramComponent() noexcept = default;

	virtual void AddHotKeys(NotNull<InputContext*> pInput) = 0;
};