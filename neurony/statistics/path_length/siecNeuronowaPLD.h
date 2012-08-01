#ifndef SIECNEURONOWAPLD_H_
#define SIECNEURONOWAPLD_H_
/*
 * siecNeuronowaPLD.h
 *
 *  Created on: Oct 25, 2010
 *      Author: piersaj
 */


#include "../../siecNeuronowaDynamika.h"

class SiecNeuronowaPLD{
private:
	SiecNeuronowaDynamika &n;
	int maxDlugoscSciezki;
	int iloscKrokowMC;
	int * rozkladDlugosciSciezki;
	int ** tablicaDlugosciSciezek;
	float apl_val;
	int softThresh;
	int hardThresh;


public:
	SiecNeuronowaPLD(SiecNeuronowaDynamika &n);
	~SiecNeuronowaPLD();


public:
    void obliczRozkladDlugosciSciezek();
    void obliczRozkladDlugosciSciezekMC();
    void zapiszRozkladDlugosciSciezekDoPliku(bool usedMC = false);
    void obliczAPL();
    float getAPL();
    float getER_APL();

    /* *********************************************
     *  gettery i settery
     * ********************************************/

    inline float getApl_val() const{
        return apl_val;
    }

    inline int getIloscKrokowMC() const{
        return iloscKrokowMC;
    }

    inline void setIloscKrokowMC(int iloscKrokowMC) {
        this->iloscKrokowMC = iloscKrokowMC;
    }

    inline void setMaxDlugoscSciezki(int maxDlugoscSciezki) {
        this->maxDlugoscSciezki = maxDlugoscSciezki;
    }
    inline int getMaxDlugoscSciezki() const {
        return maxDlugoscSciezki;
    }

    inline SiecNeuronowaDynamika & getN() const    {
        return n;
    }

    inline int *getRozkladDlugosciSciezki() const    {
        return (int*)(rozkladDlugosciSciezki);
    }

private:
    int obliczDlugoscSciezki(int i1, int i2);
    void obliczTabliceSciezek();
    void obliczTabliceSciezekV2();
    void inicjalizujTabliceSciezek();

    /*
     * te nie powinny byc publiczne!
     */
    inline int getTabDlugoscSciezki(int i1, int i2)    {
        if (tablicaDlugosciSciezek == NULL){
    		// error
    		return -1;
    	}
        if(i1 < i2)
            return tablicaDlugosciSciezek[i2][i1];

        else
            return tablicaDlugosciSciezek[i1][i2];

    }

    inline void setTabDlugoscSciezki(int i1, int i2, int val)    {
        if (tablicaDlugosciSciezek == NULL){
			// error
			return;
		}
        if(i1 < i2)
            tablicaDlugosciSciezek[i2][i1] = val;

        else
            tablicaDlugosciSciezek[i1][i2] = val;

    }
    void wypiszRozkladSciezek();

};






#endif /* SIECNEURONOWAPLD_H_ */
