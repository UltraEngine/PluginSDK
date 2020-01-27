#include "MemWriter.h"

namespace GMFSDK
{
	MemWriter::MemWriter() : pos(0), capacity(0), size(0), mem(nullptr) {}

	MemWriter::~MemWriter()
	{
		if (mem != nullptr)
		{
			free(mem);
			mem = nullptr;
		}
	}

	void MemWriter::LoseMem()
	{
		mem = nullptr;
	}

	void MemWriter::Seek(uint64_t pos)
	{
		this->pos = pos;
	}

	uint64_t MemWriter::Pos()
	{
		return pos;
	}

	void MemWriter::Resize(uint64_t size)
	{
		if (size == 0)
		{
			this->size = 0;
			this->capacity = 0;
			free(mem);
			mem = nullptr;
			return;
		}
		if (size < capacity)
		{
			this->size = size;
			return;
		}
		this->capacity = size * 1.3;
		this->size = size;
		if (mem == nullptr)
		{
			mem = malloc(this->capacity);
		}
		else
		{
			mem = realloc(mem, this->capacity);
		}
		if (mem == nullptr) throw("Failed to allocate memory.");
	}

	uint64_t MemWriter::Size()
	{
		return size;
	}

	void* MemWriter::data()
	{
		return mem;
	}

	void MemWriter::Write(const std::wstring& s)
	{
		wchar_t term = 0;
		if (!s.empty()) Write((void*)s.c_str(), s.length() * 2);
		Write(&term, 2);
	}

	void MemWriter::Write(const std::string& s)
	{
		char term = 0;
		if (!s.empty()) Write((void*)s.c_str(), s.length() * 1);
		Write(&term, 1);
	}

	void MemWriter::Write(void* src, uint64_t length)
	{
		if (length == 0) return;
		if (pos + length >= size) Resize(pos + length);
		memcpy(((char*)mem) + pos, src, length);
		pos += length;
	}
}