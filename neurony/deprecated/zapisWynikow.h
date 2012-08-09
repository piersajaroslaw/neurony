/*
 * zapisWynikow.h
 *
 *  Created on: Jul 3, 2009
 *      Author: piersaj
 */
#if 0

#ifndef ZAPISWYNIKOW_H_
#define ZAPISWYNIKOW_H_


#include <fstream>
#include <iostream>
#include <cstdlib>
#include "siecNeuronowa.h"

/*
 * zapis do .sk
 */
void zapiszWynikiSketch(SiecNeuronowa &);

void zapiszNeuronySk(SiecNeuronowa &);

void zapiszGrafyPrzeplywuSk(SiecNeuronowa &);

void zapiszRozkladStopniSk(SiecNeuronowa&);

void zapiszOsieSk(SiecNeuronowa &, std::fstream&);

void zapiszOpisSieciSk(SiecNeuronowa &, std::fstream&);

void zapiszWielkoscObrazuSk(std::fstream &str, float unit = .2);


/*
 * Zapis do plikow .txt
 */
void zapiszWynikiTxt(SiecNeuronowa &);

void zapiszWynikiRozkladStopniTxt(SiecNeuronowa &);

void zapiszWspSkupieniaTxt(SiecNeuronowa & );

void zapiszHistogramPrzyjetychIteracjiTxt(SiecNeuronowa & );

void zapiszHistogramCzasuEpokTxt(SiecNeuronowa &);

void zapiszRozkladCCm(SiecNeuronowa &);

void zapiszMacierzGrafuPrzeplywuM(SiecNeuronowa &);

char * okreslKolor(int i);

#endif /* ZAPISWYNIKOW_H_ */

#endif
