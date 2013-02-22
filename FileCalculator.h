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
	
	Circle** circlesBuffer;
	
	void doStep();
	void saveFrame();
	
	void circleCountChanged_subclass(int i);
	void maxCircleCountChanged_subclass(int i);
	
	std::fstream f;
	std::istringstream iss;
	std::string line;
	double readHexDouble();
	double readDouble();
	void saveInVar(scalar &s);
	void saveInVar(int &i);
	void readLine();
	bool hexadec;
	
public:
	FileCalculator();
	
	Circle* getCircle(int i);
	
	void fpsChanged(scalar timeInterval);
	
	void save();
	
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
};

#endif  /* _FILE_CALCULATOR_H_ */

#endif
