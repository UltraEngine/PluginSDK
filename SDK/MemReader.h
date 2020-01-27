#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace GMFSDK
{
	class MemReader
	{
		void* mem;
		uint64_t size;
		uint64_t pos;
	public:

		MemReader(void* mem, uint64_t size);
		~MemReader();

		virtual uint64_t Size();
		virtual void* data();
		virtual void Seek(uint64_t pos);
		virtual uint64_t Pos();
		virtual void Read(void* dest, uint64_t length);
		virtual std::string ReadString(const uint64_t maxlength=0);
		virtual std::wstring ReadWString(const uint64_t maxlength=0);

		template <typename T>
		void Read(T* info)
		{
			Read((void*)info, sizeof(T));
		}
	};
}