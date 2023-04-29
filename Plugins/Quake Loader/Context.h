#pragma once

#include "DLLExports.h"

using namespace UltraEngine::PluginSDK;

struct Context
{
	Context();

	std::vector<void*> memblocks;
	std::wstring log;
	void Print(const std::wstring& text)
	{
		if (not log.empty()) log += L"\n";
		log += text;
	}
	MemWriter writer;
	void* mem;
};