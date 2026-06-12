#pragma once

#include <Main.h>
#include <Settings.h>

class ActionWindow : public Nt::Window {
public:
	enum Buttons {
		BUTTON_OPEN,
		BUTTON_DELETE,
		BUTTON_CREATE,
		BUTTON_REMOVE_FROM_LIST,
		BUTTON_ADD_TO_LIST,
		BUTTON_COUNT,
	};

public:
	ActionWindow() = default;

	void Initialize(const Settings& settings, const uInt& id);

	void EnableButtons();
	void DisableButtons();

private:
	using Window::Create;
	using Window::Destroy;

private:
	std::vector<Nt::Button> m_Buttons;
};