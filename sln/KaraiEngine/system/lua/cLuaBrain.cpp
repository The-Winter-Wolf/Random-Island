#include "cLuaBrain.h"

#include <iostream>

//void LuaSceneScripts();

// Function in the luaCommands
int lua_AddMoveCommand(lua_State* L);
int lua_AddCurveCommand(lua_State* L);
int lua_AddFollowCommand(lua_State* L);
int lua_AddOrientCommand(lua_State* L);
int lua_AddRotateCommand(lua_State* L);
int lua_AddTriggerCommand(lua_State* L);

int lua_RunCommand(lua_State* L);
int lua_StopCommand(lua_State* L);
int lua_PauseCommand(lua_State* L);
int lua_DeleteCommand(lua_State* L);

int lua_RunAllCommands(lua_State* L);
int lua_StopAllCommands(lua_State* L);
int lua_PauseAllCommands(lua_State* L);
int lua_DeleteAllCommands(lua_State* L);

int lua_CreateSerialGroup(lua_State* L);
int lua_CreateParallelGroup(lua_State* L);
int lua_CreateInfinityGroup(lua_State* L);

int lua_AddGroupTrigger(lua_State* L);
int lua_AddCommandToGroup(lua_State* L);
int lua_DeleteCommandFromGroup(lua_State* L);

int lua_RunGroup(lua_State* L);
int lua_StopGroup(lua_State* L);
int lua_PauseGroup(lua_State* L);
int lua_DeleteGroup(lua_State* L);

int lua_RunAllGroups(lua_State* L);
int lua_StopAllGroups(lua_State* L);
int lua_PauseAllGroups(lua_State* L);
int lua_DeleteAllGroups(lua_State* L);

int lua_SetObjectColor(lua_State* L);
int lua_SetVisibility(lua_State* L);
int lua_GetObjectCopy(lua_State* L);

// Function in the luaSounds
int lua_TurnOnSound(lua_State* L);
int lua_TurnOffSound(lua_State* L);
int lua_CheckLocalEvent(lua_State* L);


cLuaBrain::cLuaBrain()
{
	int m_currentID = 0;

	this->m_pLuaState = luaL_newstate();

	if (this->m_pLuaState == NULL)
	{
		return;
	}
	luaL_openlibs(this->m_pLuaState);

	// Scripts for commands
	this->m_initCommandScripts();

	// Scripts for sounds
	this->m_initSoundScripts();

	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}

// Saves (and overwrites) any script
void cLuaBrain::LoadScript( std::string scriptName, std::string scriptSource )
{
	this->m_mapScripts[scriptName] = scriptSource;
	std::cout << "Script loaded successfully" << std::endl;
	return;
}

void cLuaBrain::DeleteScript( std::string scriptName )
{
	this->m_mapScripts.erase(scriptName);
	std::cout << "Script deleted successfully" << std::endl;
	return;
}

void cLuaBrain::EnterScript()
{
	std::string scriptName;
	std::string scriptSource;
	while (true)
	{
		//std::cout << "\nEnter the NAME of script to load it into lua." << std::endl;
		//std::cout << "Enter [Exit] to exit." << std::endl;

		//std::getline(std::cin >> std::ws, scriptName);
		//if (scriptName == "Exit")
		//{
		//	break;
		//}

		std::cout << "\nEnter the SCRIPT to load it into lua." << std::endl;
		std::cout << "Enter [Exit] to exit." << std::endl;

		std::getline(std::cin >> std::ws, scriptSource);
		if (scriptSource == "Exit")
		{
			break;
		}

		this->LoadScript(std::to_string(m_currentID), scriptSource);
		m_currentID++;
	}
}

// Call all the active scripts that are loaded
void cLuaBrain::Update()
{
	for( std::map< std::string /*name*/, std::string /*source*/>::iterator itScript = 
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++ )
	{
		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring( this->m_pLuaState, curLuaScript.c_str() );

		if ( error != 0 /*no error*/)	
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
						  0,	/* nargs: number of arguments pushed onto the lua stack */
						  0,	/* nresults: number of results that should be on stack at end*/
						  0);	/* errfunc: location, in stack, of error function. 
								   if 0, results are on top of stack. */
		if ( error != 0 /*no error*/)	
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append( lua_tostring(this->m_pLuaState, -1) );

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}

	return;
}

// Runs a script, but doesn't save it
void cLuaBrain::RunScript(std::string script)
{
	int error = luaL_loadstring( this->m_pLuaState, script.c_str() );

	if ( error != 0 /*no error*/)	
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
						0,	/* nargs: number of arguments pushed onto the lua stack */
						0,	/* nresults: number of results that should be on stack at end*/
						0);	/* errfunc: location, in stack, of error function. 
								if 0, results are on top of stack. */
	if ( error != 0 /*no error*/)	
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append( lua_tostring(this->m_pLuaState, -1) );

		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

void cLuaBrain::m_initCommandScripts()
{
	// Scripts for commands
	lua_pushcfunction(this->m_pLuaState, lua_AddMoveCommand);
	lua_setglobal(this->m_pLuaState, "AddMoveCommand");

	lua_pushcfunction(this->m_pLuaState, lua_AddCurveCommand);
	lua_setglobal(this->m_pLuaState, "AddCurveCommand");

	lua_pushcfunction(this->m_pLuaState, lua_AddFollowCommand);
	lua_setglobal(this->m_pLuaState, "AddFollowCommand");

	lua_pushcfunction(this->m_pLuaState, lua_AddOrientCommand);
	lua_setglobal(this->m_pLuaState, "AddOrientCommand");

	lua_pushcfunction(this->m_pLuaState, lua_AddRotateCommand);
	lua_setglobal(this->m_pLuaState, "AddRotateCommand");

	lua_pushcfunction(this->m_pLuaState, lua_AddTriggerCommand);
	lua_setglobal(this->m_pLuaState, "AddTriggerCommand");


	lua_pushcfunction(this->m_pLuaState, lua_RunCommand);
	lua_setglobal(this->m_pLuaState, "RunCommand");

	lua_pushcfunction(this->m_pLuaState, lua_StopCommand);
	lua_setglobal(this->m_pLuaState, "StopCommand");

	lua_pushcfunction(this->m_pLuaState, lua_PauseCommand);
	lua_setglobal(this->m_pLuaState, "PauseCommand");

	lua_pushcfunction(this->m_pLuaState, lua_DeleteCommand);
	lua_setglobal(this->m_pLuaState, "DeleteCommand");


	lua_pushcfunction(this->m_pLuaState, lua_RunAllCommands);
	lua_setglobal(this->m_pLuaState, "RunAllCommands");

	lua_pushcfunction(this->m_pLuaState, lua_StopAllCommands);
	lua_setglobal(this->m_pLuaState, "StopAllCommands");

	lua_pushcfunction(this->m_pLuaState, lua_PauseAllCommands);
	lua_setglobal(this->m_pLuaState, "PauseAllCommands");

	lua_pushcfunction(this->m_pLuaState, lua_DeleteAllCommands);
	lua_setglobal(this->m_pLuaState, "DeleteAllCommands");

	// Scripts for groups
	lua_pushcfunction(this->m_pLuaState, lua_CreateSerialGroup);
	lua_setglobal(this->m_pLuaState, "CreateSerialGroup");

	lua_pushcfunction(this->m_pLuaState, lua_CreateParallelGroup);
	lua_setglobal(this->m_pLuaState, "CreateParallelGroup");

	lua_pushcfunction(this->m_pLuaState, lua_CreateInfinityGroup);
	lua_setglobal(this->m_pLuaState, "CreateInfinityGroup");

	lua_pushcfunction(this->m_pLuaState, lua_AddGroupTrigger);
	lua_setglobal(this->m_pLuaState, "AddGroupTrigger");

	lua_pushcfunction(this->m_pLuaState, lua_AddCommandToGroup);
	lua_setglobal(this->m_pLuaState, "AddCommandToGroup");

	lua_pushcfunction(this->m_pLuaState, lua_DeleteCommandFromGroup);
	lua_setglobal(this->m_pLuaState, "DeleteCommandFromGroup");


	lua_pushcfunction(this->m_pLuaState, lua_RunGroup);
	lua_setglobal(this->m_pLuaState, "RunGroup");

	lua_pushcfunction(this->m_pLuaState, lua_StopGroup);
	lua_setglobal(this->m_pLuaState, "StopGroup");

	lua_pushcfunction(this->m_pLuaState, lua_PauseGroup);
	lua_setglobal(this->m_pLuaState, "PauseGroup");

	lua_pushcfunction(this->m_pLuaState, lua_DeleteGroup);
	lua_setglobal(this->m_pLuaState, "DeleteGroup");


	lua_pushcfunction(this->m_pLuaState, lua_RunAllGroups);
	lua_setglobal(this->m_pLuaState, "RunAllGroups");

	lua_pushcfunction(this->m_pLuaState, lua_StopAllGroups);
	lua_setglobal(this->m_pLuaState, "StopAllGroups");

	lua_pushcfunction(this->m_pLuaState, lua_PauseAllGroups);
	lua_setglobal(this->m_pLuaState, "PauseAllGroups");

	lua_pushcfunction(this->m_pLuaState, lua_DeleteAllGroups);
	lua_setglobal(this->m_pLuaState, "DeleteAllGroups");

	lua_pushcfunction(this->m_pLuaState, lua_SetObjectColor);
	lua_setglobal(this->m_pLuaState, "SetObjectColor");

	lua_pushcfunction(this->m_pLuaState, lua_SetVisibility);
	lua_setglobal(this->m_pLuaState, "SetVisibility");

	lua_pushcfunction(this->m_pLuaState, lua_GetObjectCopy);
	lua_setglobal(this->m_pLuaState, "GetObjectCopy");
}

void cLuaBrain::m_initSoundScripts()
{
	lua_pushcfunction(this->m_pLuaState, lua_TurnOnSound);
	lua_setglobal(this->m_pLuaState, "TurnOnSound");

	lua_pushcfunction(this->m_pLuaState, lua_TurnOffSound);
	lua_setglobal(this->m_pLuaState, "TurnOffSound");

	lua_pushcfunction(this->m_pLuaState, lua_CheckLocalEvent);
	lua_setglobal(this->m_pLuaState, "CheckLocalEvent");
}

std::string cLuaBrain::m_decodeLuaErrorToString( int error )
{
	switch ( error )
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}

	return "Lua: UNKNOWN error";
}
