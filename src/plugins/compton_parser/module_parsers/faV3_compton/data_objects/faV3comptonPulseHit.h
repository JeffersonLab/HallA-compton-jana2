#ifndef FAV3COMPTONPULSEHIT_H
#define FAV3COMPTONPULSEHIT_H

/**
 * @class faV3comptonPulseHit
 * @brief faV3compton pulse data
 */
class faV3comptonPulseHit {
public:
    uint32_t chan;
    uint32_t prescale_factor;
    uint32_t nsamples;
    uint32_t overflow;
    uint32_t underflow;
    uint32_t vpeak;
    uint32_t pulse_missed;
    uint32_t nsb_nsa_overlap;
    uint32_t sum;

    /**
     * @brief Default constructor
     *
     * Initializes all members to zero.
     */
    faV3comptonPulseHit() : chan(0), prescale_factor(0), nsamples(0), overflow(0), underflow(0),
                            vpeak(0), pulse_missed(0), nsb_nsa_overlap(0), sum(0) {}

};

#endif // FAV3COMPTONPULSEHIT_H
