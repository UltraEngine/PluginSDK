#pragma once
#include <string>
#include <vector>
#ifdef _WIN32
	#include <Windows.h>
	#include <stringapiset.h>
#else
	#include <codecvt>
#endif

#include <locale>
#include <codecvt>
#include <sstream>

namespace GMFSDK
{
	extern std::wstring Lower(const std::wstring& s);
	extern std::string Lower(const std::string& s);
	extern std::wstring ExtractExt(const std::wstring& path);
	extern std::wstring ExtractDir(const std::wstring& path);
	extern std::string ExtractExt(const std::string& path);
	extern std::string String(const float f);
	extern std::string String(const int i);
	extern std::string String(const std::wstring& s);
	extern std::wstring StripExt(std::wstring path);
	extern std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to);
	extern std::wstring StripDir(const std::wstring& path);
	extern std::wstring StripAll(const std::wstring& path);
	extern std::wstring WString(const std::string& s);
	extern std::string Right(const std::string& s, const int length);
	extern std::string Left(const std::string& s, const int length);
	extern int Find(const std::string& s, const std::string& token);
	extern std::string Replace(const std::string& s, const std::string& from, const std::string& to);
}