#ifndef _EIGEN_CALCULATOR_H_
#define _EIGEN_CALCULATOR_H_

#define NDEBUG
#include <Eigen/Dense>
using namespace Eigen;

#include <QThread>

#include "Circles.h"
#include "Calculator.h"

#define useSSE 1

#if useSSE
	#if _3D_
		//Vector4d and Vector4f are vectorized
		//typedef Eigen::Matrix<scalar, 4, 1> eVector;
		#if _double_
			typedef Vector4d eVector;
		#else
			typedef Vector4f eVector;
		#endif
	#else
		#if _double_
			//Vector2d is vectorized
			typedef Vector2d eVector;
			//typedef Eigen::Matrix<scalar, 2, 1> eVector;
		#else
			//Vector2f is not vectorized, but Vector4f is
			typedef Vector4f eVector;
			//typedef Eigen::Matrix<scalar, 4, 1> eVector;
		#endif
	#endif
#else
	#if _3D_
		//typedef Eigen::Matrix<scalar, 3, 1> eVector;
		#if _double_
			typedef Vector3d eVector;
		#else
			typedef Vector3f eVector;
		#endif
	#else
		//typedef Eigen::Matrix<scalar, 2, 1> eVector;
		#if _double_
			typedef Vector2d eVector;
		#else
			typedef Vector2f eVector;
		#endif
	#endif
#endif

class EigenCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	
	void save();
		
    eVector** renderBuffer;
    Circle* circles;
    eVector *circlesOldPos, *circlesPos, *circlesSpeed, *circlesForce;
    
    void doStep();
    void saveFrame();
    
    Circle c;
    
    scalar timeInterval;

public:
	EigenCalculator();
	
	Circle* getCircle(int i);
	
	void fpsChanged(scalar timeInterval);
	
public slots:
	void boxSizeChanged();
	void gravityChanged();
};

extern void start(EigenCalculator* clTimer);

#endif  /* _EIGEN_CALCULATOR_H_ */

