#include "cLuaBrain.h"

#include "../../commands/cCommandManager.h"
#include "../../commands/cGroupManager.h"

extern cLuaBrain g_LuaBrain;
extern cCommandManager* g_pCommandManager;
extern cGroupManager* g_pGroupManager;

int lua_AddMoveCommand(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    std::string subjectName;
    glm::vec3 endXYZ = glm::vec3(0.0f);
    float timeToMove = 0.0f;
    float speed = 0.0f;
    bool easeIn = false;
    bool easeOut = false;

    if (lua_gettop(L) < 3)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TSTRING)
    {
        subjectName = lua_tostring(L, 3);

        if (lua_type(L, 4) == LUA_TNUMBER)
            timeToMove = (float)lua_tonumber(L, 4);

        if (lua_type(L, 5) == LUA_TNUMBER)
            speed = (float)lua_tonumber(L, 5);

        if (lua_type(L, 6) == LUA_TBOOLEAN)
            easeIn = lua_toboolean(L, 6);

        if (lua_type(L, 7) == LUA_TBOOLEAN)
            easeOut = lua_toboolean(L, 7);

        g_pCommandManager->AddMoveCommand(commandName, objectName, subjectName,
            timeToMove, speed, easeIn, easeOut);
        return 0;
    }
    else if (lua_type(L, 3) == LUA_TNUMBER &&
             lua_type(L, 4) == LUA_TNUMBER &&
             lua_type(L, 5) == LUA_TNUMBER)
    {
        endXYZ.x = (float)lua_tonumber(L, 3);
        endXYZ.y = (float)lua_tonumber(L, 4);
        endXYZ.z = (float)lua_tonumber(L, 5);

        if (lua_type(L, 6) == LUA_TNUMBER)
            timeToMove = (float)lua_tonumber(L, 6);

        if (lua_type(L, 7) == LUA_TNUMBER)
            speed = (float)lua_tonumber(L, 7);

        if (lua_type(L, 8) == LUA_TBOOLEAN)
            easeIn = lua_toboolean(L, 8);

        if (lua_type(L, 9) == LUA_TBOOLEAN)
            easeOut = lua_toboolean(L, 9);

        g_pCommandManager->AddMoveCommand(commandName, objectName, endXYZ,
                                          timeToMove, speed, easeIn, easeOut);
        return 0;
    }
    else
    {
        return 3;
    }
}

int lua_AddCurveCommand(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    float time = 0.0f;
    float rotAngle = 0.0f;

    glm::vec3 point1 = glm::vec3(0.0f);
    glm::vec3 point2 = glm::vec3(0.0f);
    glm::vec3 point3 = glm::vec3(0.0f);
    glm::vec3 point4 = glm::vec3(0.0f);
    glm::vec3 point5 = glm::vec3(0.0f);

    if (lua_gettop(L) < 4)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TNUMBER)
        time = (float)lua_tonumber(L, 3);
    else return 3;

    if (lua_type(L, 4) == LUA_TNUMBER)
        rotAngle = (float)lua_tonumber(L, 4);
    else return 3;

    if (lua_type(L, 5) == LUA_TNUMBER &&
        lua_type(L, 6) == LUA_TNUMBER &&
        lua_type(L, 7) == LUA_TNUMBER)
    {
        point1.x = (float)lua_tonumber(L, 5);
        point1.y = (float)lua_tonumber(L, 6);
        point1.z = (float)lua_tonumber(L, 7);
    }
    else return 3;

    if (lua_type(L, 8) == LUA_TNUMBER &&
        lua_type(L, 9) == LUA_TNUMBER &&
        lua_type(L, 10) == LUA_TNUMBER)
    {
        point2.x = (float)lua_tonumber(L, 8);
        point2.y = (float)lua_tonumber(L, 9);
        point2.z = (float)lua_tonumber(L, 10);
    }

    if (lua_type(L, 11) == LUA_TNUMBER &&
        lua_type(L, 12) == LUA_TNUMBER &&
        lua_type(L, 13) == LUA_TNUMBER)
    {
        point3.x = (float)lua_tonumber(L, 11);
        point3.y = (float)lua_tonumber(L, 12);
        point3.z = (float)lua_tonumber(L, 13);
    }

    if (lua_type(L, 14) == LUA_TNUMBER &&
        lua_type(L, 15) == LUA_TNUMBER &&
        lua_type(L, 16) == LUA_TNUMBER)
    {
        point4.x = (float)lua_tonumber(L, 14);
        point4.y = (float)lua_tonumber(L, 15);
        point4.z = (float)lua_tonumber(L, 16);
    }

    if (lua_type(L, 17) == LUA_TNUMBER &&
        lua_type(L, 18) == LUA_TNUMBER &&
        lua_type(L, 19) == LUA_TNUMBER)
    {
        point5.x = (float)lua_tonumber(L, 17);
        point5.y = (float)lua_tonumber(L, 18);
        point5.z = (float)lua_tonumber(L, 19);
    }

    g_pCommandManager->AddCurveCommand(commandName, objectName, time, rotAngle, point1,
                                       point2, point3, point4, point5);
    return 0;
}

int lua_AddFollowCommand(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    std::string subjectName;
    float distance = 0.0f;
    float speed = 0.0f;
    glm::vec3 offset = glm::vec3(0.0f);
    bool infinity = false;

    if (lua_gettop(L) < 3)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TSTRING)
        subjectName = lua_tostring(L, 3);
    else return 3;

    if (lua_type(L, 4) == LUA_TNUMBER)
        distance = (float)lua_tonumber(L, 4);

    if (lua_type(L, 5) == LUA_TNUMBER)
        speed = (float)lua_tonumber(L, 5);

    if (lua_type(L, 6) == LUA_TNUMBER &&
        lua_type(L, 7) == LUA_TNUMBER &&
        lua_type(L, 8) == LUA_TNUMBER)
    {
        offset.x = (float)lua_tonumber(L, 6);
        offset.y = (float)lua_tonumber(L, 7);
        offset.z = (float)lua_tonumber(L, 8);
    }

    if (lua_type(L, 9) == LUA_TBOOLEAN)
        infinity = lua_toboolean(L, 9);
    
    g_pCommandManager->AddFollowCommand(commandName, objectName, subjectName,
                                        distance, speed, offset, infinity);
    return 0;
}

int lua_AddOrientCommand(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    std::string subjectName;
    glm::vec3 endXYZ = glm::vec3(0.0f);
    float timeToOrient = 0.0f;
    bool infinity = false;

    if (lua_gettop(L) < 3)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TSTRING)
    {
        subjectName = lua_tostring(L, 3);

        if (lua_type(L, 4) == LUA_TNUMBER)
            timeToOrient = (float)lua_tonumber(L, 4);

        if (lua_type(L, 5) == LUA_TBOOLEAN)
            infinity = lua_toboolean(L, 5);

        g_pCommandManager->AddOrientCommand(commandName, objectName, subjectName,
                                            timeToOrient, infinity);
        return 0;
    }
    else if (lua_type(L, 3) == LUA_TNUMBER &&
             lua_type(L, 4) == LUA_TNUMBER &&
             lua_type(L, 5) == LUA_TNUMBER)
    {
        endXYZ.x = (float)lua_tonumber(L, 3);
        endXYZ.y = (float)lua_tonumber(L, 4);
        endXYZ.z = (float)lua_tonumber(L, 5);

        if (lua_type(L, 6) == LUA_TNUMBER)
            timeToOrient = (float)lua_tonumber(L, 6);

        if (lua_type(L, 7) == LUA_TBOOLEAN)
            infinity = lua_toboolean(L, 7);

        g_pCommandManager->AddOrientCommand(commandName, objectName, endXYZ,
                                            timeToOrient, infinity);
        return 0;
    }
    else
    {
        return 3;
    }
}

int lua_AddRotateCommand(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    glm::vec3 angleXYZ = glm::vec3(0.0f);
    float timeToOrient = 0.0f;
    bool infinity = false;

    if (lua_gettop(L) < 3)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TNUMBER &&
        lua_type(L, 4) == LUA_TNUMBER &&
        lua_type(L, 5) == LUA_TNUMBER)
    {
        angleXYZ.x = (float)lua_tonumber(L, 3);
        angleXYZ.y = (float)lua_tonumber(L, 4);
        angleXYZ.z = (float)lua_tonumber(L, 5);
    }
    else return 3;

    if (lua_type(L, 6) == LUA_TNUMBER)
        timeToOrient = (float)lua_tonumber(L, 6);

    if (lua_type(L, 7) == LUA_TBOOLEAN)
        infinity = lua_toboolean(L, 7);

    g_pCommandManager->AddRotateCommand(commandName, objectName, angleXYZ, 
                                        timeToOrient, infinity);
    return 0;
}

int lua_AddTriggerCommand(lua_State* L)
{
    std::string commandName;
    std::string triggeredCmd;
    std::string triggerObj;
    std::string triggeredSbj;

    glm::vec3 triggerXYZ = glm::vec3(0.0f);

    float distance = 0.0f;
    float delayTime = 0.0f;

    if (lua_gettop(L) < 4)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        triggeredCmd = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TSTRING)
    {
        triggerObj = lua_tostring(L, 3);

        if (lua_type(L, 4) == LUA_TSTRING)
            triggeredSbj = lua_tostring(L, 4);
        else return 3;

        if (lua_type(L, 5) == LUA_TNUMBER)
            distance = (float)lua_tonumber(L, 5);
        else return 3;

        if (lua_type(L, 6) == LUA_TNUMBER)
            delayTime = (float)lua_tonumber(L, 6);
        else return 3;

        g_pCommandManager->AddTriggerCommand(commandName, triggeredCmd, 
                        triggerObj, triggeredSbj, distance, delayTime);

        return 0;
    } 
    else if (lua_type(L, 3) == LUA_TNUMBER &&
             lua_type(L, 4) == LUA_TNUMBER &&
             lua_type(L, 5) == LUA_TNUMBER)
    {
        triggerXYZ.x = (float)lua_tonumber(L, 3);
        triggerXYZ.y = (float)lua_tonumber(L, 4);
        triggerXYZ.z = (float)lua_tonumber(L, 5);

        if (lua_type(L, 6) == LUA_TSTRING)
            triggeredSbj = lua_tostring(L, 6);
        else return 3;

        if (lua_type(L, 7) == LUA_TNUMBER)
            distance = (float)lua_tonumber(L, 7);
        else return 3;

        if (lua_type(L, 8) == LUA_TNUMBER)
            delayTime = (float)lua_tonumber(L, 8);
        else return 3;

        g_pCommandManager->AddTriggerCommand(commandName, triggeredCmd,
                        triggerXYZ, triggeredSbj, distance, delayTime);

        return 0;
    }
    else
    {
        return 3;
    }
}


int lua_RunCommand(lua_State* L)
{
    std::string commandName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    g_pCommandManager->RunCommand(commandName);

    return 0;
}

int lua_StopCommand(lua_State* L)
{
    std::string commandName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    g_pCommandManager->StopCommand(commandName);

    return 0;
}

int lua_PauseCommand(lua_State* L)
{
    std::string commandName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    g_pCommandManager->PauseCommand(commandName);

    return 0;
}

int lua_DeleteCommand(lua_State* L)
{
    std::string commandName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    g_pCommandManager->DeleteCommand(commandName);

    return 0;
}


int lua_RunAllCommands(lua_State* L)
{
    g_pCommandManager->RunAllCommands();

    return 0;
}

int lua_StopAllCommands(lua_State* L)
{  
    g_pCommandManager->StopAllCommands();

    return 0;
}

int lua_PauseAllCommands(lua_State* L)
{
    g_pCommandManager->PauseAllCommands();

    return 0;
}

int lua_DeleteAllCommands(lua_State* L)
{
    g_pCommandManager->DeleteAllCommands();

    return 0;
}


int lua_CreateSerialGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->CreateSerialGroup(groupName);

    return 0;
}
 
int lua_CreateParallelGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->CreateParallelGroup(groupName);

    return 0;
}
 
int lua_CreateInfinityGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->CreateInfinityGroup(groupName);

    return 0;
}


int lua_AddGroupTrigger(lua_State* L)
{
    std::string triggerName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        triggerName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->AddGroupTrigger(triggerName);

    return 0;
}

int lua_AddCommandToGroup(lua_State* L)
{
    std::string groupName;
    std::string commandName;

    if (lua_gettop(L) < 2)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        commandName = lua_tostring(L, 2);
    else return 3;

    g_pGroupManager->AddCommandToGroup(groupName, commandName);

    return 0;
}

int lua_DeleteCommandFromGroup(lua_State* L)
{
    std::string groupName;
    std::string commandName;

    if (lua_gettop(L) < 2)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        commandName = lua_tostring(L, 2);
    else return 3;

    g_pGroupManager->DeleteCommandFromGroup(groupName, commandName);

    return 0;
}


int lua_RunGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->RunGroup(groupName);

    return 0;
}

int lua_StopGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->StopGroup(groupName);

    return 0;
}

int lua_PauseGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->PauseGroup(groupName);

    return 0;
}

int lua_DeleteGroup(lua_State* L)
{
    std::string groupName;

    if (lua_gettop(L) < 1)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        groupName = lua_tostring(L, 1);
    else return 3;

    g_pGroupManager->DeleteGroup(groupName);

    return 0;
}


int lua_RunAllGroups(lua_State* L)
{
    g_pGroupManager->RunAllGroups();

    return 0;
}

int lua_StopAllGroups(lua_State* L)
{
    g_pGroupManager->StopAllGroups();

    return 0;
}

int lua_PauseAllGroups(lua_State* L)
{
    g_pGroupManager->PauseAllGroups();

    return 0;
}

int lua_DeleteAllGroups(lua_State* L)
{
    g_pGroupManager->DeleteAllGroups();

    return 0;
}


int lua_SetObjectColor(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    bool isColor = false;  
    glm::vec4 color = glm::vec4(1.0f);

    if (lua_gettop(L) < 7)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TBOOLEAN)
        isColor = lua_toboolean(L, 3);
    else return 3;

    if (lua_type(L, 4) == LUA_TNUMBER &&
        lua_type(L, 5) == LUA_TNUMBER &&
        lua_type(L, 6) == LUA_TNUMBER &&
        lua_type(L, 7) == LUA_TNUMBER)
    {
        color.r = (float)lua_tonumber(L, 4);
        color.g = (float)lua_tonumber(L, 5);
        color.b = (float)lua_tonumber(L, 6);
        color.a = (float)lua_tonumber(L, 7);
    }
    else
    {
        return 3;
    }

    g_pCommandManager->SetObjectColor(commandName, objectName, isColor, color);

    return 0;
}

int lua_SetVisibility(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    bool isVisible = false;
   
    if (lua_gettop(L) < 3)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TBOOLEAN)
        isVisible = lua_toboolean(L, 3);
    else return 3;

    g_pCommandManager->SetVisibility(commandName, objectName, isVisible);

    return 0;
}

int lua_GetObjectCopy(lua_State* L)
{
    std::string commandName;
    std::string objectName;
    std::string friendlyName;

    glm::vec3 posXYZ = glm::vec3(0.0f);

    if (lua_gettop(L) < 6)
    {
        return 3;
    }

    if (lua_type(L, 1) == LUA_TSTRING)
        commandName = lua_tostring(L, 1);
    else return 3;

    if (lua_type(L, 2) == LUA_TSTRING)
        objectName = lua_tostring(L, 2);
    else return 3;

    if (lua_type(L, 3) == LUA_TSTRING)
        friendlyName = lua_tostring(L, 3);
    else return 3;

    if (lua_type(L, 4) == LUA_TNUMBER &&
        lua_type(L, 5) == LUA_TNUMBER &&
        lua_type(L, 6) == LUA_TNUMBER)
    {
        posXYZ.x = (float)lua_tonumber(L, 4);
        posXYZ.y = (float)lua_tonumber(L, 5);
        posXYZ.z = (float)lua_tonumber(L, 6);
    }
    else
    {
        return 3;
    }

    g_pCommandManager->GetObjectCopy(commandName, objectName, friendlyName, posXYZ);

    return 0;
}
