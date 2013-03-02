#ifdef ENGINE_READ

#ifndef _FILE_CALCULATOR_H_
#define _FILE_CALCULATOR_H_

#include <QThread>

#include "Circles.h"
#include "Calculator.h"

#include <fstream>
#include <sstream>

class FileCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	
	//circles: current positions of file reading
	//circles2: current positions to render
	Circle *circles, *circles2;
	vector** renderBuffer;
	
	void doStep();
	bool saveFrame();
	
	void circleCountChanged_subclass(int i);
	void maxCircleCountChanged_subclass(int i);
	
	scalar interval;
	
public:
	FileCalculator();
	
	Circle* getCircle(int i);
	Circle* getDirectCircle(int i);
	
	void fpsChanged(scalar timeInterval);
	
	void save();
	
	scalar getTemperature(){
		return 0;
	}
	
public slots:
	void boxSizeChanged();
	void gravityChanged();
	void updateG();
	void updateAirResistance();
	void updateWallResistance();
	void updateEModul();
	void updatePoisson();
	void updateElasticity();
	void updateSphereSize();
	void loadConfig();
};

#endif  /* _FILE_CALCULATOR_H_ */

#endif
