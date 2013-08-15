/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _DIMENSION_HPP_
#define _DIMENSION_HPP_

#include <QTextStream>

namespace SphereSim{
	
	/**
	 * \brief Helper class that saves dimension state.
	 */
	class Dimension{
	private:
		/** \brief Tells if 2D or 3D spheres are simulated. */
		static bool _is3D;
		
		/**
		 * \brief Sets the dimension state.
		 * \param b True if 3D and false if 2D.
		 */
		static void set3D(bool b){
			_is3D = b;
		}
		
	public:
		/**
		 * \brief Gets the dimension state.
		 * \return True if 3D and false if 2D.
		 */
		static bool is3D(){
			return _is3D;
		}
		
	};
}

#endif
