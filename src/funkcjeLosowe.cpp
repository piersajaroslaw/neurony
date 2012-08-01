/*
 * funkcjeLosowe.c
 *
 *  Created on: 07-Feb-2009
 *      Author: piersaj
 */



#include "funkcjeLosowe.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

float losujRUnif(){
	return (float) (rand() * 1.0f) / RAND_MAX;
}	// losuj r Unif


float losujRUnif(float max){
	return losujRUnif() * max;
}	// losuj r Unif


/* Rozklad normalny N(0,1)
 *
 * taki bardzo przyblizony,
 * chyba
 * ewentualnie pomyslec nad ladniejszym (box-miller)
 *
 * (\sum{i=1..12}{X(i)} - 12 * EX) / \sqrt{12 * D^2X} =CLT= N(0,1)
 */
float losujRNorm(){
	float suma = 0;

	for (int i=0; i< 12; i++)
		suma += (rand() * 1.0f) / RAND_MAX;

	return suma - 6.0f;
}	// losuj r Norm

/* zwraca 1 z prawdopodobiestwem d
 * i 0 z prawdopodobieństwem 1-d
 */
int losujRBool(float d){
	if (losujRUnif() < d)
		return 1;
	else
		return 0;

}	// losuj R boolean

/* rozklad Poissona z parametrem lambda za wiki
 * dla duzych n (n>500) jest przyblizane rozkladem normalnym
 */
float losujRPoiss(float lambda){
	// gdy lambda jest duze przyblizamy rozkladem normalnym N(lambda , lambda)
	if (lambda >500){
			return (int)  (sqrt(lambda) * losujRNorm() + lambda);
	}	// if

	// jak male
	// u mnie sie sypie przy 700

	double l = exp(-lambda);
	int k = 0;
	double p = 1;

	do {
		k++;
		double u = losujRUnif();
		p*= u;
	} while (p>l);

	return k-1;
}	// losujRPoiss()

int losujRInt(int max){
	return rand() % max;
}	// losujRint()

void losujZiarno(){
	srand(time(NULL));
}	// losujZiarno()

