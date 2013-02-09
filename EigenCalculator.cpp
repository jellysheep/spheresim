
#include "EigenCalculator.h"

#include <iostream>
using namespace std;

#include "NanosecondTimer.h"

#define parallelFor _Pragma("omp parallel for")

EigenCalculator::EigenCalculator():Calculator(){
	srand(1);
	srand(NanosecondTimer::getNS());
		
	bool eVectorizing = false;
	#ifdef EIGEN_VECTORIZE
		eVectorizing = true;
	#endif
	cout<<"Vectorizing: "<<eVectorizing<<endl;
	
	timeInterval = speed*speedCorrection/fps;
	circles = new Circle[circlesCount];
	circlesPos = new eVector[circlesCount];
	circlesOldPos = new eVector[circlesCount];
	circlesSpeed = new eVector[circlesCount];
	circlesForce = new eVector[circlesCount];
	
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		circles[i].size = rans(size.s0, size.s1);
		circles[i].E = E;
		circles[i].poisson = poisson;
		circles[i].mass = 4.0/3.0*pow(circles[i].size,3)*M_PI  *950; //Kautschuk
		
		circlesPos[i](0) = circles[i].size+rans(boxSize.s0-2*circles[i].size);
		circlesPos[i](1) = circles[i].size+rans(boxSize.s1-2*circles[i].size);
		#if _3D_
			circlesPos[i](2) = circles[i].size+rans(boxSize.s2-2*circles[i].size);
			#if useSSE
				//Vector4f or Vector4d
				circlesPos[i](3) = 0;
			#endif
		#endif
		circlesOldPos[i] = circlesPos[i];
		
		circlesSpeed[i] = eVector::Random();
		#if useSSE
			#if _3D_
				//Vector4f or Vector4d
				circlesSpeed[i](3) = 0;
			#else
				#if !_double_
					//Vector4f
					circlesSpeed[i](2) = 0;
					circlesSpeed[i](3) = 0;
				#endif
			#endif
		#endif
		circlesSpeed[i].normalize();
		circlesSpeed[i] *= rans(max_speed);
		//cout<<circlesSpeed[i]<<endl<<endl;
		
		circlesForce[i] = eVector::Zero();
		
		//cout<<"3D enabled: "<<_3D_<<endl<<endl;
	}
	readNum_render = min(showCirclesCount,circlesCount);
	/*c_CPU_render = new Circle*[renderBufferCount];
	for(int i = 0; i<renderBufferCount; i++){
		c_CPU_render[i] = new Circle[readNum_render];
	}
	bufferReadIndex = 0;
	bufferWriteIndex = 0;*/
	if(renderBool){
		readNum_render = min(showCirclesCount,circlesCount);
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
}

void EigenCalculator::save(){
	//not yet implemented
}

#define _G_ 0
void EigenCalculator::doStep(){
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		for(int j = i+1; j<circlesCount; j++){
			eVector force;
			
			eVector d_pos, d_n;
			scalar both_r, d, d_, R, E_;
			
			//#define c2 circle[i]
			both_r = circles[i].size + circles[j].size;
			d_pos = circlesPos[j]-circlesPos[i];
			
			//*
			#if _G_==0
			if(G == 0)
			{
				if(d_pos(0)>both_r||d_pos(0)<-both_r||d_pos(1)>both_r||d_pos(1)<-both_r
					#if _3D_
					||d_pos(2)>both_r||d_pos(2)<-both_r
					#endif
					){
					continue;
				}
			}
			#endif
			// */
			
			//d_pos.s2 = 0;
			d = d_pos.norm();
			//d = max(d,(scalar)0.00005f);
			
			/*
			d_n = d_pos/d;
			force = 10000000000000000000000000.0*pow_(d,12) *d_n;
			circles[i].force -= force;
			circles[j].force += force;
			force = 1000000000000.0*pow_(d,6) *d_n;
			circles[i].force += force;
			circles[j].force -= force;
			return;*/
			
			#if _G_
			if(G!=0)
			{
				d_n = d_pos/d;
				 // bzw. normalize(d_pos);
				// Gravitation:
				force = G*circles[i].mass*circles[j].mass/pow(max(d,(scalar)circles[i].size/10),2) *d_n;
				circlesForce[i] += force;
				
				circlesForce[j] -= force;
			}
			#endif
			
			// Abstossung:
			if (d < both_r) {
				//*
				//#if _G_==0
				if(G == 0)
				{
					d_n = d_pos/d; // bzw. normalize(d_pos);
				}
				//#endif
				// */
				// nach Kontaktmechanik mit Poisson-Zahlen:
				d_ = both_r - d;
				//*
				R = 1/((1/circles[i].size)+(1/circles[j].size));
				E_ = 1/(((1-circles[i].poisson*circles[i].poisson)/circles[i].E)+((1-circles[j].poisson*circles[j].poisson)/circles[j].E));
				force = 4.0f/3.0f*E_*sqrt(R*pow(d_,3)) *d_n;
				if(elastic!=1 && d_pos.dot((circlesSpeed[i]-circlesSpeed[j])/d)<0){ //Skalarprodukt
					force *= elastic;
				}
				//printf("id: %d id2: %d index1: %d index2: %d\n", id, id2, indices[id], indices[id2]);
				//GetSemaphor(&flags[id]);
				circlesForce[i] -= force;
				//ReleaseSemaphor(&flags[id]);
				//addForce2(id,-force);
				//GetSemaphor(&flags[id2]);
				circlesForce[j] += force;
				//ReleaseSemaphor(&flags[id2]);
				//addForce2(id2,force);
				// */
				//printf("now: id: %d id2: %d index1: %d index2: %d\n", id, id2, indices[id], indices[id2]);
			}
		}
	}
	
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		scalar d_, R, E_, force_;
		scalar htw_d_d, fact = 1.0f;
		if ((htw_d_d = (circles[i].size - circlesPos[i](0)))>0) {
			if(circlesSpeed[i](0) > 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
			force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
			circlesForce[i](0) += force_*fact;
		}else if ((htw_d_d = (circles[i].size + circlesPos[i](0) - boxSize.s0))>0) {
			if(circlesSpeed[i](0) < 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
			force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
			circlesForce[i](0) -= force_*fact;
		}
		fact = 1.0f;
		if ((htw_d_d = (circles[i].size - circlesPos[i](1)))>0) {
			if(circlesSpeed[i](1) > 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
			force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
			circlesForce[i](1) += force_*fact;
		}else if ((htw_d_d = (circles[i].size + circlesPos[i](1) - boxSize.s0))>0) {
			if(circlesSpeed[i](1) < 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
			force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
			circlesForce[i](1) -= force_*fact;
		}
		#if _3D_
			fact = 1.0f;
			if ((htw_d_d = (circles[i].size - circlesPos[i](2)))>0) {
				if(circlesSpeed[i](2) > 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
				force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
				circlesForce[i](2) += force_*fact;
			}else if ((htw_d_d = (circles[i].size + circlesPos[i](2) - boxSize.s0))>0) {
				if(circlesSpeed[i](2) < 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
				force_ = 4.0f/3.0f*E_*sqrt(R*pow(d_,3));
				circlesForce[i](2) -= force_*fact;
			}
		#endif
	//}
	//parallelFor
	//for(int i = 0; i<circlesCount; i++){
		EIGEN_ASM_COMMENT("begin");
		eVector force = circlesForce[i];
		eVector acceleration = force / circles[i].mass;
		acceleration(0) += gravity.s0;
		acceleration(1) += gravity.s1;
		#if _3D_
			acceleration(2) += gravity.s2;
		#endif
		circlesOldPos[i] += circlesSpeed[i]*timeInterval;
		circlesOldPos[i] += 0.5*acceleration*timeInterval*timeInterval;
		
		circlesSpeed[i] += acceleration*timeInterval;
		circlesForce[i] -= force;
		
		circlesPos[i] = circlesOldPos[i];
		circlesPos[i] += circlesSpeed[i]*(timeInterval/2);
		//circlesPos[i] += 0.5*acceleration*(timeInterval/2)*(timeInterval/2);
		
		EIGEN_ASM_COMMENT("end");
	}
}

void EigenCalculator::saveFrame(){
	for(int i = 0; i<circlesCount; i++){
		renderBuffer[bufferWriteIndex][i] = circlesOldPos[i];
	}
}

Circle* EigenCalculator::getCircle(int i){
	circles[i].pos.s0 = boxSize.s0-renderBuffer[bufferReadIndex][i](0);
	circles[i].pos.s1 = boxSize.s1-renderBuffer[bufferReadIndex][i](1);
	#if _3D_
		circles[i].pos.s2 = boxSize.s2-renderBuffer[bufferReadIndex][i](2);
	#endif
	return &circles[i];
}

void EigenCalculator::fpsChanged(scalar timeInt){
	timeInterval = timeInt;
}

void EigenCalculator::boxSizeChanged(){
}

void EigenCalculator::gravityChanged(){
}
