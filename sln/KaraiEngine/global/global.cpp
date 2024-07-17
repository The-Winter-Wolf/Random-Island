#include "global.h"

bool g_bConsoleMessage = false;
bool g_bConsoleSuccess = true;
bool g_bConsoleWarning = false;
bool g_bConsoleOutput = true;

int g_ShaderID = 0;

bool bIsMoveLock = false;
bool bIsMouseLock = false;

int g_SelectedLight = 0;
int g_SelectedAudio = 0;
int g_SelectedObject = 0;
int g_SelectedPhysObject = 0;

bool g_bModelHasLight = true;

bool g_bDrawDebugLightSpheres = false;
bool g_bDrawDebugObjects = false;

cObject3D* g_pDebugSphereObject = new cObject3D();

glm::vec3 WORLD_POS = glm::vec3(128.0f, 0.0f, 128.0f);

std::vector< cObject3D* > g_vpObjectToDraw;
std::vector< cObject3D* > g_vpDebugObjectToDraw;
std::vector< cObject3D* > g_vpLocalObjectToDraw;