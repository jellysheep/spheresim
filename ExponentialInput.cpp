
#include "ExponentialInput.h"
#include "ExponentialSlider.h"
#include <cmath>
#include <QDoubleSpinBox>

ExponentialInput::ExponentialInput(scalar y0, scalar y1, scalar y2):es(new ExponentialSlider(y0,y1,y2)){
	
}
