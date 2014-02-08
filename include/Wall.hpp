/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _WALL_HPP_
#define _WALL_HPP_

#include <Vector.hpp>

class QDataStream;

namespace SphereSim
{
	/** \brief Physical data of a 2D wall. */
	class Wall2D
	{
	public:
		/** \brief Position of the wall. */
		Vector2 pos;
		/** \brief Wall vector. */
		Vector2 vec;
		
		/** \brief Initialize Wall2D.
		 * \param x Wall position.
		 * \param v Wall vector. */
		Wall2D(Vector2 x, Vector2 v)
		{
			pos = x;
			vec = v;
		}
		
		/** \copybrief Wall2D */
		Wall2D():Wall2D(Vector2(0,0),Vector2(0,0))
		{
		}
		
	};
	
	/** \brief Physical data of a 3D wall. */
	class Wall3D
	{
	public:
		/** \brief Position of the wall. */
		Vector3 pos;
		/** \brief First wall vector. */
		Vector3 vec1;
		/** \brief Second wall vector. */
		Vector3 vec2;
		
		/** \brief Initialize Wall3D.
		 * \param x Wall position.
		 * \param v1 Wall vector 1.
		 * \param v2 Wall vector 2. */
		Wall3D(Vector3 x, Vector3 v1, Vector3 v2)
		{
			pos = x;
			vec1 = v1;
			vec2 = v2;
		}
		
		/** \copybrief Wall3D */
		Wall3D():Wall3D(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0))
		{
		}
		
	};
}

#endif
