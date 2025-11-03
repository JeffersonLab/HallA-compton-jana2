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
#include <JANA/JFactoryGenerator.h>     // Factory generator template

// Experiment specific components
#include "JEventSource_EVIO.h"          // EVIO file event source
#include "JEventProcessor_Compton.h"     // Main event processor
#include "JFactory_FADC250.h"           // FADC250 data factory

/**
 * @brief Main function for experiment data processing application
 * 
 * This function:
 * 1. Parses command line arguments and parameters
 * 2. Creates and configures the JANA2 application
 * 3. Registers all necessary components (event source, processor, factory)
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
    app.Add(new JEventSourceGeneratorT<JEventSource_EVIO>());  // EVIO file reader
    app.Add(new JEventProcessor_Compton());                     // Data processor
    app.Add(new JFactoryGeneratorT<JFactory_FADC250>());       // FADC250 data factory

    // Initialize and run the application
    app.Initialize();
    app.Run();

    // Retrieve and return the exit code
    return app.GetExitCode();
}
