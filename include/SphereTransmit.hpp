/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SPHERETRANSMIT_HPP_
#define _SPHERETRANSMIT_HPP_

#include <Sphere.hpp>

#include <QDataStream>

namespace SphereSim{
	/**
	 * \brief Writes size and position of a sphere to a stream.
	 * \param stream Stream to write to.
	 * \param s Sphere to send.
	 */
	void writeSphere(QDataStream& stream, Sphere& s){
		stream<<s.radius<<s.pos(0)<<s.pos(1)<<s.pos(2);
	}
	
	/**
	 * \brief Reads and updates size and position of a sphere from a stream.
	 * \param stream Stream to read from.
	 * \param s Sphere to update.
	 */
	void readSphere(QDataStream& stream, Sphere& s){
		stream>>s.radius>>s.pos(0)>>s.pos(1)>>s.pos(2);
	}
	
	/**
	 * \brief Writes size, mass, position, speed and acceleration of a sphere to a stream.
	 * \param stream Stream to write to.
	 * \param s Sphere to send.
	 */
	void writeFullSphere(QDataStream& stream, Sphere& s){
		stream<<s.radius<<s.mass;
		stream<<s.pos(0)<<s.speed(0)<<s.acc(0);
		stream<<s.pos(1)<<s.speed(1)<<s.acc(1);
		stream<<s.pos(2)<<s.speed(2)<<s.acc(2);
	}
	
	/**
	 * \brief Reads and updates size, mass, position, speed and acceleration of a sphere from a stream.
	 * \param stream Stream to read from.
	 * \param s Sphere to update.
	 */
	void readFullSphere(QDataStream& stream, Sphere& s){
		stream>>s.radius>>s.mass;
		stream>>s.pos(0)>>s.speed(0)>>s.acc(0);
		stream>>s.pos(1)>>s.speed(1)>>s.acc(1);
		stream>>s.pos(2)>>s.speed(2)>>s.acc(2);
	}
}

#endif
