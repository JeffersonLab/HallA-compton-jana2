

#ifndef _FADC250_h_
#define _FADC250_h_

#include <JANA/JObject.h>

/// JObjects are plain-old data containers for inputs, intermediate results, and outputs.
/// They have member functions for introspection and maintaining associations with other JObjects, but
/// all of the numerical code which goes into their creation should live in a JFactory instead.
/// You are allowed to include STL containers and pointers to non-POD datatypes inside your JObjects,
/// however, it is highly encouraged to keep them flat and include only primitive datatypes if possible.
/// Think of a JObject as being a row in a database table, with event number as an implicit foreign key.

struct FADC250 : public JObject {
    JOBJECT_PUBLIC(FADC250)

    uint32_t chan;
    uint32_t q;
    uint32_t t;

    /// Make it convenient to construct one of these things
    // FADC250(int x, int y, double E, double t) : x(x), y(y), E(E), t(t) {};
    FADC250(uint32_t chan, uint32_t q, uint32_t t) : chan(chan), q(q), t(t) {};


    /// Override Summarize to tell JANA how to produce a convenient string representation for our JObject.
    /// This can be called from user code, but also lets JANA automatically inspect its own data. See the
    /// CsvWriter example. Warning: Because this is slow, it should be used for debugging and monitoring 
    /// but not inside the performance critical code paths.
    void Summarize(JObjectSummary& summary) const override {
        summary.add(chan, NAME_OF(chan), "%u", "Channel number");
        summary.add(q,    NAME_OF(q),    "%u", "Charge");
        summary.add(t,    NAME_OF(t),    "%u", "Time");
    }

};


#endif // _FADC250_h_

