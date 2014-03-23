/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <SimulatedSystem.hpp>
#include <Vector.hpp>
#include <Version.hpp>
#include <Integrators.hpp>
#include <Connection.hpp>
#include <Console.hpp>

using namespace SphereSim;
using namespace SphereSim::SimulationVariables;

SimulatedSystem::SimulatedSystem()
	:vars()
{
	vars.reserve(numberOfVariables);

	addVariable(serverVersion, Object::STRING, std::string(VERSION_STR));
	addVariable(serverFloatingType, Object::STRING, std::string(TOSTR(FLOATING_TYPE)));
	addVariable(sphereCount, Object::INT, 0);
	addVariable(timeStep, Object::DOUBLE, 0.002);
	addVariable(integratorMethod, Object::INT, (int)IntegratorMethods::CashKarp54);
	addVariable(simulating, Object::BOOL, false);
	addVariable(frameSending, Object::BOOL, false);
	addVariable(collisionDetection, Object::BOOL, true);
	addVariable(gravityCalculation, Object::BOOL, false);
	addVariable(lennardJonesPotential, Object::BOOL, false);
	addVariable(maximumStepDivision, Object::INT, 16);
	addVariable(maximumStepError, Object::SCALAR, 1.0e-4);
	addVariable(sphereE, Object::SCALAR, 5000);
	addVariable(spherePoissonRatio, Object::SCALAR, 0.5);
	addVariable(wallE, Object::SCALAR, 5000);
	addVariable(wallPoissonRatio, Object::SCALAR, 0.5);
	addVariable(earthGravity, Object::VECTOR3, Vector3(0, -9.81, 0));
	addVariable(gravitationalConstant, Object::SCALAR, 6.67384e-11);
	addVariable(boxSize, Object::VECTOR3, Vector3(1, 1, 1));
	addVariable(targetTemperature, Object::SCALAR, 0);
	addVariable(periodicBoundaryConditions, Object::BOOL, false);
	addVariable(maximumTheta, Object::SCALAR, 0.5);
	addVariable(kBoltzmann, Object::SCALAR, 1.3806504e-23);
	addVariable(lenJonPotEpsilon, Object::SCALAR, 1.6540e-21);
	addVariable(lenJonPotSigma, Object::SCALAR, 0.3405e-9);
	addVariable(allVariablesReceived, Object::BOOL, false);
}

template <class T>
void SimulatedSystem::addVariable(SimulationVariables::Variable var, Object::Type type, const T& t)
{
	if(var != vars.size())
		throw new std::exception();
	vars.push_back(Object(type, t));
}

void SimulatedSystem::sendVariable(SimulationVariables::Variable var)
{
	QByteArray bytes;
	if(var == allVariablesReceived)
		bytes = QByteArray(2, Object::BOOL);
	else
	{
		Object &o = vars[var];
		bytes = o.getData();
	}

	QByteArray data;
	data.append(var/256);
	data.append(var%256);
	data.append(bytes);
	emit variableToSend(data);
}

void SimulatedSystem::sendAllVariables()
{
	for(int var = 0; var < numberOfVariables; var++)
		sendVariable((SimulationVariables::Variable) var);
}

void SimulatedSystem::receiveVariable(SimulationVariables::Variable var, QByteArray data)
{
	if(var >= numberOfVariables) return;

	Object &o = vars[var];
	if(o.setData(data))
	{
		emit variableUpdated((int)var);
		if(var == allVariablesReceived)
			emit receivedAllVariables();
	}
}

void SimulatedSystem::printUpdatedVariable(int var)
{
	Object::Type type = vars[var].getType();
	Console::out<<"Variable: "<<var<<"\tType: "<<(char)type<<"\n";
}
