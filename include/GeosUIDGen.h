#ifndef _GEOSUIDGEN_H
#define _GEOSUIDGEN_H
#include "uid.h"
#include <stdexcept>

class GeosUIDGen{
	static GeosUIDGen *_instance;
	static UID _lastUID;
	GeosUIDGen ();
	GeosUIDGen (const GeosUIDGen&);
	GeosUIDGen &operator=(const GeosUIDGen&);
public:

	static GeosUIDGen* instance(){
		if ( _instance == 0 ) {
			init();
			_instance = new GeosUIDGen();

		}
		return _instance;
	}
	static void init(UID starter = UID(0)){
		if ( _instance == 0 )
			_lastUID = starter;
		else throw std::runtime_error("GeosUIDGen::init - already initialized");
	}
	UID generate(){
		return _lastUID++;
	}

};
#endif // GEOSUIDGEN_H
