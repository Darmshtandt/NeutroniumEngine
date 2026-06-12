#pragma once
#include <Nt/Core/Math/VectorND.h>

enum class TransforType : Byte {
	POSITION, SIZE, ROTATION
};

struct UpdateObjectTransformEvent final {
	Nt::Float3D Value;
	TransforType Type;
	void* pEmmiter = nullptr;
};