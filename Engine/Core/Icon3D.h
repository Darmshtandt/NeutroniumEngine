#pragma once

#include <Nt/Graphics/Objects/Model.h>

class Icon3D : public Nt::Model {
public:
	Icon3D();

private:
	Nt::Mesh m_Mesh;
};