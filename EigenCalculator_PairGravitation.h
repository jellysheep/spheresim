
#ifndef _EIGEN_CALCULATOR_PAIR_GRAVITATION_H_
#define _EIGEN_CALCULATOR_PAIR_GRAVITATION_H_

#include "EigenCalculator_Force.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_PairGravitation : public EigenCalculator_Force<dims,_3D_>{
protected:
	void calcGravitation(int i, int j);
	
public:
	EigenCalculator_PairGravitation(EigenCalculator_Engine<dims,_3D_>* c):EigenCalculator_Force<dims,_3D_>(c){
	}
	
	virtual void calcForces();
};

#endif  /*_EIGEN_CALCULATOR_PAIR_GRAVITATION_H_*/

