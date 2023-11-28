#pragma once

#define WIN32LEAN_AND_MEAN

#include <Windows.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <NtStdH.h>
#include <Nt/Graphics.h>
#include <Nt/Physics.h>
#include <Wave.h>
#include <Sound.h>
#include <Collider.h>

#include <Lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

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

#pragma warning(disable : 4996)

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

inline uInt g_TextureCameraIndex = -1;
inline uInt g_TextureSoundIndex = -1;

__inline Bool IsValidPath(const Nt::String& rootPath, const Nt::String& verifiablePath) {
	if (rootPath.length() <= verifiablePath.length()) {
		for (uInt i = 0; i < rootPath.length(); ++i)
			if (rootPath[i] != verifiablePath[i])
				return false;
	}
	else {
		return false;
	}
	return true;
}