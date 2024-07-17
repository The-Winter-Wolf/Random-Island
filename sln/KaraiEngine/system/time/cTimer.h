#pragma once

#include <chrono>
#include <iostream>
#include <string>

class cTimer
{
public:
	cTimer(const std::string& operation);
	~cTimer();

private:
	std::string m_operation;
	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_end;
	long long m_duration;
};

