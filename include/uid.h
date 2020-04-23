#ifndef _UID_H
#define _UID_H

#if defined(__LP64__)
	#include <stdint.h>
	typedef uint64_t UID;
#else
	typedef unsigned UID;
#endif

#define NOUID UID(0)

class UIDGen{
	static UID _cur;
	static UIDGen* _instance;
    UIDGen(){}
public:
	static UIDGen* instance(){
        if ( _instance == nullptr ){
			_instance = new UIDGen;
            init(NOUID);
		}
		return _instance;
	}
    static void init(UID st){
        _cur = st;
    }

	UID generate(){
		return ++_cur;
	}
};


#endif // _UID_H
