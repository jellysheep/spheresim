#ifdef ENGINE_CUDA

#ifndef _CUDA_CALCULATOR_H_
#define _CUDA_CALCULATOR_H_

#include "EigenCalculator_defines.h"

#include <QThread>

#include "Spheres.h"
#include "Calculator.h"
#include "CudaCalculator.cuh"

template <int dims, bool _3D_>
class CudaCalculator: public Calculator, protected cuCudaCalculator {

protected:

	void doStep(){
		cu_doStep();
	}
	bool saveFrame(){
		return cu_saveFrame();
	}
	
	void save(){
		cu_save();
	}
	
	bool isFixed(int i){
		return cu_isFixed(i);
	}
	
	void loadConfig(const char* file){
		cu_loadConfig(file);
	}
	
	void sphereCountChanged_subclass(int i){
		cu_sphereCountChanged_subclass(i);
	}
	void maxSphereCountChanged_subclass(int i){
		cu_maxSphereCountChanged_subclass(i);
	}
	
public:
	CudaCalculator():cuCudaCalculator{
	}
	
	void paintGL(bool b){
		cu_paintGL(b);
	}
	
	void fpsChanged(scalar timeInterval){
		cu_fpsChanged(timeInterval);
	}
	
	Sphere* getSphere(int i){
		return cu_getSphere(i);
	}
	Sphere* getDirectSphere(int i){
		return cu_getDirectSphere(i);
	}
	
	scalar getTemperature(){
		return cu_getTemperature();
	}
	
	void boxSizeChanged(){
		cu_boxSizeChanged();
	}
	void gravityChanged(){
		cu_gravityChanged();
	}
	void updateG(){
		cu_updateG();
	}
	void updateAirResistance(){
		cu_updateAirResistance();
	}
	void updateWallResistance(){
		cu_updateWallResistance();
	}
	void updateEModul(){
		cu_updateEModul();
	}
	void updatePoisson(){
		cu_updatePoisson();
	}
	void updateElasticity(){
		cu_updateElasticity();
	}
	void updateGridSize(){
		cu_updateGridSize();
	}
	
	void loadConfig(){
		cu_loadConfig();
	}
	
	void setGridWith(scalar s){
		cu_setGridWith(s);
	}
	
	void updateMagnitude(){
		cu_updateMagnitude();
	}
};

#endif  /* _CUDA_CALCULATOR_H_ */

#endif
