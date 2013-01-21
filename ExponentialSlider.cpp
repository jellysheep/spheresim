
#include "ExponentialSlider.h"
#include <cmath>
#include <QDoubleSpinBox>

ExponentialSlider::ExponentialSlider(scalar y0, scalar y1, scalar y2){
	a = std::pow(y1-y0, 2)/(y0+y2-2*y1);
	b = (y2-y1)/(y1-y0);
	c = y0-a;
}
