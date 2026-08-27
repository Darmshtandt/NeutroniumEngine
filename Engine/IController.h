#pragma once

#include <Nt/Core/NtTypes.h>

struct IController {
	virtual ~IController() noexcept = default;
	virtual void Update() = 0;
	virtual void Toggle(Bool enabled) noexcept = 0;
};