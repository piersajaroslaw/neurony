/*
 * spektrumGrafu.h
 *
 *  Created on: Feb 18, 2010
 *      Author: piersaj
 */

#ifndef SPEKTRUMGRAFU_H_
#define SPEKTRUMGRAFU_H_
/*
 *
 * enabling GSL will require
 *  -lgsl -lglscblas
 *
 */
#ifdef WITH_GSL

#include "../../network/siecNeuronowa.h"
#include <gsl/gsl_math.h>
/* note: requires
 * gsl, gsl-devel
 *
 * sodu yum list gsl*
 * sudo yum install gsl*
 *
 * gcc -Wall file.c -lgsl -lgslcblas -lm
 *
 */

/*  NIE DOTYKAC!!
 *
 *  rozklad[i] = ilosc wpisow o wartosci i
 *  indeks =  (i*skal+shift) \in [0..len-1];
 */
extern const int spektrumLen;
extern float spektrumSkal;
extern float spektrumShift;

extern int * spektrumRozklad;


void policzRozkladSpektralny(SiecNeuronowa &);
void zapiszRozkladSpektralnyM();
void zapiszWartosciWlasneM(SiecNeuronowa &);

#endif // WITH_GSL

#endif /* SPEKTRUMGRAFU_H_ */
