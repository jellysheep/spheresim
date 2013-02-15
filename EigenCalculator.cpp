
#include "EigenCalculator.h"

#include <iostream>
using namespace std;

#include "NanosecondTimer.h"

#define parallelFor _Pragma("omp parallel for if(circlesCount>500)")

#define fixSun 0

EigenCalculator::EigenCalculator():Calculator(){
	srand(1);
	srand(NanosecondTimer::getNS());
	
	_E = E*1000000.0;
		
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
	both_r = new scalar*[circlesCount];
	
	gridWidth = sphereSize.s0;
	gridSteps = (int)(max(boxSize.s0, max(boxSize.s1, boxSize.s2))/gridWidth);
	printf("Grid steps: %5d\n", gridSteps);
	gridIndex = new int*[circlesCount];
	
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		
		both_r[i] = new scalar[circlesCount];
		
		initCircle(i);
		
	}
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		for(int j = 0; j<circlesCount; j++){
			both_r[i][j] = circles[i].size + circles[j].size;
		}
	}
	if(renderBool){
		readNum_render = min(maxShowCirclesCount,circlesCount);
		printf("Circles to render: %d\n", readNum_render);
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
	printf("EigenCalculator initialized!\n");
}

void EigenCalculator::initCircle(int i){
	circles[i].size = rans(sphereSize.s0, sphereSize.s1);
	circles[i].E = E;
	circles[i].poisson = poisson;
	
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
	
	gridIndex[i] = new int[3];
	gridIndex[i][0] = circlesPos[i](0)/gridWidth;
	gridIndex[i][1] = circlesPos[i](1)/gridWidth;
	#if _3D_
		gridIndex[i][2] = circlesPos[i](2)/gridWidth;
	#endif
	
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
	
	#if fixSun
		if(i == 0) circles[i].size = 3*sphereSize.s0;
		//if(i == 0) circles[i].mass*=5;
		if(i == 0) ceBuffer[i].hsvColor = QColor::fromHsv(0,0,0);
		if(i == 0) circlesPos[i](0) = boxSize.s0/2;
		if(i == 0) circlesPos[i](1) = boxSize.s1/2;
		#if _3D_
			if(i == 0) circlesPos[i](2) = boxSize.s2/2;
		#endif
		if(i == 0) circlesSpeed[i] = eVector::Zero();
	#endif
	circlesOldPos[i] = circlesPos[i];
	circles[i].mass = 4.0/3.0*pow(circles[i].size,3)*M_PI  *950; //Kautschuk
}


void EigenCalculator::save(){
	//not yet implemented
}

#define _G_ 1
void EigenCalculator::doStep(){
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		eVector force;
		
		eVector d_pos, d_n;
		scalar *both_r_ = &this->both_r[i][0], d, d_, R, _E_;
		for(int j = i+1; j<circlesCount; j++){
			
			//#define c2 circle[i]
			//both_r = circles[i].size + circles[j].size;
			//both_r = this->both_r[i][j];
			
			/*d_pos(0) = circlesPos[j](0)-circlesPos[i](0);
			if(abs(d_pos(0))>*both_r) continue;
			d_pos(1) = circlesPos[j](1)-circlesPos[i](1);
			if(abs(d_pos(1))>*both_r) continue;
			d_pos(2) = circlesPos[j](2)-circlesPos[i](2);
			if(abs(d_pos(2))>*both_r) continue;
			//*/
			
			#if _G_
			if(G==0)
			#endif
			{
				if(abs(gridIndex[i][0]-gridIndex[j][0]) >1) continue;
				if(abs(gridIndex[i][1]-gridIndex[j][1]) >1) continue;
				#if _3D_
					if(abs(gridIndex[i][2]-gridIndex[j][2]) >1) continue;
				#endif
			}
			
			both_r_ = &both_r[i][j];
			d_pos = circlesPos[j]-circlesPos[i];
			
			/*
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
			d = max(d,(scalar)0.00005f);
			
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
				force = G*G_fact*circles[i].mass*circles[j].mass/pow(max(d,(scalar)circles[i].size/10),2) *d_n;
				circlesForce[i] += force;
				
				circlesForce[j] -= force;
			}
			#endif
			
			// Abstossung:
			if (d < *both_r_) {
				//*
				//#if _G_==0
				if(G == 0)
				{
					d_n = d_pos/d; // bzw. normalize(d_pos);
				}
				//#endif
				// */
				// nach Kontaktmechanik mit Poisson-Zahlen:
				d_ = *both_r_ - d;
				//*
				R = 1/((1/circles[i].size)+(1/circles[j].size));
				//_E_ = 1/(((1-circles[i].poisson*circles[i].poisson)/(_E))+((1-circles[j].poisson*circles[j].poisson)/(_E)));
				_E_ = 1/(((1-(poisson*poisson))/(_E))+((1-(poisson*poisson))/(_E)));
				force = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3)) *d_n;
				//printf("Stoss! r: %5f m: %5f d: %5f R: %5f E*: %5f F: %5f\n", circles[i].size, circles[i].mass, d_, R, _E_, force.norm());
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
	
	if(wallResistance){
		//parallelFor
		for(int i = 0; i<circlesCount; i++){
			scalar d_, R, _E_, force_;
			scalar htw_d_d, fact = 1.0f;
			if ((htw_d_d = (circles[i].size - circlesPos[i](0)))>0) {
				if(circlesSpeed[i](0) > 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](0) += force_*fact;
			}else if ((htw_d_d = (circles[i].size + circlesPos[i](0) - boxSize.s0))>0) {
				if(circlesSpeed[i](0) < 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](0) -= force_*fact;
			}
			fact = 1.0f;
			if ((htw_d_d = (circles[i].size - circlesPos[i](1)))>0) {
				if(circlesSpeed[i](1) > 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](1) += force_*fact;
			}else if ((htw_d_d = (circles[i].size + circlesPos[i](1) - boxSize.s1))>0) {
				if(circlesSpeed[i](1) < 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](1) -= force_*fact;
			}
			#if _3D_
				fact = 1.0f;
				if ((htw_d_d = (circles[i].size - circlesPos[i](2)))>0) {
					if(circlesSpeed[i](2) > 0)fact = elastic;
					d_ = htw_d_d;
					R = circles[i].size;
					_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
					force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
					circlesForce[i](2) += force_*fact;
				}else if ((htw_d_d = (circles[i].size + circlesPos[i](2) - boxSize.s2))>0) {
					if(circlesSpeed[i](2) < 0)fact = elastic;
					d_ = htw_d_d;
					R = circles[i].size;
					_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
					force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
					circlesForce[i](2) -= force_*fact;
				}
			#endif
		}
	}
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		#if fixSun
			if(i==0)continue;
		#endif
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
		
		gridIndex[i][0] = (int)(circlesPos[i](0)/gridWidth);
		gridIndex[i][1] = (int)(circlesPos[i](1)/gridWidth);
		#if _3D_
			gridIndex[i][2] = circlesPos[i](2)/gridWidth;
		#endif
		
		EIGEN_ASM_COMMENT("end");
	}
}

void EigenCalculator::updateG(){
	//nothing to do
}

void EigenCalculator::updateAirResistance(){
	//nothing to do
}

void EigenCalculator::updateWallResistance(){
	//nothing to do
}

void EigenCalculator::updateEModul(){
	_E = E*1000000.0;
}

void EigenCalculator::updatePoisson(){
	//nothing to do
}

void EigenCalculator::updateElasticity(){
	//nothing to do
}

void EigenCalculator::saveFrame(){
	for(int i = 0; i<readNum_render; i++){
		renderBuffer[bufferWriteIndex][i] = circlesOldPos[i];
	}
}

Circle* EigenCalculator::getCircle(int i){
	if(renderBuffer[bufferReadIndex][i]==eVector::Zero()) return NULL;
	circles[i].pos.s0 = renderBuffer[bufferReadIndex][i](0);
	circles[i].pos.s1 = renderBuffer[bufferReadIndex][i](1);
	#if _3D_
		circles[i].pos.s2 = renderBuffer[bufferReadIndex][i](2);
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

void EigenCalculator::circleCountChanged_subclass(int i){
	//circles
	circles = newCopy<Circle>(circles, circlesCount, i);
	//circlesOldPos
	circlesOldPos = newCopy<eVector>(circlesOldPos, circlesCount, i);
	//circlesPos
	circlesPos = newCopy<eVector>(circlesPos, circlesCount, i);
	//circlesSpeed
	circlesSpeed = newCopy<eVector>(circlesSpeed, circlesCount, i);
	//circlesForce
	circlesForce = newCopy<eVector>(circlesForce, circlesCount, i);
	//gridIndex
	gridIndex = newCopy<int*>(gridIndex, circlesCount, i);
	//renderBuffer
	int readNum_render_new = min(maxShowCirclesCount,i);
	if(readNum_render_new>readNum_render)
	{
		parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	readNum_render = readNum_render_new;
	
	parallelFor
	for(int j = circlesCount; j<i; j++){
		if(j<readNum_render){
			for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
				renderBuffer[k][j] = eVector::Zero();
			}
		}
		
		initCircle(j);
	}
	//bufferReadIndex = (readNum_render+bufferWriteIndex-1)%readNum_render;
	//both_r
	both_r = newCopy<scalar*>(both_r, circlesCount, i);
	parallelFor
	for(int j = 0; j<i; j++){
		if(j<circlesCount){
			both_r[j] = newCopy<scalar>(both_r[j], circlesCount, i);
			for(int k = circlesCount; k<i; k++){
				both_r[j][k] = circles[j].size+circles[k].size;
			}
		}else{
			both_r[j] = new scalar[i];
			for(int k = 0; k<i; k++){
				both_r[j][k] = circles[j].size+circles[k].size;
			}
		}
	}
}

void EigenCalculator::maxCircleCountChanged_subclass(int i){
	//renderBuffer
	int readNum_render_new = min(i,circlesCount);
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
