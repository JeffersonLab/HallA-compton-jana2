
#include "JFactory_PhysicsEvent.h"
#include "EvioEventParser.h"

#include <JANA/JEvent.h>

/**
 * @brief Constructor for JFactory_PhysicsEvent
 * 
 * Initializes the factory with the appropriate type name and prefix.
 * Sets up the factory for processing EVIO events and creating PhysicsEvent objects.
 */
JFactory_PhysicsEvent::JFactory_PhysicsEvent() {

    // Set the type name of this class for more informative error messages
    SetTypeName(NAME_OF_THIS);

    // Set the default prefix for this factory (used as prefix for parameters)
    // In ePIC, this is usually overridden by the JOmniFactoryGenerator
    SetPrefix("physics_event_factory");

    SetLevel(JEventLevel::Block);

    // Optional: Set inputs as optional if they might not always be present
    // m_evio_events_in.SetOptional(true);

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
void JFactory_PhysicsEvent::Init() {
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
void JFactory_PhysicsEvent::ChangeRun(const JEvent& event) {
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
void JFactory_PhysicsEvent::Process(const JEvent& event) {
    LOG_DEBUG(GetLogger()) << "Inside Process() with run_number=" << event.GetRunNumber()
                          << ", event_number=" << event.GetEventNumber();

    // Get the EVIO event
    std::shared_ptr<evio::EvioEvent> evio_event = m_evio_events_in().at(0)->evio_event;
    
    // Create and use the EVIO event parser to extract physics events from the raw EVIO structure
    // The parser processes the EVIO event and extracts all physics events contained within it
    auto evio_event_parser = std::make_unique<EvioEventParser>(evio_event);
    evio_event_parser->parse();
    
    // Get the parsed physics events from the parser and set them as factory output
    // These PhysicsEvent objects will be used by the unfolder to create child events
    m_physics_events_out() = evio_event_parser->getPhysicsEvents();
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing to perform cleanup.
 */
void JFactory_PhysicsEvent::Finish() {
    LOG_DEBUG(GetLogger()) << "Inside Finish()";
}

