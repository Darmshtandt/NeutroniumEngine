#pragma once

struct Language {
private:
	struct _Menu : public Nt::ISerialization {
		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, File, File_Open,
				File_Save, File_SaveAs, File_Close, View, View_ObjectsTree,
				View_FileExplorer, View_Property, Create, Create_Primitive,
				Create_Primitive_Cube, Create_Primitive_Pyramid, Create_Entity,
				Create_Entity_Camera, Settings, Settings_Theme);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, File, File_Open,
				File_Save, File_SaveAs, File_Close, View, View_ObjectsTree, View_FileExplorer, View_Property, Create, 
				Create_Primitive, Create_Primitive_Cube, 
				Create_Primitive_Pyramid, Create_Entity, 
				Create_Entity_Camera, Settings, Settings_Theme);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String File;
		Nt::String File_Build;
		Nt::String File_New;
		Nt::String File_Open;
		Nt::String File_Save;
		Nt::String File_SaveAs;
		Nt::String File_Close;
		Nt::String View;
		Nt::String View_ObjectsTree;
		Nt::String View_FileExplorer;
		Nt::String View_Property;
		Nt::String Create;
		Nt::String Create_Primitive;
		Nt::String Create_Primitive_Cube;
		Nt::String Create_Primitive_Quad;
		Nt::String Create_Primitive_Plane;
		Nt::String Create_Primitive_Pyramid;
		Nt::String Create_Entity;
		Nt::String Create_Entity_Camera;
		Nt::String Create_Entity_Sound;
		Nt::String Create_Entity_Model;
		Nt::String Settings;
		Nt::String Settings_Theme;
		Nt::String Settings_Language;
		Nt::String Settings_Language_English;
		Nt::String Settings_Language_Russian;
		Nt::String Settings_Language_Slovak;
	};
	struct _PropertyComponent {
		Nt::String TransformWindow;
		Nt::String TextureWindow;
		Nt::String ScriptWindow;
		Nt::String RigidBodyWindow;
		Nt::String PrimitiveWindow;
		Nt::String SoundWindow;
		Nt::String ModelWindow;
	};
	struct _PropertyTexture {
		Nt::String Offset;
		Nt::String Scale;
		Nt::String Rotate;
		Nt::String ClampU;
		Nt::String ClampV;
	};
	struct _PropertyScript {
		Nt::String Script;
	};
	struct _PropertyRigidBody {
		Nt::String Active;
		Nt::String EnableCollision;
		Nt::String EnableGravitation;
		Nt::String ShowCollider;
		Nt::String Mass;
		Nt::String GravityDirection;
		Nt::String Friction;
	};
	struct _PropertyPrimitive {
		Nt::String Join;
		Nt::String Split;
		Nt::String Invisible;
	};
	struct _PropertySound {
		Nt::String Sound;
		Nt::String RolloffFactor;
		Nt::String ReferenceDistance;
		Nt::String MaxDistance;
		Nt::String Gain;
	};
	struct _PropertyModel {
		Nt::String Model;
	};
	struct _Window : public Nt::ISerialization {
		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, EngineWindow,
				ObjectsTreeWindow, PropertyWindow, FileExplorerWindow);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, EngineWindow,
				ObjectsTreeWindow, PropertyWindow, FileExplorerWindow);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String EngineWindow;
		Nt::String ObjectsTreeWindow;
		Nt::String PropertyWindow;
		Nt::String FileExplorerWindow;
		_PropertyComponent PropertyComponent;
		_PropertyTexture PropertyTexture;
		_PropertyScript PropertyScript;
		_PropertyRigidBody PropertyRigidBody;
		_PropertyPrimitive PropertyPrimitive;
		_PropertySound PropertySound;
		_PropertyModel PropertyModel;
	};
	struct _PropertyWindow : public Nt::ISerialization {
		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, Position,
				Size, Size_Width, Size_Height, Size_Length, Angle);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, Position,
				Size, Size_Width, Size_Height, Size_Length, Angle);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String Position;
		Nt::String Size;
		Nt::String Size_Width;
		Nt::String Size_Height;
		Nt::String Size_Length;
		Nt::String Angle;
	};
	struct _ProjectMenagerWindow {
		struct _ActionWindow {
			Nt::String Open;
			Nt::String Create;
			Nt::String Delete;
			Nt::String RemoveFromList;
			Nt::String AddToList;
		};
		struct _CreationWindow {
			Nt::String ProjectName;
			Nt::String ProjectPath;
		};

		_ActionWindow ActionWindow;
		_CreationWindow CreationWindow;
	};
	struct _Messages {
		Nt::String AddingFile;
	};

public:
	Language() {
		Menu.File = "File";
		Menu.File_Build = "Build";
		Menu.File_New = "New";
		Menu.File_Open = "Open";
		Menu.File_Save = "Save";
		Menu.File_SaveAs = "Save as";
		Menu.File_Close = "Close";
		Menu.View = "View";
		Menu.View_ObjectsTree = "Objects tree window";
		Menu.View_FileExplorer = "File Explorer window";
		Menu.View_Property = "Property window";
		Menu.Create = "Create";
		Menu.Create_Primitive = "Primitive";
		Menu.Create_Primitive_Cube = "Cube";
		Menu.Create_Primitive_Quad = "Quad";
		Menu.Create_Primitive_Plane = "Plane";
		Menu.Create_Primitive_Pyramid = "Pyramid";
		Menu.Create_Entity = "Entity";
		Menu.Create_Entity_Camera = "Camera";
		Menu.Create_Entity_Sound = "Sound";
		Menu.Create_Entity_Model = "Model";
		Menu.Settings = "Settings";
		Menu.Settings_Theme = "Theme";
		Menu.Settings_Language = "Language";
		Menu.Settings_Language_English = "English";
		Menu.Settings_Language_Russian = "Russian";
		Menu.Settings_Language_Slovak = "Slovak";

		Window.EngineWindow = "Engine";
		Window.ObjectsTreeWindow = "Objects tree";
		Window.PropertyWindow = "Property";
		Window.FileExplorerWindow = "File explorer";

		Window.PropertyComponent.TransformWindow = "Transform";
		Window.PropertyComponent.TextureWindow = "Texture";
		Window.PropertyComponent.ScriptWindow = "Script";
		Window.PropertyComponent.RigidBodyWindow = "Rigid body";
		Window.PropertyComponent.PrimitiveWindow = "Primitive";
		Window.PropertyComponent.SoundWindow = "Sound";
		Window.PropertyComponent.ModelWindow = "Model";

		Window.PropertyTexture.Offset = "Offset";
		Window.PropertyTexture.Scale = "Scale";
		Window.PropertyTexture.Rotate = "Rotate";
		Window.PropertyTexture.ClampU = "Clamp U";
		Window.PropertyTexture.ClampV = "Clamp V";

		Window.PropertyScript.Script = "Script";

		Window.PropertyRigidBody.Active = "Active";
		Window.PropertyRigidBody.EnableCollision = "Collision detect";
		Window.PropertyRigidBody.EnableGravitation = "Gravity";
		Window.PropertyRigidBody.ShowCollider = "Show collider";
		Window.PropertyRigidBody.Mass = "Mass";
		Window.PropertyRigidBody.GravityDirection = "Gravity direction";
		Window.PropertyRigidBody.Friction = "Friction";

		Window.PropertyPrimitive.Join = "Join";
		Window.PropertyPrimitive.Split = "Split";
		Window.PropertyPrimitive.Invisible = "Invisible";

		Window.PropertySound.Sound = "Sound";
		Window.PropertySound.RolloffFactor = "Rolloff factor";
		Window.PropertySound.ReferenceDistance = "Reference distance";
		Window.PropertySound.MaxDistance = "Max ditance";
		Window.PropertySound.Gain = "Gain";

		Window.PropertyModel.Model = "Model";

		PropertyWindow.Position = "Position";
		PropertyWindow.Size = "Size";
		PropertyWindow.Size_Width = "Width";
		PropertyWindow.Size_Height = "Height";
		PropertyWindow.Size_Length = "Length";
		PropertyWindow.Angle = "Angle";

		ProjectMenagerWindow.ActionWindow.Open = "Open";
		ProjectMenagerWindow.ActionWindow.Create = "Create";
		ProjectMenagerWindow.ActionWindow.Delete = "Delete";
		ProjectMenagerWindow.ActionWindow.RemoveFromList = "Remove from list";
		ProjectMenagerWindow.ActionWindow.AddToList = "Add to list";

		ProjectMenagerWindow.CreationWindow.ProjectName = "Project name";
		ProjectMenagerWindow.CreationWindow.ProjectPath = "Project path";

		Messages.AddingFile = "To add a file, place it in the project's root folder.";
	}

	void SetSk() {
		Menu.File = L"Súbor";
		Menu.File_Build = "Stavať";
		Menu.File_New = "Nový";
		Menu.File_Open = L"Otvoriť";
		Menu.File_Save = L"Uložiť";
		Menu.File_SaveAs = L"Uložiť ako";
		Menu.File_Close = L"Zavrieť";
		Menu.View = L"Zobraziť";
		Menu.View_ObjectsTree = L"Okno zoznamu objektov";
		Menu.View_FileExplorer = L"Okno súboru prieskumníku";
		Menu.View_Property = L"Okno vlastností";
		Menu.Create = L"Tvorbanie";
		Menu.Create_Primitive = "Primitiv";
		Menu.Create_Primitive_Cube = "Kocka";
		Menu.Create_Primitive_Quad = "Quad";
		Menu.Create_Primitive_Plane = "Rovina";
		Menu.Create_Primitive_Pyramid = "Pyramída";
		Menu.Create_Entity = "Entita";
		Menu.Create_Entity_Camera = "Kamera";

		Menu.Settings = L"Nastavenia";
		Menu.Settings_Theme = L"Téma";
		Menu.Settings_Language = "Jazyk";
		Menu.Settings_Language_English = "Angličtina";
		Menu.Settings_Language_Russian = "Ruský";
		Menu.Settings_Language_Slovak = "Slovenský";

		Window.EngineWindow = L"Engine";
		Window.ObjectsTreeWindow = L"Zoznam objektov";
		Window.PropertyWindow = L"Vlastnosti";
		Window.FileExplorerWindow = L"Prieskumník";

		Window.PropertyComponent.TransformWindow = "Transformovať";
		Window.PropertyComponent.TextureWindow = "Textúra";
		Window.PropertyComponent.ScriptWindow = "Skript";
		Window.PropertyComponent.RigidBodyWindow = "Pevné telo";
		Window.PropertyComponent.PrimitiveWindow = "Primitiv";
		Window.PropertyComponent.SoundWindow = "Zvuk";
		Window.PropertyComponent.ModelWindow = "Model";


		PropertyWindow.Position = L"Poloha";
		PropertyWindow.Size = L"Veľkosť";
		PropertyWindow.Size_Width = L"Šírka";
		PropertyWindow.Size_Height = L"Výška";
		PropertyWindow.Size_Length = L"Dĺžka";
		PropertyWindow.Angle = L"Uhol";

		Window.PropertyTexture.Offset = "Offset";
		Window.PropertyTexture.Scale = "Mierka";
		Window.PropertyTexture.Rotate = "Točiť sa";
		Window.PropertyTexture.ClampU = "Svorka U";
		Window.PropertyTexture.ClampV = "Svorka V";

		Window.PropertyScript.Script = "Skript";

		Window.PropertyRigidBody.Active = "Aktívne";
		Window.PropertyRigidBody.EnableCollision = "Detekcia kolízie";
		Window.PropertyRigidBody.EnableGravitation = "Gravitácia";
		Window.PropertyRigidBody.ShowCollider = "Zobraziť zrážač";
		Window.PropertyRigidBody.Mass = "Omša";
		Window.PropertyRigidBody.GravityDirection = "Smer gravitácie";
		Window.PropertyRigidBody.Friction = "Trenie";

		Window.PropertyPrimitive.Join = "Pripojte sa";
		Window.PropertyPrimitive.Split = "Rozdeliť";
		Window.PropertyPrimitive.Invisible = "Neviditeľný";

		Window.PropertySound.Sound = "Zvuk";
		Window.PropertySound.RolloffFactor = "Roll Off faktor";
		Window.PropertySound.ReferenceDistance = "Referenčná vzdialenosť";
		Window.PropertySound.MaxDistance = "Maximálna vzdialenosť";
		Window.PropertySound.Gain = "Získať";

		Window.PropertyModel.Model = "Model";

		ProjectMenagerWindow.ActionWindow.Open = "Otvoriť";
		ProjectMenagerWindow.ActionWindow.Create = "Vytvorte";
		ProjectMenagerWindow.ActionWindow.Delete = "Odstrániť";
		ProjectMenagerWindow.ActionWindow.RemoveFromList = "Vymazať zo zoznamu";
		ProjectMenagerWindow.ActionWindow.AddToList = "Pridať do zoznamu";

		ProjectMenagerWindow.CreationWindow.ProjectName = "Názov projektu";
		ProjectMenagerWindow.CreationWindow.ProjectPath = "Cesta projektu";

		Messages.AddingFile = "Ak chcete pridať súbor, umiestnite ho do koreňového priečinka projektu.";
	}
	void SetRu() {
		Menu.File = L"Файл";
		Menu.File_Build = "Собрать";
		Menu.File_New = "Новый";
		Menu.File_Open = L"Открыть";
		Menu.File_Save = L"Сохранить";
		Menu.File_SaveAs = L"Сохранить как";
		Menu.File_Close = L"Закрыть";
		Menu.View = L"Вид";
		Menu.View_ObjectsTree = L"Окно объектов";
		Menu.View_FileExplorer = L"Окно файлового проводник";
		Menu.View_Property = L"Окно свойств";
		Menu.Create = L"Создание";
		Menu.Create_Primitive = "Примитив";
		Menu.Create_Primitive_Cube = "Куб";
		Menu.Create_Primitive_Quad = "Квад";
		Menu.Create_Primitive_Plane = "Плоскость";
		Menu.Create_Primitive_Pyramid = "Пирамида";
		Menu.Create_Entity = "Сущности";
		Menu.Create_Entity_Camera = "Камера";

		Menu.Settings = L"Настройки";
		Menu.Settings_Theme = L"Тема";
		Menu.Settings_Language = "Язык";
		Menu.Settings_Language_English = "Английский";
		Menu.Settings_Language_Russian = "Русский";
		Menu.Settings_Language_Slovak = "Словацкий";

		Window.EngineWindow = L"Движек";
		Window.ObjectsTreeWindow = L"Список объектов";
		Window.PropertyWindow = L"Свойства";
		Window.FileExplorerWindow = L"Файловый проводник";

		Window.PropertyComponent.TransformWindow = "Трансформация";
		Window.PropertyComponent.TextureWindow = "Текстура";
		Window.PropertyComponent.ScriptWindow = "Скрипт";
		Window.PropertyComponent.RigidBodyWindow = "Твердое тело";
		Window.PropertyComponent.PrimitiveWindow = "Примитив";
		Window.PropertyComponent.SoundWindow = "Звук";
		Window.PropertyComponent.ModelWindow = "Модель";

		PropertyWindow.Position = L"Позиция";
		PropertyWindow.Size = L"Размер";
		PropertyWindow.Size_Width = L"Ширина";
		PropertyWindow.Size_Height = L"Высота";
		PropertyWindow.Size_Length = L"Длина";
		PropertyWindow.Angle = L"Угол поворота";

		Window.PropertyTexture.Offset = "Сдвиг";
		Window.PropertyTexture.Scale = "Масштаб";
		Window.PropertyTexture.Rotate = "Угол поворота";
		Window.PropertyTexture.ClampU = "Ограничить по U";
		Window.PropertyTexture.ClampV = "Ограничить по V";

		Window.PropertyScript.Script = "Скрипт";

		Window.PropertyRigidBody.Active = "Активный";
		Window.PropertyRigidBody.EnableCollision = "Обработка столкновений";
		Window.PropertyRigidBody.EnableGravitation = "Гравитация";
		Window.PropertyRigidBody.ShowCollider = "Отобрать коллайдеры";
		Window.PropertyRigidBody.Mass = "Масса";
		Window.PropertyRigidBody.GravityDirection = "Направление гравитации";
		Window.PropertyRigidBody.Friction = "Сила трения";

		Window.PropertyPrimitive.Join = "Соединить";
		Window.PropertyPrimitive.Split = "Разделить";
		Window.PropertyPrimitive.Invisible = "Невидимый";

		Window.PropertySound.Sound = "Звук";
		Window.PropertySound.RolloffFactor = "Коэффициент спада";
		Window.PropertySound.ReferenceDistance = "Базовое расстояние";
		Window.PropertySound.MaxDistance = "Максимальное расстояние";
		Window.PropertySound.Gain = "Прирост";

		Window.PropertyModel.Model = "Модель";

		ProjectMenagerWindow.ActionWindow.Open = "Окрыть";
		ProjectMenagerWindow.ActionWindow.Create = "Создать";
		ProjectMenagerWindow.ActionWindow.Delete = "Удалить";
		ProjectMenagerWindow.ActionWindow.RemoveFromList = "Убрать из списка";
		ProjectMenagerWindow.ActionWindow.AddToList = "Добавить в список";

		ProjectMenagerWindow.CreationWindow.ProjectName = "Názov projektu";
		ProjectMenagerWindow.CreationWindow.ProjectPath = "Cesta projektu";

		Messages.AddingFile = "Для того чтобы загрузить файл, добавте его в корневую папку проекта.";
	}

	void Load(const std::string& filePath) {
		std::ifstream file(filePath, std::ios::binary);
		if (file.is_open()) {
			Nt::Serialization::ReadAll(file, Menu, Window, PropertyWindow);
		}
		else {
			Nt::Log::Warning("Failed to open language");
			WARNING_MSG(L"Failed to open language", L"Warning");
		}
	}
	void Save(const std::string& filePath) {
		std::ofstream file(filePath, std::ios::binary);
		if (file.is_open()) {
			Nt::Serialization::WriteAll(file, Menu, Window, PropertyWindow);
			file.close();
		}
		else {
			Nt::Log::Warning("Failed to save language");
			WARNING_MSG(L"Failed to save language", L"Warning");
		}
	}

	_Menu Menu;
	_Window Window;
	_PropertyWindow PropertyWindow;
	_ProjectMenagerWindow ProjectMenagerWindow;
	_Messages Messages;
};