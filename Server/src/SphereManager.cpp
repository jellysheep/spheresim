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

SphereManager::SphereManager(){
	qDebug()<<"ActionSender: constructor called";
}

int SphereManager::addSphere(Sphere s){
	qDebug()<<"ActionSender: addSphere";
	spheres.append(s);
	return getCount();
}
int SphereManager::addSphere(){
	return addSphere(Sphere());
}

int SphereManager::removeSphere(unsigned int i){
	qDebug()<<"ActionSender: removeSphere";
	if(i>=0 && getCount()>i){
		spheres.remove(i);
	}
	return getCount();
}
int SphereManager::removeLastSphere(){
	if(getCount()>0){
		return removeSphere(getCount()-1);
	}else{
		return getCount();
	}
}

int SphereManager::getCount(){
	qDebug()<<"ActionSender: getGount";
	return spheres.size();
}
