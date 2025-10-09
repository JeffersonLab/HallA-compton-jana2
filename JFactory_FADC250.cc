
#include "JFactory_FADC250.h"
#include "EvioEventParser.h"

#include <JANA/JEvent.h>

/**
 * @brief Constructor for JFactory_FADC250
 * 
 * Initializes the factory with the appropriate type name and prefix.
 * Sets up the factory for processing EVIO events and creating FADC250 hits.
 */
JFactory_FADC250::JFactory_FADC250() {

    // Set the type name of this class for more informative error messages
    SetTypeName(NAME_OF_THIS);

    // Set the default prefix for this factory (used as prefix for parameters)
    // In ePIC, this is usually overridden by the JOmniFactoryGenerator
    SetPrefix("fadc250_factory");

    // Short name distinguishes this output when multiple outputs have the same type (e.g. int).
    // Usage: event.Get<int>("event_number") retrieves this specific output.
    m_event_number_out.SetShortName("event_number");

    // Optional: Set inputs as optional if they might not always be present
    // m_hits_in.SetOptional(true);

    // Optional: Set factory flags as needed
    // m_hits_out.SetNotOwnerFlag(true);
    // Note: When there are multiple outputs, the NOT_OBJECT_OWNER flag should be set
    // on the Output, not on the factory itself.
}

/**
 * @brief Initialize the factory
 * 
 * Called once at the start of processing to set up the factory.
 * By the time Init() is called:
 * - Logger has been configured
 * - Declared Parameter values have been fetched
 * - Declared Services have been fetched
 */
void JFactory_FADC250::Init() {
    LOG_DEBUG(GetLogger()) << "Inside Init()";

    // Unlike v1, you don't have to fetch any parameters or services in here, as this is done automatically now.
    // However, you may wish to fetch data from your JServices, if it isn't keyed off of the run number.
    // This is also where you should initialize your algorithm, if necessary.
}

/**
 * @brief Handle run changes
 * 
 * Called when the run number changes during processing.
 * This is where you should fetch any data from your JServices that IS keyed off of the run number.
 * 
 * @param event Reference to the current JANA2 event
 */
void JFactory_FADC250::ChangeRun(const JEvent& event) {
    LOG_DEBUG(GetLogger()) << "Inside ChangeRun() with run_number=" << event.GetRunNumber();
}

/**
 * @brief Process an event
 * 
 * This method processes the event. The input helpers are already filled by the time
 * this method is called. You can access the data using the () operator. 
 * Parameter values may also be accessed using the () operator.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JFactory_FADC250::Process(const JEvent& event) {
    LOG_DEBUG(GetLogger()) << "Inside Process() with run_number=" << event.GetRunNumber()
                          << ", event_number=" << event.GetEventNumber();

    // Get the EVIO event
    std::shared_ptr<evio::EvioEvent> evio_event = m_hits_in().at(0)->evio_event;
    
    // Create and use the EVIO event parser to extract hits
    auto evio_event_parser = std::make_unique<EvioEventParser>(evio_event);
    evio_event_parser->parse();
    
    // Get the parsed hits from the parser
    std::shared_ptr<EventHits> hits = evio_event_parser->getHits();

    // Populate the output data bundles
    // JANA2 will store and retrieve them automatically once this method returns
    m_waveform_hits_out() = hits->waveforms;
    m_pulse_hits_out() = hits->pulses;

    // Lets us pass the event number back to the event source so that it can be set
    m_event_number_out().push_back(new int(evio_event_parser->getEventNumber()));
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing to perform cleanup.
 */
void JFactory_FADC250::Finish() {
    LOG_DEBUG(GetLogger()) << "Inside Finish()";
}
