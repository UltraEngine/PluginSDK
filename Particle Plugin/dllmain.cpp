// dllmain.cpp : Defines the entry point for the DLL application.
#include <math.h>
#include <stdint.h>
#include "Particle Plugin.h"
#include "mt19937ar.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#include <string>
 
ParticleModifier::ParticleModifier()
{
    emissionshape = EMISSION_SHAPE_BOX;
    area[0] = 2;
    area[1] = 2;
    area[2] = 2;
}

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
    std::string s =
        "{"
        "\"turboPlugin\":{"
        "\"title\":\"Particle Plugin Example.\","
        "\"description\":\"Shows how to modify particle behavior.\","
        "\"author\":\"Josh Klint\","
        "\"url\":\"www.leadwerks.com\""
        "}"
        "}";

    if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize, s.length()));
    return s.length();
}

ParticleModifier* CreateParticleModifier()
{
    return new ParticleModifier;
}

void FreeParticleModifier(ParticleModifier* m)
{
    delete m;
}

/*void SetFloatProperty(const wchar_t* name, const double* value, const int count)
{
    if (name == L"area" && count == 3)
    {
        properties.area[0] = value[0];
        properties.area[1] = value[1];
        properties.area[2] = value[2];
    }
}

void SetStringProperty(const wchar_t* name, const wchar_t* value)
{
    if (name == L"emissionshape")
    {
        if (value == L"BOX")
        {
            properties.emissionshape = EMISSION_SHAPE_BOX;
        }
        else if (value == L"CYLINDER")
        {
            properties.emissionshape = EMISSION_SHAPE_CYLINDER;
        }
    }
}*/

float Random(const float min, const float max)
{
    float f = (float)genrand_real1();
    return (max - min) * f + min;
}

void EmitParticle(ParticleModifier* mod, ParticleSystem* particlesystem, Particle* particle)
{
    if (mod->emissionshape == EMISSION_SHAPE_BOX)
    {
        particle->position[0] = Random(-mod->area[0], mod->area[0]);
        particle->position[1] = Random(-mod->area[1], mod->area[1]);
        particle->position[2] = Random(-mod->area[2], mod->area[2]);
    }
    else if (mod->emissionshape == EMISSION_SHAPE_CYLINDER)
    {
        particle->position[0] = Random(-mod->area[0], mod->area[0]);
        particle->position[1] = Random(-mod->area[1], mod->area[1]);
        particle->position[2] = Random(-mod->area[2], mod->area[2]);
        auto l = sqrt(particle->position[0] * particle->position[0] + particle->position[1] * particle->position[1] + particle->position[2] * particle->position[2]);
        if (l > 0.0f)
        {
            particle->position[0] /= l;
            particle->position[1] /= l;
            particle->position[2] /= l;
        }
    }
    particle->position[0] += particlesystem->matrix[12];
    particle->position[1] += particlesystem->matrix[13];
    particle->position[2] += particlesystem->matrix[14];
}

//void UpdateParticle(const ParticleSystem& particlesystem, Particle& particle)
//{
//
//}

void EmitParticle64f(ParticleModifier* mod, const dParticleSystem* particlesystem, dParticle* particle)
{
    if (mod->emissionshape == EMISSION_SHAPE_BOX)
    {
        particle->position[0] = Random(-mod->area[0], mod->area[0]);
        particle->position[1] = Random(-mod->area[1], mod->area[1]);
        particle->position[2] = Random(-mod->area[2], mod->area[2]);
    }
    else if (mod->emissionshape == EMISSION_SHAPE_CYLINDER)
    {
        particle->position[0] = Random(-mod->area[0], mod->area[0]);
        particle->position[1] = Random(-mod->area[1], mod->area[1]);
        particle->position[2] = Random(-mod->area[2], mod->area[2]);
        auto l = sqrt(particle->position[0] * particle->position[0] + particle->position[1] * particle->position[1] + particle->position[2] * particle->position[2]);
        if (l > 0.0)
        {
            particle->position[0] /= l;
            particle->position[1] /= l;
            particle->position[2] /= l;
        }
    }
    particle->position[0] += particlesystem->matrix[12];
    particle->position[1] += particlesystem->matrix[13];
    particle->position[2] += particlesystem->matrix[14];
}

//void UpdateParticle64f(const ParticleSystem& particlesystem, dParticle& particle)
//{
//
//}
