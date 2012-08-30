/*
 * spektrumGrafuLapack.cpp
 *
 *  Created on: May 24, 2010
 *      Author: piersaj
 *
 *  http://seehuhn.de/pages/linear#lapack-easy
 *  cc testdgtsv.c -o testdgtsv -llapack -lblas
 */


#include "spektrumGrafuLapack.h"

#ifdef  WITH_LAPACK


#include "cstdio"
#include "../../utils/logger.h"

/* deklarujemy funkcje pochodzaca z biblioteki lapack
 *
 * S SP EV
 * Single precision - float
 * Symmetric Packed (!= sparse) matrix
 * EigenValues
 *
 */
extern "C" void sspev_(char * JOBZ, char * UPLO, long * N, float * AP, float * W,
		float * Z, long * LDZ, float * WORK, long * INFO);

float * wartosciWlasne=NULL;

long timer1, timer2;

/*  Pakowanie macierzy
 *
 *  	if UPLO = 'U',
 *  		AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j;
 *      if UPLO = 'L',
 *      	AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n.
 *
 *  0  1  3  6 ...
 *     2  4  7
 *        5  8
 *           9
 */
void wypelnijMacierzLapack(SiecNeuronowa &n, float * ap){
	long N = n.getIloscNeuronow();
	if (ap == NULL){
		ap = new float[N*(N-1)/2 + N];
	}	// fi

	for (int i=0; i< N*(N-1)/2 + N; i++ ){
		ap[i] = 0;
	}	// fi

	for (int i=0; i<n.getIloscKrawedzi(); i++){
		Polaczenie & k = n.getKrawedz(i);
		if (k.getSumaLadunku()>0){
			int x = k.getIndeks1() < k.getIndeks2() ? k.getIndeks1() : k.getIndeks2();
			int y = k.getIndeks1() > k.getIndeks2() ? k.getIndeks1() : k.getIndeks2();
			//  x = wiersz
			//  y = kolumna
			//  x <= y

			int indeks = (y+1)*y / 2  + (x+1) - 1;
				// tyle jest we wczesniejszych kolumnach
				// + aktualna kolumna
				// + poprawka na indeks bo liczymy od 0

			ap[indeks] = k.getSumaLadunku();
		}	// fi
	}	// for i

}	// wypelnijMacierz


void policzWartosciWlasneLapack(SiecNeuronowa &n){
	policzWartosciWlasneLapackSilent(n, false);
}	// policzWartosciWlasneLapack


void policzWartosciWlasneLapackSilent(SiecNeuronowa &n, bool silent){
	if (!silent){
		logJP.timestamp_nl();
		logJP << "Lapack eigs:\n";
	}	// fi

	// zadanie, N == tylko wartosci wlasne
	char jobs = 'N';

	// postac macierzy == górny / dolny trójkat
	char uplo = 'U';

	// rozmiar macierzy
	long N = n.getIloscNeuronow();

	// macierz
	float *ap = NULL;
	ap = new float[ N*(N-1)  / 2 + N];

	if (wartosciWlasne == NULL)
		wartosciWlasne = new float[N];

	// ilosc wektorow
	long ldz = 1;
	float * z = new float[N*ldz];

	// workspace
	float * work = new float[3*N];

	// rezultat
	long info1 = 0;

	if (ap == NULL || work == NULL || wartosciWlasne == NULL || z == NULL){
		logJP << "Lapack.pWWL(): new[] error\n";
	}	// if

	if (!silent){
		logJP << "filling up the matrix: ";
	}	// fi

	wypelnijMacierzLapack(n, ap);

	if (!silent){
		logJP << "done!   ";
		logJP << "_sspev() start\n";
	}	// fi

	timer1 = time(NULL);
	sspev_( &jobs, &uplo, &N, ap, wartosciWlasne, z, &ldz, work, &info1 );

	timer2 = time(NULL);
	timer2 -=timer1;


	if (!silent){
		logJP << "time: " << timer2 / 3600 << "h " <<  timer2 %3600 / 60 << " m "  << timer2 % 60 << "s\n";
	}	// fi

	if (info1 != 0){
		fprintf(stderr, "_sspev() failure with error %ld\n", info1);
	}	// if

	if (!silent){
		logJP. timestamp() << "   _sspev() end\n";
	}	// fi

	delete [] ap;
	delete [] work;
	delete [] z;
	// wartosciWlasne nie sa usuwane !!!

//	zapiszWartosciWlasneLapack(n);
}	// policz WartosciWlasne



float * getWartosciWlasneLapack(){
	return wartosciWlasne;
}

void zapiszWartosciWlasneLapack(SiecNeuronowa &n){
	char buffer[80], filename[80];

	if (n.getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_spectrum.m",
				n.getConfig()->getResultsFolder(),
				n.getTimestamp(),
				n.getIloscNeuronow()/1000,
				n.getIloscKrawedzi()/1000000,
				n.getIloscIteracji() / 1000000
				);
	} else {
		sprintf(filename, "%s/spectrum.m", n.getConfig()->getResultsFolder());
	}	// if .. else

	std::fstream str;
	str.open(filename, std::fstream::out);

	if (str == NULL) {
		std::cerr << "zapiszWartościWlasneLapack() nie mozna utworzyc pliku\n\n";
		return;
	}


	str << "# function ret = " << filename << "()\n\n";
	str << "Y = [";

	if (wartosciWlasne != NULL) {
		for (int i = 0; i < n.getIloscNeuronow(); i++) {
			str << wartosciWlasne[i] << ",\n ";
		} // for
	} // if

	str << "];\n\n";
	str << "U = sort(Y)(end:-1:1);\n" << "s = 50;\n" << "k =  " << (int) (.8
			* n.getIloscNeuronow()) << ";\n" << "\n" << "hold off;\n"
			<< "loglog(U(1:s), \"b+\");\n"
			<< "hold on;\n"
			<< "loglog((s:k), U(s:k), \"r+\");\n"
			<< "C = s:k;\n"
			<< "a = polyfit(transpose(log(s:k)), log(U(s:k)), 1);\n"
			<< "plot(exp(log(C)),  exp(a(1).*log(C) .+ a(2)), \"g\");\n"
			<< "loglog((k:length(U)), U(k:end), \"b+\");\n" << "hold on;\n"

			<< "grid(\'on\');\n"
			<< "legend(\"All eigenvalues\", \"Truncated only\", \"Estimated slope\");\n"
			<< "S = sprintf(\"a = %f\", a(1));\n"
			<< "text(11, 170, S, \'fontsize\', 14);\n"
			<< "S = sprintf(\"positive eigs = %d\", length(find(Y>0)));\n"
			<< "text(11, 17, S, \'fontsize\', 14);\n"
			<< "S = sprintf(\"all eigs = %d\", length(Y));\n"
			<< "text(11, 1.7, S, \'fontsize\', 14);\n"
			<< "S = sprintf(\"trunc range = %d..%d\", s, k);\n"
			<< "text(11, .17, S, \'fontsize\', 14);\n";


	str << "# print (\"/home/piersaj/Desktop/nn/"
			<< n.getTimestamp();

	sprintf(buffer, "_WartWlasne_lapack_%.1fk_%.1fM_%.0dM---sort-loglog.svg",
			n.getIloscNeuronow()/ 1000.0,
			n.getIloscKrawedzi() / 1000000.0
			// TDOO zakomentowalismy
			,
			(int) n.getIloscIteracji() / 1000000);

	str << buffer << "\" , \"-color\"); \n";

	str.close();

	return;
}	// zapiszWartosciWlasneLapack


/*
 *  INSTRUKCJA
 */

/*
*      SUBROUTINE SSPEV( JOBZ, UPLO, N, AP, W, Z, LDZ, WORK, INFO )
*
*  -- LAPACK driver routine (version 3.2) --
*  -- LAPACK is a software package provided by Univ. of Tennessee,    --
*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
*     November 2006
*
*     .. Scalar Arguments ..
      CHARACTER          JOBZ, UPLO
      INTEGER            INFO, LDZ, N
*     ..
*     .. Array Arguments ..
      REAL               AP( * ), W( * ), WORK( * ), Z( LDZ, * )
*     ..
*
*  Purpose
*  =======
*
*  SSPEV computes all the eigenvalues and, optionally, eigenvectors of a
*  real symmetric matrix A in packed storage.
*
*  Arguments
*  =========
*
*  JOBZ    (input) CHARACTER*1
*          = 'N':  Compute eigenvalues only;
*          = 'V':  Compute eigenvalues and eigenvectors.
*
*  UPLO    (input) CHARACTER*1
*          = 'U':  Upper triangle of A is stored;
*          = 'L':  Lower triangle of A is stored.
*
*  N       (input) INTEGER
*          The order of the matrix A.  N >= 0.
*
*  AP      (input/output) REAL array, dimension (N*(N+1)/2)
*          On entry, the upper or lower triangle of the symmetric matrix
*          A, packed columnwise in a linear array.  The j-th column of A
*          is stored in the array AP as follows:
*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j;
*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n.
*
*          On exit, AP is overwritten by values generated during the
*          reduction to tridiagonal form.  If UPLO = 'U', the diagonal
*          and first superdiagonal of the tridiagonal matrix T overwrite
*          the corresponding elements of A, and if UPLO = 'L', the
*          diagonal and first subdiagonal of T overwrite the
*          corresponding elements of A.
*
*  W       (output) REAL array, dimension (N)
*          If INFO = 0, the eigenvalues in ascending order.
*
*  Z       (output) REAL array, dimension (LDZ, N)
*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal
*          eigenvectors of the matrix A, with the i-th column of Z
*          holding the eigenvector associated with W(i).
*          If JOBZ = 'N', then Z is not referenced.
*
*  LDZ     (input) INTEGER
*          The leading dimension of the array Z.  LDZ >= 1, and if
*          JOBZ = 'V', LDZ >= max(1,N).
*
*  WORK    (workspace) REAL array, dimension (3*N)
*
*  INFO    (output) INTEGER
*          = 0:  successful exit.
*          < 0:  if INFO = -i, the i-th argument had an illegal value.
*          > 0:  if INFO = i, the algorithm failed to converge; i
*                off-diagonal elements of an intermediate tridiagonal
*                form did not converge to zero.
*/



#endif // WITH_LAPACK
