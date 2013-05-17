
#ifndef _EIGEN_CALCULATOR_PAIR_COLLIDER_H_
#define _EIGEN_CALCULATOR_PAIR_COLLIDER_H_

#include "EigenCalculator_Force.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

#define P EigenCalculator_PairCollider<dims,_3D_>

template <int dims, bool _3D_>
class EigenCalculator_PairCollider : public virtual EigenCalculator_Force<dims,_3D_>{
protected:
	scalar both_r_, _E, d, d_, R, _E_;
	eVector d_pos, d_n, force;
	inline void collideBalls(int i, int j){
		if(i == j) return;
		both_r_ = F::both_r[i][j], _E = F::_E;
		d_pos = F::spheresPos[j]-F::spheresPos[i];
		
		//d_pos.s2 = 0;
		d = d_pos.norm();
		
		// Abstossung:
		if (d < both_r_) {
			
			d = std::max(d,(scalar)0.00005f);
			
			//printf("Spheres %d and %d are colliding!\n", i, j);
			d_n = d_pos/d; // bzw. normalize(d_pos);
			
			// nach Kontaktmechanik mit Poisson-Zahlen:
			d_ = both_r_ - d;
			//*
			R = 1/((1/F::spheres[i].size)+(1/F::spheres[j].size));
			//_E_ = 1/(((1-spheres[i].poisson*spheres[i].poisson)/(_E))+((1-spheres[j].poisson*spheres[j].poisson)/(_E)));
			_E_ = 1/(((1-(poisson*poisson))/(_E))+((1-(poisson*poisson))/(_E)));
			force = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3)) *d_n;
			//printf("Stoss! r: %5f m: %5f d: %5f R: %5f E*: %5f F: %5f\n", spheres[i].size, spheres[i].mass, d_, R, _E_, force.norm());
			if(elastic!=1 && d_pos.dot((F::spheresSpeed[i]-F::spheresSpeed[j])/d)<0){ //Skalarprodukt
				force *= elastic;
			}
			#pragma atomic
				F::spheresForce[i] -= force;
			#pragma atomic
				F::spheresForce[j] += force;
		}
	}
	
public:
	EigenCalculator_PairCollider(EigenCalculator_Engine<dims,_3D_>* c):EigenCalculator_Force<dims,_3D_>(c){
	}
	
	virtual void calcForces();
	
	virtual void spheresCountChanged(int c);
};

#endif  /*_EIGEN_CALCULATOR_PAIR_COLLIDER_H_*/

