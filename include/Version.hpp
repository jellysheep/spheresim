/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _VERSION_HPP_
#define _VERSION_HPP_

#ifndef VERSION
	#define VERSION unknown
#endif

#define TOSTR2(x) #x
#define TOSTR(x) TOSTR2(x)
#define VERSION_STR TOSTR(VERSION)

#endif
