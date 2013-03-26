#ifndef _EXPONENTIAL_SLIDER_H_
#define _EXPONENTIAL_SLIDER_H_

#include <QSlider>
#include "Spheres.h"

class QDoubleSpinBox;

class ExponentialSlider : public QSlider {

	Q_OBJECT // must include this if you use Qt signals/slots	 

protected:
	scalar a,b,c;

public:
	ExponentialSlider(scalar y0, scalar y1, scalar y2);
	
};


#endif  /* _EXPONENTIAL_SLIDER_H_ */
