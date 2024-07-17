#include "cTimer.h"

cTimer::cTimer(const std::string& operation)
{
	this->m_operation = operation;
	m_start = std::chrono::high_resolution_clock::now();
}

cTimer::~cTimer()
{
	m_end = std::chrono::high_resolution_clock::now();
	m_duration = std::chrono::duration_cast<std::chrono::microseconds> (m_end - m_start).count();
	std::cout << m_operation << " took " << m_duration << " microseconds. " << std::endl;
}
