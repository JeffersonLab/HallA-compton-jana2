

#ifndef _EvioEventWrapper_h_
#define _EvioEventWrapper_h_

#include <JANA/JObject.h>

#include "eviocc.h"

/// JObjects are plain-old data containers for inputs, intermediate results, and outputs.
/// They have member functions for introspection and maintaining associations with other JObjects, but
/// all of the numerical code which goes into their creation should live in a JFactory instead.
/// You are allowed to include STL containers and pointers to non-POD datatypes inside your JObjects,
/// however, it is highly encouraged to keep them flat and include only primitive datatypes if possible.
/// Think of a JObject as being a row in a database table, with event number as an implicit foreign key.


struct EvioEventWrapper : public JObject {
    JOBJECT_PUBLIC(EvioEventWrapper)

    std::shared_ptr<evio::EvioEvent> evio_event;

    EvioEventWrapper(std::shared_ptr<evio::EvioEvent> evio_event) : evio_event(evio_event) {};

    /// Override Summarize to tell JANA how to produce a convenient string representation for our JObject.
    /// This can be called from user code, but also lets JANA automatically inspect its own data. See the
    /// CsvWriter example. Warning: Because this is slow, it should be used for debugging and monitoring 
    /// but not inside the performance critical code paths.

    void Summarize(JObjectSummary& summary) const override {
        std::string eventStr = evio_event->toString();
        summary.add(eventStr, "evio_event", "%s", "EVIO event summary");
    }

};


#endif // _EvioEventWrapper_h_

