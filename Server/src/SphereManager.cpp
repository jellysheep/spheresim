/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <SphereManager.hpp>

#include <QDebug>

using namespace SphereSim;

SphereManager::SphereManager():sphCalc(),spheres(sphCalc.getSpheres()){
	qDebug()<<"SphereManager: constructor called";
}

unsigned int SphereManager::addSphere(Sphere s){
	qDebug()<<"SphereManager: addSphere";
	spheres.append(s);
	return getCount();
}
unsigned int SphereManager::addSphere(){
	return addSphere(Sphere());
}

unsigned int SphereManager::removeSphere(unsigned int i){
	qDebug()<<"SphereManager: removeSphere";
	if(getCount()>i){
		spheres.remove(i);
	}
	return getCount();
}
unsigned int SphereManager::removeLastSphere(){
	if(getCount()>0){
		return removeSphere(getCount()-1);
	}else{
		return getCount();
	}
}

unsigned int SphereManager::getCount(){
	qDebug()<<"SphereManager: getGount";
	return spheres.size();
}

unsigned int SphereManager::updateSphere(unsigned int i, Sphere s){
	qDebug()<<"SphereManager: updateSphere";
	qDebug()<<"SphereManager: before:";
	qDebug()<<"SphereCalculator: pos "<<i<<": "<<s.pos(0)<<s.pos(1)<<s.pos(2);
	qDebug()<<"SphereCalculator: speed "<<i<<": "<<s.speed(0)<<s.speed(1)<<s.speed(2);
	qDebug()<<"SphereCalculator: acc "<<i<<": "<<s.acc(0)<<s.acc(1)<<s.acc(2);
	if(getCount()>i){
		spheres[i] = s;
	}
	qDebug()<<"SphereManager: after:";
	qDebug()<<"SphereCalculator: pos "<<i<<": "<<s.pos(0)<<s.pos(1)<<s.pos(2);
	qDebug()<<"SphereCalculator: speed "<<i<<": "<<s.speed(0)<<s.speed(1)<<s.speed(2);
	qDebug()<<"SphereCalculator: acc "<<i<<": "<<s.acc(0)<<s.acc(1)<<s.acc(2);
	return getCount();
}

Sphere SphereManager::getSphere(unsigned int i){
	qDebug()<<"SphereManager: getSphere";
	if(getCount()>i){
		return spheres[i];
	}else{
		return Sphere();
	}
}

unsigned int SphereManager::calculateStep(){
	qDebug()<<"SphereManager: calculateStep";
	return sphCalc.doStep();
}

SphereCalculator& SphereManager::getSphereCalculator(){
	return sphCalc;
}
