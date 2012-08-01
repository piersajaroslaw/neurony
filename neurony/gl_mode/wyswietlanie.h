/*
 * wyswietlanie.h
 *
 *  Created on: 07-Dec-2008
 *      Author: piersaj
 *
 *  2008-12-07
 *  	skopiowanie z innego pliku :P
 *  	i dopisanie kilku swoich podejsc
 *  2008-12-12
 *  	dodany gryzon i... przerzucony do oddzielnego pliku
 *  2008-12-17
 *  	dodany dobor progu odcinajacego i DisplayList-y
 *  2009-04-19
 *  	dodano sterowanie progami dla MNK - qa/ ws
 *  	dodano reset kamery - r
 *  2009-04-26
 *  	dodano ukrywanie punktow o zerowym ladunku - t
 *  	dodano wyswietlanie tekstu 2d
 *  2009-06-12
 *  	dorzucono tryb wyswietlania krawedzi - y u i - nic, drzewka, krawedzi o najwiekszym przeplywie
 *  200-07-09
 *  	dodan tryb pokazywania lub nie wykresow --- p (plot :))
 *
 *
 *  -lGL -lGLU -lSDL -lSDL_ttf
 * http://www.ferdychristant.com/blog/articles/DOMM-72MPPE
 * kompilowac / dodac do libraries!!!!
 *
 */

#ifndef WYSWIETLANIE_H_
#define WYSWIETLANIE_H_

/*
 *   can be switched on in wyswietlanie.h
 *   it will require to add -lGL -lGLU -lSDL -lSDL_ttf
 *
 */
#ifndef WITH_GL
#undef WITH_GL
#endif


#ifdef WITH_GL


#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "../neurony/siecNeuronowa.h"
#include "../neurony/siecNeuronowaRozkladStopni.h"
#include "wyswietlanieSieci.h"
#include "wyswietlanieTekstu.h"
#include "nawigacja.h"




#define window_width  800
#define window_height 600



void setupGL(SiecNeuronowaRozkladStopni&);
void main_loop(SiecNeuronowa&, bool);
bool events(SiecNeuronowaRozkladStopni& );
void obslugaKlawiszyParametrow(SDL_Event event, SiecNeuronowaRozkladStopni & siec);

/* *******************************************
 *
 *  Implementacja
 *  przerzucić do osobnego pliku!
 *
 * *******************************************/

SDL_Surface *screen = NULL;

/* inincjalizacja gl
 *
 * skopiowane z sieciowego tutoriala
 */
void setupGL(SiecNeuronowaRozkladStopni& siecRS){
	// Initialize SDL with best video mode
	SDL_Init(SDL_INIT_VIDEO);
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	int vidFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;

	if (info->hw_available) {
		vidFlags |= SDL_HWSURFACE;
	} else {
		vidFlags |= SDL_SWSURFACE;
	}	// else

	int bpp = info->vfmt->BitsPerPixel;

	screen = SDL_SetVideoMode(window_width, window_height, bpp, vidFlags);

	SDL_WM_SetCaption("Neurons - simulation","Neurons - simulation");

	tekstInit();

	glViewport( 0, 0, window_width, window_height );

	glMatrixMode( GL_PROJECTION );

	glEnable( GL_DEPTH_TEST );

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	gluPerspective( 90, (float)window_width/window_height, 0.1, 100 );

	glMatrixMode( GL_MODELVIEW );

	glClearColor(1,1,1,1);
	uaktualnijListe(siecRS);



}	// glSetup()


/* glowna petla
 *
 * obsluga klawiszy - rysowanie etc
 */
void main_loop(SiecNeuronowaRozkladStopni& siecRS /* , bool czySymulowac=false */){

	while( events(siecRS) )	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();

		ustawKamere();
		// na koniec centrujemy na jej srodku
		glTranslatef( -siecRS.getN().getN().getMaxPos()/2.0 ,
				-siecRS.getN().getN().getMaxPos()/2.0 , -siecRS.getN().getN().getMaxPos()/2.0);
		rysujGraf(siecRS.getN().getN());
		rysujTekst(siecRS);

		SDL_GL_SwapBuffers();

	}	// while
}	// main-loop- function()




/* obsulga zdarzen - na razie tylko klawisze, gryzon i exit
 * tylko?
 */
bool events(SiecNeuronowaRozkladStopni& siec){
	SDL_Event event;
	if( SDL_PollEvent(&event) )	{
		switch( event.type )		{
			case SDL_QUIT    : return false; break;
			case SDL_KEYDOWN:{
				obslugaKlawiszyParametrow(event, siec);
				break;
			}	// case
		}	// switch
		obslugaMyszki(event);
		obslugaKlawiatury(event);

	}	// if

	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION){
		obslugaRuchuMyszki(event);
		obslugaKolkaMyszki(event);
	}	// if
	obslugaKlawiszyNawigacyjnych();

	return true;
}	// events

/* obsluga klawiszy
 *
 * TODO podzielic tematycznie?
 *
 */
void obslugaKlawiszyParametrow(SDL_Event event, SiecNeuronowaRozkladStopni & siecRS){


	// prog mnk
	if (event.key.keysym.sym == SDLK_a){
		zmniejszProgMin();
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_q){
		zwiekszProgMin();
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_w){
		zwiekszProgMax();
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_s){
		zmniejszProgMax();
		uaktualnijListe(siecRS);
	}	// if

	// tryb krawedzi
	if (event.key.keysym.sym == SDLK_y){
		setPokazkrawedzie(KRAWEDZIE_ZADNE);
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_u){
		setPokazkrawedzie(KRAWEDZIE_DRZEWKA);
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_i){
		setPokazkrawedzie(KRAWEDZIE_PRZEPLYW);
		uaktualnijListe(siecRS);
	}	// if

	// progi krawedzi
	if (event.key.keysym.sym == SDLK_EQUALS){
		zwiekszProg();
		uaktualnijListe(siecRS);
	}	// if
	if (event.key.keysym.sym == SDLK_MINUS){
		zmniejszProg();
		uaktualnijListe(siecRS);
	}	// if

	// reset kamery
	if (event.key.keysym.sym == SDLK_r){
		resetujKamere();
	}	// if

	// zerowe neurony
	if (event.key.keysym.sym == SDLK_t){
		if (czyPokazywacZeroweNeurony()){
			ukryjZeroweNeurony();
		} else {
			pokazujZeroweNeurony();
		}	// if
		uaktualnijListe(siecRS);
	}	// if

	// pokazywanie wykresow
	if (event.key.keysym.sym == SDLK_p){
		if (czyPokazywacWykresy()){
			setPokazWykresy(false);
		} else {
			setPokazWykresy(true);
		}	// if .. else
		uaktualnijListe(siecRS);
	}	// if p

}	// obslugaKlawiszyParametrow();


#endif // zero

#endif /* WYSWIETLANIE_H_ */
