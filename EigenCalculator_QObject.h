#ifdef ENGINE_CPP

#ifndef _EIGEN_CALCULATOR_QOBJECT_H_
#define _EIGEN_CALCULATOR_QOBJECT_H_

#include "Spheres.h"
#include "Calculator.h"

class EigenCalculator_QObject: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	
	virtual void save()=0;
		
	virtual void doStep()=0;
	virtual bool saveFrame()=0;
	
	virtual void sphereCountChanged_subclass(int i)=0;
	virtual void maxSphereCountChanged_subclass(int i)=0;
	
	virtual void loadConfig(const char* file)=0;
	
	virtual bool isFixed(int i)=0;

public:
	EigenCalculator_QObject():Calculator(){}
	
	virtual Sphere* getSphere(int i)=0;
	virtual Sphere* getDirectSphere(int i)=0;
	
	virtual void fpsChanged(scalar timeInterval)=0;
	
	virtual scalar getTemperature()=0;
	
	virtual void paintGL(bool b)=0;
	
public slots:
	virtual void boxSizeChanged()=0;
	virtual void gravityChanged()=0;
	virtual void updateG()=0;
	virtual void updateAirResistance()=0;
	virtual void updateWallResistance()=0;
	virtual void updateEModul()=0;
	virtual void updatePoisson()=0;
	virtual void updateElasticity()=0;
	virtual void updateGridSize()=0;
	
	virtual void loadConfig()=0;
};

#endif  /* _EIGEN_CALCULATOR_QOBJECT_H_ */

#endif
