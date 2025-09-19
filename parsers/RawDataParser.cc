#include "RawDataParser.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw FADC250 data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting waveform and pulse hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param event_hits Reference to event hits container (will be updated)
 */
void RawDataParser::parseRawData(std::shared_ptr<evio::BaseStructure> data_block, 
                                uint32_t rocid, 
                                std::shared_ptr<EventHits> event_hits) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information
    uint32_t block_slot = 0;
    uint32_t module_id = 0;
    uint32_t trigger_num = 0;
    uint32_t timestamp1 = 0;
    uint32_t timestamp2 = 0;
    int block_nevents = -1;  // -1 indicates no block header processed yet
    
    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
        
        // Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
            
            if (data_type == 0) { // Block header
                block_slot = getBitsInRange(d, 26, 22);
                module_id = getBitsInRange(d, 21, 18);
                block_nevents = getBitsInRange(d, 7, 0);
            } else if (data_type == 1) { // Block trailer
                if (block_nevents != 0) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data format — block trailer word before reading in all events"
                    );
                }
                block_nevents = -1;
            } else if (data_type == 2) { // Event header
                if (block_nevents <= 0) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data format — event header before block header"
                    );
                }
                block_nevents--;
                auto eh_slot = getBitsInRange(d, 26, 22);
                if (eh_slot != block_slot) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data — event slot(%d) !=  block slot(%d)", 
                        eh_slot, block_slot
                    );
                }
                trigger_num = getBitsInRange(d, 21, 0);
            } else if (data_type == 3) { // Trigger time
                if (block_nevents < 0) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data format — trigger time word before block & event header"
                    );
                }
                timestamp1 = getBitsInRange(d, 23, 0);
                auto d2 = data_words[++j];
                timestamp2 = getBitsInRange(d2, 23, 0);
            } else if (data_type == 4) { // Waveform data
                if (block_nevents < 0) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data format — waveform data word before block & event header"
                    );
                }
                uint32_t chan = getBitsInRange(d, 26, 23);
                uint32_t waveform_len = getBitsInRange(d, 11, 0);
                
                // Parse waveform data and add to event hits
                FADC250WaveformHit hit = parseWaveformData(
                    data_words, j, trigger_num, timestamp1, timestamp2,
                    rocid, block_slot, module_id, chan, waveform_len
                );
                event_hits->waveforms.push_back(new FADC250WaveformHit(hit));
            } else if (data_type == 9) { // Pulse data
                if (block_nevents < 0) {
                    throw JException(
                        "RawDataParser::parseRawData: Invalid data format — pulse data word before block & event header"
                    );
                }
                uint32_t chan = getBitsInRange(d, 18, 15);
                uint32_t pedestal_quality = getBitsInRange(d, 14, 14);
                uint32_t pedestal_sum = getBitsInRange(d, 13, 0);
                
                // Parse pulse data and add to event hits
                auto hits = parsePulseData(
                    data_words, j, trigger_num, timestamp1, timestamp2,
                    rocid, block_slot, module_id, chan, pedestal_quality, pedestal_sum
                );
                for (auto& hit : hits) {
                    event_hits->pulses.push_back(new FADC250PulseHit(hit));
                }
            }
        }
    }
}

/**
 * @brief Extract bits from a 32-bit word
 * 
 * Utility function to extract a range of bits from a 32-bit word.
 * 
 * @param x The input word
 * @param high High bit position (inclusive)
 * @param low Low bit position (inclusive)
 * @return Extracted bits
 */
uint32_t RawDataParser::getBitsInRange(uint32_t x, int high, int low) {
    return (x >> low) & ((1u << (high - low + 1)) - 1);
}

/**
 * @brief Parse waveform data from data words
 * 
 * This method parses waveform data from the data words, extracting ADC samples
 * and creating a FADC250WaveformHit object.
 * 
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
FADC250WaveformHit RawDataParser::parseWaveformData(
    const std::vector<uint32_t>& data_words,
    size_t& index,
    uint32_t trigger_num,
    uint32_t timestamp1,
    uint32_t timestamp2,
    uint32_t rocid,
    uint32_t slot,
    uint32_t module_id,
    uint32_t chan,
    uint32_t waveform_len) {
    
    // Create waveform hit with basic information
    FADC250WaveformHit hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan);
    
    // Calculate number of waveform words (each word contains 2 samples)
    auto nwaveform_words = (waveform_len + 1) / 2;
    
    // Parse waveform samples from continuation words
    for (size_t k = index + 1; k < index + 1 + nwaveform_words; k++) {
        if (k >= data_words.size()) break;
        
        auto ww = data_words[k]; // waveform word
        auto ww_word_type = getBitsInRange(ww, 31, 31);
        if (ww_word_type != 0) {
            throw JException(
                "RawDataParser::parseWaveformData: Invalid data format — lesser words than required for getting all waveform samples"
            );
        }
        
        // Extract first sample (bits 28-16)
        auto x_notvalid = getBitsInRange(ww, 29, 29);
        if (!x_notvalid) {
            hit.addSample(getBitsInRange(ww, 28, 16));
        }
        
        // Extract second sample (bits 12-0)
        auto x_plus_notvalid = getBitsInRange(ww, 13, 13);
        if (!x_plus_notvalid) {
            hit.addSample(getBitsInRange(ww, 12, 0));
        }
    }
    
    // Validate waveform length
    if (waveform_len != hit.getWaveformSize()) {
        throw JException(
            "RawDataParser::parseWaveformData: Invalid data — Header given waveform size (%d) != Obtained waveform size (%d)",
            waveform_len, hit.getWaveformSize()
        );
    }
    
    // Update index to skip over waveform continuation words
    index += nwaveform_words;
    
    return hit;
}

/**
 * @brief Parse pulse data from data words
 * 
 * This method parses pulse data from the data words, extracting pulse analysis
 * information and creating FADC250PulseHit objects.
 * 
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
std::vector<FADC250PulseHit> RawDataParser::parsePulseData(
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
    uint32_t pedestal_sum) {
    
    std::vector<FADC250PulseHit> pulse_hits;
    
    // Parse continuation word pairs (Words 2+3, repeated per pulse)
    while (true) {
        if (index + 2 >= data_words.size()) break; // no more words, avoid overrun
        
        uint32_t w2 = data_words[++index];
        uint32_t w3 = data_words[++index];
        
        // Check word type = 0 (continuation words)
        if (getBitsInRange(w2, 31, 31) != 0 || getBitsInRange(w3, 31, 31) != 0) {
            // If we encounter non-continuation word, backtrack and break
            index -= 2;
            break;
        }
        
        // Create pulse hit with basic information
        FADC250PulseHit pulse_hit(trigger_num, timestamp1, timestamp2, rocid, slot, module_id, chan,
                               pedestal_quality, pedestal_sum);
        
        // Word 2: pulse integral information
        pulse_hit.integral_sum = getBitsInRange(w2, 29, 12);
        pulse_hit.integral_quality = getBitsInRange(w2, 11, 9);
        pulse_hit.nsamples_above_th = getBitsInRange(w2, 8, 0);
        
        // Word 3: pulse timing and peak information
        pulse_hit.coarse_time = getBitsInRange(w3, 29, 21);
        pulse_hit.fine_time = getBitsInRange(w3, 20, 15);
        pulse_hit.pulse_peak = getBitsInRange(w3, 14, 3);
        pulse_hit.time_quality = getBitsInRange(w3, 2, 0);
        
        pulse_hits.push_back(pulse_hit);
    }
    
    return pulse_hits;
}
