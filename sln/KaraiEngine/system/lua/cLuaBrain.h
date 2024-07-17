#pragma once

extern "C" 
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <string>
#include <map>

class cLuaBrain
{
public:
	cLuaBrain();
	~cLuaBrain();

	void Update();

	void EnterScript();

	void LoadScript(std::string scriptName, std::string scriptSource);

	void DeleteScript(std::string scriptName);

	void RunScript(std::string script);

private:
	std::map< std::string /*scriptName*/, 
			  std::string /*scriptSource*/ > m_mapScripts;

	lua_State* m_pLuaState;

	int m_currentID;

	void m_initCommandScripts();
	void m_initSoundScripts();

	std::string m_decodeLuaErrorToString(int error);
};

