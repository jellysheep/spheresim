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

SphereCalculator::SphereCalculator(){
	qDebug()<<"SphereCalculator: constructor called";
	updateData();
	boxSize = Vector3(1,1,1);
	timeStep = 0.002;
	setIntegratorMethod(IntegratorMethods::RungeKutta4);
}

QVector<Sphere>& SphereCalculator::getSpheres(){
	return spheres;
}

void SphereCalculator::calculateForces(){
	static Scalar E = 5000, poisson = 0.5;
	
	Sphere* s = &sphArr[0];
	Sphere _s;
	Scalar d, force, E_;
	E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
	Vector3 _force;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		_force = Vector3(0,-9.81*_s.mass,0);
		for(int dim = 0; dim<3; dim++){
			if((d = (_s.radius - _s.pos(dim))) > 0){
				force = 4.0f/3.0f*E_*std::sqrt(_s.radius*std::pow(d,3));
				_force(dim) += force;
			}
			if((d = (_s.radius + _s.pos(dim) - boxSize(dim))) > 0){
				force = 4.0f/3.0f*E_*std::sqrt(_s.radius*std::pow(d,3));
				_force(dim) -= force;
			}
		}
		s->acc = _force/_s.mass;
	}
}

quint16 SphereCalculator::doStep(){
	updateData();
	(this->*stepMethod)();
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

void SphereCalculator::doEulerCauchyStep(){
	calculateForces();
	Sphere* s = &sphArr[0];
	Sphere _s;
	Scalar timeStepSqr = timeStep*timeStep;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		s->pos += _s.speed*timeStep + 0.5f*_s.acc*timeStepSqr;
		s->speed += _s.acc*timeStep;
	}
}

void SphereCalculator::doMidpointStep(){
	QVector<Vector3> oldPos(sphCount), oldAcc(sphCount);
	Sphere* s = &sphArr[0];
	Sphere _s;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		oldAcc[i] = s->acc;
		oldPos[i] = s->pos;
		s->pos += s->speed*timeStep/2;
	}
	calculateForces();
	s = &sphArr[0];
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		s->pos = oldPos[i] + s->speed*timeStep + 0.5*s->acc*timeStep*timeStep;
		s->speed += s->acc*timeStep;
	}
}

void SphereCalculator::doRungeKutta4Step(){
	quint16 steps = doRungeKutta4Step_internal(timeStep);
}

quint16 SphereCalculator::doRungeKutta4Step_internal(Scalar step){
	QVector<Vector3> k1(sphCount), k2(sphCount), k3(sphCount), k4(sphCount),
		k5(sphCount), k6(sphCount), k7(sphCount), k8(sphCount), oldPos(sphCount);
	Scalar q, q_max = 0;
	Sphere* s = &sphArr[0];
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		oldPos[i] = s->pos;
		k1[i] = s->speed;
	}
	calculateForces();
	s = &sphArr[0];
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		k5[i] = s->acc;
		k2[i] = k1[i] + 0.5*step*k5[i];
		s->pos += 0.5*step*k1[i];
		s->speed = k1[i] + 0.5*step*k5[i];
	}
	calculateForces();
	s = &sphArr[0];
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		k6[i] = s->acc;
		k3[i] = k1[i] + 0.5*step*k6[i];
		s->pos = oldPos[i] + 0.5*step*k2[i];
		s->speed = k1[i] + 0.5*step*k6[i];
		
		q = std::abs((k3[i][0]-k2[i][0])/(k2[i][0]-k1[i][0]));
		q_max = fmax(q, q_max);
		q = std::abs((k3[i][1]-k2[i][1])/(k2[i][1]-k1[i][1]));
		q_max = fmax(q, q_max);
		q = std::abs((k3[i][2]-k2[i][2])/(k2[i][2]-k1[i][2]));
		q_max = fmax(q, q_max);
	}
	if(q_max>0.1){
		quint16 steps = 0;
		steps += doRungeKutta4Step_internal(step/2);
		steps += doRungeKutta4Step_internal(step/2);
		return steps;
	}else{
		calculateForces();
		s = &sphArr[0];
		for(quint16 i = 0; i<sphCount; ++i, ++s){
			k7[i] = s->acc;
			k4[i] = k1[i] + step*k7[i];
			s->pos = oldPos[i] + 0.5*step*k3[i];
			s->speed = k1[i] + step*k7[i]; //
		}
		calculateForces();
		s = &sphArr[0];
		for(quint16 i = 0; i<sphCount; ++i, ++s){
			k8[i] = s->acc;
			
			s->pos = oldPos[i] + step/6.0 * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
			s->speed = k1[i] + step/6.0 * (k5[i] + 2*k6[i] + 2*k7[i] + k8[i]);
		}
		return 1;
	}
}

void SphereCalculator::doLeapfrogStep(){
	QVector<Vector3> oldAcc(sphCount);
	Sphere* s = &sphArr[0];
	Sphere _s;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		oldAcc[i] = s->acc;
		s->pos += s->speed*timeStep;
	}
	calculateForces();
	s = &sphArr[0];
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		s->pos += 0.5*s->acc*timeStep*timeStep;
		s->speed += 0.5*(oldAcc[i]+s->acc)*timeStep;
	}
}

void SphereCalculator::doSemiImplicitEulerStep(){
	calculateForces();
	Sphere* s = &sphArr[0];
	Sphere _s;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		s->speed += _s.acc*timeStep;
		s->pos += s->speed*timeStep;
	}
}

void SphereCalculator::setIntegratorMethod(quint8 integrMethod){
	integratorMethod = integrMethod;
	
	switch(integrMethod){
	case IntegratorMethods::EulerCauchy:
		stepMethod = &SphereCalculator::doEulerCauchyStep;
		break;
	case IntegratorMethods::Midpoint:
		stepMethod = &SphereCalculator::doMidpointStep;
		break;
	case IntegratorMethods::RungeKutta4:
		stepMethod = &SphereCalculator::doRungeKutta4Step;
		break;
	case IntegratorMethods::Leapfrog:
		stepMethod = &SphereCalculator::doLeapfrogStep;
		break;
	case IntegratorMethods::SemiImplicitEuler:
		stepMethod = &SphereCalculator::doSemiImplicitEulerStep;
		break;
	default:
		integrMethod = IntegratorMethods::RungeKutta4;
		stepMethod = &SphereCalculator::doRungeKutta4Step;
		break;
	}
}

quint8 SphereCalculator::getIntegratorMethod(){
	return integratorMethod;
}
