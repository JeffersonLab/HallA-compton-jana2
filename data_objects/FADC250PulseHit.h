#ifndef FADC250PULSEHIT_H
#define FADC250PULSEHIT_H

#include "FADC250Hit.h"

/**
 * Class representing a pulse hit with pulse analysis data
 */
class FADC250PulseHit : public FADC250Hit {
public:
    uint32_t pedestal_quality;
    uint32_t pedestal_sum;
    uint32_t integral_sum;
    uint32_t integral_quality;
    uint32_t nsamples_above_th;
    uint32_t coarse_time;
    uint32_t fine_time;
    uint32_t pulse_peak;
    uint32_t time_quality;

    // Constructor
    FADC250PulseHit() : FADC250Hit(), pedestal_quality(0), pedestal_sum(0), integral_sum(0),
                 integral_quality(0), nsamples_above_th(0), coarse_time(0),
                 fine_time(0), pulse_peak(0), time_quality(0) {}
    
    FADC250PulseHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
             uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan,
             uint32_t pedestal_quality, uint32_t pedestal_sum)
        : FADC250Hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan),
          pedestal_quality(pedestal_quality), pedestal_sum(pedestal_sum),
          integral_sum(0), integral_quality(0), nsamples_above_th(0),
          coarse_time(0), fine_time(0), pulse_peak(0), time_quality(0) {}
};

#endif // FADC250PULSEHIT_H
