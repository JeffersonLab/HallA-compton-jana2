
#include "JEventSource_EVIO.h"
#include "EvioEventWrapper.h"

#include <JANA/JApplication.h>
#include <JANA/JEvent.h>


/// Include headers to any JObjects you wish to associate with each event
// #include "Hit.h"

/// There are two different ways of instantiating JEventSources
/// 1. Creating them manually and registering them with the JApplication
/// 2. Creating a corresponding JEventSourceGenerator and registering that instead
///    If you have a list of files as command line args, JANA will use the JEventSourceGenerator
///    to find the most appropriate JEventSource corresponding to that filename, instantiate and register it.
///    For this to work, the JEventSource constructor has to have the following constructor arguments:


JEventSource_EVIO::JEventSource_EVIO() : JEventSource() {
    SetTypeName(NAME_OF_THIS); // Provide JANA with class name
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventSource_EVIO::Open() {

    /// Open is called exactly once when processing begins.
    
    /// Get any configuration parameters from the JApplication
    // GetApplication()->SetDefaultParameter("JEventSource_EVIO:random_seed", m_seed, "Random seed");

    /// For opening a file, get the filename via:
    const std::string resource_name = GetResourceName();
    /// Open the file here!
    m_evio_reader = std::make_unique<evio::EvioReader>(resource_name);
}

void JEventSource_EVIO::Close() {

    /// Close is called exactly once when processing ends. This is where you should close your files or sockets.
    /// It is important to do that here instead of in Emit() because we want everything to be cleanly closed 
    /// even when JANA is terminated via Ctrl-C or via a timeout.
    m_evio_reader->close();
}

JEventSource::Result JEventSource_EVIO::Emit(JEvent& event) {

    std::shared_ptr<evio::EvioEvent> evio_event = m_evio_reader->parseNextEvent();

    if (evio_event == nullptr) {
        return Result::FailureFinished;
    }

    if (evio_event->getChildren().empty()) { // no data
        return Result::FailureTryAgain;
    }

    EvioEventWrapper* wrapper = new EvioEventWrapper(evio_event); 
    event.SetEventNumber(evio_event->getEventNumber());
    event.Insert(wrapper);  // can't pass in shared ptr directly so to avoid deletion of evio_event passing it inside wrapper
    return Result::Success;

}

std::string JEventSource_EVIO::GetDescription() {

    /// GetDescription() helps JANA explain to the user what is going on
    return "";
}


template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string resource_name) {

    /// CheckOpenable() decides how confident we are that this EventSource can handle this resource.
    ///    0.0        -> 'Cannot handle'
    ///    (0.0, 1.0] -> 'Can handle, with this confidence level'
    
    /// To determine confidence level, feel free to open up the file and check for magic bytes or metadata.
    /// Returning a confidence <- {0.0, 1.0} is perfectly OK!
    
    // Check if the file has .evio extension  
    if (resource_name.size() >= 5 &&   
        resource_name.substr(resource_name.size() - 5) == ".evio") {  
        return 1.0;  // High confidence for .evio files  
    }  
    return 0.0;  // Cannot handle other file types  

}