// EventHits.h
#ifndef _EVENT_HITS_H_
#define _EVENT_HITS_H_

#include "FADC250WaveformHit.h"
#include "FADC250PulseHit.h"

struct EventHits {
    std::vector<FADC250WaveformHit*> waveforms;
    std::vector<FADC250PulseHit*> pulses;
};

#endif
