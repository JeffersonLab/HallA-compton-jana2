#ifndef _EVENT_HITS_ITSCALER_H_
#define _EVENT_HITS_ITSCALER_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "ITScalerHit.h"

/**
 * @class EventHits_ITScaler
 * @brief Container for all ITScaler detector hits in a single event
 *
 * Owns scaler hit pointers and knows how to insert them into a JEvent.
 */
class EventHits_ITScaler : public EventHits {
public:
    std::vector<ITScalerHit*> scalers;

    void insertIntoEvent(JEvent& event) override {
        event.Insert(scalers);
    }
};

#endif // _EVENT_HITS_ITSCALER_H_
