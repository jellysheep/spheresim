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

quint16 SphereManager::addSphere(Sphere s){
	spheres.append(s);
	return getCount();
}
quint16 SphereManager::addSphere(){
	return addSphere(Sphere());
}

quint16 SphereManager::removeSphere(quint16 i){
	if(getCount()>i){
		spheres.remove(i);
	}
	return getCount();
}
quint16 SphereManager::removeLastSphere(){
	if(getCount()>0){
		return removeSphere(getCount()-1);
	}else{
		return getCount();
	}
}

quint16 SphereManager::getCount(){
	return spheres.size();
}

quint16 SphereManager::updateSphere(quint16 i, Sphere s){
	if(getCount()>i){
		spheres[i] = s;
	}
	return getCount();
}

Sphere SphereManager::getSphere(quint16 i){
	if(getCount()>i){
		return spheres[i];
	}else{
		return Sphere();
	}
}

quint32 SphereManager::calculateStep(){
	return sphCalc.doStep();
}

SphereCalculator& SphereManager::getSphereCalculator(){
	return sphCalc;
}
