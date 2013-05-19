#ifdef ENGINE_READ

#ifndef _FILE_CALCULATOR_H_
#define _FILE_CALCULATOR_H_

#include <QThread>

#include "Spheres.h"
#include "Calculator.h"

#include <fstream>
#include <sstream>

class FileCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	
	//spheres: current positions of file reading
	//spheres2: current positions to render
	Sphere *spheres, *spheres2;
	vector** renderBuffer;
	
	void doStep();
	bool saveFrame();
	
	void sphereCountChanged_subclass(int i);
	void maxSphereCountChanged_subclass(int i);
	
	scalar interval;
	bool isFixed(int i);
	
public:
	FileCalculator();
	
	Sphere* getSphere(int i);
	Sphere* getSphereWithSpeed(int i);
	Sphere* getDirectSphere(int i);
	void setSphere(int i, Sphere* s);
	
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
	void updateGridSize();
	void loadConfig();
	void updateMagnitude();
};

#endif  /* _FILE_CALCULATOR_H_ */

#endif
