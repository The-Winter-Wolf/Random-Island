#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <string>

class cConsoleLogs
{
public:
	~cConsoleLogs();

	static cConsoleLogs* GetConsoleLogs();

	void (cError)(const std::string message, const char* fileName, const std::size_t lineNumber);
	void (cWarning)(const std::string message, const char* fileName, const std::size_t lineNumber);
	void (cMessage)(const std::string message, const char* fileName, const std::size_t lineNumber);
	void (cSuccess)(const std::string message, const char* fileName, const std::size_t lineNumber);
	void cOutput(const std::string message);

private:
	cConsoleLogs();
	static cConsoleLogs* m_pConsole;

	HANDLE hConsole;
	WORD  wOriginalAttr;

	float m_timer;

	static bool bConsoleWarning;
	static bool bConsoleMessage;
	static bool bConsoleSuccess;
	static bool bConsoleOutput;

	static unsigned int ERROR_FONT_COLOR;
	static unsigned int WARNING_FONT_COLOR;
	static unsigned int MESSAGE_FONT_COLOR;
	static unsigned int SUCCESS_FONT_COLOR;
	static unsigned int OUTPUT_FONT_COLOR;

	void m_SetColorOriginal();
	void m_SetColor(int attribute);
};

#define LOG_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define cError(message) cError(message, LOG_FILENAME, __LINE__)
#define cWarning(message) cWarning(message, LOG_FILENAME, __LINE__)
#define cMessage(message) cMessage(message, LOG_FILENAME, __LINE__)
#define cSuccess(message) cSuccess(message, LOG_FILENAME, __LINE__)