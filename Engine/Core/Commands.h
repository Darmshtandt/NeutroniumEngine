#pragma once

#include <Objects/Object.h>

struct SelectObjectCommand final {
	Object* pObject;
};
struct MultiSelectObjectCommand final {
	Object* pObject;
};
struct DeselectObjectCommand final {
	Object* pObject;
};
struct DeselectAllObjectsCommand final
{
};