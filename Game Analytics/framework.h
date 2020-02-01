#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <vector>
#include <string>

#ifdef _WIN32
#include "curl/Include/curl/curl.h"
#include "openssl/include/openssl/hmac.h"
#include "openssl/include/openssl/evp.h"
#endif
#ifdef __linux__
#ifndef __i386__
    //sudo apt-get install libcurl4-openssl-dev
#include <curl/curl.h>
#endif
#endif

//#include "Analytics.h"