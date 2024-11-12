#pragma once

class Selector;
class ObjectsTree;

class GameSound;
class GameModel;
class GameCamera;

class Scene {
public:
	struct Layer {
		std::vector<Layer> Layers;
		Nt::String Name;
	};

	using LayerContainer = std::vector<Layer>;

	enum class Versions {
		TEST_0_1,
		LAST
	};

public:
	Scene();
	Scene(const Scene& scence);
	~Scene();

	void Start();
	void Stop();

	Object* RayCastObject(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint = nullptr);

	void BindObjectsTree(ObjectsTree* objectsTreePtr) noexcept;

	void AddObject(Object* pObject);
	void RemoveObject(const Object* pObject);
	void RemoveSelected(Selector* pSelector);

	void Clear();

	void Update(const Float& time);
	void Render(Nt::Renderer* pRenderer) const;

	void AllowLayerOverlap(const Nt::String& layerName_1, const Nt::String& layerName_2, const Bool& isAllow);

	Bool Load(const std::string& fileName);
	void Save(const std::string& fileName);

	void Copy(Selector* selectorPtr);
	void Cut(Selector* selectorPtr);
	void Paste(Selector* selectorPtr);
	void ClearBuffer();

	Object* operator [] (const uInt& index) const;

	Lua* GetLua() const noexcept;
	const ObjectContainer& GetObjects() const;
	Object* GetObjectPtr(const uInt& index) const;
	Object* GetObjectPtrByName(const Nt::String& name) const;
	GameSound* GetSoundPtrByName(const Nt::String& name) const;
	GameModel* GetModelPtrByName(const Nt::String& name) const;
	GameCamera* GetCameraPtrByName(const Nt::String& name) const;


private:
	Lua* m_pLua = nullptr;
	Versions m_Version = Versions::LAST;
	ObjectContainer m_Objects;
	ObjectContainer m_Clipboard;
	ObjectsTree* m_ObjectsTreePtr;
	std::vector<Layer> m_DisjointLayers;

private:
	void _Initialize();
};