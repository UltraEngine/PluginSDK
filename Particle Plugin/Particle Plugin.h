#pragma once

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the PARTICLEPLUGIN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// PARTICLEPLUGIN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
/*
#ifdef PARTICLEPLUGIN_EXPORTS
#define PARTICLEPLUGIN_API __declspec(dllexport)
#else
#define PARTICLEPLUGIN_API __declspec(dllimport)
#endif

// This class is exported from the dll
class PARTICLEPLUGIN_API CParticlePlugin {
public:
	CParticlePlugin(void);
	// TODO: add your methods here.
};

extern PARTICLEPLUGIN_API int nParticlePlugin;

PARTICLEPLUGIN_API int fnParticlePlugin(void);
*/
#include <stdint.h>

// add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

struct ParticleSystem
{
    const uint32_t size;
    const float matrix[16];
};

struct Particle
{
	const uint32_t size;
	const uint32_t index;
	float position[3];
	float color[4];
	float radius;
	float rotation;
	float falloff;
	float velocity[3];
	const float startposition[3];
	const float startcolor[4];
	const float startmatrix[16];
};

struct dParticleSystem
{
    const uint32_t size;
    const double matrix[16];
};

struct dParticle
{
	const uint32_t size;
	const uint32_t index;
	double position[3];
	double color[4];
	double radius;
	double rotation;
	double falloff;
	double velocity[3];
	const double startposition[3];
	const double startcolor[4];
	const double startmatrix[16];
};

enum EmissionShape
{
	EMISSION_SHAPE_BOX = 0,
	EMISSION_SHAPE_CYLINDER = 1
};

class ParticleModifier
{
public:
	EmissionShape emissionshape;
	double area[3];

	ParticleModifier();
};

extern "C"
{
	__declspec(dllexport) ParticleModifier* CreateParticleModifier();
	__declspec(dllexport) void FreeParticleModifier(ParticleModifier* mod);
    __declspec(dllexport) int GetPluginInfo(unsigned char* cs, int maxsize);
   // __declspec(dllexport) void SetFloatProperty(const wchar_t* name, const double* value, const int count);
   // __declspec(dllexport) void SetStringProperty(const wchar_t* name, const wchar_t* value);
    __declspec(dllexport) void EmitParticle(ParticleModifier* mod, ParticleSystem* particlesystem, Particle* particle);
    __declspec(dllexport) void EmitParticle64f(ParticleModifier* mod, const dParticleSystem* particlesystem, dParticle* particle);
}