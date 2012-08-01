/*
 * nawigacja.h
 *
 * 2008-12-12
 * 		przerzucono tu nawigacje klawiszami z innego pliku
 * 		i do tego obsluga myszki (ale tylko w zakresie okna!
 * 		nie wiem jak łapać ruch myszki z poza okna gdy lmb jest w ciśnięty - na pewno się da :P)
 *
 *
 *  Created on: 12-Dec-2008
 *      Author: Piersa Jaroslaw
 */

#ifndef OBSLUGAMYSZKI_H_
#define OBSLUGAMYSZKI_H_

#include <GL/gl.h>


#ifdef WITH_GL


#include <GL/glu.h>
#include <SDL/SDL.h>
#include "wyswietlanie.h"

bool myszka;
int myszkaPosX = -1;
int myszkaPosY = -1;

float angleH = 0;
float angleV = 0;
float odlegloscKamery = 15;

float krok = .2;

// tablica pamietajaca czy klawidz jest wcisniety
bool key[321];

void obslugaMyszki(SDL_Event);
void obslugaKolkaMyszki(SDL_Event);
void obslugaRuchuMyszki(SDL_Event);
void ustawKamere();

void obslugaKlawiatury(SDL_Event event);
void obslugaKlawiszyNawigacyjnych();

/* *********************************************
 *
 * implementacja
 * przeniesc do nawigacja.cpp
 *
 * ********************************************/

void ustawKamere(){
	// najpierw sie odsuwamy
	glTranslatef( 0  , 0, - odlegloscKamery);
	// potem nachyamy
	glRotatef(angleV, 1, 0, 0);
	// dalej obracamy wokol kostki
	glRotatef(angleH, 0, 1, 0);
}	// ustawia kamere

void resetujKamere(){
	angleH = 0;
	angleV = 0;
	odlegloscKamery = 15;
}	// resetujKamere

void obslugaMyszki(SDL_Event event){

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
		myszka = true;
		return;
	}	// fi

	if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT){
		myszka = false;
		myszkaPosX = myszkaPosY = -1;
		return;
	}	// fi


}	// obslugaMyszki


void obslugaRuchuMyszki(SDL_Event event){
	if (!myszka)
		return;

	if (myszkaPosX == -1){
		myszkaPosX = (int) event.motion.x;
		myszkaPosY = (int) event.motion.y;
	} else {
		int dx = myszkaPosX - (int) event.motion.x;
		int dy = myszkaPosY - (int) event.motion.y;

		myszkaPosX = (int) event.motion.x;
		myszkaPosY = (int) event.motion.y;

		if (dx > 100 || dx< -100 ||  dy >100 || dy < -100){
			return;
		}	// fi

		angleV -= dy * krok;
		if (angleV < -89.9){
			angleV = -89.9;
		}	// if
		if (angleV > 89.9){
			angleV = 89.9;
		}	// if


		angleH -= dx * krok;
	}	// if
}	// obsluga myszki


void obslugaKolkaMyszki(SDL_Event event){

	if (event.button.button == SDL_BUTTON_WHEELDOWN){
		odlegloscKamery += krok;
		if (odlegloscKamery >50)
			odlegloscKamery = 50;
	}	// fi

	if (event.button.button == SDL_BUTTON_WHEELUP){
		odlegloscKamery -= krok;
		if (odlegloscKamery < 2 * krok)
			odlegloscKamery = krok;
	}	// fi
}	// mouseWheel


void obslugaKlawiatury(SDL_Event event){
	switch (event.type){
		case SDL_KEYDOWN : key[ event.key.keysym.sym ]=true ;   break;
		case SDL_KEYUP   : key[ event.key.keysym.sym ]=false;   break;
	}
}	// obslugaKlawiatury

void obslugaKlawiszyNawigacyjnych(){
	// Check keypresses
	if( key[SDLK_PAGEDOWN] )
		{ angleV =  (angleV < -89.5) ? -89.5 : angleV-.5 ; 		}
	if( key[SDLK_PAGEUP] )
		{ angleV =  (angleV > +89.5) ? 89.5 : angleV+.5 ; }
	if( key[SDLK_RIGHT] )
		{ angleH-=0.5; }
	if( key[SDLK_LEFT ] )
		{ angleH+=0.5; }
	if (key[SDLK_UP]){
		odlegloscKamery = ( odlegloscKamery <= .2+.2   ?  0.2 : odlegloscKamery -.2);
	}
	if (key[SDLK_DOWN])
		{odlegloscKamery += .2;}
}	// obslugaKlawiszy

#endif // ZERO

#endif /* OBSLUGAMYSZKI_H_ */
