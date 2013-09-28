/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <ButcherTableau.hpp>
#include <Console.hpp>

#include <QDebug>

using namespace SphereSim;

ButcherTableau::ButcherTableau():ButcherTableau(0){
}

ButcherTableau::ButcherTableau(quint8 _order):order(_order),a(),b(),b_(),c(){
}

ButcherTableau::ButcherTableau(quint8 _order, const Scalar* _a, const Scalar* _b, const Scalar* _b_, const Scalar* _c):ButcherTableau(_order){
	quint16 counter = 0;
	Scalar checksum;
	for(quint8 x = 0; x<order; x++){
		checksum = 0.0;
		for(quint8 y = 0; y<order; y++){
			a[x][y] = _a[counter++];
			checksum += a[x][y];
		}
		b[x] = _b[x];
		b_[x] = _b_[x];
		c[x] = _c[x];
		if(fabs(checksum-c[x])>0.0001*(checksum+c[x])){
			Console::redBold<<"Error: Butcher tableau checksum is wrong!\n";
		}
	}
}
