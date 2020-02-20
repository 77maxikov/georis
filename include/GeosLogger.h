#ifndef _GEOSLOGGER_H
#define _GEOSLOGGER_H
#ifndef _DEBUG
	#include <fstream>
	class GeosLogger:public std::fstream
{
   static GeosLogger *_instance;
   GeosLogger();
   public:
   static GeosLogger& instance(){
      if (!_instance)
         _instance = new GeosLogger;
      return *_instance;
   };
};
	#define LOG GeosLogger::instance()

#else
	#include <iostream>
	#define LOG std::cout
#endif


#endif // _GELOGGER_H
