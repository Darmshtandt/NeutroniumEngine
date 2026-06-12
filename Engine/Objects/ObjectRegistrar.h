#pragma once

#include <Objects/ObjectFactory.h>

template <class _Base, class _Ty>
class ObjectRegistrar {
public:
	ObjectRegistrar() {
		ObjectFactory<_Base>::Instance().Register(_Ty::GetClassToken(), [] (const std::string& name) {
			return new _Ty(name);
			});
	}
};

template <class _Ty>
using EntityRegistrar = ObjectRegistrar<Entity, _Ty>;

template <class _Ty>
using PrimitiveRegistrar = ObjectRegistrar<Primitive, _Ty>;