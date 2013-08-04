/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _WALL_HPP_
#define _WALL_HPP_

#include <Vector.hpp>

class QDataStream;

namespace SphereSim{
	/**
	 * \brief Holds physical data of a 2D wall.
	 */
	class Wall2D{
	public:
		/** \brief Stores the position of the wall. */
		Vector2 pos;
		/** \brief Stores the wall vector. */
		Vector2 vec;
		
		Wall2D(Vector2 x, Vector2 v){
			pos = x;
			vec = v;
		}
		Wall2D():Wall2D(Vector2(0,0),Vector2(0,0)){
		}
		
	};
	
	/**
	 * \brief Holds physical data of a 3D wall.
	 */
	class Wall3D{
	public:
		/** \brief Stores the position of the wall. */
		Vector3 pos;
		/** \brief Stores the first wall vector. */
		Vector3 vec1;
		/** \brief Stores the second wall vector. */
		Vector3 vec2;
		
		Wall3D(Vector3 x, Vector3 v1, Vector3 v2){
			pos = x;
			vec1 = v1;
			vec2 = v2;
		}
		Wall3D():Wall3D(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0)){
		}
		
	};
}

#endif
