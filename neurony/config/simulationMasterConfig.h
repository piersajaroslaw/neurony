/*
 * simulationMasterConfig.h
 *
 *  Created on: Mar 30, 2011
 *      Author: piersaj
 *
 *  A config file used to store settings :P
 *
 *  2011-03-30
 *  	created
 *  2011-04-08
 *   	added a few more fields
 *   	class was put into a good use :))
 */

#ifndef SIMULATIONMASTERCONFIG_H_
#define SIMULATIONMASTERCONFIG_H_


class SimulationConfig {
private:
	/*
	 *  network settings
	 */
	int geometry;
	float radius;
	float neuronsDensity;
	float connectivityAlpha;
	float inverseTemperatureBeta;

	float initialChargeMean;
	float initialChargeVar;

	/*
	 * dynamics settings
	 */

	long iterationsNumber;
	float longRangeSynapseThreshold;
	bool flagChargeRemovalOnLongSynapse;
	float probabilityOfChargeRemovalOnLongSynapse;

	bool chargeReappear;
	bool edgesRemoval;

	int threads_number;

	/*
	 *  logging settings
	 */
	char * timestamp;
	char * confFile;
	char resultsFolder[255];

	bool useTimestampInFilenames;
	bool logMaster;
	bool logProgress;

	bool useGLmode;


	/*
	 * results settings
	 */

	int kohonenEdgesLimit;
	long kohonenChargeThreshold;
	bool calculateSpectrum;
	bool calculateSpectrumEvolution;

	bool calculateSynapseEvolution;
	bool calculateNodesEvolution;

	bool calculateDegreeDistribution;
	bool calculatePathLengthDistribution;
	bool calculatePathLengthDistributionMC;
	bool calculateClasteringCoeffitientDistribution;
	bool calculateFailureResiliency;
	bool calculateNetworkDegeneracy;
	bool calculateDegreeCorrelation;


public:
	SimulationConfig();
	SimulationConfig(char * confpath);
	~SimulationConfig();

private:
	// note that TS is autamaticly picked from system time
	void setTimeStamp();
	void readReadConfigFromFile();

public:
    bool getCalculateFailureResiliency() const;
    bool getCalculateDegreeDistribution() const;
    bool getCalculateClasteringCoeffitientDistribution() const;
    bool getCalculateNodesEvolution() const;
    bool getCalculatePathLengthDistribution() const;
    bool getCalculatePathLengthDistributionMC() const;
    bool getCalculateSpectrum() const;
    bool getCalculateSpectrumEvolution() const;
    bool getCalculateSynapseEvolution() const;
    bool getChargeReappear() const;
    char *getConfFile() const;
    char *getResultsFolder() const;
    float getConnectivityAlpha() const;
    bool getEdgesRemoval() const;
    bool getFlagChargeRemovalOnLongSynapse() const;
    int getGeometry() const;
    float getInitialChargeMean() const;
    float getInitialChargeVar() const;
    float getInverseTemperatureBeta() const;
    bool getLogMaster() const;
    bool getLogProgress() const;
    float getLongRangeSynapseThreshold() const;
    float getNeuronsDensity() const;
    float getProbabilityOfChargeRemovalOnLongSynapse() const;
    float getRadius() const;
    int getThreads_number() const;
    char *getTimestamp() const;
    bool getUseGLmode() const;

    /*
     * settery
     */
    void setCalculateFailureResiliency(bool calculateFailureResiliency);
    void setCalculateDegreeDistribution(bool calculateCegreeDistribution);
    void setCalculateClasteringCoeffitientDistribution(bool calculateClasteringCoeffitientDistribution);
    void setCalculateNodesEvolution(bool calculateNodesEvolution);
    void setCalculatePathLengthDistribution(bool calculatePathLengthDistribution);
    void setCalculatePathLengthDistributionMC(bool calculatePathLengthDistributionMC);
    void setCalculateSpectrum(bool calculateSpectrum);
    void setCalculateSpectrumEvolution(bool calculateSpectrumEvolution);
    void setCalculateSynapseEvolution(bool calculateSynapseEvolution);
    void setChargeReappear(bool chargeReappear);
    void setConnectivityAlpha(float connectivityAlpha);
    void setEdgesRemoval(bool edgesRemoval);
    void setFlagChargeRemovalOnLongSynapse(bool flagChargeRemovalOnLongSynapse);
    void setGeometry(int geometry);
    void setInitialChargeMean(float initialChargeMean);
    void setInitialChargeVar(float initialChargeVar);
    void setInverseTemperatureBeta(float inverseTemperatureBeta);
    void setLogMaster(bool logMaster);
    void setLogProgress(bool logProgress);
    void setLongRangeSynapseThreshold(float longRangeSynapseThreshold);
    void setNeuronsDensity(float neuronsDensity);
    void setProbabilityOfChargeRemovalOnLongSynapse(float probabilityOfChargeRemovalOnLongSynapse);
    void setRadius(float radius);
    void setThreads_number(int threads_number);
    void setUseGLmode(bool useGLmode);
    void setResultsFolder(const char * path);
    bool getUseTimestampInFilenames() const;
    void setUseTimestampInFilenames(bool useTimestampInFilenames);
    long getIterationsNumber() const;
    void setIterationsNumber(long  iterationsNumber);
    int getKohonenEdgesLimit() const;
    void setKohonenEdgesLimit(int kohonenEdgesLimit);

    long getKohonenChargeThreshold() const;
    void setKohonenChargeThreshold(long kohonenChargeThreshold);

    bool getCalculateNetworkDegeneracy();
    void setCalculateNetworkDegeneracy(bool calculate);
    bool getCalculateDegreeCorrelation();
    void setCalculateDegreeCorrelation(bool calculateDegreeCorrelation);

};



#endif /* SIMULATIONMASTERCONFIG_H_ */
