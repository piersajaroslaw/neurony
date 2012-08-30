/*
 * spektrumGrafuLapack.h
 *
 *  Created on: May 24, 2010
 *      Author: piersaj
 */

#ifndef SPEKTRUMGRAFULAPACK_H_
#define SPEKTRUMGRAFULAPACK_H_

#define WITH_LAPACK
//#undef WITH_LAPACK

/*
 * requires -llapack
 *
 * sudo yum install lapack lapack-devel
 */
#ifdef WITH_LAPACK

#include "../../network/siecNeuronowa.h"


/*
http://seehuhn.de/pages/linear#lapack-easy
cc testdgtsv.c -o testdgtsv -llapack -lblas
*/



void policzWartosciWlasneLapack(SiecNeuronowa &n);
void policzWartosciWlasneLapackSilent(SiecNeuronowa &n, bool silent = false);
void zapiszWartosciWlasneLapack(SiecNeuronowa &n);
float * getWartosciWlasneLapack();

extern float * wartosciWlasne;

#endif // WITH_LAPACK
#endif /* SPEKTRUMGRAFULAPACK_H_ */
