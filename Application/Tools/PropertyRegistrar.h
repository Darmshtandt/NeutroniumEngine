#pragma once

#include <Tools/PropertyFactory.h>

template <class _Ty>
class PropertyRegistrar {
public:
	PropertyRegistrar(const std::string& className) {
		PropertyFactory::Instance().Register(className, [] (NotNull<Selector*> pSelector, NotNull<Scene*> pScene) {
			return new _Ty(pSelector, pScene);
			});
	}
};