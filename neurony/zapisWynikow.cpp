/*
 * zapisWynikow.cpp
 *
 *  Created on: Jul 3, 2009
 *      Author: piersaj
 *
 * 2009-07-03
 * 		pliczek do zapisu wynikow sieci w formacie sketcha
 * 		rysowanie neuronow
 * 2009-07-06
 * 		przerzucono tu zapis do plikow tekstowych
 * 		dodano rysowanie wykresy rozkladu stopni (.sk)
 * 2009-07-09
 * 		kilka korekt do wydrukow w .sk
 * 2009-07-17
 * 		korekty: dodano wstawianie global{ set [unit = .2pt] }
 *		dodano definiowanie rysunku def nazwa {...}
 *		dodano rysowanie zdrfiniowanego rysunku z obrotem  put { view{...} {nazwa} }
 * 2009-11-??
 * 		dodany zapis do skryptu matlabowego i ... stale cos jest tam poprawiane :P
 *
 */
#if 0

#include "zapisWynikow.h"
#include "../src/logger.h"


void ostrzezenieOZuzyciuPamieciSk(std::fstream &str);

/* *************************************
 *
 * *************************************/

/* techniczne wrucenie ostrzezenia o pamieciozernosci PSTricks (przy 20k+ neuronach)
 * nie widze powodu dla ktorego to mailoby byc w naglowkach...
 */
void ostrzezenieOZuzyciuPamieciSk(std::fstream &str){
	str << "% UWAGA: ze moze sie posypac przy duzej sieci.\n" ;
	str << "% W takiej sytuacji mozna zwiekszyc limit pamieci w pliku:\n" ;
	str << "% kpsewhich texmf.cnf " ;
}	// ostrzezenieOZuzyciuPamieciSk()

/* ******************************
 *
 * Format sketcha
 *
 * *****************************/

/* zapis wynikow w plaintexcie
 */
void zapiszWynikiTxt(SiecNeuronowa& n){
	logJP << (char * )"Zapis wynikow...";

//	zapiszRozkladCCm(n);
//	zapiszWspSkupieniaTxt(n);
//	zapiszWynikiRozkladStopniTxt(n);
//	zapiszHistogramPrzyjetychIteracjiTxt(n);
//	zapiszHistogramCzasuEpokTxt(n);

	zapiszWynikiSketch(n);
	logJP << (char *)"   ok\n";
}	// zapisz wyniki


/* zapis wszystkich smieci do plikow sk
 */
void zapiszWynikiSketch(SiecNeuronowa& n){

	// chwilowo zakomentowalizmy te grafiki bo za dlugo sie schodzi
//	zapiszGrafyPrzeplywuSk(n);
//	zapiszRozkladStopniSk(n);
	zapiszNeuronySk(n);
	return;
}	// zapiszWynikiSketch()

/* Zapis pozycji neuronow w przestrzeni
 *
 */
void zapiszNeuronySk(SiecNeuronowa & n){
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wynikiNeurony-%Y-%m-%d-%H-%M-%S.sk",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}
	zapiszOpisSieciSk(n, str);

	ostrzezenieOZuzyciuPamieciSk(str);

	zapiszOsieSk(n, str);
	str << "{axes}\n\n";

	int LICZNIK = 100;
	for (int i=0; i< n.getIloscNeuronow(); i++){
		if (i % LICZNIK == 0){
			str << "\ndots [dotsize=1pt] ";
		}	// fi

		str << "(" << n.getNeuron(i).getX() <<"," <<  n.getNeuron(i).getY() << ',' <<
			n.getNeuron(i).getZ() << ')' ;
	}	// for

	str << "\n";
	zapiszWielkoscObrazuSk(str);


	str.close();

	return;
}	// zapiszWynikiSketch()

/* zapis krawedzi drzewek przeplywu
 */
void zapiszGrafyPrzeplywuSk(SiecNeuronowa& n){
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,80,"wyniki/wynikiKrawedzie1-%Y-%m-%d-%H-%M-%S.sk",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}

	zapiszOpisSieciSk(n, str);
	zapiszOsieSk(n, str);

	str << "def edges {\n";
	str << "{axes}\n";

	for (int i=0; i<n.getIloscKrawedzi(); i++){
		Polaczenie k = n.getKrawedz(i);
		if ( k.getKolor()>=1 ){
			Neuron & n1 = n.getNeuron(k.getIndeks1());
			Neuron & n2 = n.getNeuron(k.getIndeks2());

			str << "line [linewidth=.2pt, linecolor="<< okreslKolor(k.getKolor())
				<< "]  (" << n1.getX() << ',' << n1.getY() << ',' << n1.getZ() << ')';
			str << "(" << n2.getX() << ',' << n2.getY() << ',' << n2.getZ() << ")\n";
		}	// if
	}	// for
	str << "} % def edges {\n\n";

	str << "put { view((40,25,30),(15,15,15)) } {edges}";
	zapiszWielkoscObrazuSk(str);

	str.close();
	return;
}	// zapiszWynikiKrawedzie1()

/* wrzuca osie do strumienia
 */
void zapiszOsieSk(SiecNeuronowa &n, std::fstream &str){
	// rysowanie osi
	str << "\n\n% plot axes\n";
	str << "def O (0,0,0)\n"
		<< "def axes { \n def sz "<< n.getMaxPos() << "\n"
		//<< " line [arrows=<->] (sz,0,0)(O)(0,sz,0)\n"
		<< " line [arrows=->] (O)(0,sz,0)\n"
		<< " line [arrows=->] (O)(0,0,sz)\n"
		<< " line [arrows=->] (O)(sz,0,0)\n"
		//<< " line [linewidth=.2pt,linecolor=blue,linestyle=dashed] (O)(0,0,-10)\n"
		//???
		<< " special |\\uput[r]#1{$x$}\\uput[u]#2{$y$}\\uput[l]#3{$z$}|\n (sz,0,0)(0,sz,0)(0,0,sz)\n"
		<< "}\n"
		<< "\n";
}	// rysujOsie

/* wrzuca skrocony opis sieci do pliku strumienia
 */
void zapiszOpisSieciSk(SiecNeuronowa &n, std::fstream &str){
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime (buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);

	str << '\n';
	str << "% Data        " << buffer << '\n';
	str << "% Il neuronow "<< n.getIloscNeuronow() << '\n';
	str << "% Il krawedzi "<< n.getIloscKrawedzi() << '\n';
	str << "% Il iteracji "<< (int) n.getIloscIteracji() << "\n\n";
}	// zapiszOpisSieci

/* zapis tablicy z rozkladem stopni wejsciowych in-degree
 *
 *
 */
void zapiszRozkladStopniSk(SiecNeuronowa& n){

	logJP <<  "zapiszRozkladStopniSk() <<< this function is deprecated - dont call me!\n\n";
	if (1+1 == 2)
		return;

	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wynikiRozkladStopni-%Y-%m-%d-%H-%M-%S.sk",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszRoskladStopniSk() nie mozna utworzyc pliku\n\n";
		return;
	}

	zapiszOpisSieciSk(n, str);

	float maxx = 0; //n.getArgx(n.getLen()-1) + 1;
	float maxy = 0; //n.getArgy(0) + 1;

	// osie
	str << "\n\n% plot axes\n";
	str << "def O (0,0)\n"
		<< "def axes { \n "
		<< " line [arrows=<->, linewidth=.2pt] (0,"<<maxy<<")(O)("<< maxx<<",0)\n"
			//<< " line [linewidth=.2pt,linecolor=blue,linestyle=dashed] (O)(0,0,-10)\n"
		<< " special |\\uput[r]#1{$x$}\\uput[u]#2{$y$}|\n ( "<< maxx
		<<",0,0)(0,"<< maxy
		<< " ,0)\n"
		<< "}\n"
		<< "{axes}\n\n";

	// co to jest ten sz i po co tu???

	str << "line [linewidth=.2pt, linecolor=red]";
//	for (int i=0; i< n.getLen(); i++){
//		str << '(' << n.getArgx(i) << ',' << n.getArgy(i) << ')';
//	}	// for
	str << "\n\n";

	zapiszWielkoscObrazuSk(str);

	str.close();
	return;
}	// zapiszRozkladStopniSk()

void zapiszWielkoscObrazuSk(std::fstream &str, float unit ){
	str << "global{\n set [ unit="
			<< unit
			<<"cm]\n} % global\n\n";
}	// zapiszWielkoscObrazu

/*
 */
char *  okreslKolor(int i){
	// nie wiem dlaczego +1 ale lepiej zostawie
	switch ((i+1)% 9){
		case 0: return (char *)"black";
		case 1: return (char *)"blue";
		case 2: return (char *)"red";
		case 3: return (char *)"green";
		case 4: return (char *)"yellow";
		case 5: return (char *)"purple";
		case 6: return (char *)"cyan";
		case 7: return (char *)"orange";
		case 8: return (char *)"lightblue";
		case 9: return (char *)"brown";

		case 10:
		default:
			return (char *)"gray";
	}	// switch

}	// okresl kolor


/* *******************************************************************
 *
 *  Format txt
 *
 * *******************************************************************/



/* zapis tablicy z rozkladem stopni wejsciowych in-degree
 */
void zapiszWynikiRozkladStopniTxt(SiecNeuronowa& n){

	logJP <<  "zapiszWynikiRozkladStopniTxt() <<< this function is deprecated - dont call me!\n\n";
	if (1+1 == 2)
		return;

	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wyniki_rozkladStopni_%Y_%m_%d_%H_%M_%S.m",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}

	str << "# Il neuronow "<< n.getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< n.getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) n.getIloscIteracji() << '\n';


	str << "Y = [\n";

//	for (int i=0; i< n.getLen(); i++){
//		str << n.getArgx(i) << ' ' << n.getArgy(i) << ";\n'";
//	}	// for
	str << "];";

	str << "plot(Y(:,1), Y(:,2), \"r\");\n";
	str << "# loglog(Y(:,1), Y(:,2), \"b+\");\n";

	str.close();

	return;
}	// zapiszWyniki()

void zapiszRozkladCCm(SiecNeuronowa & n){
	if (1+1==2)
		return;

	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,80,"wyniki/wyniki_rozkladCC_%Y_%m_%d_%H_%M_%S.m",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}

	strftime (buffer,80,"wyniki_rozkladCC_%Y_%m_%d_%H_%M_%S",timeinfo);

	str << "# Il neuronow "<< n.getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< n.getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) n.getIloscIteracji() << '\n';

	str << "function ret = "<< buffer << "()\n\n";
	str << "Y = [";

//	for (int i=0; i< n.getCcRozkladLen(); i++){
//		str << i << " , " << n.getCcRozklad()[i] << " ;\n ";
//	}	// for

	str << "];\n\n";

	str <<  "  plot (Y(:,1), Y(:,2), \"b+\");\n";
	str <<  "  axis([0,100,0,1], \"autox\");\n";

	str << "#  plot (Y(:,1), Y(:,2), \"b+\");\n"<<
		"#  axis([0,100,0,1], \"autox\");\n"<<
		"  X = Y(:,1);\n" <<
		"  Z = Y(end:-1:1 , 2);\n"<<
		"#  usuwamy -1 z wykresu\n"<<
		"  Z(find(Z<0)) = 0;\n"<<
		"  Z = cumsum(Z);\n"<<
		"  Z = Z(end:-1:1);\n"<<
		"  a = polyfit(X, Z, 1);\n"<<
		"#  plot(X, Z, \"r\");\n"<<
		"#  hold on;\n"<<
		"#  plot((1:100:500) , a(1) .* (1:100:500) .+ a(2), \"m\");\n"<<
		"#  hold off;\n"<<
		"#  loglog(X, Z, \"r*\");\n" <<
		"# print (\"/home/piersaj/Desktop/nn/";
	strftime (buffer,80,"%Y_%m_%d",timeinfo);

	str << buffer;
	sprintf(buffer, "_sfera_%.1fk_%dM_%dM----XXX.eps", n.getIloscNeuronow()/1000.0, n.getIloscKrawedzi()/1000000,
			(int) n.getIloscIteracji() / 1000000);
	str << buffer << "\" , \"-color\", \"-dpdf\"); \n";
	str << "endfunction\n";
	str.close();

	return;
}	// zapiszWynikiRozkladCC

/* zapis wykresy wspolczynnika klasteryzacji (cc)
 * zalozenie ze jest juz policzony!!!
 */
void zapiszWspSkupieniaTxt(SiecNeuronowa& n){
	if (1+1==2){
		return;
	}
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wyniki-wspSkupienia-%Y-%m-%d-%H-%M-%S.txt",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszWspSkupienia() nie mozna utworzyc pliku\n\n";
		return;
	}

//	std::deque<pairIntInt> &w = n.getWspolczynnikSkupienia();
//
//	for (unsigned int i=0; i< w.size(); i++){
//
//		pairIntInt & p = w.front();
//		w.pop_front();
//		w.push_back(p);
//
//		str << p.klucz << " " << p.wartosc << "\n";
//	}	// while

	str.flush();
	str.close();
}	// zapiszWspSkupienia

/* zapis histogramu zmian w trakcie symylacji do plaintextu
 */
void zapiszHistogramPrzyjetychIteracjiTxt(SiecNeuronowa & n){
	return;

	/*
	 *
	 *
	 *
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wyniki-histPrzyjetychZmian-%Y-%m-%d-%H-%M-%S.txt",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszHistPrzyjetychZmian() nie mozna utworzyc pliku\n\n";
		return;
	}



	std::deque<int> &v= n.getHistogramPrzyjetychIteracji();

	// takie zabezpieczenie zeby ie bylo za malo danych w histogramie
	while (v.size()<100){
		v.push_back( 0 );
	}	// while

	for (int i=0; i<100; i++){
		str << v.front() << "\n";
		v.push_back( v.front());
		v.pop_front();
	}	// while

	str.flush();
	str.close();
	*
	*
	*/
}	// zapiszHistogramPrzyjetychIteracji

/* zapis wykresu czasu liczenia sie epok
 */
void zapiszHistogramCzasuEpokTxt(SiecNeuronowa & n){
	return;
/*
 *
 *
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	//strftime (buffer,80,"wyniki-%I-%c.txt",timeinfo);
	strftime (buffer,80,"wyniki/wyniki-histCzasuEpok-%Y-%m-%d-%H-%M-%S.txt",timeinfo);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszHistCzasuEpok() nie mozna utworzyc pliku\n\n";
		return;
	}

	std::deque<int> &v= n.getHistogramCzasuObliczen();

	// takie zabezpieczenie zeby nie bylo za malo danych w histogramie
	while (v.size()<100){
		v.push_back( 0 );
	}	// while

	for (int i=0; i<100; i++){
		str << v.front() << "\n";
		v.push_back( v.front());
		v.pop_front();
	}	// while

	str.flush();
	str.close();

	*
	*
	*/

}	// zapiszHistogramCzasuEpok

/* zapisuje macierz do pliku .m
 */
void zapiszMacierzGrafuPrzeplywuM(SiecNeuronowa &n){
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,80,"wyniki/macierzGrafu_%Y_%m_%d_%H_%M_%S.m",timeinfo);

	char filename[255];
	sprintf(filename, "wyniki/macierzGrafu_%s__%d_n__%dM_k__%ldM_it.m", buffer,
			n.getIloscNeuronow(), n.getIloscKrawedzi()/1000000,
			n.getIloscIteracji() / 1000000);

	std::fstream str;
	str.open(buffer , std::fstream::out );

	if (str == NULL){
		std::cerr<< "zapiszMacierz() nie mozna utworzyc pliku\n\n";
		return;
	}

	str << "# Il neuronow "<< n.getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< n.getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) n.getIloscIteracji() << '\n';

	// wektor wspolrzednych i
	str << "I___ = [";
	int l=0;
	for (int i=0; i< n.getIloscKrawedzi(); i++){
		Polaczenie & k = n.getKrawedz(i);
		if (k.getRoznicaLadunku() > 0){
			l++;
			str << (k.getIndeks1() > k.getIndeks2() ? k.getIndeks1()+1 : k.getIndeks2()+1) << "; ";
			if (l== 255){
				l = 0;
				str << "\n";
			}	// fi
		}	// fi
	}	// for
	str<< "];\n\n";

	// wektor wspolrzednych j
	str << "J___ = [";
	l=0;
	for (int i=0; i< n.getIloscKrawedzi(); i++){
		Polaczenie & k = n.getKrawedz(i);
		if (k.getRoznicaLadunku() > 0){
			l++;
			str << (k.getIndeks1() < k.getIndeks2() ? k.getIndeks1()+1 : k.getIndeks2()+1) << "; ";
			if (l== 255){
				l = 0;
				str << "\n";
			}	// fi
		}	// fi
	}	// for
	str<< "];\n\n";


	// wektor wartisci
	str << "SV__ = [";
	l=0;
	for (int i=0; i< n.getIloscKrawedzi(); i++){
		Polaczenie & k = n.getKrawedz(i);
		if (k.getRoznicaLadunku() > 0){
			l++;
			str << k.getRoznicaLadunku() << "; ";
			if (l== 255){
				l = 0;
				str << "\n";
			}	// fi
		}	// fi
	}	// for
	str<< "];\n\n";


	// reszta
	str <<  "Y = sparse(I___, J___, SV__, "<< n.getIloscNeuronow()<< " , "
		<< n.getIloscNeuronow() << " )\n";
	str <<  "Y1 = sparse( 1:" << n.getIloscNeuronow() << " , 1:" << n.getIloscNeuronow() <<
			" , diag(Y));\n";
	str <<  "Y = Y'+ Y - Y1;\n";

	str << "clear I___;\nclear J___;\nclear SV__;\nclear Y1;\n";
	str << "# spy(Y);\n";

	str << "# print (\"/home/piersaj/Desktop/nn/";
	strftime (buffer,80,"%Y_%m_%d",timeinfo);

	str << buffer;
	sprintf(buffer, "_macierz_%.1fk_%dM_%dM----XXX.eps", n.getIloscNeuronow()/1000.0, n.getIloscKrawedzi()/1000000,
			(int) n.getIloscIteracji() / 1000000);
	str << buffer << "\" , \"-color\", \"-dpdf\"); \n";
	str.close();

	return;
}	// zapiszMacierzGrafuPrzeplywuM

#endif
