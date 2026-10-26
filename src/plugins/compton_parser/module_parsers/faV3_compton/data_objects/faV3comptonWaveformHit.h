#ifndef FAV3COMPTONWAVEFORMHIT_H
#define FAV3COMPTONWAVEFORMHIT_H

/**
 * @class faV3comptonWaveformHit
 * @brief faV3compton waveform data
 */
class faV3comptonWaveformHit {
public:
    uint32_t chan;
    uint32_t prescale;
    std::vector<uint32_t> waveform;
    uint32_t pulse_number;

    /**
     * @brief Default constructor
     *
     * Initializes all members to zero.
     */
    faV3comptonWaveformHit() : chan(0), prescale(0), pulse_number(0), waveform{} {};

    void addSample(uint32_t sample) {
        waveform.push_back(sample);
    }

    size_t getWaveformSize() const {
        return waveform.size();
    }

    void clearWaveform() {
        waveform.clear();
    }

};

#endif // FAV3COMPTONWAVEFORMHIT_H
