
#ifndef _JEventProcessor_MOLLER_h_
#define _JEventProcessor_MOLLER_h_

#include <fstream>

#include <JANA/JEventProcessor.h>
#include "EventHits.h"

/**
 * @class JEventProcessor_MOLLER
 * @brief Main event processor for MOLLER experiment data analysis
 * 
 * This processor receives FADC250 detector hits (both waveform and pulse hits)
 * and outputs detailed information about each hit to a text file.
 */
class JEventProcessor_MOLLER : public JEventProcessor {

private:
    Input<FADC250WaveformHit> m_waveform_hits_in {this};  ///< Input: FADC250 waveform hits
    Input<FADC250PulseHit> m_pulse_hits_in {this};        ///< Input: FADC250 pulse hits
    std::ofstream m_outfile;                              ///< Output file stream

public:

    JEventProcessor_MOLLER();
    virtual ~JEventProcessor_MOLLER() = default;

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};

#endif // _JEventProcessor_MOLLER_h_

