#ifndef _EVENT_HITS_H_
#define _EVENT_HITS_H_

#include "FADC250WaveformHit.h"
#include "FADC250PulseHit.h"

/**
 * @struct EventHits
 * @brief Container for all detector hits in a single event
 * 
 * This structure holds all the detector hits extracted from a single EVIO event.
 * It contains separate vectors for waveform hits and pulse hits.
 */
struct EventHits {
    std::vector<FADC250WaveformHit*> waveforms;
    std::vector<FADC250PulseHit*> pulses;
};

#endif // _EVENT_HITS_H_
