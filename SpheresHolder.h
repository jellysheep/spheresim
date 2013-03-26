#ifndef _CIRCLESHOLDER_H_
#define _CIRCLESHOLDER_H_

#include "Circles.h"

class CirclesHolder {

	//Q_OBJECT // must include this if you use Qt signals/slots
	
protected:
	Circle* circles;
	
public:
	CirclesHolder(){
		
	}

};

#endif  /* _CIRCLESHOLDER_H_ */

