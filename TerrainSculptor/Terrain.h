#pragma once

class Terrain : public Entity {
public:
	Terrain(const std::string& name) :
		Entity(name, EntityTypes::TERRAIN)
	{
	}

private:
};