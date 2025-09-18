
#ifndef _JFactory_FADC250_h_
#define _JFactory_FADC250_h_

#include <JANA/Components/JOmniFactory.h>
#include "EventHits.h"
#include "EvioEventWrapper.h"

class JFactory_FADC250 : public JFactory {

    Input<EvioEventWrapper> m_hits_in {this};
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
