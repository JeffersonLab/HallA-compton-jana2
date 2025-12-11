#ifndef _EVENT_HITS_FADC_H_
#define _EVENT_HITS_FADC_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "FADC250WaveformHit.h"
#include "FADC250PulseHit.h"

/**
 * @class EventHits_FADC
 * @brief Container for all FADC detector hits in a single event
 *
 * Owns waveform and pulse hit pointers and knows how to insert
 * them into a JEvent.
 */
class EventHits_FADC : public EventHits {
public:
    std::vector<FADC250WaveformHit*> waveforms;
    std::vector<FADC250PulseHit*> pulses;

    void insertIntoEvent(JEvent& event) override {
        event.Insert(waveforms);
        event.Insert(pulses);
    }
};

#endif // _EVENT_HITS_FADC_H_
