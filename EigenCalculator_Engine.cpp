#ifdef ENGINE_CPP

#include "EigenCalculator_Engine.h"

#include "EigenCalculator_PairCollider.h"
#include "EigenCalculator_EfficientPairCollider.h"
#include "EigenCalculator_StripeCollider.h"
#include "EigenCalculator_CellSortCollider.h"
#include "EigenCalculator_CellCountCollider.h"

#include "EigenCalculator_PairGravitation.h"
#include "EigenCalculator_CellGravitation.h"

#include <iostream>
using namespace std;


#include <QFileDialog>

#include "NanosecondTimer.h"
#include "GLWidget.h"

#define fixSun 0

template<int dims, bool _3D_>
template<class Force>
void EigenCalculator_Engine<dims,_3D_>::addForce(){
	if(numForces<maxNumForces){
		forces[numForces++] = new Force(this);
	}
}

template<int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::addForce(EigenCalculator_Force<dims,_3D_>* force){
	if(numForces<maxNumForces){
		forces[numForces++] = force;
	}
}

int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}

template <int dims, bool _3D_>
EigenCalculator_Engine<dims,_3D_>::EigenCalculator_Engine():Calculator(){
	//omp_set_num_threads(4);
	srand(2);
	//srand(NanosecondTimer::getNS());
	
	cout<<"Info: Calculating with "<<omp_get_num_threads()<<"|"<<omp_thread_count()<<" threads.\n";
	
	leapfrogFirstStepMade = false;
	
	_E = curUnit.size*curUnit.size*E*1000000.0;
		
	bool eVectorizing = false;
	#ifdef EIGEN_VECTORIZE
		eVectorizing = true;
	#endif
	cout<<"Vectorizing: "<<eVectorizing<<endl;
	
	timeInterval = speed*speedCorrection/fps;
	spheres = new Sphere[spheresCount];
	spheresPos = new eVector[spheresCount];
	spheresOldPos = new eVector[spheresCount];
	spheresSpeed = new eVector[spheresCount];
	spheresForce = new eVector[spheresCount];
	both_r = new scalar*[spheresCount];
	
	maxNumForces = 10;
	forces = new EigenCalculator_Force<dims,_3D_>*[maxNumForces];
	numForces = 0;
	
	//addForce(new EigenCalculator_PairCollider<dims,_3D_>(this));
	//addForce(new EigenCalculator_EfficientPairCollider<dims,_3D_>(this));
	//addForce(new EigenCalculator_StripeCollider<dims,_3D_>(this));
	//addForce(new EigenCalculator_CellSortCollider<dims,_3D_>(this));
	addForce(new EigenCalculator_CellCountCollider<dims,_3D_>(this));
	
	//~ if(magnitude == 1){
		addForce(new EigenCalculator_PairGravitation<dims,_3D_>(this));
	//~ }else{
		//~ addForce(new EigenCalculator_CellGravitation<dims,_3D_>(this));
	//~ }
	
	
	k = new eVector*[spheresCount];
	
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		
		both_r[i] = new scalar[spheresCount];
		
		initSphere(i);
		
		k[i] = new eVector[9];
		
	}
	
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		for(int j = 0; j<spheresCount; j++){
			both_r[i][j] = spheres[i].size + spheres[j].size;
		}
	}
	if(renderBool){
		readNum_render = min(maxShowSpheresCount,spheresCount);
		printf("Spheres to render: %d\n", readNum_render);
		renderBuffer = new eVector*[renderBufferCount];
		for(int i = 0; i<renderBufferCount; i++){
			renderBuffer[i] = new eVector[readNum_render];
		}
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
	}
	
	gridWidth = curUnit.size*boxSize.s0;
	
	printf("EigenCalculator_Engine initialized!\n");
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::initSphere(int i){
	spheres[i].size = rans(curUnit.size*sphereSize.s0, curUnit.size*sphereSize.s1);
	spheres[i].E = E;
	spheres[i].poisson = poisson;
	spheres[i].fixed = 0;
	
	spheresPos[i].setZero();
	//spheresPos[i](0) = spheres[i].size+rans(curUnit.size*boxSize.s0-2*spheres[i].size);
	spheresPos[i](0) = 2*spheres[i].size+(i%(int)(sqrt(spheresCount)+1))*((curUnit.size*boxSize.s0-4*spheres[i].size)/(sqrt(spheresCount)+1))+rans(-spheres[i].size,spheres[i].size);
	spheresPos[i](1) = 2*spheres[i].size+(i/(int)(sqrt(spheresCount)+1))*((curUnit.size*boxSize.s1-4*spheres[i].size)/(sqrt(spheresCount)+1))+rans(-spheres[i].size,spheres[i].size);
	if(_3D_){
		spheresPos[i](2) = spheres[i].size+rans(curUnit.size*boxSize.s2-2*spheres[i].size);
	}
	spheresOldPos[i] = spheresPos[i];
	
	
	spheresSpeed[i] = eVector::Random();
	#if useSSE
		if(_3D_){
			spheresSpeed[i](3) = 0;
		}
		#if _double_==0
			else{
				spheresSpeed[i](2) = 0;
			}
		#endif
	#endif
	spheresSpeed[i].normalize();
	spheresSpeed[i] *= rans(curUnit.size*max_speed);
	//cout<<spheresSpeed[i]<<endl<<endl;
	
	spheresForce[i].setZero();
	
	//cout<<"3D enabled: "<<(_3D_?1:0)<<endl<<endl;
	
	#if fixSun
		if(i == 0) spheres[i].size = 3*curUnit.size*sphereSize.s0;
		//if(i == 0) spheres[i].mass*=5;
		if(i == 0) ceBuffer[i].hsvColor = QColor::fromHsv(0,0,0);
		if(i == 0) spheresPos[i](0) = curUnit.size*boxSize.s0/2;
		if(i == 0) spheresPos[i](1) = curUnit.size*boxSize.s1/2;
		if(_3D_){
			if(i == 0) spheresPos[i](2) = curUnit.size*boxSize.s2/2;
		}
		if(i == 0) spheresSpeed[i] = eVector::Zero();
	#endif
	spheresOldPos[i] = spheresPos[i];
	spheres[i].mass = 4.0/3.0*pow(spheres[i].size,3)*M_PI  *950; //Kautschuk
	//printf("Sphere: radius %10f m;   mass: %10f kg\n", spheres[i].size, spheres[i].mass);
}


template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::save(){
	//not yet implemented
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::calcWallResistance(){
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		if(spheres[i].fixed == 1) continue;
		scalar d_, R, _E_;
		scalar force_, htw_d_d, fact = 1.0f;
		if ((htw_d_d = (spheres[i].size - spheresPos[i](0)))>0) {
			//printf("Sphere %d collides with left wall\n", i);
			if(spheresSpeed[i](0) > 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](0) += force_*fact;
			curWallForces[0] += force_*fact;
		}else if ((htw_d_d = (spheres[i].size + spheresPos[i](0) - curUnit.size*boxSize.s0))>0) {
			//printf("Sphere %d collides with right wall\n", i);
			if(spheresSpeed[i](0) < 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](0) -= force_*fact;
			curWallForces[1] += force_*fact;
		}
		fact = 1.0f;
		if ((htw_d_d = (spheres[i].size - spheresPos[i](1)))>0) {
			//printf("Sphere %d collides with bottom wall\n", i);
			if(spheresSpeed[i](1) > 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](1) += force_*fact;
			curWallForces[2] += force_*fact;
		}else if ((htw_d_d = (spheres[i].size + spheresPos[i](1) - curUnit.size*boxSize.s1))>0) {
			//printf("Sphere %d collides with top wall\n", i);
			if(spheresSpeed[i](1) < 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](1) -= force_*fact;
			curWallForces[3] += force_*fact;
		}
		if(_3D_){
			fact = 1.0f;
			if ((htw_d_d = (spheres[i].size - spheresPos[i](2)))>0) {
				if(spheresSpeed[i](2) > 0)fact = elastic;
				d_ = htw_d_d;
				R = spheres[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				spheresForce[i](2) += force_*fact;
				curWallForces[4] += force_*fact;
			}else if ((htw_d_d = (spheres[i].size + spheresPos[i](2) - curUnit.size*boxSize.s2))>0) {
				if(spheresSpeed[i](2) < 0)fact = elastic;
				d_ = htw_d_d;
				R = spheres[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				spheresForce[i](2) -= force_*fact;
				curWallForces[5] += force_*fact;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#define sqr(x) ((x)*(x))
template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::methodEulerPolygon(){
	
	for(int i = 0; i<numForces; i++){
		forces[i]->calcForces();
	}
	
	if(wallResistance){
		calcWallResistance();
	}
	
	timeInterval = std::min((double)timeInterval, 1.0/minFps);
	
	//sumUpForces():
	//printf("interval: %10f\n", timeInterval);
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		if(spheres[i].fixed == 0){
			eVector force;
			#if fixSun
				if(i==0)continue;
			#endif
			EIGEN_ASM_COMMENT("begin");
			spheresForce[i] -= airResistance*(0.47*M_PI*sqr(spheres[i].size))*0.5*spheresSpeed[i]*std::abs(spheresSpeed[i].norm());
			force = spheresForce[i];
			/*
			printf("sphere %d force: %5f %5f\n", i, spheresForce[i](0), spheresForce[i](1));
			printf("sphere %d speed: %5f %5f\n", i, spheresSpeed[i](0), spheresSpeed[i](1));
			printf("sphere %d pos: %5f %5f\n", i, spheresPos[i](0), spheresPos[i](1));
			printf("sphere %d old pos: %5f %5f\n", i, spheresOldPos[i](0), spheresOldPos[i](1));
			printf("sphere %d mass: %5f\n", i, spheres[i].mass);
			QThread::msleep(10);// */
			eVector acceleration = force / spheres[i].mass;
			acceleration(0) += gravity.s0;
			acceleration(1) += gravity.s1;
			//printf("sphere %d gravity: %5f %5f\n", i, gravity.s0, gravity.s1);
			if(_3D_){
				acceleration(2) += gravity.s2;
			}
			spheresOldPos[i] += spheresSpeed[i]*timeInterval;
			spheresOldPos[i] += 0.5*acceleration*timeInterval*timeInterval;
			
			spheresSpeed[i] += acceleration*timeInterval;
			spheresForce[i] -= force;
			
			#if 1
				if(wallResistance){
					spheresOldPos[i](0) = std::min(spheresOldPos[i](0), curUnit.size*boxSize.s0);
					spheresOldPos[i](1) = std::min(spheresOldPos[i](1), curUnit.size*boxSize.s1);
					if(_3D_) spheresOldPos[i](2) = std::min(spheresOldPos[i](2), curUnit.size*boxSize.s2);
					spheresOldPos[i](0) = std::max(spheresOldPos[i](0), (scalar)0.0);
					spheresOldPos[i](1) = std::max(spheresOldPos[i](1), (scalar)0.0);
					if(_3D_) spheresOldPos[i](2) = std::max(spheresOldPos[i](2), (scalar)0.0);
				}
			#endif
		}
		spheresPos[i] = spheresOldPos[i];
		spheresPos[i] += spheresSpeed[i]*(timeInterval/2);
		//spheresPos[i] += 0.5*acceleration*(timeInterval/2)*(timeInterval/2);
		
		
		/*printf("sphere %d speed: %5f %5f\n", i, spheresSpeed[i](0), spheresSpeed[i](1));
		printf("sphere %d pos: %5f %5f\n", i, spheresPos[i](0), spheresPos[i](1));
		printf("sphere %d old pos: %5f %5f\n", i, spheresOldPos[i](0), spheresOldPos[i](1));// */
		
		EIGEN_ASM_COMMENT("end");
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::addStandardForces(){
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		spheresForce[i] -= airResistance*(0.47*M_PI*sqr(spheres[i].size))*0.5*spheresSpeed[i]*std::abs(spheresSpeed[i].norm());
		spheresForce[i](0) += spheres[i].mass*gravity.s0;
		spheresForce[i](1) += spheres[i].mass*gravity.s1;
		if(_3D_){
			spheresForce[i](2) += spheres[i].mass*gravity.s2;
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::methodLeapfrog(){
	// description here: https://de.wikipedia.org/wiki/Leapfrog-Verfahren#Darstellung_als_Mehrschrittverfahren
	
	timeInterval = std::min((double)timeInterval, 1.0/minFps);
	
	for(int i = 0; i<spheresCount; i++){
		spheresForce[i].setZero();
	}
	
	if(!leapfrogFirstStepMade){
		leapfrogFirstStepMade = true;
		for(int i = 0; i<spheresCount; i++){
			spheresOldPos[i] = spheresPos[i];
		}
		//////
		for(int i = 0; i<numForces; i++){
			forces[i]->calcForces();
		}
		if(wallResistance){
			calcWallResistance();
		}
		addStandardForces();
		//////
		for(int i = 0; i<spheresCount; i++){
			spheresPos[i] = spheresOldPos[i] + spheresSpeed[i]*timeInterval + 0.5*spheresForce[i]/spheres[i].mass*timeInterval*timeInterval;
		}
	}else{
		//////
		for(int i = 0; i<numForces; i++){
			forces[i]->calcForces();
		}
		if(wallResistance){
			calcWallResistance();
		}
		addStandardForces();
		//////
		eVector tmp;
		for(int i = 0; i<spheresCount; i++){
			//tmp is x(t-2)
			tmp = spheresOldPos[i];
			//spheresOldPos is x(t-1)
			spheresOldPos[i] = spheresPos[i];
			//spheresPos becomes x(t)
			spheresPos[i] = 2*spheresOldPos[i] - tmp + spheresForce[i]/spheres[i].mass*timeInterval*timeInterval;
			
			spheresSpeed[i] = (spheresPos[i]-spheresOldPos[i])/timeInterval;
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::methodRungeKutta(){
	
	timeInterval = std::min((double)timeInterval, 1.0/minFps);
	
	//k1:
	for(int i = 0; i<spheresCount; i++){
		k[i][1] = spheresSpeed[i];
		spheresPos[i] = spheresOldPos[i];
	}
	//k5:
	for(int i = 0; i<numForces; i++){
		forces[i]->calcForces();
	}
	if(wallResistance){
		calcWallResistance();
	}
	addStandardForces();
	//k2:
	for(int i = 0; i<spheresCount; i++){
		k[i][5] = spheresForce[i];
		spheresForce[i].setZero();
		k[i][2] = k[i][1]+0.5*timeInterval*k[i][5];
		spheresPos[i] = spheresOldPos[i]+0.5*timeInterval*k[i][1];
		spheresSpeed[i] = k[i][1]+0.5*timeInterval*k[i][5];
	}
	//k6:
	for(int i = 0; i<numForces; i++){
		forces[i]->calcForces();
	}
	if(wallResistance){
		calcWallResistance();
	}
	addStandardForces();
	//k3:
	for(int i = 0; i<spheresCount; i++){
		k[i][6] = spheresForce[i];
		spheresForce[i].setZero();
		k[i][3] = k[i][1]+0.5*timeInterval*k[i][6];
		spheresPos[i] = spheresOldPos[i]+0.5*timeInterval*k[i][2];
		spheresSpeed[i] = k[i][1]+0.5*timeInterval*k[i][6];
	}
	//k7:
	for(int i = 0; i<numForces; i++){
		forces[i]->calcForces();
	}
	if(wallResistance){
		calcWallResistance();
	}
	addStandardForces();
	//k4:
	for(int i = 0; i<spheresCount; i++){
		k[i][7] = spheresForce[i];
		spheresForce[i].setZero();
		k[i][4] = k[i][1]+timeInterval*k[i][7];
		spheresPos[i] = spheresOldPos[i]+timeInterval*k[i][3];
		spheresSpeed[i] = k[i][1]+0.5*timeInterval*k[i][7];
	}
	//k8:
	for(int i = 0; i<numForces; i++){
		forces[i]->calcForces();
	}
	if(wallResistance){
		calcWallResistance();
	}
	addStandardForces();
	//result:
	for(int i = 0; i<spheresCount; i++){
		k[i][8] = spheresForce[i];
		spheresForce[i].setZero();
	}
	
	for(int i = 0; i<spheresCount; i++){
		spheresPos[i] = spheresOldPos[i]+timeInterval/6.0*(k[i][1]+2*k[i][2]+2*k[i][3]+k[i][4]);
		spheresOldPos[i] = spheresPos[i];
		spheresSpeed[i] = k[i][1]+timeInterval/6.0*(k[i][5]+2*k[i][6]+2*k[i][7]+k[i][8]);
	}
	
	
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::doStep(){
	if(forceCounter == 0){
		for(int i = 0; i<numWalls; i++){
			curWallForces[i] = 0;
		}
	}
	forceCounter++;
	int x = (int)ceil((spheresCount-1)/2.0);
	
	/*for(int i = 0; i<spheresCount; i++){
		spheresForce[i].setZero();
	}*/
	
	if(usemethodRungeKutta){
		methodRungeKutta();
	}else{
		//methodEulerPolygon();
		methodLeapfrog();
	}
	
	#if useSSE
		if(_3D_){
			for(int i = 0; i<spheresCount; i++){
				spheresPos[i](3) = 0;
			}
		}
		#if _double_==0
			else{
				for(int i = 0; i<spheresCount; i++){
					spheresPos[i](2) = 0;
					spheresPos[i](3) = 0;
				}
			}
		#endif
	#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateG(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateAirResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateWallResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateEModul(){
	_E = curUnit.size*curUnit.size*E*1000000.0;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updatePoisson(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateElasticity(){
	//nothing to do
}

template <int dims, bool _3D_>
bool EigenCalculator_Engine<dims,_3D_>::saveFrame(){
	for(int i = 0; i<readNum_render; i++){
		renderBuffer[bufferWriteIndex][i] = spheresOldPos[i];
	}
	return true;
}

template <int dims, bool _3D_>
bool EigenCalculator_Engine<dims,_3D_>::isFixed(int i){
	return spheres[i].fixed == 1;
}

#define k_b 1.3806488E-23
template <int dims, bool _3D_>
scalar EigenCalculator_Engine<dims,_3D_>::getTemperature(){
	//1m : 65pm = 1.0E12 : 1
	//950kg/m^3 : 1.25kg/m^3 = 760 : 1
	const static double mass = 14*1.66E-27;
	double E_kin = 0;
	for(int i = 0; i<spheresCount; i++){
		E_kin += 0.5*mass*pow(spheresSpeed[i].norm(),2);
		//E_kin += 0.5*spheres[i].mass*pow(spheresSpeed[i].norm(),2);
	}
	//printf("E_kin: %5f\n", E_kin);
	double temp = 2.0/3.0 * E_kin/k_b;///(1.0E24*0.015*0.015);
	return temp;
}

template <int dims, bool _3D_>
Sphere* EigenCalculator_Engine<dims,_3D_>::getDirectSphere(int i){
	if(renderBuffer[bufferWriteIndex][i]==eVector::Zero()) return NULL;
	spheres[i].pos.s0 = renderBuffer[bufferWriteIndex][i](0);
	spheres[i].pos.s1 = renderBuffer[bufferWriteIndex][i](1);
	if(_3D_){
		spheres[i].pos.s2 = renderBuffer[bufferWriteIndex][i](2);
	}
	return &spheres[i];
}

template <int dims, bool _3D_>
Sphere* EigenCalculator_Engine<dims,_3D_>::getSphere(int i){
	if(renderBuffer[bufferReadIndex][i]==eVector::Zero()) return NULL;
	#if 0
		//"live" view
		spheres[i].pos.s0 = spheresPos[i](0);
		spheres[i].pos.s1 = spheresPos[i](1);
		if(_3D_){
			spheres[i].pos.s2 = spheresPos[i](2);
		}
	#else
		//normal view
		spheres[i].pos.s0 = renderBuffer[bufferReadIndex][i](0);
		spheres[i].pos.s1 = renderBuffer[bufferReadIndex][i](1);
		if(_3D_){
			spheres[i].pos.s2 = renderBuffer[bufferReadIndex][i](2);
		}
	#endif
	return &spheres[i];
}
template <int dims, bool _3D_>
Sphere* EigenCalculator_Engine<dims,_3D_>::getSphereWithSpeed(int i){
	if(renderBuffer[bufferReadIndex][i]==eVector::Zero()) return NULL;
	spheres[i].pos.s0 = spheresPos[i](0);
	spheres[i].pos.s1 = spheresPos[i](1);
	if(_3D_){
		spheres[i].pos.s2 = spheresPos[i](2);
	}
	spheres[i].speed.s0 = spheresSpeed[i](0);
	spheres[i].speed.s1 = spheresSpeed[i](1);
	if(_3D_){
		spheres[i].speed.s2 = spheresSpeed[i](2);
	}
	return &spheres[i];
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::setSphere(int i, Sphere* s){
	spheresPos[i](0) = s->pos.s0;
	spheresPos[i](1) = s->pos.s1;
	if(_3D_){
		spheresPos[i](2) = s->pos.s2;
	}
	spheresSpeed[i](0) = s->speed.s0;
	spheresSpeed[i](1) = s->speed.s1;
	if(_3D_){
		spheresSpeed[i](2) = s->speed.s2;
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::fpsChanged(scalar timeInt){
	timeInterval = timeInt;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::boxSizeChanged(){
	updateGridSize();
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateGridSize(){
	////TODO
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::gravityChanged(){
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::sphereCountChanged_subclass(int i){
	///k:
	k = newCopy(k, spheresCount, i);
	for(int j = spheresCount; j<i; j++){
		k[j] = new eVector[9];
	}
	///spheres
	spheres = newCopy<Sphere>(spheres, spheresCount, i);
	///spheresOldPos
	spheresOldPos = newCopy<eVector>(spheresOldPos, spheresCount, i);
	///spheresPos
	spheresPos = newCopy<eVector>(spheresPos, spheresCount, i);
	///spheresSpeed
	spheresSpeed = newCopy<eVector>(spheresSpeed, spheresCount, i);
	///spheresForce
	spheresForce = newCopy<eVector>(spheresForce, spheresCount, i);
	///gridIndex
	////TODO
	//gridIndex = newCopy<int*>(gridIndex, spheresCount, i);
	///position indexes
	
	///TODO
	/*posX = newCopy<Pos>(posX, spheresCount, i);
	posY = newCopy<Pos>(posY, spheresCount, i);
	if(_3D_){
		posZ = newCopy<Pos>(posZ, spheresCount, i);
	}
	/*
	if(i<(spheresCount/2)){
		for(int j = i; j<spheresCount; j++){
			posX[posX[j].posOfSphere].sphereAtPos = posX[j].sphereAtPos;
			posX[posY[j].posOfSphere].sphereAtPos = posY[j].sphereAtPos;
			if(_3D_){
				posX[posZ[j].posOfSphere].sphereAtPos = posZ[j].sphereAtPos;
			}
		}
	}else // * /
	{
		for(int j = 0; j<i; j++){
			posX[j].sphereAtPos = j;
			posX[j].posOfSphere = j;
			posY[j].sphereAtPos = j;
			posY[j].posOfSphere = j;
			if(_3D_){
				posZ[j].sphereAtPos = j;
				posZ[j].posOfSphere = j;
			}
		}
	}
	//parallelFor
	for(int j = spheresCount; j<i; j++){
		posX[j].sphereAtPos = j;
		posX[j].posOfSphere = j;
		posY[j].sphereAtPos = j;
		posY[j].posOfSphere = j;
		if(_3D_){
			posZ[j].sphereAtPos = j;
			posZ[j].posOfSphere = j;
		}
	}*/
	///renderBuffer
	int readNum_render_new = min(maxShowSpheresCount,i);
	if(readNum_render_new>readNum_render)
	{
		//parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	readNum_render = readNum_render_new;
	
	//parallelFor
	for(int j = spheresCount; j<i; j++){
		if(j<readNum_render){
			for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
				renderBuffer[k][j] = eVector::Zero();
			}
		}
		
		initSphere(j);
	}
	//bufferReadIndex = (readNum_render+bufferWriteIndex-1)%readNum_render;
	///both_r
	both_r = newCopy<scalar*>(both_r, spheresCount, i);
	parallelFor
	for(int j = 0; j<i; j++){
		if(j<spheresCount){
			both_r[j] = newCopy<scalar>(both_r[j], spheresCount, i);
			for(int k = spheresCount; k<i; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
			}
		}else{
			both_r[j] = new scalar[i];
			for(int k = 0; k<i; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
			}
		}
	}
	
	///update all forces:
	for(int j = 0; j<numForces; j++){
		forces[j]->spheresCountChanged(i);
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::maxSphereCountChanged_subclass(int i){
	//renderBuffer
	int readNum_render_new = min(i,spheresCount);
	if(readNum_render_new>readNum_render)
	{
		parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	
	parallelFor
	for(int j = readNum_render; j<readNum_render_new; j++){
		for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
			renderBuffer[k][j] = eVector::Zero();
		}
	}
	readNum_render = readNum_render_new;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::loadConfig(const char* file){
	f2.open(file, std::fstream::in);
	bool b = true;
	if (f2.is_open())
	{
		
		/*
		sets->beginGroup("General");
		int sets_i = 0;
		
		readSetting(use3D);
		readSetting(spheresCount);
		readSetting(maxShowSpheresCount);
		readSetting(boxSize.s0);
		readSetting(boxSize.s1);
		if(use3D){
			readSetting(boxSize.s2);
		}
		readSetting(sphereSize.s0);
		readSetting(sphereSize.s1);
		readSetting(renderFpsMax);
		readSetting(renderFps);
		
		readSetting(speed);
		readSetting(fps);
		readSetting(minFps);
		
		readSetting(max_speed);
		readSetting(E);
		readSetting(poisson);
		readSetting(elastic);
		readSetting(gravity_abs);
		readSetting(G_fact);
		readSetting(airResistance);
		readSetting(wallResistance);
		
		sets->endGroup();
		*/
		//readLine();
		int _3d;
		saveInVar(f2, _3d);
		std::cout<<"_3d: "<<_3d<<std::endl;
		
		if(_3d<0 || _3d>1){
			f2.close();
			loadConfig(file);
			return;
		}
		
		//*
		if((_3d!=0) != _3D_){
			if(_3d == 0){
				std::cerr<<"You have to open this file with 2D viewer."<<std::endl;
			}else{
				std::cerr<<"You have to open this file with 3D viewer."<<std::endl;
			}
			return;
		}// */
		use3D = (_3d!=0);
		
		int newSpheresCount;
		saveInVar(f2, newSpheresCount);
		printf("spheresCount: %5d\n", newSpheresCount);
		int newMaxSpheresCount;
		saveInVar(f2, newMaxSpheresCount);
		
		saveInVar(f2, boxSize.s0);
		saveInVar(f2, boxSize.s1);
		if(_3D_) saveInVar(f2, boxSize.s2);
		saveInVar(f2, sphereSize.s0);
		saveInVar(f2, sphereSize.s1);
		saveInVar(f2, renderFpsMax);
		timeInterval = 1000/renderFpsMax;
		saveInVar(f2, renderFps);
		saveInVar(f2, speed);
		saveInVar(f2, fps);
		saveInVar(f2, minFps);
		saveInVar(f2, max_speed);
		saveInVar(f2, E);
		saveInVar(f2, poisson);
		saveInVar(f2, elastic);
		
		saveInVar(f2, gravity_abs);
		printf("gravity: %5f\n", gravity_abs);
		glWidget->updateGravity();
		
		saveInVar(f2, G_fact);
		saveInVar(f2, airResistance);
		int _wallResistance;
		saveInVar(f2, _wallResistance);
		wallResistance = (_wallResistance != 0);
		
		
		if(newSpheresCount>=0){
			sphereCountChanged(newSpheresCount);
			//spheresCount = newSpheresCount;
		}

		if(newMaxSpheresCount>=0){
			maxSphereCountChanged(newMaxSpheresCount);
			//maxShowSpheresCount = newMaxSpheresCount;
		}
		
		int fixed;
		for(int i = 0; i<newSpheresCount; i++){
			printf("reading sphere %5d...\n", i);
			saveInVar(f2, fixed);
			spheres[i].fixed = fixed;
			saveInVar(f2, spheres[i].size);
			spheres[i].size *= curUnit.size;
			printf("sphere size: %5f\n", spheres[i].size);
			saveInVar(f2, spheres[i].mass);
			saveInVar(f2, spheresPos[i](0));
			spheresPos[i](0) += boxSize.s0/2;
			spheresPos[i](0) *= curUnit.size;
			saveInVar(f2, spheresPos[i](1));
			spheresPos[i](1) += boxSize.s1/2;
			spheresPos[i](1) *= curUnit.size;
			//if(fixed==0) 
			//spheresPos[i](0) += 0.001*((rand()%1024)/1024.0);
			//if(fixed==0) 
			//spheresPos[i](1) += 0.001*((rand()%1024)/1024.0);
			if(_3D_){
				saveInVar(f2, spheresPos[i](2));
				spheresPos[i](2) += boxSize.s2/2;
				spheresPos[i](2) *= curUnit.size;
				//if(fixed==0) 
				//spheresPos[i](2) += 0.001*((rand()%1024)/1024.0);
			}
			#if useSSE
				if(_3D_){
					spheresPos[i](3) = 0;
				}
				#if _double_==0
					else{
						spheresPos[i](2) = 0;
						spheresPos[i](3) = 0;
					}
				#endif
			#endif
			spheresOldPos[i] = spheresPos[i];
			//printf("sphere pos: %5f %5f\n", spheresPos[i](0), spheresPos[i](1));
			//printf("sphere oldPos: %5f %5f\n", spheresOldPos[i](0), spheresOldPos[i](1));
			saveInVar(f2, spheresSpeed[i](0));
			spheresSpeed[i](0) *= curUnit.size;
			saveInVar(f2, spheresSpeed[i](1));
			spheresSpeed[i](1) *= curUnit.size;
			if(_3D_){
				saveInVar(f2, spheresSpeed[i](2));
				spheresSpeed[i](2) *= curUnit.size;
			}
			#if useSSE
				if(_3D_){
					spheresSpeed[i](3) = 0;
				}
				#if _double_==0
					else{
						spheresSpeed[i](2) = 0;
						spheresSpeed[i](3) = 0;
					}
				#endif
			#endif
			//printf("sphere speed: %5f %5f\n", spheresSpeed[i](0), spheresSpeed[i](1));
			spheresForce[i].setZero();
		}
		
		for(int j = 0; j<spheresCount; j++){
			for(int k = 0; k<spheresCount; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
				both_r[k][j] = spheres[j].size+spheres[k].size;
			}
		}
		
		initialized = true;
		
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		
		printf("Spheres to render: %d\n", readNum_render);
		
		///update all forces:
		for(int j = 0; j<numForces; j++){
			forces[j]->updateGridSize();
		}
		
		f2.close();
		
		//emit fileLoaded();
		
	}
	else
	{
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::loadConfig_(){
	const char* file = (std::string(filename)+getConfigFileExtension()).c_str();
	
	const char* filter = (std::string("SphereSim View File (*.")+getConfigFileExtension()+")").c_str();
	QString str = QFileDialog::getOpenFileName(0, ("Open file"), (std::string("./save.")+getConfigFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
	file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	loadConfig(file);
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::paintGL(bool b){
	Calculator::paintGL(b);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(0.5);
	
	
	for(int i = 0; i<numForces; i++){
		forces[i]->paintGL();
	}

	//return;
	for(double x = 0; x < curUnit.size*boxSize.s0; x+=gridWidth){
		for(double y = 0; y < curUnit.size*boxSize.s1; y+=gridWidth){
			glBegin(GL_LINE_STRIP);
			glVertex2d(x+gridWidth, y);
			glVertex2d(x+gridWidth, y+gridWidth);
			glVertex2d(x, y+gridWidth);
			glEnd();
		}
	}
	
	return;
	eVector pos;
	for(int i = 0; i<spheresCount; i++){
		//pos = renderBuffer[bufferReadIndex][i];
		pos = spheresPos[i];
		glWidget->renderText(pos[0], pos[1], pos[2], QString::number(	//posCell[i].posOfSphere));
																		//cellOfSphere[i]));
																		i));
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::setGridWith(scalar s){
	gridWidth = s;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateMagnitude(){
}

template class EigenCalculator_Engine<2,false>;
template class EigenCalculator_Engine<3,true>;

#endif
