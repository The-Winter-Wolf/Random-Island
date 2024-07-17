#include "cLuaBrain.h"

#include "../../objects/audio/cAudioInstaller.h"
extern cAudioInstaller* g_pAudioInstaller;

#include "../../world/player/cPlayer.h"
extern cPlayer* g_pPlayer;

#include "../../scenes/cSceneManager.h"
extern cSceneManager* g_pSceneManager;

extern cLuaBrain g_LuaBrain;

#include <glm/glm.hpp>

int lua_TurnOnSound(lua_State* L)
{
    int channelId = -1;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TNUMBER)
        channelId = lua_tonumber(L, 1);
    else return 3;

    g_pAudioInstaller->SetPaused(channelId, false);

    return 0;
}

int lua_TurnOffSound(lua_State* L)
{
    int channelId = -1;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TNUMBER)
        channelId = lua_tonumber(L, 1);
    else return 3;

    g_pAudioInstaller->SetPaused(channelId, true);

    return 0;
}

int lua_CheckLocalEvent(lua_State* L)
{
    if ( ! g_pAudioInstaller->GetPaused(0))
    {
        return 0;
    }

    glm::vec2 eventPos = glm::vec2(1058.72f, 1114.73f);

    glm::vec2 pos = glm::vec3(0.0f);
    pos.x = g_pPlayer->GetPlayerPos().x;
    pos.y = g_pPlayer->GetPlayerPos().z;

    float distance = glm::distance(pos, eventPos);

    if (distance < 2.5f)
    {
        g_LuaBrain.RunScript("TurnOnSound(0)");
        g_pSceneManager->Input(0);
    }   

    return 0;
}