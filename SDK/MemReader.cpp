#include "MemReader.h"

namespace GMFSDK
{
	MemReader::MemReader(void* mem, uint64_t size) : pos(0)
	{
		this->mem = mem;
		this->size = size;
	}

	MemReader::~MemReader()
	{
		size = 0;
		mem = nullptr;
		pos = 0;
	}

	uint64_t MemReader::Size()
	{
		return size;
	}

	std::string MemReader::ReadString(const uint64_t maxlength)
	{
		std::vector<char> s;
		char c;
		while (true)
		{
			Read(&c, 1);
			if (c == 0) break;
			s.push_back(c);
			if (maxlength != 0)
			{
				if (s.size() == maxlength) break;
			}
		}
		s.push_back(0);
		return std::string(&s[0]);
	}

	void* MemReader::data()
	{
		return mem;
	}

	std::wstring MemReader::ReadWString(const uint64_t maxlength)
	{
		std::vector<wchar_t> s;
		wchar_t c;
		_ASSERT(sizeof(wchar_t) == 2);
		while (true)
		{
			Read(&c, 2);
			if (c == 0) break;
			s.push_back(c);
			if (maxlength != 0)
			{
				if (s.size() == maxlength) break;
			}
		}
		s.push_back(0);
		return std::wstring(&s[0]);
	}

	void MemReader::Seek(uint64_t pos)
	{
		this->pos = pos;
	}

	uint64_t MemReader::Pos()
	{
		return pos;
	}

	void MemReader::Read(void* dest, uint64_t length)
	{
		if (pos + length > size)
		{
			printf("Memory accessed outside of bounds.");
			throw("Memory accessed outside of bounds.");
		}
		memcpy(dest, ((char*)mem) + pos, length);
		pos += length;
	}
}
