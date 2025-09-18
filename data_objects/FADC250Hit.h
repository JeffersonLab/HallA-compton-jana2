#ifndef FADC250HIT_H
#define FADC250HIT_H

#include <cstdint>

/**
 * Base class for all FADC250 hits
 */
class FADC250Hit {
public:
    uint64_t trigger_num;
    uint32_t timestamp1;
    uint32_t timestamp2;
    uint32_t rocid;
    uint32_t slot;
    uint32_t module_id;
    uint32_t chan;

    // Constructor
    FADC250Hit() : trigger_num(0), timestamp1(0), timestamp2(0), 
            rocid(0), slot(0), module_id(0), chan(0) {}
    
    FADC250Hit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
        uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan)
        : trigger_num(trigger_num), timestamp1(timestamp1), timestamp2(timestamp2),
          rocid(rocid), slot(slot), module_id(module_id), chan(chan) {}
    
    virtual ~FADC250Hit() = default;
};

#endif // FADC250HIT_H
