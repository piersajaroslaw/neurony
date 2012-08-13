/*
 * simulationMasterConfig.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: piersaj
 *   2011-03-30
 *   	created a class stub
 *   2011-03-31
 *   	added default values
 *   2011-04-08
 *   	added a few more fields
 *   	class was put into a good use :))
 *
 *
 */

#include "simulationMasterConfig.h"
#include "iostream"
#include <cstring>

SimulationConfig::SimulationConfig(char * confPath){
	SimulationConfig();
	this->confFile = confPath;
}	// this()

/*
 * a constructor wit default values
 */
SimulationConfig::SimulationConfig(){
	geometry = 3;
	radius = 10 ;
	neuronsDensity = 10;
	connectivityAlpha = 2.5;
	inverseTemperatureBeta = 1000;
	initialChargeMean = 1000;
	initialChargeVar  = 1000;

	/*
	 * dynamics settings
	 */
	longRangeSynapseThreshold = -1;
	flagChargeRemovalOnLongSynapse = true;
	probabilityOfChargeRemovalOnLongSynapse = .1;

	chargeReappear = false;
	edgesRemoval = true;

	threads_number = 2;

	/*
	 *  logging settings
	 */
//		char * timestamp;
	confFile = (char*) "./neurons.conf";
	strcpy(resultsFolder, "./");

	logMaster = true;
	logProgress = true;
	useGLmode = false;
	useTimestampInFilenames = true;

	/*
	 * results settings
	 */

	calculateSpectrum = true;
	calculateSpectrumEvolution = true;

	calculateSynapseEvolution = true;
	calculateNodesEvolution = true;
	calculateDegreeDistribution = true;
	calculatePathLengthDistribution = true;
	calculateClasteringCoeffitientDistribution = true;
	calculateNetworkDegeneracy = true;

	setTimeStamp();
}	// this


SimulationConfig::~SimulationConfig(){
}	// ~this()
/* *********************************
 *
 * Non - default setters and getters
 *
 * *********************************/
void SimulationConfig::setTimeStamp(){

}

bool SimulationConfig::getUseTimestampInFilenames() const
{
    return useTimestampInFilenames;
}

long SimulationConfig::getIterationsNumber() const
{
    return iterationsNumber;
}

int SimulationConfig::getKohonenEdgesLimit() const
{
    return kohonenEdgesLimit;
}




void SimulationConfig::setKohonenEdgesLimit(int kohonenEdgesLimit)
{
    this->kohonenEdgesLimit = kohonenEdgesLimit;
}


long SimulationConfig::getKohonenChargeThreshold() const
{
    return kohonenChargeThreshold;
}

void SimulationConfig::setKohonenChargeThreshold(long kohonenChargeThreshold)
{
    this->kohonenChargeThreshold = kohonenChargeThreshold;
}

void SimulationConfig::setIterationsNumber(long  iterationsNumber)
{
    this->iterationsNumber = iterationsNumber;
}

void SimulationConfig::setUseTimestampInFilenames(bool useTimestampInFilenames)
{
    this->useTimestampInFilenames = useTimestampInFilenames;
}

void SimulationConfig::setUseGLmode(bool useGLmode){
//    this->useGLmode = useGLmode;
	if (useGLmode == true){
		std::cerr << "Config.setUseGL(): GL mode is enforced to be disabled\n";
	}
	this->useGLmode = false;
}


void SimulationConfig::setGeometry(int geometry){
	if (geometry >= 0 &&  geometry <= 8){
		this->geometry = geometry;
	} else {
		std::cerr << "Config.setGeometry(): incorrect newGeometry submitted "
				<< geometry <<"\n";
		geometry = 3;
	}	// if .. else

}	// setGeometry

/* ************************************************
 *
 *  getters and setters - do not touch this!
 *
 * ************************************************/
bool SimulationConfig::getCalculateFailureResiliency() const
{
    return calculateFailureResiliency;
}

bool SimulationConfig::getCalculateDegreeDistribution() const
{
    return calculateDegreeDistribution;
}

bool SimulationConfig::getCalculateClasteringCoeffitientDistribution() const
{
    return calculateClasteringCoeffitientDistribution;
}

bool SimulationConfig::getCalculateNodesEvolution() const
{
    return calculateNodesEvolution;
}

bool SimulationConfig::getCalculatePathLengthDistribution() const
{
    return calculatePathLengthDistribution;
}
bool SimulationConfig::getCalculatePathLengthDistributionMC() const
{
    return calculatePathLengthDistributionMC;
}

bool SimulationConfig::getCalculateSpectrum() const
{
    return calculateSpectrum;
}

bool SimulationConfig::getCalculateSpectrumEvolution() const
{
    return calculateSpectrumEvolution;
}

bool SimulationConfig::getCalculateSynapseEvolution() const
{
    return calculateSynapseEvolution;
}

bool SimulationConfig::getChargeReappear() const
{
    return chargeReappear;
}

char *SimulationConfig::getConfFile() const
{
    return confFile;
}

char *SimulationConfig::getResultsFolder() const
{
    return (char*) resultsFolder;
}

float SimulationConfig::getConnectivityAlpha() const
{
    return connectivityAlpha;
}

bool SimulationConfig::getEdgesRemoval() const
{
    return edgesRemoval;
}

bool SimulationConfig::getFlagChargeRemovalOnLongSynapse() const
{
    return flagChargeRemovalOnLongSynapse;
}

int SimulationConfig::getGeometry() const
{
    return geometry;
}

float SimulationConfig::getInitialChargeMean() const
{
    return initialChargeMean;
}

float SimulationConfig::getInitialChargeVar() const
{
    return initialChargeVar;
}

float SimulationConfig::getInverseTemperatureBeta() const
{
    return inverseTemperatureBeta;
}

bool SimulationConfig::getLogMaster() const
{
    return logMaster;
}

bool SimulationConfig::getLogProgress() const
{
    return logProgress;
}

float SimulationConfig::getLongRangeSynapseThreshold() const
{
    return longRangeSynapseThreshold;
}

float SimulationConfig::getNeuronsDensity() const
{
    return neuronsDensity;
}

float SimulationConfig::getProbabilityOfChargeRemovalOnLongSynapse() const
{
    return probabilityOfChargeRemovalOnLongSynapse;
}

float SimulationConfig::getRadius() const
{
    return radius;
}

int SimulationConfig::getThreads_number() const
{
    return threads_number;
}

char *SimulationConfig::getTimestamp() const
{
    return timestamp;
}

bool SimulationConfig::getUseGLmode() const
{
    return useGLmode;
}


void SimulationConfig::setCalculateFailureResiliency(bool param)
{
    this->calculateFailureResiliency= param;
}

void SimulationConfig::setCalculateDegreeDistribution(bool calculateDegreeDistribution)
{
    this->calculateDegreeDistribution = calculateDegreeDistribution;
}

void SimulationConfig::setCalculateClasteringCoeffitientDistribution(bool calculateClasteringCoeffitientDistribution)
{
    this->calculateClasteringCoeffitientDistribution = calculateClasteringCoeffitientDistribution;
}

void SimulationConfig::setCalculateNodesEvolution(bool calculateNodesEvolution)
{
    this->calculateNodesEvolution = calculateNodesEvolution;
}

void SimulationConfig::setCalculatePathLengthDistribution(bool calculatePathLengthDistribution)
{
    this->calculatePathLengthDistribution = calculatePathLengthDistribution;
}

void SimulationConfig::setCalculatePathLengthDistributionMC(bool calculatePathLengthDistributionMC)
{
    this->calculatePathLengthDistributionMC = calculatePathLengthDistributionMC;
}

void SimulationConfig::setCalculateSpectrum(bool calculateSpectrum)
{
    this->calculateSpectrum = calculateSpectrum;
}

void SimulationConfig::setCalculateSpectrumEvolution(bool calculateSpectrumEvolution)
{
    this->calculateSpectrumEvolution = calculateSpectrumEvolution;
}

void SimulationConfig::setCalculateSynapseEvolution(bool calculateSynapseEvolution)
{
    this->calculateSynapseEvolution = calculateSynapseEvolution;
}

void SimulationConfig::setChargeReappear(bool chargeReappear)
{
    this->chargeReappear = chargeReappear;
}

void SimulationConfig::setConnectivityAlpha(float connectivityAlpha)
{
    this->connectivityAlpha = connectivityAlpha;
}

void SimulationConfig::setEdgesRemoval(bool edgesRemoval)
{
    this->edgesRemoval = edgesRemoval;
}

void SimulationConfig::setFlagChargeRemovalOnLongSynapse(bool flagChargeRemovalOnLongSynapse)
{
    this->flagChargeRemovalOnLongSynapse = flagChargeRemovalOnLongSynapse;
}



void SimulationConfig::setInitialChargeMean(float initialChargeMean)
{
    this->initialChargeMean = initialChargeMean;
}

void SimulationConfig::setInitialChargeVar(float initialChargeVar)
{
    this->initialChargeVar = initialChargeVar;
}

void SimulationConfig::setInverseTemperatureBeta(float inverseTemperatureBeta)
{
    this->inverseTemperatureBeta = inverseTemperatureBeta;
}

void SimulationConfig::setLogMaster(bool logMaster)
{
    this->logMaster = logMaster;
}

void SimulationConfig::setLogProgress(bool logProgress)
{
    this->logProgress = logProgress;
}

void SimulationConfig::setLongRangeSynapseThreshold(float longRangeSynapseThreshold)
{
    this->longRangeSynapseThreshold = longRangeSynapseThreshold;
}

void SimulationConfig::setNeuronsDensity(float neuronsDensity)
{
    this->neuronsDensity = neuronsDensity;
}

void SimulationConfig::setProbabilityOfChargeRemovalOnLongSynapse(float probabilityOfChargeRemovalOnLongSynapse)
{
    this->probabilityOfChargeRemovalOnLongSynapse = probabilityOfChargeRemovalOnLongSynapse;
}

void SimulationConfig::setRadius(float radius)
{
    this->radius = radius;
}

void SimulationConfig::setThreads_number(int threads_number)
{
    this->threads_number = threads_number;
}

void SimulationConfig::setResultsFolder(const char* path)
{
    strcpy(resultsFolder, path);
}


bool SimulationConfig::getCalculateNetworkDegeneracy(){
	return calculateNetworkDegeneracy;
}

void SimulationConfig::setCalculateNetworkDegeneracy(bool calculateNetworkDegeneracy) {
	this->calculateNetworkDegeneracy = calculateNetworkDegeneracy;
}

bool SimulationConfig::getCalculateDegreeCorrelation(){
	return calculateDegreeCorrelation;
}

void SimulationConfig::setCalculateDegreeCorrelation(bool val) {
	this->calculateDegreeCorrelation = val;
}
