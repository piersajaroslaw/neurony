/*
 * spektrumGrafu.cpp
 *
 *  Created on: Feb 17, 2010
 *      Author: piersaj
 *
 *  2010-02-17
 *  	utworzenie (podobno, ostatnio nie pisze zmian w plikach)
 *  2010-04-09
 *  	koreta przy wydruku do skryptu octava
 */


#ifdef WITH_GSL

#include "../../network/siecNeuronowa.h"
#include "spektrumGrafuGsl.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_eigen.h"
#include "../../../src/logger.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdio>

const int spektrumLen = 1000;
float spektrumSkal;
float spektrumShift;

int * spektrumRozklad;

gsl_vector *spektrum;
gsl_matrix * macierzSasiedztwa;
gsl_eigen_symm_workspace * workspace;

bool inicjalizacjaSpektrum(SiecNeuronowa &n);
void przepiszMacierz(SiecNeuronowa &n);
void deinicjalizacjaSpektrum();

/* *********************************************
 *
 * ********************************************/

bool inicjalizacjaSpektrum(SiecNeuronowa &n) {
	spektrum = NULL;
	macierzSasiedztwa = NULL;
	workspace = NULL;

	spektrumRozklad = NULL;

	spektrum = gsl_vector_alloc(n.getIloscNeuronow());
	if (spektrum == NULL) {
		std::cerr << "spInit(): alloc error\n";
		return false;
	} // fi

	macierzSasiedztwa = gsl_matrix_alloc(n.getIloscNeuronow(),
			n.getIloscNeuronow());
	if (macierzSasiedztwa == NULL) {
		std::cerr << "spInit(): alloc error\n";
		deinicjalizacjaSpektrum();
		return false;
	} // fi

	przepiszMacierz(n);

	workspace = gsl_eigen_symm_alloc(n.getIloscNeuronow());
	if (workspace == NULL) {
		std::cerr << "spInit(): alloc error\n";
		deinicjalizacjaSpektrum();
		return false;
	} // fi

	return true;
} // initSpektrum

void deinicjalizacjaSpektrum() {
	if (workspace != NULL) {
		gsl_eigen_symm_free(workspace);
		workspace = NULL;
	} // fi

	if (macierzSasiedztwa != NULL) {
		gsl_matrix_free(macierzSasiedztwa);
		macierzSasiedztwa = NULL;
	} // fi


	if (spektrum != NULL) {
		gsl_vector_free(spektrum);
		spektrum = NULL;
	} // if


} // deinit

void przepiszMacierz(SiecNeuronowa &n) {
	if (macierzSasiedztwa == NULL) {
		std::cerr << "spWypelnienieMacierzySasiedztwa(): m==null\n";
		return;
	} // fi

	for (int i = 0; i < n.getIloscNeuronow(); i++) {
		for (int j = 0; j < n.getIloscNeuronow(); j++) {
			gsl_matrix_set(macierzSasiedztwa, i, j, 0);
			if (i == j) {
				gsl_matrix_set(macierzSasiedztwa, i, j, +1);
			} // if
		} // for j
	} // for i

	for (int i = 0; i < n.getIloscKrawedzi(); i++) {
		Polaczenie & k = n.getKrawedz(i);
		int i1 = k.getIndeks1();
		int i2 = k.getIndeks2();

		if (k.getRoznicaLadunku() >= 1) {
			int d = k.getRoznicaLadunku();
			gsl_matrix_set(macierzSasiedztwa, i1, i2, d);
			gsl_matrix_set(macierzSasiedztwa, i2, i1, d);
			/* od/zakomentowac wydruki
			 */
			//			printf("a(%d,%d) = %d;\n", i1+1, i2+1, d);
			//			printf("a(%d,%d) = %d;\n", i2+1, i1+1, d);
		} // fi
	} // for i
} // przepiszMacierz();

void policzRozkladSpektralny(SiecNeuronowa &n) {
	long tic = time(NULL);
	if (inicjalizacjaSpektrum(n) == false) {
		return;
	} // fi


	logJP.timestamp() << "gsl_eigen_symm(): start\n";
	int res = gsl_eigen_symm(macierzSasiedztwa, spektrum, workspace);
	logJP << "gsl_eigen_symm(): koniec " << res << "\n";

	long toc = time(NULL);
	toc -= tic;
	logJP << "czas obliczeń " << ((int)toc / 3600) <<"h " << ((int)toc %3600 / 60)
		<< "m " << ((int)toc % 60) << "s \n";

	logJP.timestamp();

	// znajdujemy min i max
	double smax = -100000;
	double smin = +100000;
	for (int i = 0; i < n.getIloscNeuronow(); i++) {
		double val = gsl_vector_get(spektrum, i);
		if (val > smax)
			smax = val;
		if (val < smin)
			smin = val;
	} // for

	/* min..max -> 0..(len-1)
	 * y=ax+b
	 *
	 * 0 = min a + b
	 * len = max a + b
	 * a = len / (max-min)
	 * b = - min a
	 *
	 *
	 * konwersja y->x
	 * x = (y-b)/a
	 */

	spektrumSkal = spektrumLen / (smax - smin);
	spektrumShift = -spektrumSkal * smin;

	char buff[50];
	sprintf(buff,"spSkal = %f\nspShift = %f\nspLen=%d\n", spektrumSkal,
			spektrumShift, spektrumLen);
	logJP << buff;

	spektrumRozklad = new int[spektrumLen];
	for (int i = 0; i < spektrumLen; i++) {
		spektrumRozklad[i] = 0;
	} // for i

	for (int i = 0; i < n.getIloscNeuronow(); i++) {
		float val = gsl_vector_get(spektrum, i);
		int indeks = (int) (val * spektrumSkal + spektrumShift);
		if (indeks >= spektrumLen)
			indeks = spektrumLen - 1;
		if (indeks < 0)
			indeks = 0;

		spektrumRozklad[indeks]++;
	} // for i

	zapiszWartosciWlasneM(n);
	zapiszRozkladSpektralnyM();

	deinicjalizacjaSpektrum();

	delete[] spektrumRozklad;
} // policzRozklad();

void zapiszWartosciWlasneM(SiecNeuronowa &n) {
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "wyniki/wyniki_WartWlasne_GSL_%Y_%m_%d_%H_%M_%S.m",
			timeinfo);

	std::fstream str;
	str.open(buffer, std::fstream::out);

	if (str == NULL) {
		std::cerr << "zapiszWartościWlasne() nie mozna utworzyc pliku\n\n";
		return;
	}

	strftime(buffer, 80, "wyniki_rozkladSpektralny_%Y_%m_%d_%H_%M_%S", timeinfo);

	str << "# function ret = " << buffer << "()\n\n";
	str << "Y = [";

	if (spektrum != NULL) {
		for (int i = 0; i < n.getIloscNeuronow(); i++) {
			str << gsl_vector_get(spektrum, i) << ",\n ";
		} // for
	} // if

	str << "];\n\n";
	str << "U = sort(Y)(end:-1:1);\n" << "s = 50;\n" << "k =  " << (int) (.8
			* n.getIloscNeuronow()) << ";\n" << "\n" << "hold off;\n"
			<< "loglog(U, \"b+\");\n" << "hold on;\n"
			<< "loglog((s:k), U(s:k), \"r+\");\n"

			<< "C = s:k;\n"

			<< "a = polyfit(transpose(log(s:k)), log(U(s:k)), 1);\n"
			<< "plot(exp(log(C)),  exp(a(1).*log(C) .+ a(2)), \"g\");\n"


			<< "grid(\'off\');\n"
			<< "legend(\"all eigenvalues\", \"truncated only\", \"estimated slope\");\n"
			<< "S = sprintf(\"a = %f\", a(1));\n"
			<< "text(11, 170, S, \'fontsize\', 14);\n"
			<< "S = sprintf(\"positive eigs = %d\", length(find(Y>0)));\n"
			<< "text(11, 17, S, \'fontsize\', 14);\n";

	strftime(buffer, 80, "%Y_%m_%d", timeinfo);

	str << "# print (\"/home/piersaj/Desktop/nn/"
			<< buffer;

	sprintf(buffer, "_WartWlasne_%.1fk_%.1fM____M---sort-loglog.eps",
			n.getIloscNeuronow()/ 1000.0,
			n.getIloscKrawedzi() / 1000000.0
			// zakomantowalismy
		/*	, (int) n.getIloscIteracji() / 1000000 */);

	str << buffer << "\" , \"-color\", \"-dpdf\"); \n";

	str.close();

	return;
} // void zapiszWartościWlasneM()

void zapiszRozkladSpektralnyM() {
	char buffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "wyniki/wyniki_rozkladSpektralny_%Y_%m_%d_%H_%M_%S.m",
			timeinfo);

	std::fstream str;
	str.open(buffer, std::fstream::out);

	if (str == NULL) {
		std::cerr << "zapiszRozkladSpektralny() nie mozna utworzyc pliku\n\n";
		return;
	}

	strftime(buffer, 80, "wyniki_rozkladSpektralny_%Y_%m_%d_%H_%M_%S", timeinfo);

	//	str << "# Il neuronow "<< n.getIloscNeuronow() << '\n';
	//	str << "# Il krawedzi "<< n.getIloscKrawedzi() << '\n';
	//	str << "# Il iteracji "<< (int) n.getIloscIteracji() << '\n';

	str << "function ret = " << buffer << "()\n\n";
	str << "Y = [";

	for (int i = 0; i < spektrumLen; i++) {
		str << (i - spektrumShift) / spektrumSkal << " , "
				<< (spektrumRozklad[i] * 1.0) / spektrumLen << " ;\n ";
	} // for

	str << "];\n\n";

	str << "plot(Y(:,1), Y(:,2), \"r+\");\n"
			<< "# sta = round(length(Y(:,1)) * .05);\n"
			<< "# st = round(length(Y(:,1)) * .70);\n"
			<< "# X = log(Y(:,1));\n"
			<< "# Z = (log(cumsum(Y(:,2)(end:-1:1))))(end:-1:1);\n"

	<< "# hold off;\n"

	<< "# plot (X, Z, \"b+\");\n" << "# hold on;\n"
			<< "# plot(X(sta:1:st), Z(sta:1:st)  , \"r+\");\n"

	<< "# a = polyfit(  X(sta:1:st), Z(sta:1:st)  , 1);\n"

	<< "# x = -4.5:.1:0;\n" << "# plot(x, a(1).*x + a(2), \"g\");\n"
			<< "# text(-4, 3,  sprintf(\"a=%f\", a(1)));\n" << "# hold off;\n"
			<< "# print (\"/home/piersaj/Desktop/nn/";
	strftime(buffer, 80, "%Y_%m_%d", timeinfo);

	str << buffer;
	//	sprintf(buffer, "_sfera_%.1fk_%dM_%dM----XX.eps", n.getIloscNeuronow()/1000.0, n.getIloscKrawedzi()/1000000,
	//			(int) n.getIloscIteracji() / 1000000);

	sprintf(buffer, "_sfera_XXk_XXM_XXM----XXX.eps");
	str << buffer << "\" , \"-color\", \"-dpdf\"); \n";
	str << "endfunction\n";
	str.close();

	return;
} // zapiszRozkladSpektralny



#endif
