/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <SphereCalculator.hpp>
#include <Console.hpp>

#include <QDebug>
#include <cmath>

using namespace SphereSim;

static const Scalar E = 5000, poisson = 0.5;
static const Scalar E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));

SphereCalculator::SphereCalculator(){
	qDebug()<<"SphereCalculator: constructor called";
	updateData();
	boxSize = Vector3(1,1,1);
	timeStep = 0.002;
	setIntegratorMethod(IntegratorMethods::RungeKuttaFehlberg54);
	calculationCounter = 0;
}

QVector<Sphere>& SphereCalculator::getSpheres(){
	return spheres;
}

Vector3 SphereCalculator::sphereAcceleration(quint16 sphereIndex, Sphere sphere, Scalar timeDiff){
	Scalar d, force;
	Vector3 _force, acc;

	_force = Vector3(0,-9.81*sphere.mass,0);
	for(quint8 dim = 0; dim<3; dim++){
		if((d = (sphere.radius - sphere.pos(dim))) > 0){
			force = 4.0f/3.0f*E_*std::sqrt(sphere.radius*std::pow(d,3));
			_force(dim) += force;
		}
		if((d = (sphere.radius + sphere.pos(dim) - boxSize(dim))) > 0){
			force = 4.0f/3.0f*E_*std::sqrt(sphere.radius*std::pow(d,3));
			_force(dim) -= force;
		}
	}
	acc = _force/sphere.mass;
	calculationCounter++;
	return acc;
}

quint32 SphereCalculator::doStep(){
	updateData();
	integrateRungeKuttaStep();
	return 1;
}

void SphereCalculator::updateData(){
	sphArr = spheres.data();
	sphCount = spheres.size();
}

void SphereCalculator::setTimeStep(Scalar timeSt){
	timeStep = timeSt;
}

Scalar SphereCalculator::getTimeStep(){
	return timeStep;
}

void SphereCalculator::integrateRungeKuttaStep(){
	Sphere* s = &sphArr[0];
	Sphere sphere, origSphere;
	Vector3 acc, pos, speed, pos_, speed_, error_pos, error_speed;
	Scalar error_pos_, error_speed_;
	const quint8 integratorOrder = butcherTableau.order;
	Vector3 k_acc[integratorOrder];
	Vector3 k_speed[integratorOrder];
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex){
		integrateRungeKuttaStep(sphereIndex, timeStep, 0.0);
	}
}

quint32 SphereCalculator::integrateRungeKuttaStep(quint16 sphereIndex, Scalar stepLength, Scalar timeDiff){
	Sphere sphere = sphArr[sphereIndex];
	Sphere origSphere = sphere;
	const quint8 integratorOrder = butcherTableau.order;
	Vector3 k_acc[integratorOrder];
	Vector3 k_speed[integratorOrder];
	
	k_acc[0] = sphereAcceleration(sphereIndex, sphere, timeDiff);
	k_speed[0] = sphere.speed;
	for(quint8 n = 1; n<integratorOrder; n++){
		sphere.pos = origSphere.pos;
		for(quint8 j = 0; j<n; j++){
			sphere.pos += stepLength*butcherTableau.a[n][j]*k_speed[j];
		}
		k_acc[n] = sphereAcceleration(sphereIndex, sphere, timeDiff);
		
		k_speed[n] = origSphere.speed;
		for(quint8 j = 0; j<n; j++){
			k_speed[n] += stepLength*butcherTableau.a[n][j]*k_acc[j];
		}
	}
	
	Vector3 pos = origSphere.pos;
	Vector3 pos_ = pos;
	Vector3 speed = origSphere.speed;
	Vector3 speed_ = speed;
	for(quint8 j = 0; j<integratorOrder; j++){
		pos += stepLength*butcherTableau.b[j]*k_speed[j];
		pos_ += stepLength*butcherTableau.b_[j]*k_speed[j];
		speed += stepLength*butcherTableau.b[j]*k_acc[j];
		speed_ += stepLength*butcherTableau.b_[j]*k_acc[j];
	}
	
	Scalar error_pos_ = (pos-pos_).norm();
	Scalar error_speed_ = (speed-speed_).norm();
	if(error_pos_>1.0e-05 || error_speed_>1.0e-05){
		quint32 stepCount = 0;
		stepCount += integrateRungeKuttaStep(sphereIndex, stepLength/2, timeDiff);
		stepCount += integrateRungeKuttaStep(sphereIndex, stepLength/2, timeDiff+(stepLength/2));
		return stepCount;
	}else{
		sphArr[sphereIndex].pos = pos;
		sphArr[sphereIndex].speed = speed;
		sphArr[sphereIndex].acc = (speed-origSphere.speed)/stepLength;
		return 1;
	}
}

void SphereCalculator::setIntegratorMethod(quint8 integrMethod){
	integratorMethod = integrMethod;
	
	if(integrMethod == IntegratorMethods::HeunEuler21){
		const Scalar a[4] = 	{0.0,	0.0,
								1.0,	0.0};
		const Scalar b[2] = 	{1/2.0,	1/2.0};
		const Scalar b_[2] = 	{1.0,	0.0};
		const Scalar c[2] = 	{0.0,	1.0};
		butcherTableau = ButcherTableau(2, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::BogackiShampine32){
		const Scalar a[16] = 	{0.0,	0.0,	0.0,	0.0,
								1/2.0,	0.0,	0.0,	0.0,
								0.0,	3/4.0,	0.0,	0.0,
								2/9.0,	1/3.0,	4/9.0,	0.0};
		const Scalar b[4] = 	{2/9.0,	1/3.0,	4/9.0,	0.0};
		const Scalar b_[4] = 	{7/24.0,	1/4.0,	1/3.0,	1/8.0};
		const Scalar c[4] = 	{0.0,	1/2.0,	3/4.0,	1.0};
		butcherTableau = ButcherTableau(4, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::CashKarp54){
		const Scalar a[36] = 	{0.0,			0.0,		0.0,			0.0,			0.0,			0.0,
								1/5.0,			0.0,		0.0,			0.0,			0.0,			0.0,
								3/40.0,			9/40.0,		0.0,			0.0,			0.0,			0.0,
								3/10.0,			-9/10.0,	6/5.0,			0.0,			0.0,			0.0,
								-11/54.0,		5/2.0,		-70/27.0,		35/27.0,		0.0,			0.0,
								1631/55296.0,	175/512.0,	575/13824.0,	44275/110592.0,	253/4096.0,		0.0};
		const Scalar b[6] = 	{37/378.0,		0.0,		250/621.0,		125/594.0,		0.0,			512/1771.0};
		const Scalar b_[6] = 	{2825/27648.0,	0.0,		18575/48384.0,	13525/55296.0,	277/14336.0,	1/4.0};
		const Scalar c[6] = 	{0.0,			1/5.0,		3/10.0,			3/5.0,			1.0,			7/8.0};
		butcherTableau = ButcherTableau(6, a, b, b_, c);
	}else if(integrMethod == IntegratorMethods::DormandPrince54){
		const Scalar a[49] = 	{0.0,			0.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								1/5.0,			0.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								3/40.0,			9/40.0,			0.0,			0.0,		0.0,			0.0,		0.0,
								44/45.0,		-56/15.0,		32/9.0,			0.0,		0.0,			0.0,		0.0,
								19372/6561.0,	-25360/2187.0,	64448/6561.0,	-212/729.0,	0.0,			0.0,		0.0,
								9017/3168.0,	-355/33.0,		46732/5247.0,	49/176.0,	-5103/18656.0,	0.0,		0.0,
								35/384.0,		0.0,			500/1113.0,		125/192.0,	-2187/6784.0,	11/84.0,	0.0};
		const Scalar b[7] = 	{35/384.0,		0.0,			500/1113.0,		125/192.0,	-2187/6784.0,	11/84.0,	0.0};
		const Scalar b_[7] = 	{5179/57600.0,	0.0,			7571/16695.0,	393/640.0,	-92097/339200.0,187/2100.0,	1/40.0};
		const Scalar c[7] = 	{0.0,			1/5.0,			3/10.0,			4/5.0,		8/9.0,			1.0,		1.0};
		butcherTableau = ButcherTableau(7, a, b, b_, c);
	}else{
		integratorMethod = IntegratorMethods::RungeKuttaFehlberg54;
		const Scalar a[36] = 	{0.0,			0.0,			0.0,			0.0,			0.0,		0.0,
								1/4.0,			0.0,			0.0,			0.0,			0.0,		0.0,
								3/32.0,			9/32.0,			0.0,			0.0,			0.0,		0.0,
								1932/2197.0,	-7200/2197.0,	7296/2197.0,	0.0,			0.0,		0.0,
								439/216.0,		-8.0,			3680/513.0,		-845/4104.0,	0.0,		0.0,
								-8/27.0,		2.0,			-3544/2565.0,	1859/4104.0,	-11/40.0,	0.0};
		const Scalar b[6] = 	{16/135.0,		0.0,			6656/12825.0,	28561/56430.0,	-9/50.0,	2/55.0};
		const Scalar b_[6] = 	{25/216.0,		0.0,			1408/2565.0,	2197/4104.0,	-1/5.0,		0.0};
		const Scalar c[6] = 	{0.0,			1/4.0,			3/8.0,			12/13.0,		1.0,		1/2.0};
		butcherTableau = ButcherTableau(6, a, b, b_, c);
	}
}

quint8 SphereCalculator::getIntegratorMethod(){
	return integratorMethod;
}

quint32 SphereCalculator::popCalculationCounter(){
	if(sphCount>0){
		quint32 counter = calculationCounter/sphCount;
		calculationCounter = 0;
		return counter;
	}else{
		return 0;
	}
}

quint32 SphereCalculator::doSomeSteps(quint32 steps){
	updateData();
	for(quint32 i = 0; i<steps; i++){
		integrateRungeKuttaStep();
	}
	return 1;
}

Scalar SphereCalculator::getTotalEnergy(){
	updateData();
	Scalar totalEnergy = 0.0, sphereEnergy, d;
	Sphere sphere;
	for(quint16 sphereIndex = 0; sphereIndex<sphCount; ++sphereIndex){
		sphere = sphArr[sphereIndex];
		sphereEnergy = sphere.mass*9.81*sphere.pos(1);
		sphereEnergy += 0.5*sphere.mass*sphere.speed.squaredNorm();
		
		for(quint8 dim = 0; dim<3; dim++){
			if((d = (sphere.radius - sphere.pos(dim))) > 0){
				sphereEnergy += 8.0/15.0*E_*sqrt(sphere.radius)*pow(d, 2.5);
			}
			if((d = (sphere.radius + sphere.pos(dim) - boxSize(dim))) > 0){
				sphereEnergy += 8.0/15.0*E_*sqrt(sphere.radius)*pow(d, 2.5);
			}
		}
		totalEnergy += sphereEnergy;
	}
	return totalEnergy;
}