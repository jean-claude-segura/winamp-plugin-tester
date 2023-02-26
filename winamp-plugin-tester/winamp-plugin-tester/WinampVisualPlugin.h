#pragma once

#include <windows.h>

struct winampVisModule;

typedef int(__cdecl* funcConfig)(const winampVisModule*);
typedef int(__cdecl* funcInit)(const winampVisModule*);
typedef int(__cdecl* funcRender)(const winampVisModule*);
typedef void(__cdecl* procQuit)(const winampVisModule*);
typedef void(__cdecl* procUserData)(void);

struct winampVisModule
{
    /*
    TwinampVisModule = record*/
    char* description;        // description of module
    HWND hwndParent;         // parent window (filled in by calling app)
    HINSTANCE hDllInstance;        // instance handle to this DLL (filled in by calling app)
    unsigned _int32 sRate;     // sample rate (filled in by calling app)
    unsigned _int32 nCh;     // number of channels (filled in...)
    unsigned _int32 latencyMs;     // latency from call to Render to actual drawing
    unsigned _int32 delayMs;     // delay between calls to Render (in ms)

    // the data is filled in according to the respective Nch entry
    unsigned _int32 spectrumNCh;     // Number of channels
    unsigned _int32 waveformNCh;     // Number of channels
    unsigned char spectrumData[2][576];     // waveform data   (values from 0-255)
    unsigned char waveformData[2][576];     // spectrum data   (values from 0-255)

    // functions that winamp calls to configure the plugin, initialise ...
        //userData: procedure; cdecl;  // user data, optional*/

    funcConfig Config;
    funcInit Init;
    funcRender Render;
    procQuit Quit;
    procUserData userData;
};

typedef winampVisModule* (__cdecl* GetModule)(int);

struct winampVisHeader
{
    int version;
    char* description;
    GetModule getModule;
};

// PWinampVisModule = ^ TwinampVisModule;
static winampVisModule* lpWinampVisModule;
static HMODULE handleLib;

typedef winampVisHeader* (__cdecl* WinampVisGetHeader)();
