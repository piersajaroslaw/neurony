/*
 * wyswietlanieSieci.cpp
 *
 * 2009-05-09
 * 		przerzucono do innego pliku
 * 		uwaga - zmienne tez trzeba przerzucic :P
 * 2009-05-30
 * 		zmiana kolorowania drzewek 3floaty zamienione na 1 short
 * 		zmiany w kolorach mnk i wytresu przeplywu
 * 2009-06-12
 * 		dorzucony tryb wyswietlania krawedzi
 * 2009-06-19
 * 		wyswietlanie wykresow histogramu zmian i cc
 *
 *  Created on: May 9, 2009
 *      Author: piersaj
 */


#ifdef WITH_GL

#include "wyswietlanieSieci.h"
#include "../neurony/statistics/siecNeuronowaRozkladStopni.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

/* *******************************
 *
 * zmienne
 *
 * ******************************/

int prog = 9;
GLuint listaRysowania;

int progMin = 0;
int progMax = 100;
const int PROG_MNK = 100;

int pokazKrawedzie = 0;



bool pokazZeroweNeurony = 1;
bool pokazWykresy = 1;


/*  *********************************************
 *
 * funkcje
 *
 * **********************************************/


void rysujGraf(SiecNeuronowa& siec){
	glCallList(listaRysowania);
	glFlush();
}	// rysuj

void rysujWierzcholki(SiecNeuronowa& siec, bool pokazZerowe = true){

	GLUquadric *sphere;
	float maxL = siec.znajdzMaksymalnyLadunekNeuronu();
	float maxW = siec.getMaksymalneWsparcie();

	for (int i=0; i< siec.getIloscNeuronow(); i++){

		Neuron n = siec.getNeuron(i);

		float colR =  (n.getLadunek()) / maxL;
		float colB = (n.getWsparcie()) / maxW;

		if ( !pokazZerowe && colR == 0.00){
			continue;
		}	// if

		colR = colR<0 ? 0 : colR;
		colR = colR>1 ? 1 : colR;

		colB = colB<0 ? 0 : colB;
		colB = colB>1 ? 1 : colB;



		// jak co to mozna za pomoca tego, ale sfery sa jednak fajniejsze
		// glPointSize(1+ 4*col);

		// jezeli jest maly to punkty niebieskie
		if ( colR <= 0.2 && colB <=.9){
			glColor3f(0, 0, 1);
			glBegin(GL_POINTS);
			glVertex3f(n.getX(), n.getY(), n.getZ());
			glEnd();

		} else {
			// wpw duza sfera i kolory
			glColor3f(colR, 0, colB);

			glPushMatrix();
			glTranslatef(n.getX(), n.getY(), n.getZ());
			sphere = gluNewQuadric();
			gluSphere(sphere, .1 , 3, 3);
			glPopMatrix();
		}	// if.. else
	}	// for i
}	// rysujWierzcholki()

void rysujKrawedzie(SiecNeuronowa& siec){
	glLineWidth(1);
	glPushMatrix();


	if (siec.getIloscKrawedzi() > 100000){
		// takie glupie zabezpieczenie przed zbyt duza iloscia krawedzi
		prog = 2 > prog ? 1 : prog;
	}	// if

	float progOdcinajacy = (prog / (float)MAX_PROG);

	float max = siec.znajdzMaksymalnyPrzelpywKrawedzi();

	for (int i=0; i< siec.getIloscKrawedzi(); i++){

		Polaczenie p = siec.getKrawedz(i);

		float colG = 0;
//		if (p.getCzyWPuliLosowanychKrawedzi())
//			colG = .5;

		float colR = (p.getladunekDo1() + p.getladunekDo2()) / max;
		colR = colR<0 ? 0 : colR;
		colR = colR>1 ? 1 : colR;



		if (colR < progOdcinajacy)
			continue;

		Neuron v1 = siec.getNeuron( p.getIndeks1());
		Neuron v2 = siec.getNeuron( p.getIndeks2());

		glColor3f(colR, colG, 0);
		glBegin(GL_LINES);
		glVertex3f(v1.getX(), v1.getY(), v1.getZ());
		glVertex3f(v2.getX(), v2.getY(), v2.getZ());
		glEnd();
	}	// for i

	glPopMatrix();
}	// rysujKrawedzie()

void rysujDrzewka(SiecNeuronowa& siec){
	glLineWidth(1);
	glPushMatrix();

	int maxNarysowanych = 10000;

	for (int i=0; i< siec.getIloscKrawedzi(); i++){

		Polaczenie p = siec.getKrawedz(i);

		if (p.getKolor()== 0 /* && p.getKolorG() == 0 && p.getKolorB() == 0 */){
			continue;
		}	// fi

		float r,g,b;
		okreslKolorDrzewka(p.getKolor(), r, g, b);

		glColor3f(r, g, b);

		Neuron v1 = siec.getNeuron( p.getIndeks1());
		Neuron v2 = siec.getNeuron( p.getIndeks2());

		glBegin(GL_LINES);
		glVertex3f(v1.getX(), v1.getY(), v1.getZ());
		glVertex3f(v2.getX(), v2.getY(), v2.getZ());
		glEnd();

		maxNarysowanych --;

		if (maxNarysowanych <=0)
			break;
	}	// for i

	glPopMatrix();
}	// rysujDrzewka

void rysujKlatke(float m){
	glPushMatrix();

	glColor3f(0, 0, 0);
	glLineWidth(1);

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0);
	glVertex3f(0, m, 0);
	glVertex3f(m, m, 0);
	glVertex3f(m, 0, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, m);
	glVertex3f(0, m, m);
	glVertex3f(m, m, m);
	glVertex3f(m, 0, m);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, m);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0, m, 0);
	glVertex3f(0, m, m);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(m, 0, 0);
	glVertex3f(m, 0, m);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(m, m, 0);
	glVertex3f(m, m, m);
	glEnd();

	glPopMatrix();
}	// rysujKlatke

void rysujWykresPrzplywuLadunku(SiecNeuronowaRozkladStopni & siecRS){

	SiecNeuronowa& siec = siecRS.getN().getN();
	glBegin(GL_LINE_STRIP);
	glColor3f(1,1,0);

	int d = (int) (siecRS.getLen() * progMin * 1.0 / PROG_MNK);
	int e = (int) (siecRS.getLen() * progMax) * 1.0 / PROG_MNK;

	glColor3f(0 , 0.5 , 0);
	for (int i=0; i< d; i++){
		glVertex3f(siecRS.getDdLogX(i), siecRS.getDdLogY(i),  siec.getMaxPos());
	}	// for


	glColor3f(0,1,0);
	for (int i=d; i< e; i++){
		glVertex3f(siecRS.getDdLogX(i), siecRS.getDdLogY(i),  siec.getMaxPos());
	}	// for

	glColor3f(0 , 0.5 , 0);
	for (int i= e; i< siecRS.getLen(); i++){
		glVertex3f(siecRS.getDdLogX(i), siecRS.getDdLogY(i),  siec.getMaxPos());
	}	// for

	glEnd();
} 	// rysujWykresPrzpluwuLadunku(

void rysujProstaMNK(SiecNeuronowaRozkladStopni &ndd){
	SiecNeuronowa &n = ndd.getN().getN();

	ndd.obliczProstaMNK( (double) progMin/PROG_MNK , (double) progMax/PROG_MNK );

	float a = ndd.getMnkA();
	float b = ndd.getMnkB();
	float max = n.getMaxPos();

	glColor3f(1,0,0);
	glBegin(GL_LINES);


	if (-b/a < max)
		glVertex3f( -b/a ,  0 , max);
	else
		glVertex3f( max, a*max +b  , max);

	if ( b < max)
		glVertex3f(0 , b, max);
	else
		glVertex3f( (max-b)/a , max, max);

//	ax+b = max
//	x = max-b/a
	glEnd();
}	// rysujProstaMNK

void rysujWykresWspolczynnikaKlasteryzacji(SiecNeuronowa& siec){
	if (1+1==2){
		printf("wyświetlanieSieci.rysujWykresWspolczynnikaKlasteryzacji(): wylaczyc mnie\n");
		return;
	}	// fi
	glColor3f(0,1,0);

//	std::deque<pairIntInt> & kolejka = siec.getWspolczynnikSkupienia();
	float max = 0,  maxpos = siec.getMaxPos();

	glBegin(GL_LINE_STRIP);

//	for (unsigned int i=0; i< kolejka.size(); i++){
//		pairIntInt & akt = kolejka.front();
//		kolejka.pop_front();
//		kolejka.push_back(akt);
//
//		// wspolczynnik normalizujacy
//
//
//		if (akt.wartosc < 1000)
//			max = akt.klucz;
//
//	}	// for i
//
//	for (unsigned int i=0; i< kolejka.size(); i++){
//		pairIntInt & akt = kolejka.front();
//		kolejka.pop_front();
//		kolejka.push_back(akt);
//
//		if (akt.klucz <= max)
//			glVertex3f( maxpos, akt.wartosc / 1000.0 * maxpos, maxpos -  akt.klucz * maxpos / max );
//
//	}	// for i

	glEnd();
}	//rysujWykresWspolczynnikaKlasteryzacji()

void rysujHistogramZmian(SiecNeuronowa& siec){
	return;
/*
	glColor3f(0,1,0);

	std::deque<int> &kolejka = siec.getHistogramPrzyjetychIteracji();
	float max = 0, akt, maxpos = siec.getMaxPos();

	glBegin(GL_LINE_STRIP);

	for (int i=0; i< 100; i++){
		akt = kolejka.front();
		kolejka.pop_front();
		kolejka.push_back(akt);

		// wspolczynnik normalizujacy
		if (i==0){
			max = akt / siec.getMaxPos();
		}	// i

		glVertex3f( 0, akt / max,  i * maxpos / 100 );
	}	// for i

	glEnd();


*/
}	// rysujHistogramZmian()

/*  *******************************************
 *
 *  kontrola flag wyswietlania
 *
 * ********************************************/
void zwiekszProg(){
	if (prog<MAX_PROG)
		prog++;
}	// zwiekszProg

void zmniejszProg(){
	if (prog>0)
		prog--;
}	// zmniejszProg

void zwiekszProgMin(){
	if (progMin< progMax-1)
		progMin++;
}	// zwiekszProg



void zmniejszProgMin(){
	if (progMin>0)
		progMin--;
}	// zmniejszProg

void zwiekszProgMax(){
	if (progMax<PROG_MNK)
		progMax++;
}	// zwiekszProg

void zmniejszProgMax(){
	if (progMax> progMin+1)
		progMax--;
}	// zmniejszProg

void pokazujZeroweNeurony(){
	pokazZeroweNeurony =   true;
}	//pokazujZerowe

void ukryjZeroweNeurony(){
	pokazZeroweNeurony =   false;
}	// ukryjZerowe()

bool czyPokazywacZeroweNeurony(){
	return pokazZeroweNeurony;
}	// czyPokazywacZeroweNeurony

int getPokazkrawedzie(){
	return pokazKrawedzie;
}	// getPokazKrawedzie

void setPokazkrawedzie(int val){
	if (val==0 || val==1 || val==2)
		pokazKrawedzie = val;
}	// getPokazKrawedzie


void setPokazWykresy(bool val){
	pokazWykresy =   val;
}	//pokazujZerowe


bool czyPokazywacWykresy(){
	return pokazWykresy;
}	// czyPokazywacWykresy()


void uaktualnijListe(SiecNeuronowaRozkladStopni& siecRS){
	SiecNeuronowa siec = siecRS.getN().getN();

	glDeleteLists(listaRysowania, 1);
	listaRysowania = glGenLists(1);
	glNewList(listaRysowania, GL_COMPILE);
	rysujKlatke(siec.getMaxPos());
	rysujWierzcholki(siec, pokazZeroweNeurony);

	switch (pokazKrawedzie){
		case KRAWEDZIE_PRZEPLYW:
			rysujKrawedzie(siec);
			break;
		case KRAWEDZIE_DRZEWKA:
			rysujDrzewka(siec);
			break;
		case KRAWEDZIE_ZADNE:
			break;
		default:
			break;
	}	// switch


	if (pokazWykresy == true){
		rysujWykresPrzplywuLadunku(siecRS);
		rysujWykresWspolczynnikaKlasteryzacji(siec);
		rysujProstaMNK(siecRS);
		rysujHistogramZmian(siec);
	}	// fi



//	rysujTekst(siec);
	glEndList();

}	// uaktualnijListe()


#endif
