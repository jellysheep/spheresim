#ifndef _EXPONENTIAL_INPUT_H_
#define _EXPONENTIAL_INPUT_H_

#include <QSlider>
#include "Circles.h"

class QDoubleSpinBox;
class ExponentialSlider;

class ExponentialInput : public QWidget {

	Q_OBJECT // must include this if you use Qt signals/slots	 

protected:
	scalar a,b,c;
	QDoubleSpinBox* sb;
	ExponentialSlider* es;

public:
	ExponentialInput(scalar y0, scalar y1, scalar y2);
	
};


#endif  /* _EXPONENTIAL_INPUT_H_ */
