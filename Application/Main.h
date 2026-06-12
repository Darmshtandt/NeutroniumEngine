#pragma once

#define WIN32_LEAN_AND_MEAN

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

#include <NtStdH.h>
#include <Nt/Graphics.h>
#include <Nt/Physics.h>

#include <Nt/Graphics/Sound/Sound.h>
#include <Nt/Collider.h>
#include <Nt/Light.h>

constexpr std::string_view APPLICATION_NAME = "Neutronium Engine";
constexpr const char* LANG_FOLDER = "\\Lang";

namespace std {
	using namespace std::filesystem;
}

using std::cout;
using std::endl;

_NODISCARD
__inline Bool IsValidPath(const Nt::String& rootPath, const Nt::String& verifiablePath) noexcept {
	if (rootPath.length() > verifiablePath.length())
		return false;

	for (uInt i = 0; i < rootPath.length(); ++i) {
		if (rootPath[i] != verifiablePath[i])
			return false;
	}
	return true;
}