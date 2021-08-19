#include "Utilities.h"

namespace GMFSDK
{
	std::string String(const float f) {
		std::stringstream out;
		out << f;
		auto s = out.str();
		if (s.find(".") == -1) s += ".0";
		return s;
	}

	std::string String(const int i)
	{
		std::stringstream out;
		out << i;
		return out.str();
	}

	//Removes the file extension from a path
	std::wstring StripExt(std::wstring path)
	{
		size_t p = path.find_last_of(L".");
		if (p == std::wstring::npos) return path;
		size_t g1 = path.find_last_of(L"/");
		size_t g2 = path.find_last_of(L"\\");
		size_t p2 = g1;

		//Curse unsigned integers
		if (g1 == std::wstring::npos)
		{
			p2 = g2;
		}
		else
		{
			if (g2 != std::wstring::npos)
			{
				if (g2 > g1) p2 = g2;
			}
		}

		if (p2 != std::wstring::npos)
		{
			if (p2 > p) return path;
		}
		return path.substr(0, p);
	}

	std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to)
	{
		std::wstring str = s;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}

	std::wstring StripDir(const std::wstring& path)
	{
		std::wstring cleanpath = Replace(path, L"\\", L"/");
		size_t p = cleanpath.find_last_of(L"/");
		if (p != std::wstring::npos) return cleanpath.substr(p + 1);
		return cleanpath;
	}

	std::wstring StripAll(const std::wstring& path)
	{
		return StripExt(StripDir(path));
	}

	std::string String(const std::wstring& s)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
		return converter.to_bytes(s);
	}

	//Converts a string to a unicode string
	std::wstring WString(const std::string& s)
	{
#ifdef _WIN32
		std::vector<wchar_t> ws(s.length()+1);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), s.length(), &ws[0], s.length());
		ws[ws.size() - 1] = 0;
		return std::wstring(&ws[0]);
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.from_bytes(s);
#endif
	}
}