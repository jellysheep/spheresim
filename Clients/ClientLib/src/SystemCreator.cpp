/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <ActionSender.hpp>
#include <SystemCreator.hpp>

#include <QTimer>
#include <random>
#include <chrono>

using namespace SphereSim;

SystemCreator::SystemCreator(ActionSender* sender)
{
	actionSender = sender;
}

Scalar SystemCreator::createArgonGasSystem(quint16 sphereCount)
{
	quint16 sphereCountSqrt = (quint16)sqrt(sphereCount);
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	
	Scalar boxLength = sphereCountSqrt/8.0*20.0e-9;
	actionSender->updateBoxSize(Vector3(boxLength, boxLength, boxLength));
	float radius = 0.017*boxLength;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration timepoint = now.time_since_epoch();
	std::default_random_engine generator(timepoint.count());
	std::uniform_real_distribution<float> distribution(-radius/4, radius/4);
	std::uniform_real_distribution<float> distribution2(-1, 1);
	
	Sphere s;
	s.radius = 1.5*radius;
	s.speed.setZero();
	s.acc.setZero();
	s.mass = 6.6335e-26;
	Vector3 boxSize(boxLength,boxLength,boxLength);
	boxSize /= 2;
	for(unsigned int i = 0; i<sphereCount; i++)
	{
		actionSender->addSphere();
		s.pos = boxSize;
		s.pos(0) += 7.5f*8/sphereCountSqrt*radius*((sphereCountSqrt-1)/2.0-(i%sphereCountSqrt));// + distribution(generator);
		s.pos(1) += 7.5f*8/sphereCountSqrt*radius*((sphereCountSqrt-1)/2.0-(i/sphereCountSqrt));// + distribution(generator);
		s.pos(2) = 0;
		s.speed(0) = 10*distribution2(generator);
		s.speed(1) = 10*distribution2(generator);
		actionSender->updateSphere(i, s);
	}
	
	actionSender->updateCollisionDetection(false);
	actionSender->updateGravityCalculation(false);
	actionSender->updateGravitationalConstant(1.0e-4);
	actionSender->updateLennardJonesPotentialCalculation(true);
	actionSender->updateEarthGravity(Vector3(0,0,0));
	actionSender->updateWallE(0);
	
	return boxLength;
}
