#ifndef _MOOLOG_H
#define _MOOLOG_H

//#define NOLOG
#define CONSOLE_DEBUG

#ifdef NOLOG
#include <iostream>
class nullog{
public:
    template<typename T>  nullog& operator << (const T &v){return *this;}
    // function that takes a custom stream, and returns it
    typedef std::ostream & (*MyStreamManipulator)(std::ostream&);
    // take in a function with the custom signature
    nullog& operator<<(MyStreamManipulator manip) {return *this;}
};
#define MOOLOG nullog()
#else
#ifdef CONSOLE_DEBUG
#include <iostream>
#define MOOLOG std::cout
#else
#include <fstream>
class MooLogger:public std::fstream
{
    static MooLogger *_instance;
    MooLogger();
public:
    static MooLogger& instance(){
        if (!_instance)
            _instance = new MooLogger;
        return *_instance;
    }
};
#define MOOLOG MooLogger::instance()
#endif
#endif





#endif // MOOLOG_H
