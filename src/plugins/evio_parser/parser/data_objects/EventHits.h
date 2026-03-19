#ifndef _EVENT_HITS_H_
#define _EVENT_HITS_H_

#include <JANA/JEvent.h>
/**
 * @class EventHits
 * @brief Abstract base for inserting detector hits into a JEvent
 *
 * Derived classes own concrete hit containers and implement how
 * those hits are inserted into a JEvent.
 */
class EventHits {
public:
    virtual ~EventHits() = default;
    virtual void insertIntoEvent(JEvent& event) = 0;
};

#endif // _EVENT_HITS_H_
