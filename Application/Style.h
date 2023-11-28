#pragma once

struct Style {
	struct Text {
		Nt::uInt3D Color = { 0, 0, 0 };
		uInt Weight = 400;
	};
	struct Handle {
		Nt::uInt3D BackgroundColor = { 255, 255, 255 };
		Nt::uInt3D BorderColor = { 128, 128, 128 };
	};
	struct HandleElement : public Handle {
		Text Text;
	};

	struct MainWindow : public Handle {
	};
	struct ObjectsTreeWindow : public Handle {
		HandleElement TreeView;
	};
	struct EngineWindow : public Handle {
	};
	struct PropertyWindow : public Handle {
		HandleElement TextEdits;
		HandleElement Buttons;
		Text Texts;
		Text HeaderTexts;
	};
	struct FileExplorerWindow : public Handle {
		HandleElement TextEdits;
		Text Texts;
	};
	struct ProjectMenagerWindow : public Handle {
		Handle ActionWindow;
		Handle CreationWindow;
		Handle ProjectList;
		Handle Buttons;
		Handle TextEdits;
		Text Headers;
		Text Texts;
	};

	Style() {
		Main.BackgroundColor = { 1, 14, 18 };
		Main.BorderColor = { 45, 38, 91 };

		ObjectsTree.BackgroundColor = { 0, 43, 54 };
		ObjectsTree.BorderColor = { 45, 38, 91 };
		ObjectsTree.TreeView.Text.Color = { 40, 217, 252 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 128, 128, 255 };
		Engine.BorderColor = { 45, 38, 91 };

		Property.BackgroundColor = { 0, 43, 54 };
		Property.BorderColor = { 45, 38, 91 };
		Property.HeaderTexts.Color = { 40, 217, 252 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 131, 148, 150 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 0, 53, 66 };
		Property.TextEdits.Text.Color = { 42, 161, 152 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 0, 43, 54 };
		FileExplorer.BorderColor = { 45, 38, 91 };
		FileExplorer.Texts.Color = { 131, 148, 150 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 0, 53, 66 };
		FileExplorer.TextEdits.Text.Color = { 42, 161, 152 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 0, 43, 54 };
		ProjectMenager.BorderColor = { 45, 38, 91 };

		ProjectMenager.ProjectList.BackgroundColor = { 0, 21, 27 };
		ProjectMenager.ProjectList.BorderColor = { 45, 38, 91 };
		ProjectMenager.ActionWindow.BackgroundColor = { 0, 83, 108 };
		ProjectMenager.ActionWindow.BorderColor = { 45, 38, 91 };
		ProjectMenager.CreationWindow.BackgroundColor = { 0, 83, 108 };
		ProjectMenager.CreationWindow.BorderColor = { 45, 38, 91 };
		ProjectMenager.Buttons.BackgroundColor = { 0, 83, 108 };
		ProjectMenager.Buttons.BorderColor = { 45, 38, 91 };
		ProjectMenager.Headers.Color = { 42, 161, 152 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 131, 148, 150 };
		ProjectMenager.Texts.Weight = 100;
	}

	void Load(const std::string& filePath) {
		std::ifstream file(filePath);
		if (file.is_open()) {
			file.read((char*)this, sizeof(*this));
			file.close();
		}
		else {
			//Save(fileName);
		}
	}
	void Save(const std::string& filePath) {
		std::ofstream file(filePath);
		if (file.is_open()) {
			file.write((char*)this, sizeof(*this));
			file.close();
		}
		else {
			Nt::Log::Warning("Failed to save style");
			WARNING_MSG(L"Failed to save style", L"Warning");
		}
	}

	void SetSolarizedGreenDarkTheme() {
		Main.BackgroundColor = { 1, 18, 14 };
		Main.BorderColor = { 45, 91, 38 };

		ObjectsTree.BackgroundColor = { 0, 54, 43 };
		ObjectsTree.BorderColor = { 45, 91, 38 };
		ObjectsTree.TreeView.Text.Color = { 40, 252, 217 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 128, 255, 128 };
		Engine.BorderColor = { 45, 91, 38 };

		Property.BackgroundColor = { 0, 54, 43 };
		Property.BorderColor = { 45, 91, 38 };
		Property.HeaderTexts.Color = { 40, 252, 217 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 131, 150, 148 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 0, 66, 53 };
		Property.TextEdits.Text.Color = { 42, 152, 161 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 0, 54, 43 };
		FileExplorer.BorderColor = { 45, 91, 38 };
		FileExplorer.Texts.Color = { 131, 150, 148 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 0, 66, 53 };
		FileExplorer.TextEdits.Text.Color = { 42, 152, 161 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 0, 54, 43 };
		ProjectMenager.BorderColor = { 45, 91, 38 };

		ProjectMenager.ProjectList.BackgroundColor = { 0, 27, 21 };
		ProjectMenager.ProjectList.BorderColor = { 45, 91, 38 };
		ProjectMenager.ActionWindow.BackgroundColor = { 0, 108, 83 };
		ProjectMenager.ActionWindow.BorderColor = { 45, 91, 38 };
		ProjectMenager.CreationWindow.BackgroundColor = { 0, 108, 83 };
		ProjectMenager.CreationWindow.BorderColor = { 45, 91, 38 };
		ProjectMenager.Buttons.BackgroundColor = { 0, 108, 83 };
		ProjectMenager.Buttons.BorderColor = { 45, 91, 38 };
		ProjectMenager.Headers.Color = { 42, 152, 161 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 131, 150, 148 };
		ProjectMenager.Texts.Weight = 100;
	}
	void SetSolarizedRedDarkTheme() {
		Main.BackgroundColor = { 18, 1, 14 };
		Main.BorderColor = { 91, 45, 38 };

		ObjectsTree.BackgroundColor = { 54, 0, 43 };
		ObjectsTree.BorderColor = { 91, 45, 38 };
		ObjectsTree.TreeView.Text.Color = { 252, 40, 217 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 255, 128, 128 };
		Engine.BorderColor = { 91, 45, 38 };

		Property.BackgroundColor = { 54, 0, 43 };
		Property.BorderColor = { 91, 45, 38 };
		Property.HeaderTexts.Color = { 252, 40, 217 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 150, 131, 148 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 66, 0, 53 };
		Property.TextEdits.Text.Color = { 152, 42, 161 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 54, 0, 43 };
		FileExplorer.BorderColor = { 91, 45, 38 };
		FileExplorer.Texts.Color = { 150, 131, 148 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 66, 0, 53 };
		FileExplorer.TextEdits.Text.Color = { 152, 42, 161 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 54, 0, 43 };
		ProjectMenager.BorderColor = { 91, 45, 38 };

		ProjectMenager.ProjectList.BackgroundColor = { 27, 0, 21 };
		ProjectMenager.ProjectList.BorderColor = { 91, 45, 38 };
		ProjectMenager.ActionWindow.BackgroundColor = { 108, 0, 83 };
		ProjectMenager.ActionWindow.BorderColor = { 91, 45, 38 };
		ProjectMenager.CreationWindow.BackgroundColor = { 108, 0, 83 };
		ProjectMenager.CreationWindow.BorderColor = { 91, 45, 38 };
		ProjectMenager.Buttons.BackgroundColor = { 108, 0, 83 };
		ProjectMenager.Buttons.BorderColor = { 91, 45, 38 };
		ProjectMenager.Headers.Color = { 152, 42, 161 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 150, 131, 148 };
		ProjectMenager.Texts.Weight = 100;
	}
	void SetWhite() {
		Main.BackgroundColor = { 255, 255, 255 };
		Main.BorderColor = { 196, 196, 196 };

		ObjectsTree.BackgroundColor = { 255, 255, 255 };
		ObjectsTree.BorderColor = { 196, 196, 196 };
		ObjectsTree.TreeView.Text.Color = { 40, 217, 252 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 196, 196, 196 };
		Engine.BorderColor = { 196, 196, 196 };

		Property.BackgroundColor = { 255, 255, 255 };
		Property.BorderColor = { 196, 196, 196 };
		Property.HeaderTexts.Color = { 40, 217, 252 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 131, 148, 150 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 255, 255, 255 };
		Property.TextEdits.Text.Color = { 42, 161, 152 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 255, 255, 255 };
		FileExplorer.BorderColor = { 196, 196, 196 };
		FileExplorer.Texts.Color = { 131, 148, 150 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 255, 255, 255 };
		FileExplorer.TextEdits.Text.Color = { 42, 161, 152 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 255, 255, 255 };
		ProjectMenager.BorderColor = { 196, 196, 196 };

		ProjectMenager.ProjectList.BackgroundColor = { 255, 255, 255 };
		ProjectMenager.ProjectList.BorderColor = { 196, 196, 196 };
		ProjectMenager.ActionWindow.BackgroundColor = { 255, 255, 255 };
		ProjectMenager.ActionWindow.BorderColor = { 196, 196, 196 };
		ProjectMenager.CreationWindow.BackgroundColor = { 255, 255, 255 };
		ProjectMenager.CreationWindow.BorderColor = { 196, 196, 196 };
		ProjectMenager.Buttons.BackgroundColor = { 255, 255, 255 };
		ProjectMenager.Buttons.BorderColor = { 196, 196, 196 };
		ProjectMenager.Headers.Color = { 42, 161, 152 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 131, 148, 150 };
		ProjectMenager.Texts.Weight = 100;
	}
	void SetDark() {
		Main.BackgroundColor = { 128, 128, 128 };
		Main.BorderColor = { 200, 200, 200 };

		ObjectsTree.BackgroundColor = { 128, 128, 128 };
		ObjectsTree.BorderColor = { 200, 200, 200 };
		ObjectsTree.TreeView.Text.Color = { 40, 217, 252 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 128, 128, 128 };
		Engine.BorderColor = { 200, 200, 200 };

		Property.BackgroundColor = { 128, 128, 128 };
		Property.BorderColor = { 200, 200, 200 };
		Property.HeaderTexts.Color = { 40, 217, 252 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 131, 148, 150 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 128, 128, 128 };
		Property.TextEdits.Text.Color = { 42, 161, 152 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 128, 128, 128 };
		FileExplorer.BorderColor = { 200, 200, 200 };
		FileExplorer.Texts.Color = { 131, 148, 150 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 128, 128, 128 };
		FileExplorer.TextEdits.Text.Color = { 42, 161, 152 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 128, 128, 128 };
		ProjectMenager.BorderColor = { 200, 200, 200 };

		ProjectMenager.ProjectList.BackgroundColor = { 128, 128, 128 };
		ProjectMenager.ProjectList.BorderColor = { 200, 200, 200 };
		ProjectMenager.ActionWindow.BackgroundColor = { 128, 128, 128 };
		ProjectMenager.ActionWindow.BorderColor = { 200, 200, 200 };
		ProjectMenager.CreationWindow.BackgroundColor = { 128, 128, 128 };
		ProjectMenager.CreationWindow.BorderColor = { 200, 200, 200 };
		ProjectMenager.Buttons.BackgroundColor = { 128, 128, 128 };
		ProjectMenager.Buttons.BorderColor = { 200, 200, 200 };
		ProjectMenager.Headers.Color = { 42, 161, 152 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 131, 148, 150 };
		ProjectMenager.Texts.Weight = 100;
	}
	void SetBlack() {
		Main.BackgroundColor = { 0, 0, 0 };
		Main.BorderColor = { 128, 128, 128 };

		ObjectsTree.BackgroundColor = { 0, 0, 0 };
		ObjectsTree.BorderColor = { 128, 128, 128 };
		ObjectsTree.TreeView.Text.Color = { 40, 217, 252 };
		ObjectsTree.TreeView.Text.Weight = 600;
		ObjectsTree.TreeView.BackgroundColor = ObjectsTree.BackgroundColor;

		Engine.BackgroundColor = { 128, 128, 255 };
		Engine.BorderColor = { 128, 128, 128 };

		Property.BackgroundColor = { 0, 0, 0 };
		Property.BorderColor = { 128, 128, 128 };
		Property.HeaderTexts.Color = { 40, 217, 252 };
		Property.HeaderTexts.Weight = 800;
		Property.Texts.Color = { 131, 148, 150 };
		Property.Texts.Weight = 600;
		Property.TextEdits.BackgroundColor = { 0, 0, 0 };
		Property.TextEdits.Text.Color = { 42, 161, 152 };
		Property.TextEdits.Text.Weight = 700;
		Property.Buttons = Property.TextEdits;

		FileExplorer.BackgroundColor = { 0, 0, 0 };
		FileExplorer.BorderColor = { 128, 128, 128 };
		FileExplorer.Texts.Color = { 131, 148, 150 };
		FileExplorer.Texts.Weight = 600;
		FileExplorer.TextEdits.BackgroundColor = { 0, 0, 0 };
		FileExplorer.TextEdits.Text.Color = { 42, 161, 152 };
		FileExplorer.TextEdits.Text.Weight = 700;

		ProjectMenager.BackgroundColor = { 0, 0, 0 };
		ProjectMenager.BorderColor = { 128, 128, 128 };

		ProjectMenager.ProjectList.BackgroundColor = { 0, 0, 0 };
		ProjectMenager.ProjectList.BorderColor = { 128, 128, 128 };
		ProjectMenager.ActionWindow.BackgroundColor = { 0, 0, 0 };
		ProjectMenager.ActionWindow.BorderColor = { 128, 128, 128 };
		ProjectMenager.CreationWindow.BackgroundColor = { 0, 0, 0 };
		ProjectMenager.CreationWindow.BorderColor = { 128, 128, 128 };
		ProjectMenager.Buttons.BackgroundColor = { 0, 0, 0 };
		ProjectMenager.Buttons.BorderColor = { 128, 128, 128 };
		ProjectMenager.Headers.Color = { 42, 161, 152 };
		ProjectMenager.Headers.Weight = 600;
		ProjectMenager.Texts.Color = { 131, 148, 150 };
		ProjectMenager.Texts.Weight = 100;
	}

	MainWindow Main;
	ObjectsTreeWindow ObjectsTree;
	EngineWindow Engine;
	PropertyWindow Property;
	FileExplorerWindow FileExplorer;
	ProjectMenagerWindow ProjectMenager;
};