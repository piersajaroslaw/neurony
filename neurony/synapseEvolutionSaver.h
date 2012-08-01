/*
 * synapseEvolutionSaver.h
 *
 *  Created on: Mar 16, 2011
 *      Author: piersaj
 *
 *  2011-03-16
 *  	Synapses saver - namely graph saver,
 *  	note - the graph is huge and so it should be saved with caution
 *  2011-03-24
 *  	added printout to svg  (xml) rather than to
 */

#ifndef SYNAPSEEVOLUTIONSAVER_H_
#define SYNAPSEEVOLUTIONSAVER_H_


#include "fileWriter.h"
#include "network/siecNeuronowa.h"

class SynapsesEvolutionSaver{

private:
	SiecNeuronowa * n;
	char * filename;
	NNFileWrter * writer;
	int currentSnapshot;
	int maxSnapshot;
    long chargeThreshold;
    float fractionThreshold;
    int edgesLimit;
    long maxFlow;

    // how many synapses of this [i] charge and above
    long *precalcThresholds;
    long precalcThresholdsLen;

    // Kohonen
    float * kohPosX, *kohPosY;
    // positon
    int maxPosX, maxPosY;
    long iterations, currentIteration;


    int svgCanvasSizeX, svgCanvasSizeY;


public:
    SynapsesEvolutionSaver(SiecNeuronowa*);
    ~SynapsesEvolutionSaver();

    void generateSvgSnapshot();

protected:
    void generateHeader();
    void generateCalculationTic();
    void generateFooter();

//    void generateHeaderM();
//    void generateFooterM();
//    void generateCalculationTicM();


    /*
     *  settety i gettery
     */
public:
    int getEdgesLimit() const;
    void setEdgesLimit(int  el);
    long getChargeThreshold() const;
    void setChargeThreshold(long  chargeThreshold);


private:
    // @depracated - dont use it!
    float getFractionThreshold() const;
    void setFractionThreshold(float fractionThreshold);

private:
    int pickEdgeColorR(long);
    /*
     * Kohonen
     */

    void precalculateThresholds();
    void initKohonen();
    void doSingleStepKohonen();
    long findClosestIndexTo(float x, float y);
    void selfOrganizeKohonen();
    void deinitKohonen();
    void normalisePositions();

    void pickRandomPointSquare(float &x, float &y);
    void pickRandomPointCircle(float &x, float &y);

    /*
     * random subgraph
     */
    bool * isNodePicked;
    void pickRandomSubgraph();

};



#endif /* SYNAPSEEVOLUTIONSAVER_H_ */
