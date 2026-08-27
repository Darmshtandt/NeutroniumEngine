#pragma once

#include <Objects/Object.h>

struct SelectObjectCommand final {
	ObjectPtr pObject;
};
struct MultiSelectObjectCommand final {
	ObjectPtr pObject;
};
struct DeselectObjectCommand final {
	ObjectPtr pObject;
};
struct DeselectAllObjectsCommand final
{
};