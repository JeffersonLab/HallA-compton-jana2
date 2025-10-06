#ifndef RAWDATAPARSER_H
#define RAWDATAPARSER_H

#include <vector>
#include <memory>
#include "eviocc.h"
#include "EventHits.h"

/**
 * Class for parsing raw data blocks and extracting hits
 */
class RawDataParser {
public:
    /**
     * Parse a raw data block and extract hits
     * @param data_block The data block to parse
     * @param rocid ROC ID
     * @param event_hits Reference to event hits container (will be updated)
     */
    static void parseRawData(std::shared_ptr<evio::BaseStructure> data_block, 
                            uint32_t rocid, 
                            std::shared_ptr<EventHits> event_hits);

private:
    /**
     * Extract bits from a 32-bit word
     * @param x The input word
     * @param high High bit position (inclusive)
     * @param low Low bit position (inclusive)
     * @return Extracted bits
     */
    static uint32_t getBitsInRange(uint32_t x, int high, int low);
    
    /**
     * Parse waveform data from data words
     * @param data_words Vector of data words
     * @param index Current index in data_words (will be updated)
     * @param trigger_num Trigger number
     * @param timestamp1 First timestamp
     * @param timestamp2 Second timestamp
     * @param rocid ROC ID
     * @param slot Slot number
     * @param module_id Module ID
     * @param chan Channel number
     * @param waveform_len Expected waveform length
     * @return Parsed waveform hit
     */
    static FADC250WaveformHit parseWaveformData(
        const std::vector<uint32_t>& data_words,
        size_t& index,
        uint32_t trigger_num,
        uint32_t timestamp1,
        uint32_t timestamp2,
        uint32_t rocid,
        uint32_t slot,
        uint32_t module_id,
        uint32_t chan,
        uint32_t waveform_len
    );
    
    /**
     * Parse pulse data from data words
     * @param data_words Vector of data words
     * @param index Current index in data_words (will be updated)
     * @param trigger_num Trigger number
     * @param timestamp1 First timestamp
     * @param timestamp2 Second timestamp
     * @param rocid ROC ID
     * @param slot Slot number
     * @param module_id Module ID
     * @param chan Channel number
     * @param pedestal_quality Pedestal quality
     * @param pedestal_sum Pedestal sum
     * @return Vector of parsed pulse hits
     */
    static std::vector<FADC250PulseHit> parsePulseData(
        const std::vector<uint32_t>& data_words,
        size_t& index,
        uint32_t trigger_num,
        uint32_t timestamp1,
        uint32_t timestamp2,
        uint32_t rocid,
        uint32_t slot,
        uint32_t module_id,
        uint32_t chan,
        uint32_t pedestal_quality,
        uint32_t pedestal_sum
    );
};

#endif // RAWDATAPARSER_H