#pragma once
#include <string>
#include <vector>
#ifdef _WIN32
	#include <Windows.h>
	#include <stringapiset.h>
#else
	#include <codecvt>
#endif

namespace GMFSDK
{
	extern std::wstring StripExt(std::wstring path);
	extern std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to);
	extern std::wstring StripDir(const std::wstring& path);
	extern std::wstring StripAll(const std::wstring& path);
	extern std::wstring WString(const std::string& s);
}