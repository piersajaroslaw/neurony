/*
 * simulationConfigParser.h
 *
 *  Created on: Apr 8, 2011
 *      Author: piersaj
 *
 *  2011-04-08
 *  	created
 *  	nicely built and made it work :)
 *  2011-07-15
 *  	it may was cool but libxml2 was problematic :((
 *  	the code was disabled
 */

#ifndef SIMULATIONCONFIGPARSER_H_
#define SIMULATIONCONFIGPARSER_H_

/*
 * note rquires a few weird libs
 *
 * for compilation flags   (Build-Settings->Compile->Misc)
 *  `pkg-config  libxml++-2.6  --cflags`
 *
 * for linking flags   (Build-Settings->Compile->Misc)
 *  `pkg-config  libxml++-2.6  --libs`
 *
 */
// #define	WITH_XML_LIB2 0


#ifdef WITH_XML_LIB2

#include "simulationMasterConfig.h"
#include "glibmm-2.4/glibmm/ustring.h"

/* Reading configuration from xmlfile
 * uses xmllib
 */
class SimulationConfigParser{
	const char * DEFAULT_PATHNAME;
	char * pathname;
	// note: this should persist through destructor!
	SimulationConfig * conf;

public:
	SimulationConfigParser();
	SimulationConfigParser(char * pathname);
	SimulationConfig * getSimulationConfig();

private:
	void parseXmlConfig();
	void iAmSureThereWillBeMore();

	void setMasterConfigEntry(Glib::ustring & paramname, Glib::ustring & paramval);

	void setMasterConfigEntryNetworkSettings(Glib::ustring & paramname, char* paramval);
	void setMasterConfigEntryDynamicsSettings(Glib::ustring & paramname, char* paramval);
	void setMasterConfigEntryLoggingSettings(Glib::ustring & paramname, char* paramval);
	void setMasterConfigEntryResultsSettings(Glib::ustring & paramname, char* paramval);


};	// class

#endif // LIB_XML2

#endif /* SIMULATIONCONFIGPARSER_H_ */
