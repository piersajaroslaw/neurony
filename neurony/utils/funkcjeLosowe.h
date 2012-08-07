/*
 * funkcjeLosowe.h
 *
 *  Created on: 06-Dec-2008
 *      Author: piersaj
 *
 *  2008-12-05
 *  	korekty do Poissona (przyblizenie rozkladem normalnym),
 *  2008-12-06
 *  	przerzucono tu funkcje losujace z innego pliku, ktory jest teraz o ok 100 linii czytelniejszy
 */


#ifndef FUNKCJELOSOWE_H_
#define FUNKCJELOSOWE_H_

/* losuje liczbe z rozkladu jednostajnego na (0..1)
 *
 */
float losujRUnif();
/* losuje liczbe  z rozkladu jednostajnego na (0..max)
 *
 */
float losujRUnif(float);
/* losuje liczbe z rozkladu normalnego standardowego N(0,1)
 *
 */
float losujRNorm();
/* losuje liczbe z rozkladu Poissona z parametrem lambda > 0
 *
 */
float losujRPoiss(float);
/* losuje liczbe z rozkladu zerojedynkowego z parametrem  0<p<1
 * P( X == 1 ) == p
 * P( X == 0 ) == 1-p
 *
 */
int losujRBool(float);

/* losuje liczbe calkowita z przedzialu 0.. max-1
 *
 */
int losujRInt(int max);

/* inicjalizuje generator / ziarno
 *
 */
void losujZiarno();


#endif /* FUNKCJELOSOWE_H_ */
