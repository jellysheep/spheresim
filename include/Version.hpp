/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _VERSION_HPP_
#define _VERSION_HPP_

#ifndef VERSION
    #define VERSION unknown
#endif /*VERSION*/

#define TOSTR2(x) #x
#define TOSTR(x) TOSTR2(x)
#define VERSION_STR TOSTR(VERSION)

#endif /*_VERSION_HPP_*/
