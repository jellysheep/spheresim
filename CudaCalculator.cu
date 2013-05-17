#ifdef ENGINE_CUDA

#include "cuCudaCalculator.h"

#include <iostream>
using namespace std;

#include "NanosecondTimer.h"

template <int dims, bool _3D_>
cuCudaCalculator<dims,_3D_>::cu_cuCudaCalculator(){
	printf("CudaCalculator initialized!\n");
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_initSphere(int i){
}


template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_save(){
	//not yet implemented
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_calcWallResistance(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////
template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_doStep(){
}
/////////////////////////////////////////////////////////////////////////////////////////////////

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateG(){
	//nothing to do
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateAirResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateWallResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateEModul(){
	//_E = curUnit.size*curUnit.size*E*1000000.0;
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updatePoisson(){
	//nothing to do
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateElasticity(){
	//nothing to do
}

template <int dims, bool _3D_>
bool cuCudaCalculator<dims,_3D_>::cu_saveFrame(){
	return true;
}

template <int dims, bool _3D_>
bool cuCudaCalculator<dims,_3D_>::cu_isFixed(int i){
	return false;
}

#define k_b 1.3806488E-23
template <int dims, bool _3D_>
scalar cuCudaCalculator<dims,_3D_>::cu_getTemperature(){
	return 0;
}

template <int dims, bool _3D_>
Sphere* cuCudaCalculator<dims,_3D_>::cu_getDirectSphere(int i){
	return NULL;
}

template <int dims, bool _3D_>
Sphere* cuCudaCalculator<dims,_3D_>::cu_getSphere(int i){
	return NULL;
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_fpsChanged(scalar timeInt){
	timeInterval = timeInt;
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_boxSizeChanged(){
	updateGridSize();
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateGridSize(){
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_gravityChanged(){
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_sphereCountChanged_subclass(int i){
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_maxSphereCountChanged_subclass(int i){
}


template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_loadConfig(const char* file){
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_loadConfig(){
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_paintGL(bool b){
	Calculator::paintGL(b);
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_setGridWith(scalar s){
	gridWidth = s;
}

template <int dims, bool _3D_>
void cuCudaCalculator<dims,_3D_>::cu_updateMagnitude(){
}

template class cuCudaCalculator<2,false>;
template class cuCudaCalculator<3,true>;

#endif
