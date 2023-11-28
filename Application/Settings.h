#pragma once

struct Settings {
private:
	struct _ProjectMenagerWindow {
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

		ProjectMenagerWindow.Size = Nt::Float2D(Nt::GetMonitorSize()) / 1.5f;
		ProjectMenagerWindow.ProjectListRect.LeftTop = { 20, 30 };
		ProjectMenagerWindow.ProjectListRect.RightBottom = ProjectMenagerWindow.Size;
		ProjectMenagerWindow.ProjectListRect.RightBottom -= ProjectMenagerWindow.ProjectListRect.LeftTop * 2;
		ProjectMenagerWindow.ProjectListRect.Right = Float(ProjectMenagerWindow.ProjectListRect.Right) / 1.25f;

		ProjectMenagerWindow.ActionWindowRect.Left = ProjectMenagerWindow.ProjectListRect.Left * 2;
		ProjectMenagerWindow.ActionWindowRect.Left += ProjectMenagerWindow.ProjectListRect.Right;
		ProjectMenagerWindow.ActionWindowRect.Right = ProjectMenagerWindow.Size.x - ProjectMenagerWindow.ActionWindowRect.Left;
		ProjectMenagerWindow.ActionWindowRect.Bottom = ProjectMenagerWindow.Size.y;
		ProjectMenagerWindow.ActionWindowRect.RightBottom -= 16;

		Fullscreen = false;
	}

	void ComputeWindowRect(const Nt::IntRect& newMainWIndowRect) noexcept {
		MainWindowRect = newMainWIndowRect;

		ObjectsTreeWindowRect.LeftTop = { 0, 0 };
		ObjectsTreeWindowRect.Right = MainWindowRect.Right * 0.15f;
		ObjectsTreeWindowRect.Bottom = MainWindowRect.Bottom * 0.70f;

		EngineWindowRect.Left = ObjectsTreeWindowRect.Right;
		EngineWindowRect.Top = 0;
		EngineWindowRect.Right = MainWindowRect.Right * 0.85f - EngineWindowRect.Left;
		EngineWindowRect.Bottom = ObjectsTreeWindowRect.Bottom;

		PropertyWindowRect.Left = EngineWindowRect.Right + ObjectsTreeWindowRect.Right;
		PropertyWindowRect.Top = 0;
		PropertyWindowRect.Right = MainWindowRect.Right - MainWindowRect.Right * 0.85f;
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
	Nt::uIntRect MainWindowRect;
	Nt::uIntRect ObjectsTreeWindowRect;
	Nt::uIntRect EngineWindowRect;
	Nt::uIntRect PropertyWindowRect;
	Nt::uIntRect FileExplorerWindowRect;
	_ProjectMenagerWindow ProjectMenagerWindow;
	Float Projection_FOV;
	Float Projection_Near;
	Float Projection_Far;
	Bool Fullscreen;
	static const std::string FileName;
};

inline const std::string Settings::FileName = "..\\Settings.nteset";