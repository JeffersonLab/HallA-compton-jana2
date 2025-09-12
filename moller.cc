#include <JANA/JApplication.h>           // For JApplication class 
#include <JANA/CLI/JMain.h>             // for command line options parsing 
#include <JANA/Services/JParameterManager.h>  // For JParameterManager  
#include <JANA/JEventSourceGeneratorT.h>
#include <JANA/JFactoryGenerator.h>     // For JFactoryGeneratorT template  

#include "JEventSource_EVIO.h"
#include "JEventProcessor_MOLLER.h"
#include "JFactory_FADC250.h"

int main(int argc, char* argv[]) {

    auto options = jana::ParseCommandLineOptions(argc, argv);
    auto params = new JParameterManager();

    for(auto pair: options.params) {
        params->SetParameter(pair.first, pair.second);
    }
    // Instantiate the JApplication with the parameter manager    
    JApplication app(params);

    // Add the event source filenames from command line  
    for(auto event_source : options.eventSources) {  
        app.Add(event_source);  
    } 

    // // Register components
    app.Add(new JEventSourceGeneratorT<JEventSource_EVIO>());
    app.Add(new JEventProcessor_MOLLER());
    app.Add(new JFactoryGeneratorT<JFactory_FADC250>());

    // Initialize and run the application
    app.Initialize();
    app.Run();

    // Retrieve and return the exit code
    return app.GetExitCode();
}