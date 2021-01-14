//#include "stdafx.h"
#include "mooLog.h"

#include <ctime>
#include <stdexcept>

#ifndef NOLOG
#ifndef CONSOLE_DEBUG 
MooLogger* MooLogger::_instance = 0;
MooLogger::MooLogger():std::fstream()
{
	char buf[80];
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buf,sizeof(buf),"log%y%m%d%H%M%S.log",timeinfo);

	open(buf,ios_base::out|ios_base::trunc);
	if (!is_open()) throw std::runtime_error("Can't start log");
}
#endif
#endif
