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

using namespace SphereSim;

SphereManager::SphereManager(){
	count = 0;
	spheres = NULL;
}

int SphereManager::addSphere(Sphere s){
	return count;
}
int SphereManager::addSphere(){
	return addSphere(Sphere());
}
