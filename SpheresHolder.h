#ifndef _SPHERESHOLDER_H_
#define _SPHERESHOLDER_H_

#include "Spheres.h"

class SpheresHolder {

	//Q_OBJECT // must include this if you use Qt signals/slots
	
protected:
	Sphere* spheres;
	
public:
	SpheresHolder(){
		
	}

};

#endif  /* _SPHERESHOLDER_H_ */

