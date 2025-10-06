
#include "JEventSource_EVIO.h"
#include "EvioEventWrapper.h"
#include "EvioEventParser.h"

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


/**
 * @brief Constructor for JEventSource_EVIO
 * 
 * Initializes the event source with the appropriate type name and callback style.
 */
JEventSource_EVIO::JEventSource_EVIO() : JEventSource() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with class name
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

/**
 * @brief Open the input source
 * Called once at the beginning of processing
 */
void JEventSource_EVIO::Open() {

    /// Get any configuration parameters from the JApplication
    // GetApplication()->SetDefaultParameter("JEventSource_EVIO:random_seed", m_seed, "Random seed");
    
    // For opening a file, get the filename via:
    const std::string resource_name = GetResourceName();
    /// Open the file here!
    m_evio_reader = std::make_unique<evio::EvioReader>(resource_name);
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing to perform cleanup. This is where you should close your
  files or sockets. It is important to do that here instead of in Emit() because we want
 * everything to be cleanly closed even when JANA is terminated via Ctrl-C or via a timeout.
 */
void JEventSource_EVIO::Close() {
    // Close the EVIO file reader
    m_evio_reader->close();
}

/**
 * @brief Emit the next event
 * 
 * 
 * @param event Reference to the JANA2 event to populate
 * @return Result indicating success, failure, or end of file
 */
JEventSource::Result JEventSource_EVIO::Emit(JEvent& event) {

    // Read the next event from the EVIO file
    std::shared_ptr<evio::EvioEvent> evio_event = m_evio_reader->parseNextEvent();

    // Check for end of file
    if (evio_event == nullptr) {
        return Result::FailureFinished;
    }

    // Skip events with no data
    if (evio_event->getChildren().empty()) {
        return Result::FailureTryAgain;
    }

    // Check if this is a run control event. Apart from extracting the run number,
    // these events are not useful for further processing, so get run number and skip.
    if (EvioEventParser::isRunControlEvent(evio_event, m_run_number)) {
        return Result::FailureTryAgain;
    }

    // Create a wrapper for the EVIO event and add it to the JANA2 event
    EvioEventWrapper* wrapper = new EvioEventWrapper(evio_event); 
    event.SetEventNumber(evio_event->getEventNumber());
    event.SetRunNumber(m_run_number);
    event.Insert(wrapper);  // can't pass in shared ptr directly so to avoid deletion of evio_event passing it inside wrapper
    return Result::Success;
}

/**
 * @brief Get description of this event source
 * @return Description string
 */
std::string JEventSource_EVIO::GetDescription() {
    return "EVIO event source for MOLLER experiment data";
}

/**
 * @brief Check if this event source can handle a given file
 * 
 * Determines the confidence level that this event source can handle a given
 * file.
 * 
 * @param resource_name Name/path of the resource to check
 * @return Confidence level (0.0 = cannot handle, {0.0, 1.0}= Can handle, with this confidence level)
 */
template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string resource_name) {
    
    /// To determine confidence level, feel free to open up the file and check for magic bytes or metadata.
    
    // Check if the file has .evio extension  
    if (resource_name.size() >= 5 &&   
        resource_name.substr(resource_name.size() - 5) == ".evio") {  
        return 1.0;  // High confidence for .evio files  
    }  
    
    return 0.0;  // Cannot handle other file types  
}