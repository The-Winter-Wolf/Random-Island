#pragma once

#include "../objects/object_3D/cObject3D.h"

#include <glm/glm.hpp>
#include <vector>


// debug mods
#define OBJECT_CHANGE_MODE	0
#define LIGHT_CHANGE_MODE	0
#define AUDIO_CHANGE_MODE	0

// config format
#define JSON_CONFIG 1
#define XML_CONFIG 0

#include "../system/window/cConsoleLogs.h"
extern cConsoleLogs* g_pConsole;

extern bool bIsMoveLock;
extern bool bIsMouseLock;

extern bool g_bConsoleMessage;
extern bool g_bConsoleSuccess;
extern bool g_bConsoleWarning;
extern bool g_bConsoleOutput;

extern int g_ShaderID;

extern int g_SelectedLight;
extern int g_SelectedAudio;
extern int g_SelectedObject;
extern int g_SelectedPhysObject;

extern bool g_bModelHasLight;

extern bool g_bDrawDebugObjects;
extern bool g_bDrawDebugLightSpheres;

extern glm::vec3 WORLD_POS;
extern cObject3D* g_pDebugSphereObject;

extern std::vector< cObject3D* > g_vpObjectToDraw;
extern std::vector< cObject3D* > g_vpDebugObjectToDraw;
extern std::vector< cObject3D* > g_vpLocalObjectToDraw;
