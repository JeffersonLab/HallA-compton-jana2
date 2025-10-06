
#ifndef _JFactory_FADC250_h_
#define _JFactory_FADC250_h_

#include <JANA/JFactory.h>
#include "EventHits.h"
#include "EvioEventWrapper.h"

/**
 * @class JFactory_FADC250
 * @brief JANA2 factory for FADC250 detector hits
 * 
 * This factory takes EVIO events as input and produces FADC250 detector hits
 * (both waveform and pulse hits) as output. It uses the EvioEventParser to
 * extract detector data from the raw EVIO event structures.
 */
class JFactory_FADC250 : public JFactory {

private:
    // Declare Inputs
    Input<EvioEventWrapper> m_hits_in {this};

    // Declare Outputs
    Output<FADC250WaveformHit> m_waveform_hits_out{this};
    Output<FADC250PulseHit> m_pulse_hits_out{this};

public:
    JFactory_FADC250();
    
    void Init() override;
    void ChangeRun(const JEvent& event) override;
    void Process(const JEvent& event) override;
    void Finish() override;
};

#endif // _JFactory_FADC250_h_
