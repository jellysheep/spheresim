/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include <Vector.hpp>

/**
 * \brief Holds physical data of one sphere.
 */
class Sphere{
public:
	Vector3 pos, speed, acc;
	Scalar mass;
};

#endif
