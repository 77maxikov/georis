#ifndef _UID_H
#define _UID_H

#if defined(__LP64__)
	#include <stdint.h>
	typedef uint64_t UID;
#else
	typedef unsigned UID;
#endif

class UIDGen{
	static UID _cur;
	static UIDGen* _instance;
    UIDGen(){}
public:
	static UIDGen* instance(){
        if ( _instance == nullptr ){
			_instance = new UIDGen;
			init(0);
		}
		return _instance;
	}
	static void init(unsigned st){ _cur = st;}
	UID generate(){
		return ++_cur;
	}
};
#define NOUID UID(0)

#endif // _UID_H
