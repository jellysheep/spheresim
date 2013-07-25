
#ifndef _VERSION_HPP_
#define _VERSION_HPP_

#ifndef VERSION
	#define VERSION unknown
#endif

#define TOSTR2(x) #x
#define TOSTR(x) TOSTR2(x)
#define VERSION_STR TOSTR(VERSION)

#endif
