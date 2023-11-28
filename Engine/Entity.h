#pragma once

enum class EntityTypes {
	NONE,
	CAMERA,
	SOUND,
	MODEL,
};

class Entity : public Object {
public:
	Entity(const std::string& name, const EntityTypes& type) :
		Object(ObjectTypes::ENTITY, name),
		m_Type(type)
	{ 
	}

	void Write(std::ostream & stream) const override {
		Object::Write(stream);
		Nt::Serialization::WriteAll(stream, m_Type);
	}
	void Read(std::istream & stream) override {
		Object::Read(stream);
		Nt::Serialization::ReadAll(stream, m_Type);
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}
	constexpr uInt ClassType() const noexcept {
		return uInt(ObjectTypes::ENTITY);
	}

	void SetEntityType(const EntityTypes& type) {
		m_Type = type;
	}

	virtual Object* GetCopy() const override {
		return new Entity(*this);
	}
	EntityTypes GetEntityType() const noexcept {
		return m_Type;
	}

private:
	EntityTypes m_Type;
};

__inline Entity* UpcastObjectToEntity(Object* pObject) {
	if (pObject == nullptr)
		Raise("Object pointer is null");
	if (pObject->ObjectType != ObjectTypes::ENTITY)
		Raise("Object pointer is not Entity");

	Entity* pEntity = dynamic_cast<Entity*>(pObject);
	if (pEntity == nullptr)
		Raise("Failed to upcast Object to Entity.");
	return pEntity;
}