/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _DIMENSION_HPP_
#define _DIMENSION_HPP_

#include <QTextStream>

namespace SphereSim
{

	/** \brief State of a 2D or 3D simulation. */
	class Dimension
	{
	private:
		/** \brief Flag if 2D or 3D spheres are simulated. */
		static bool _is3D;

		/** \brief Update the dimension state.
		 * \param b True if 3D and false if 2D. */
		static void updateIs3D(bool b)
		{
			_is3D = b;
		}

	public:
		Dimension() = delete;
		Dimension(const Dimension&) = delete;
		Dimension& operator=(const Dimension&) = delete;

		/** \brief Get the dimension state.
		 * \return True if 3D and false if 2D. */
		static bool getIs3D()
		{
			return _is3D;
		}

	};
}

#endif
