#pragma once

#define WIN32LEAN_AND_MEAN

#ifdef WIN32
#	define _BitScanForward64 _BitScanForward
#endif
#ifdef WIN32
#	define _BitScanReverse64 _BitScanReverse
#endif

#pragma warning(disable : 4996)


#include <thread>
#include <mutex>

#include <Windows.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/Exact_integer.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Nef_polyhedron_3.h>

#include <NtStdH.h>
#include <Nt/Graphics.h>
#include <Nt/Physics.h>
#include <Wave.h>
#include <Sound.h>
#include <Collider.h>

extern "C" {
#include <Lua/lua.hpp>
}
#include <LuaBridge/LuaBridge.h>

#pragma comment(lib, "OpenAL32")
#pragma comment(lib, "OpenAL32")

#ifdef _DEBUG
#	pragma comment(lib, "NeutroniumCore32d")
#	pragma comment(lib, "NeutroniumGraphics32d")
#	pragma comment(lib, "NeutroniumPhysics32d")
#else
#	pragma comment(lib, "NeutroniumCore32")
#	pragma comment(lib, "NeutroniumGraphics32")
#	pragma comment(lib, "NeutroniumPhysics32")
#endif

#define APPLICATION_NAME "Neutronium Engine"

namespace std {
	using namespace std::filesystem;
}

using std::cout;
using std::endl;

using Nt::Float;
using Nt::Double;
using Nt::LDouble;
using Nt::Int;
using Nt::uInt;
using Nt::Long;
using Nt::uLong;
using Nt::LLong;
using Nt::uLLong;
using Nt::Short;
using Nt::uShort;
using Nt::Bool;
using Nt::Char;
using Nt::wChar;
using Nt::cString;
using Nt::cwString;
using Nt::Word;
using Nt::DWord;

__inline Bool IsValidPath(const Nt::String& rootPath, const Nt::String& verifiablePath) {
	if (rootPath.length() > verifiablePath.length())
		return false;

	for (uInt i = 0; i < rootPath.length(); ++i) {
		if (rootPath[i] != verifiablePath[i])
			return false;
	}
	return true;
}