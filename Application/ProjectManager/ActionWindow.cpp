// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ProjectManager/ActionWindow.h>

void ActionWindow::Initialize(const Settings& settings, const uInt& id) {
	const auto buttonProc = [&] (const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		if (uMsg == WM_COMMAND && HIWORD(param_1) == BN_CLICKED && m_hParent != nullptr)
			SendMessage(m_hParent, uMsg, param_1, param_2);
		return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
		};

	SetProcedure(buttonProc);
	RemoveStyles(WS_OVERLAPPEDWINDOW);
	SetBackgroundColor(settings.Style["ProjectManager.ActionWindow.BackgroundColor"]);
	Create(settings.ProjectManagerWindow.ActionWindowRect, "");
	Show();

	const Language languageStruct = settings.Language;

	constexpr Nt::IntRect paddingRect = { 10, 10, 10, 10 };
	constexpr uInt buttonGap = 5;

	Nt::IntRect buttonRect;
	buttonRect.LeftTop = paddingRect.LeftTop;
	buttonRect.Right = m_ClientRect.Right - buttonRect.Left - paddingRect.Right;
	buttonRect.Bottom = 25;

	const std::string buttonTexts[BUTTON_COUNT] = {
		settings.Language["Action.Open"],
		settings.Language["Action.Delete"],
		settings.Language["Action.Create"],
		settings.Language["Action.RemoveFromList"],
		settings.Language["Action.AddToList"],
	};

	for (uInt i = 0; i < BUTTON_COUNT; ++i) {
		Nt::Button button;
		button.SetParent(*this);
		button.SetID(id + i);
		button.AddStyles(BS_CENTER | BS_VCENTER | WS_VISIBLE);
		button.Create(buttonRect, buttonTexts[i]);

		if (i < BUTTON_CREATE || i == BUTTON_REMOVE_FROM_LIST)
			button.DisableWindow();
		m_Buttons.emplace_back(std::move(button));

		buttonRect.Top += buttonRect.Bottom + buttonGap;
	}
}

void ActionWindow::EnableButtons() {
	for (uInt i = 0; i < BUTTON_CREATE; ++i)
		m_Buttons[i].EnableWindow();
	m_Buttons[BUTTON_REMOVE_FROM_LIST].EnableWindow();
}

void ActionWindow::DisableButtons() {
	for (uInt i = 0; i < BUTTON_CREATE; ++i)
		m_Buttons[i].DisableWindow();
	m_Buttons[BUTTON_REMOVE_FROM_LIST].DisableWindow();
}

