#pragma once
#include <stdint.h>
#include <memory>
#include <string.h>
#include <string>

namespace UltraEngine::PluginSDK
{
	class MemWriter
	{
		void* mem;
		uint64_t size;
		uint64_t pos;
		uint64_t capacity;
		bool owner;
	public:

		MemWriter();
		MemWriter(void* data, uint64_t sz);
		~MemWriter();

		virtual void LoseMem();
		virtual uint64_t Size();
		virtual void Seek(uint64_t pos);
		virtual uint64_t Pos();
		virtual void Write(void* src, uint64_t length);
		virtual void Write(const std::wstring& s, const bool nullterminated = true);
		virtual void Write(const std::string& s, const bool nullterminated = true);
		virtual void Resize(uint64_t size);
		virtual void* data();

		template <typename T>
		void Write(T* info)
		{
			Write((void*)info, sizeof(T));
		}
	};
}