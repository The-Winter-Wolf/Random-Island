#include "cConsoleLogs.h"

#include "../../global/global.h"

#include <iostream>

#include <Windows.h>
#define WIN32_LEAN_AND_MEAN 

cConsoleLogs* cConsoleLogs::m_pConsole = NULL;

bool cConsoleLogs::bConsoleMessage = g_bConsoleMessage;
bool cConsoleLogs::bConsoleSuccess = g_bConsoleSuccess;
bool cConsoleLogs::bConsoleWarning = g_bConsoleWarning;
bool cConsoleLogs::bConsoleOutput = g_bConsoleOutput;

// BLACK = 0;
// BLUE = 1;
// GREEN = 2;
// CYAN = 3;
// RED = 4;
// MAGENTA = 5;
// BROWN = 6;
// WHITE = 7;
// GRAY = 8;
// INTENSE_BLUE = 9;
// INTENSE_GREEN = 10;
// INTENSE_CYAN = 11;
// INTENSE_RED = 12;
// INTENSE_MAGENTA = 13;
// YELLOW = 14;
// INTENSE_WHITE = 15;

unsigned int cConsoleLogs::ERROR_FONT_COLOR = 12;
unsigned int cConsoleLogs::WARNING_FONT_COLOR = 14;
unsigned int cConsoleLogs::MESSAGE_FONT_COLOR = 11;
unsigned int cConsoleLogs::SUCCESS_FONT_COLOR = 10;

cConsoleLogs* cConsoleLogs::GetConsoleLogs()
{
	if (cConsoleLogs::m_pConsole == NULL)
	{
		cConsoleLogs::m_pConsole = new cConsoleLogs();
	}
	return cConsoleLogs::m_pConsole;
}

cConsoleLogs::cConsoleLogs()
{
	this->m_timer = 0.0f;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	_CONSOLE_SCREEN_BUFFER_INFO originalInfo;
	GetConsoleScreenBufferInfo(hConsole, &originalInfo);
	wOriginalAttr = originalInfo.wAttributes;
}

cConsoleLogs::~cConsoleLogs()
{
	m_SetColorOriginal();
}

void (cConsoleLogs::cError)(std::string message, const char* fileName, const std::size_t lineNumber)
{
	this->m_SetColor(ERROR_FONT_COLOR);
	std::cout << "<ERROR> ";

	this->m_SetColor(8);
	std::cout << fileName << " (" << lineNumber << "): ";

	this->m_SetColorOriginal();
	std::cout << message << std::endl;

	return;
}

void (cConsoleLogs::cWarning)(std::string message, const char* fileName, const std::size_t lineNumber)
{
	if (!bConsoleWarning) return;

	this->m_SetColor(WARNING_FONT_COLOR);
	std::cout << "<WARNING> ";

	this->m_SetColor(8);
	std::cout << fileName << " (" << lineNumber << "): ";

	this->m_SetColorOriginal();
	std::cout << message << std::endl;

	return;
}

void (cConsoleLogs::cMessage)(std::string message, const char* fileName, const std::size_t lineNumber)
{
	if (!bConsoleMessage) return;

	this->m_SetColor(MESSAGE_FONT_COLOR);
	std::cout << "<MESSAGE> ";
	
	this->m_SetColor(8);
	std::cout << fileName << " (" << lineNumber << "): ";

	this->m_SetColorOriginal();
	std::cout << message << std::endl;

	return;
}

void (cConsoleLogs::cSuccess)(std::string message, const char* fileName, const std::size_t lineNumber)
{
	if (!bConsoleSuccess) return;

	this->m_SetColor(SUCCESS_FONT_COLOR);
	std::cout << "<SUCCESS> ";
	
	this->m_SetColor(8);
	std::cout << fileName << " (" << lineNumber << "): ";

	this->m_SetColorOriginal();
	std::cout << message << std::endl;

	return;
}

void cConsoleLogs::cOutput(std::string message)
{
	if (!bConsoleOutput) return;

	this->m_SetColorOriginal();
	std::cout << message << std::endl;

	return;
}

void cConsoleLogs::m_SetColorOriginal()
{
	SetConsoleTextAttribute(hConsole, wOriginalAttr);
}

void cConsoleLogs::m_SetColor(int attribute)
{
	SetConsoleTextAttribute(hConsole, attribute);
}
