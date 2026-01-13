/**
 * @file compton.cc
 * @brief Main application entry point
 * 
 * This file sets up the JANA2 application framework and registers all necessary
 * components for processing compton polarimeter data from EVIO files.
 */

#include <JANA/JApplication.h>           // Core JANA2 application framework
#include <JANA/CLI/JMain.h>             // Command line options parsing utilities
#include <JANA/Services/JParameterManager.h>  // Parameter management system
#include <JANA/JEventSourceGeneratorT.h>     // Event source generator template

// Experiment specific components
#include "JEventSource_EVIO.h"              // EVIO file event source
#include "JEventProcessor_Compton.h"        // Main event processor
#include "JEventUnfolder_EVIO.h"            // Event unfolder
#include "JEventService_FilterDB.h"         // Service for ROC/bank filtering
#include "JEventService_BankParsersMap.h"   // Service for mapping bank IDs to bank parsers
#include "BankParser_FADC.h"                // FADC250 bank parser implementation
#include "BankParser_FADCScaler.h"          // FADC scaler bank parser implementation
#include "BankParser_TIScaler.h"            // TI scaler bank parser implementation
#include "BankParser_HelicityDecoder.h"     // Helicity decoder bank parser implementation

/**
 * @brief Main function for experiment data processing application
 * 
 * This function:
 * 1. Parses command line arguments and parameters
 * 2. Creates and configures the JANA2 application
 * 3. Registers all necessary components and services
 * 4. Initializes and runs the application
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Application exit code
 */
int main(int argc, char* argv[]) {

    // Parse command line options and extract parameters
    auto options = jana::ParseCommandLineOptions(argc, argv);
    auto params = new JParameterManager();

    // Set all parsed parameters in the parameter manager
    for(auto pair: options.params) {
        params->SetParameter(pair.first, pair.second);
    }
    
    // Create the main JANA2 application with parameter manager
    JApplication app(params);

    // Register input EVIO files from command line arguments
    for(auto event_source : options.eventSources) {  
        app.Add(event_source);  
    } 

    // Register all application components
    app.Add(new JEventSourceGeneratorT<JEventSource_EVIO>());  // EVIO file reader (creates PhysicsEvent objects in ProcessParallel)
    app.Add(new JEventUnfolder_EVIO());                        // Event unfolder
    app.Add(new JEventProcessor_Compton());                    // Data processor

    // Register services
    app.ProvideService(std::make_shared<JEventService_FilterDB>());
    app.ProvideService(std::make_shared<JEventService_BankParsersMap>());

    // Initialize the application
    app.Initialize(); // This will initialize the application and make services available

    // Register bank parser implementations
    auto bank_parsers_svc = app.GetService<JEventService_BankParsersMap>();
    bank_parsers_svc->addParser(250, new BankParser_FADC());
    bank_parsers_svc->addParser(9250, new BankParser_FADCScaler());
    bank_parsers_svc->addParser(9001, new BankParser_TIScaler());
    bank_parsers_svc->addParser(0xdec, new BankParser_HelicityDecoder());

    // Run the application
    app.Run();

    // Retrieve and return the exit code
    return app.GetExitCode();
}
