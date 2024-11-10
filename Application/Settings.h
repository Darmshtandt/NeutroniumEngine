#pragma once

struct Settings {
private:
	struct _ProjectManagerWindow {
		Nt::uIntRect ProjectListRect;
		Nt::uIntRect ActionWindowRect;
		Nt::uInt2D Size;
	};

public:
	Settings() {
		Styles = { };

		Nt::IntRect mainWindowRect;
		mainWindowRect.RightBottom = Nt::GetMonitorSize();
		mainWindowRect.LeftTop = (Nt::GetMonitorSize() - mainWindowRect.RightBottom) / 2;
		ComputeWindowRect(mainWindowRect);

		Projection_FOV = 60.f;
		Projection_Near = 0.01f;
		Projection_Far = 1000.f;

		ProjectManagerWindow.Size = Nt::Float2D(Nt::GetMonitorSize()) / 1.5f;
		ProjectManagerWindow.ProjectListRect.LeftTop = { 20, 30 };
		ProjectManagerWindow.ProjectListRect.RightBottom = ProjectManagerWindow.Size;
		ProjectManagerWindow.ProjectListRect.RightBottom -= ProjectManagerWindow.ProjectListRect.LeftTop * 2;
		ProjectManagerWindow.ProjectListRect.Right = uInt(Float(ProjectManagerWindow.ProjectListRect.Right) / 1.25f);

		ProjectManagerWindow.ActionWindowRect.Left = ProjectManagerWindow.ProjectListRect.Left * 2;
		ProjectManagerWindow.ActionWindowRect.Left += ProjectManagerWindow.ProjectListRect.Right;
		ProjectManagerWindow.ActionWindowRect.Right = ProjectManagerWindow.Size.x - ProjectManagerWindow.ActionWindowRect.Left;
		ProjectManagerWindow.ActionWindowRect.Bottom = ProjectManagerWindow.Size.y;
		ProjectManagerWindow.ActionWindowRect.RightBottom -= 16;

		Fullscreen = false;
	}

	void ComputeWindowRect(const Nt::IntRect& newMainWindowRect) noexcept {
		MainWindowRect = newMainWindowRect;

		ObjectsTreeWindowRect.LeftTop = { 0, 0 };
		ObjectsTreeWindowRect.Right = uInt(Float(MainWindowRect.Right) * 0.15f);
		ObjectsTreeWindowRect.Bottom = uInt(Float(MainWindowRect.Bottom) * 0.70f);

		EngineWindowRect.Left = ObjectsTreeWindowRect.Right;
		EngineWindowRect.Top = 0;
		EngineWindowRect.Right = uInt(Float(MainWindowRect.Right) * 0.85f) - EngineWindowRect.Left;
		EngineWindowRect.Bottom = ObjectsTreeWindowRect.Bottom;

		PropertyWindowRect.Left = EngineWindowRect.Right + ObjectsTreeWindowRect.Right;
		PropertyWindowRect.Top = 0;
		PropertyWindowRect.Right = MainWindowRect.Right - uInt(Float(MainWindowRect.Right) * 0.85f);
		PropertyWindowRect.Bottom = MainWindowRect.Bottom;

		FileExplorerWindowRect.Left = 0;
		FileExplorerWindowRect.Top = ObjectsTreeWindowRect.Bottom;
		FileExplorerWindowRect.Right = PropertyWindowRect.Left;
		FileExplorerWindowRect.Bottom = MainWindowRect.Bottom - ObjectsTreeWindowRect.Bottom;
	}

	void Load() {
		std::ifstream file(FileName);
		if (file.is_open()) {
			file.read((char*)this, sizeof(*this));
			file.close();
		}
		else {
			//Save();
		}
	}
	void Save() {
		std::ofstream file(FileName);
		if (file.is_open()) {
			file.write((char*)this, sizeof(*this));
			file.close();
		}
		else {
			Nt::Log::Warning("Failed to save settings");
			WARNING_MSG(L"Failed to save settings", L"Warning");
		}
	}

	Style Styles;
	Language CurrentLanguage;
	Nt::IntRect MainWindowRect;
	Nt::IntRect ObjectsTreeWindowRect;
	Nt::IntRect EngineWindowRect;
	Nt::IntRect PropertyWindowRect;
	Nt::IntRect FileExplorerWindowRect;
	_ProjectManagerWindow ProjectManagerWindow;
	Float Projection_FOV;
	Float Projection_Near;
	Float Projection_Far;
	Bool Fullscreen;
	static const std::string FileName;
};

inline const std::string Settings::FileName = "..\\Settings.nteset";