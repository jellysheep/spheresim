/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SIMULATEDSYSTEM_HPP_
#define _SIMULATEDSYSTEM_HPP_

#include <Object.hpp>
#include <Actions.hpp>

#include <QObject>
#include <QByteArray>
#include <vector>
#include <set>

namespace SphereSim
{
	/** \brief Storage for physical constants and other parameters. */
	class SimulatedSystem : public QObject
	{
		Q_OBJECT

	private:
		std::vector<Object> vars;

		template <typename T>
		void addVariable(SimulationVariables::Variable var, Object::Type type,
			const T& t);

		void sendVariable(SimulationVariables::Variable var);

	public:
		SimulatedSystem();

		SimulatedSystem(const SimulatedSystem&) = delete;
		SimulatedSystem& operator=(const SimulatedSystem&) = delete;

		template <typename T>
		const T get(SimulationVariables::Variable var) const
		{
			return vars[var].get<T>();
		}

		template <typename T>
		const T& getRef(SimulationVariables::Variable var) const
		{
			return vars[var].getRef<T>();
		}

		template <typename T>
		void set(SimulationVariables::Variable var, const T& t)
		{
			if (vars[var].set<T>(t))
			{
				sendVariable(var);
				emit variableUpdated((int)var);
			}
		}

		void receiveVariable(SimulationVariables::Variable var, QByteArray data);

	signals:
		void variableToSend(QByteArray data);

		void variableUpdated(int var);

		void receivedAllVariables();

	public slots:
		void sendAllVariables();

		void printUpdatedVariable(int var);

	};
}

#endif
