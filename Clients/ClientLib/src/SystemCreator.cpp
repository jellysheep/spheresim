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

Scalar getMaxwellBoltzmannDistribution(Scalar random)
{
	double s = random;
	double x;
	if (s < 0.346691)
	{
		x = 1.0642622768277956e-6 * sqrt(2.1347980205189531e12 * pow(s, 0.6666666666666666) +
			1.0323831697580514e12 * pow(s, 1.3333333333333333) + 6.775644406955992e11 * s * s +
			5.0331929839908344e11 * pow(s, 2.6666666666666665) +
			3.9996925883820276e11 * pow(s, 3.3333333333333335) + 3.3163359755658606e11 * pow(s, 4) +
			2.831279191020564e11 * pow(s, 4.666666666666667));
	}
	else if (s < 0.796567)
	{
		x = 0.014527571050476862 + s * (9.294742731011485 +
			s * (-37.43519439668487 + s * (105.40885039276776 +
			s * (-183.6190579712269 + s * (196.7067172374017 +
			s * (-118.99569035897035 + 31.66450688674963 * s))))));
	}
	else
	{
		x = (-665.6950297578719 + s * (13652.127578664917 +
			s * (-47641.68123439248 + s * (73592.91391855325 +
			s * (-60775.51067786972 + (27088.54652502034 - 5250.64365989075 * s) * s))))) /
			(2004.625993588326 + s * (-5790.900684890041 + (5572.531218092137 - 1786.242621106215 * s) * s));
	}
	return x;
}

using namespace SphereSim;

SystemCreator::SystemCreator(ActionSender* sender)
{
	actionSender = sender;
}

Scalar SystemCreator::createArgonGasSystem(quint16 sphereCount, Scalar targetTemperature)
{
	quint16 sphereCountSqrt = (quint16)sqrt(sphereCount);
	sphereCount = sphereCountSqrt*sphereCountSqrt;
	
	Scalar boxLength = sphereCountSqrt/8.0*2.5e-9;
	actionSender->updateBoxSize(Vector3(boxLength, boxLength, boxLength));
	float radius = 0.017*boxLength;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration timepoint = now.time_since_epoch();
	std::default_random_engine generator(timepoint.count());
	std::uniform_real_distribution<float> distribution(-radius/4, radius/4);
	std::uniform_real_distribution<float> distribution2(0, 1);
	
	Sphere s;
	s.radius = 0.5*radius;
	s.speed.setZero();
	s.acc.setZero();
	s.mass = 6.6335e-26;
	Vector3 boxSize(boxLength,boxLength,boxLength);
	boxSize /= 2;
	Scalar speed, angle1, angle2;
	Scalar a = pow(0.66*1.3806504e-23*targetTemperature/s.mass, 1.0/2.0);
	for(unsigned int i = 0; i<sphereCount; i++)
	{
		actionSender->addSphere();
		s.pos = boxSize;
		s.pos(0) += 7.5f*8/sphereCountSqrt*radius*((sphereCountSqrt-1)/2.0-(i%sphereCountSqrt));
		s.pos(1) += 7.5f*8/sphereCountSqrt*radius*((sphereCountSqrt-1)/2.0-(i/sphereCountSqrt));
		s.pos(2) += boxLength/2*distribution2(generator);
		speed = a*getMaxwellBoltzmannDistribution(distribution2(generator));
		angle1 = 2*M_PI*distribution2(generator);
		angle2 = 2*M_PI*distribution2(generator);
		s.speed(0) = speed*sin(angle1)*cos(angle2);
		s.speed(1) = speed*sin(angle1)*sin(angle2);
		s.speed(2) = speed*cos(angle1);
		actionSender->updateSphere(i, s);
	}
	actionSender->updateTargetTemperature(targetTemperature);
	
	actionSender->updateCollisionDetection(false);
	actionSender->updateGravityCalculation(false);
	actionSender->updateGravitationalConstant(1.0e-4);
	actionSender->updateLennardJonesPotentialCalculation(true);
	actionSender->updateEarthGravity(Vector3(0,0,0));
	actionSender->updateWallE(0);
	actionSender->updatePeriodicBoundaryConditions(true);
	
	return boxLength;
}