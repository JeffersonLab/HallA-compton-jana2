#ifndef FADC250WAVEFORM_H
#define FADC250WAVEFORM_H

#include "FADC250Hit.h"
#include <vector>

/**
 * Class representing a waveform hit with sample data
 */
class FADC250WaveformHit : public FADC250Hit {
public:
    std::vector<uint32_t> waveform;

    // Constructor
    FADC250WaveformHit() : FADC250Hit() {}
    
    FADC250WaveformHit(uint64_t trigger_num, uint64_t timestamp1, uint64_t timestamp2,
                uint32_t rocid, uint32_t slot, uint32_t module_id, uint32_t chan)
        : FADC250Hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan) {}
    
    // Add waveform sample
    void addSample(uint32_t sample) {
        waveform.push_back(sample);
    }
    
    // Get waveform size
    size_t getWaveformSize() const {
        return waveform.size();
    }
    
    // Clear waveform data
    void clearWaveform() {
        waveform.clear();
    }
};

#endif // FADC250WAVEFORM_H
