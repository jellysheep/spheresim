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

#include <QDebug>
#include <cmath>

using namespace SphereSim;

SphereCalculator::SphereCalculator(){
	qDebug()<<"SphereCalculator: constructor called";
	updateData();
	boxSize = Vector3(1,1,1);
	timeStep = 0.002;
}

QVector<Sphere>& SphereCalculator::getSpheres(){
	return spheres;
}

void SphereCalculator::calculateForces(){
	qDebug()<<"SphereCalculator: calculating "<<sphCount<<" sphere forces";
	
	static Scalar E = 50000, poisson = 0.5;
	
	Sphere* s = &sphArr[0];
	Sphere _s;
	Scalar d, force, E_;
	E_ = 1/(((1-poisson*poisson)/E)+((1-poisson*poisson)/E));
	Vector3 _force;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		_s.pos += 0.5f*timeStep*_s.speed;
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

void SphereCalculator::moveStep(){
	qDebug()<<"SphereCalculator: moving "<<sphCount<<" spheres";
	Sphere* s = &sphArr[0];
	Sphere _s;
	Scalar timeStepSqr = timeStep*timeStep;
	for(quint16 i = 0; i<sphCount; ++i, ++s){
		_s = *s;
		s->pos += s->speed*timeStep + 0.5f*_s.acc*timeStepSqr;
		s->speed += _s.acc*timeStep;
	}
}

quint16 SphereCalculator::doStep(){
	updateData();
	calculateForces();
	moveStep();
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
