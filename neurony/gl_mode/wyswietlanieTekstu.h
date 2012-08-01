/*
 * wyswietlanieTekstu.h
 *
 *  Created on: Apr 26, 2009
 *      Author: piersaj
 *
 *   Pliczek do pisania po powierzchniach - tj wyswietlania stanu sieci
 *   wymaga SDL_ttf // sudo yum install SDL_ttf-devel
 *   -lSDL_ttf
 *
 *   2009-04-26
 *   	utworzenie + nauka jak to dziala
 */

#ifndef WYSWIETLANIETEKSTU_H_
#define WYSWIETLANIETEKSTU_H_

#ifdef WITH_GL

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "../neurony/siecNeuronowaRozkladStopni.h"

void tekstInit();
void tekstClose();
void rysujTekst();

void rysujTekst(SiecNeuronowa&);
void beginOrtho();
void endOrtho();

extern int progMin, progMax;

TTF_Font * font = NULL;

/* ********************************
 *
 *  Implementacja - przeniesc do osobnego pliku
 *
 * *****************************/

void tekstInit(){
	if(!TTF_WasInit() && TTF_Init()==-1) {
	    printf("TTF_Init: %s\n", TTF_GetError());
	    return;
	}	// fi

	font = TTF_OpenFont("font/FreeMono.ttf", 18);

	if(!font) {
	    printf("TTF_OpenFont: %s\n", TTF_GetError());
	}	// if

}	// tekstInit()

void tekstClose(){
	if (TTF_WasInit()){
		TTF_CloseFont(font);
		TTF_Quit();
	}	// fi
}	// tekstClose()

void rysujTekst(SiecNeuronowaRozkladStopni &siec){
	if (!TTF_WasInit()){
		return;
	}	// if

	SDL_Color color={0, 0, 0};
	SDL_Surface *text_surface = NULL;

	glColor3f(1,1,1);

	char tekst[200];
	sprintf(tekst, "min=%d max=%d  approx = %.3fx+%.3f neurons=%d", progMin, progMax, siec.getMnkA(), siec.getMnkB(),
			siec.getN().getIloscNeuronow() );

	if(!(text_surface = TTF_RenderText_Blended(font, tekst, color))) {
		std::cerr << "renderText failed\n";
	    return;
	}	// if

	unsigned Texture = 0;

	/*Generate an OpenGL 2D texture from the SDL_Surface*.*/
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_surface->w, text_surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);


	// skalar do teksu czcionka 18p
	float s = 650 /*text_surface->w*/;

	beginOrtho();
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3d(0,0,-0.1);
	glTexCoord2d(0, 0); glVertex3d(0, text_surface->h/s, -0.1);
	glTexCoord2d(1, 0); glVertex3d(text_surface->w/s, text_surface->h/s, -0.1);
	glTexCoord2d(1, 1); glVertex3d(text_surface->w/s, 0, -0.1);
	glEnd();
	endOrtho();

	glDeleteTextures(1, &Texture);
	SDL_FreeSurface(text_surface);

	return;
}	// rysujTekst();

void DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha = 255) {
	if ((srcimg == NULL) || (alpha == 0)) return; //If theres no image, or its 100% transparent.
	SDL_Rect src, dst;

	src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
	dst.x = dx;  dst.y = dy;  dst.w = src.w;  dst.h = src.h;
	if (alpha != 255) SDL_SetAlpha(srcimg, SDL_SRCALPHA, alpha);
	SDL_BlitSurface(srcimg, &src, dstimg, &dst);
}	// DrawImage

void beginOrtho(){
	// begin ortho
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, .001, 100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}	// beginOrtho()

void endOrtho(){
	// end ortho
	glMatrixMode(GL_PROJECTION);
	//Load the previous Projection matrix (Generaly, it is a Perspective projection)
	glPopMatrix();
	//Select the Modelview matrix stack
	glMatrixMode(GL_MODELVIEW);
	//Load the previous Modelview matrix
	glPopMatrix();
}	// endOrtho()

#endif // zero

#endif /* WYSWIETLANIETEKSTU_H_ */
