
#ifndef _JFactory_PhysicsEvent_h_
#define _JFactory_PhysicsEvent_h_

#include <JANA/JFactory.h>
#include "PhysicsEvent.h"
#include "EvioEventWrapper.h"

/**
 * @class JFactory_PhysicsEvent
 * @brief JANA2 factory for PhysicsEvent objects
 * 
 * This factory takes EVIO events as input and produces PhysicsEvent objects
 * as output. It uses the EvioEventParser to extract physics events from
 * the raw EVIO event structures.
 */
class JFactory_PhysicsEvent : public JFactory {

private:
    // Declare Inputs
    /// Input: EVIO event wrappers from the event source
    /// These wrappers contain shared pointers to the raw EVIO event structures
    Input<EvioEventWrapper> m_evio_events_in {this};

    // Declare Outputs
    /// Output: Physics events extracted from the EVIO block-level event
    /// Multiple physics events may be contained within a single EVIO block event
    Output<PhysicsEvent> m_physics_events_out{this};

public:
    JFactory_PhysicsEvent();
    
    void Init() override;
    void ChangeRun(const JEvent& event) override;
    void Process(const JEvent& event) override;
    void Finish() override;
};

#endif // _JFactory_PhysicsEvent_h_

