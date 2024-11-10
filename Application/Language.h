#pragma once

class Language {
public:
	struct _Menu : public Nt::ISerialization {
		enum {
			TEXT_FILE,
			TEXT_FILE_BUILD,
			TEXT_FILE_NEW,
			TEXT_FILE_OPEN,
			TEXT_FILE_SAVE,
			TEXT_FILE_SAVEAS,
			TEXT_FILE_CLOSE,
			TEXT_VIEW,
			TEXT_VIEW_OBJECTSTREE,
			TEXT_VIEW_FILEEXPLORER,
			TEXT_VIEW_PROPERTY,
			TEXT_CREATE,
			TEXT_CREATE_PRIMITIVE,
			TEXT_CREATE_PRIMITIVE_CUBE,
			TEXT_CREATE_PRIMITIVE_QUAD,
			TEXT_CREATE_PRIMITIVE_PLANE,
			TEXT_CREATE_PRIMITIVE_PYRAMID,
			TEXT_CREATE_ENTITY,
			TEXT_CREATE_ENTITY_CAMERA,
			TEXT_CREATE_ENTITY_SOUND,
			TEXT_CREATE_ENTITY_MODEL,
			TEXT_SETTINGS,
			TEXT_SETTINGS_THEME,
			TEXT_SETTINGS_LANGUAGE,
			TEXT_SETTINGS_LANGUAGE_ENGLISH,
			TEXT_SETTINGS_LANGUAGE_RUSSIAN,
			TEXT_SETTINGS_LANGUAGE_SLOVAK,
			TEXT_COUNT
		};

		void Write(std::ostream& stream) const override {
			for (uInt i = 0; i < TEXT_COUNT; ++i)
				Nt::Serialization::WriteAll(stream, Texts[i]);
		}
		void Read(std::istream& stream) override {
			for (uInt i = 0; i < TEXT_COUNT; ++i)
				Nt::Serialization::ReadAll(stream, Texts[i]);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String Texts[TEXT_COUNT];
	};
	struct _PropertyWindow : public Nt::ISerialization {
		struct _Component {
			enum {
				TEXT_TRANSFORMWINDOW,
				TEXT_TEXTUREWINDOW,
				TEXT_SCRIPTWINDOW,
				TEXT_RIGIDBODYWINDOW,
				TEXT_PRIMITIVEWINDOW,
				TEXT_SOUNDWINDOW,
				TEXT_MODELWINDOW,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Texture {
			enum {
				TEXT_OFFSET,
				TEXT_SCALE,
				TEXT_ROTATE,
				TEXT_CLAMPU,
				TEXT_CLAMPV,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Script {
			enum {
				TEXT_SCRIPT,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _RigidBody {
			enum {
				TEXT_ACTIVE,
				TEXT_ENABLECOLLISION,
				TEXT_ENABLEGRAVITATION,
				TEXT_SHOWCOLLIDER,
				TEXT_MASS,
				TEXT_GRAVITYDIRECTION,
				TEXT_FRICTION,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Primitive {
			enum {
				TEXT_JOIN,
				TEXT_SPLIT,
				TEXT_INVISIBLE,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Sound {
			enum {
				TEXT_SOUND,
				TEXT_ROLLOFFFACTOR,
				TEXT_REFERENCEDISTANCE,
				TEXT_MAXDISTANCE,
				TEXT_GAIN,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Model {
			enum {
				TEXT_MODEL,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Transform {
			enum {
				TEXT_POSITION,
				TEXT_POSITION_X, TEXT_POSITION_Y, TEXT_POSITION_Z,
				TEXT_SIZE,
				TEXT_SIZE_WIDTH,
				TEXT_SIZE_HEIGHT,
				TEXT_SIZE_LENGTH,
				TEXT_ANGLE,
				TEXT_ANGLE_ROLL, TEXT_ANGLE_PITCH, TEXT_ANGLE_YAW,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};

		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, Transform, 
				Component, Texture, Script, RigidBody, 
				Primitive, Sound, Model);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, Transform,
				Component, Texture, Script, RigidBody,
				Primitive, Sound, Model);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		_Transform Transform;
		_Component Component;
		_Texture Texture;
		_Script Script;
		_RigidBody RigidBody;
		_Primitive Primitive;
		_Sound Sound;
		_Model Model;
	};

	struct _WindowNames : public Nt::ISerialization {
		enum {
			TEXT_ENGINEWINDOW,
			TEXT_OBJECTSTREEWINDOW,
			TEXT_PROPERTYWINDOW,
			TEXT_FILEEXPLORERWINDOW,
			TEXT_COUNT
		};

		void Write(std::ostream& stream) const override {
			for (uInt i = 0; i < TEXT_COUNT; ++i)
				Nt::Serialization::WriteAll(stream, Texts[i]);
		}
		void Read(std::istream& stream) override {
			for (uInt i = 0; i < TEXT_COUNT; ++i)
				Nt::Serialization::ReadAll(stream, Texts[i]);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String Texts[TEXT_COUNT];
	};
	struct _ProjectManagerWindow {
		struct _Action {
			enum {
				TEXT_OPEN,
				TEXT_DELETE,
				TEXT_CREATE,
				TEXT_REMOVEFROMLIST,
				TEXT_ADDTOLIST,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};
		struct _Creation {
			enum {
				TEXT_PROJECTNAME,
				TEXT_PROJECTPATH,
				TEXT_COUNT
			};

			Nt::String Texts[TEXT_COUNT];
		};

		_Action Action;
		_Creation Creation;
	};
	struct _Messages {
		enum {
			TEXT_ADDINGFILE,
			TEXT_COUNT
		};

		Nt::String Texts[TEXT_COUNT];
	};

public:
	Language() {
		Menu.Texts[_Menu::TEXT_FILE] = "File";
		Menu.Texts[_Menu::TEXT_FILE_BUILD] = "Build";
		Menu.Texts[_Menu::TEXT_FILE_NEW] = "New";
		Menu.Texts[_Menu::TEXT_FILE_OPEN] = "Open";
		Menu.Texts[_Menu::TEXT_FILE_SAVE] = "Save";
		Menu.Texts[_Menu::TEXT_FILE_SAVEAS] = "Save as";
		Menu.Texts[_Menu::TEXT_FILE_CLOSE] = "Close";
		Menu.Texts[_Menu::TEXT_VIEW] = "View";
		Menu.Texts[_Menu::TEXT_VIEW_OBJECTSTREE] = "Objects tree window";
		Menu.Texts[_Menu::TEXT_VIEW_FILEEXPLORER] = "File Explorer window";
		Menu.Texts[_Menu::TEXT_VIEW_PROPERTY] = "Property window";
		Menu.Texts[_Menu::TEXT_CREATE] = "Create";
		Menu.Texts[_Menu::TEXT_CREATE_PRIMITIVE] = "Primitive";
		Menu.Texts[_Menu::TEXT_CREATE_PRIMITIVE_CUBE] = "Cube";
		Menu.Texts[_Menu::TEXT_CREATE_PRIMITIVE_QUAD] = "Quad";
		Menu.Texts[_Menu::TEXT_CREATE_PRIMITIVE_PLANE] = "Plane";
		Menu.Texts[_Menu::TEXT_CREATE_PRIMITIVE_PYRAMID] = "Pyramid";
		Menu.Texts[_Menu::TEXT_CREATE_ENTITY] = "Entity";
		Menu.Texts[_Menu::TEXT_CREATE_ENTITY_CAMERA] = "Camera";
		Menu.Texts[_Menu::TEXT_CREATE_ENTITY_SOUND] = "Sound";
		Menu.Texts[_Menu::TEXT_CREATE_ENTITY_MODEL] = "Model";
		Menu.Texts[_Menu::TEXT_SETTINGS] = "Settings";
		Menu.Texts[_Menu::TEXT_SETTINGS_THEME] = "Theme";
		Menu.Texts[_Menu::TEXT_SETTINGS_LANGUAGE] = "Language";
		Menu.Texts[_Menu::TEXT_SETTINGS_LANGUAGE_ENGLISH] = "English";
		Menu.Texts[_Menu::TEXT_SETTINGS_LANGUAGE_RUSSIAN] = "Russian";
		Menu.Texts[_Menu::TEXT_SETTINGS_LANGUAGE_SLOVAK] = "Slovak";

		Window.Texts[_WindowNames::TEXT_ENGINEWINDOW] = "Engine";
		Window.Texts[_WindowNames::TEXT_OBJECTSTREEWINDOW] = "Objects tree";
		Window.Texts[_WindowNames::TEXT_PROPERTYWINDOW] = "Property";
		Window.Texts[_WindowNames::TEXT_FILEEXPLORERWINDOW] = "File explorer";


		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_TRANSFORMWINDOW] = "Transform";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_TEXTUREWINDOW] = "Texture";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_SCRIPTWINDOW] = "Script";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_RIGIDBODYWINDOW] = "Rigid body";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_PRIMITIVEWINDOW] = "Primitive";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_SOUNDWINDOW] = "Sound";
		PropertyWindow.Component.Texts[_PropertyWindow::_Component::TEXT_MODELWINDOW] = "Model";

		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_POSITION] = "Position";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_POSITION_X] = "X";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_POSITION_Y] = "Y";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_POSITION_Z] = "Z";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_SIZE] = "Size";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_SIZE_WIDTH] = "Width";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_SIZE_HEIGHT] = "Height";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_SIZE_LENGTH] = "Length";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_ANGLE] = "Angle";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_ANGLE_ROLL] = "Roll";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_ANGLE_PITCH] = "Pitch";
		PropertyWindow.Transform.Texts[_PropertyWindow::_Transform::TEXT_ANGLE_YAW] = "Yaw";

		PropertyWindow.Texture.Texts[_PropertyWindow::_Texture::TEXT_OFFSET] = "Offset";
		PropertyWindow.Texture.Texts[_PropertyWindow::_Texture::TEXT_SCALE] = "Scale";
		PropertyWindow.Texture.Texts[_PropertyWindow::_Texture::TEXT_ROTATE] = "Rotate";
		PropertyWindow.Texture.Texts[_PropertyWindow::_Texture::TEXT_CLAMPU] = "Clamp U";
		PropertyWindow.Texture.Texts[_PropertyWindow::_Texture::TEXT_CLAMPV] = "Clamp V";

		PropertyWindow.Script.Texts[_PropertyWindow::_Script::TEXT_SCRIPT] = "Script";

		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_ACTIVE] = "Active";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_ENABLECOLLISION] = "Collision detect";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_ENABLEGRAVITATION] = "Gravity";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_SHOWCOLLIDER] = "Show collider";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_MASS] = "Mass";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_GRAVITYDIRECTION] = "Gravity direction";
		PropertyWindow.RigidBody.Texts[_PropertyWindow::_RigidBody::TEXT_FRICTION] = "Friction";

		PropertyWindow.Primitive.Texts[_PropertyWindow::_Primitive::TEXT_JOIN] = "Join";
		PropertyWindow.Primitive.Texts[_PropertyWindow::_Primitive::TEXT_SPLIT] = "Split";
		PropertyWindow.Primitive.Texts[_PropertyWindow::_Primitive::TEXT_INVISIBLE] = "Invisible";

		PropertyWindow.Sound.Texts[_PropertyWindow::_Sound::TEXT_SOUND] = "Sound";
		PropertyWindow.Sound.Texts[_PropertyWindow::_Sound::TEXT_ROLLOFFFACTOR] = "Rolloff factor";
		PropertyWindow.Sound.Texts[_PropertyWindow::_Sound::TEXT_REFERENCEDISTANCE] = "Reference distance";
		PropertyWindow.Sound.Texts[_PropertyWindow::_Sound::TEXT_MAXDISTANCE] = "Max ditance";
		PropertyWindow.Sound.Texts[_PropertyWindow::_Sound::TEXT_GAIN] = "Gain";

		PropertyWindow.Model.Texts[_PropertyWindow::_Model::TEXT_MODEL] = "Model";

		ProjectManagerWindow.Action.Texts[_ProjectManagerWindow::_Action::TEXT_OPEN] = "Open";
		ProjectManagerWindow.Action.Texts[_ProjectManagerWindow::_Action::TEXT_CREATE] = "Create";
		ProjectManagerWindow.Action.Texts[_ProjectManagerWindow::_Action::TEXT_DELETE] = "Delete";
		ProjectManagerWindow.Action.Texts[_ProjectManagerWindow::_Action::TEXT_REMOVEFROMLIST] = "Remove from list";
		ProjectManagerWindow.Action.Texts[_ProjectManagerWindow::_Action::TEXT_ADDTOLIST] = "Add to list";

		ProjectManagerWindow.Creation.Texts[_ProjectManagerWindow::_Creation::TEXT_PROJECTNAME] = "Project name";
		ProjectManagerWindow.Creation.Texts[_ProjectManagerWindow::_Creation::TEXT_PROJECTPATH] = "Project path";

		Messages.Texts[_Messages::TEXT_ADDINGFILE] = "To add a file, place it in the project's root folder.";
	}

#if 0
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

		Window.Component.TransformWindow = "Transformovať";
		Window.Component.TextureWindow = "Textúra";
		Window.Component.ScriptWindow = "Skript";
		Window.Component.RigidBodyWindow = "Pevné telo";
		Window.Component.PrimitiveWindow = "Primitiv";
		Window.Component.SoundWindow = "Zvuk";
		Window.Component.ModelWindow = "Model";


		PropertyWindow.Position = L"Poloha";
		PropertyWindow.Size = L"Veľkosť";
		PropertyWindow.Size_Width = L"Šírka";
		PropertyWindow.Size_Height = L"Výška";
		PropertyWindow.Size_Length = L"Dĺžka";
		PropertyWindow.Angle = L"Uhol";

		Window.Texture.Offset = "Offset";
		Window.Texture.Scale = "Mierka";
		Window.Texture.Rotate = "Točiť sa";
		Window.Texture.ClampU = "Svorka U";
		Window.Texture.ClampV = "Svorka V";

		Window.Script.Script = "Skript";

		Window.RigidBody.Active = "Aktívne";
		Window.RigidBody.EnableCollision = "Detekcia kolízie";
		Window.RigidBody.EnableGravitation = "Gravitácia";
		Window.RigidBody.ShowCollider = "Zobraziť zrážač";
		Window.RigidBody.Mass = "Omša";
		Window.RigidBody.GravityDirection = "Smer gravitácie";
		Window.RigidBody.Friction = "Trenie";

		Window.Primitive.Join = "Pripojte sa";
		Window.Primitive.Split = "Rozdeliť";
		Window.Primitive.Invisible = "Neviditeľný";

		Window.Sound.Sound = "Zvuk";
		Window.Sound.RolloffFactor = "Roll Off faktor";
		Window.Sound.ReferenceDistance = "Referenčná vzdialenosť";
		Window.Sound.MaxDistance = "Maximálna vzdialenosť";
		Window.Sound.Gain = "Získať";

		Window.Model.Model = "Model";

		ProjectManagerWindow.Action.Open = "Otvoriť";
		ProjectManagerWindow.Action.Create = "Vytvorte";
		ProjectManagerWindow.Action.Delete = "Odstrániť";
		ProjectManagerWindow.Action.RemoveFromList = "Vymazať zo zoznamu";
		ProjectManagerWindow.Action.AddToList = "Pridať do zoznamu";

		ProjectManagerWindow.Creation.ProjectName = "Názov projektu";
		ProjectManagerWindow.Creation.ProjectPath = "Cesta projektu";

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

		Window.Component.TransformWindow = "Трансформация";
		Window.Component.TextureWindow = "Текстура";
		Window.Component.ScriptWindow = "Скрипт";
		Window.Component.RigidBodyWindow = "Твердое тело";
		Window.Component.PrimitiveWindow = "Примитив";
		Window.Component.SoundWindow = "Звук";
		Window.Component.ModelWindow = "Модель";

		PropertyWindow.Position = L"Позиция";
		PropertyWindow.Size = L"Размер";
		PropertyWindow.Size_Width = L"Ширина";
		PropertyWindow.Size_Height = L"Высота";
		PropertyWindow.Size_Length = L"Длина";
		PropertyWindow.Angle = L"Угол поворота";

		Window.Texture.Offset = "Сдвиг";
		Window.Texture.Scale = "Масштаб";
		Window.Texture.Rotate = "Угол поворота";
		Window.Texture.ClampU = "Ограничить по U";
		Window.Texture.ClampV = "Ограничить по V";

		Window.Script.Script = "Скрипт";

		Window.RigidBody.Active = "Активный";
		Window.RigidBody.EnableCollision = "Обработка столкновений";
		Window.RigidBody.EnableGravitation = "Гравитация";
		Window.RigidBody.ShowCollider = "Отобрать коллайдеры";
		Window.RigidBody.Mass = "Масса";
		Window.RigidBody.GravityDirection = "Направление гравитации";
		Window.RigidBody.Friction = "Сила трения";

		Window.Primitive.Join = "Соединить";
		Window.Primitive.Split = "Разделить";
		Window.Primitive.Invisible = "Невидимый";

		Window.Sound.Sound = "Звук";
		Window.Sound.RolloffFactor = "Коэффициент спада";
		Window.Sound.ReferenceDistance = "Базовое расстояние";
		Window.Sound.MaxDistance = "Максимальное расстояние";
		Window.Sound.Gain = "Прирост";

		Window.Model.Model = "Модель";

		ProjectManagerWindow.Action.Open = "Окрыть";
		ProjectManagerWindow.Action.Create = "Создать";
		ProjectManagerWindow.Action.Delete = "Удалить";
		ProjectManagerWindow.Action.RemoveFromList = "Убрать из списка";
		ProjectManagerWindow.Action.AddToList = "Добавить в список";

		ProjectManagerWindow.Creation.ProjectName = "Názov projektu";
		ProjectManagerWindow.Creation.ProjectPath = "Cesta projektu";

		Messages.AddingFile = "Для того чтобы загрузить файл, добавте его в корневую папку проекта.";
	}
#endif

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
	_WindowNames Window;
	_PropertyWindow PropertyWindow;
	_ProjectManagerWindow ProjectManagerWindow;
	_Messages Messages;
};