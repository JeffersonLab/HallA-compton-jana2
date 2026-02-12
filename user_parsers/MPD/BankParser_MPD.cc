#include "BankParser_MPD.h"
#include "EventHits_MPD.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw MPD data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting MPD hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 */
void BankParser_MPD::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information
    uint32_t block_slot = 0;
    int block_nevents = -1;  // -1 indicates no block header processed yet
    int event_index = 0;
    

    MPDHit* current_mpd_hit = nullptr; // one event has one mpd hit only

    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
        
        // Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
            
            if (data_type == 0) { // Block header
                block_slot = getBitsInRange(d, 26, 22);
                block_nevents = getBitsInRange(d, 7, 0);
            } else if (data_type == 1) { // Block trailer              
                if (block_nevents != 0) {
                    throw JException(
                        "BankParser_MPD::parse: Invalid data format — block trailer word before reading in all events"
                    );
                }
                block_nevents = -1;
                if (current_mpd_hit != nullptr) { // insert last event into physics_events
                    auto event_hits = std::make_shared<EventHits_MPD>();
                    event_hits->mpd.push_back(current_mpd_hit);
                    PhysicsEvent* physics_event = new PhysicsEvent(trigger_data.first_event_number + event_index, event_hits);
                    physics_events.push_back(physics_event);
                }
                event_index = 0;
                current_mpd_hit = nullptr;
            } else if (data_type == 2) { // Event header
                if (current_mpd_hit != nullptr) { // insert previous event into physics_events first
                    auto event_hits = std::make_shared<EventHits_MPD>();
                    event_hits->mpd.push_back(current_mpd_hit);
                    PhysicsEvent* physics_event = new PhysicsEvent(trigger_data.first_event_number + event_index, event_hits);
                    physics_events.push_back(physics_event);
                    event_index++;
                }
                if (block_nevents <= 0) {
                    throw JException(
                        "BankParser_MPD::parse: Invalid data format — event header before block header"
                    );
                }
                block_nevents--;
                current_mpd_hit = new MPDHit();
                current_mpd_hit->rocid = rocid;
                current_mpd_hit->slot = block_slot;
                current_mpd_hit->trigger_num = getBitsInRange(d, 15, 0);
            } else if (data_type == 3) { // Trigger time
                if (block_nevents < 0) {
                    throw JException(
                        "BankParser_MPD::parse: Invalid data format — trigger time word before block & event header"
                    );
                }
                auto trigger_time_low = getBitsInRange(d, 23, 0);
                auto d2 = data_words[++j];
                auto trigger_time_high = getBitsInRange(d2, 23, 0);
                current_mpd_hit->trigger_time = (trigger_time_high << 24) | trigger_time_low;
            } else if (data_type == 5) { // MPD Frame
                if (block_nevents < 0) {
                    throw JException(
                        "BankParser_MPD::parse: Invalid data format — waveform data word before block & event header"
                    );
                }
                current_mpd_hit->fiber_id = getBitsInRange(d, 20, 16);
                current_mpd_hit->mpd_id = getBitsInRange(d, 4, 0);

                auto d2 = data_words[++j];
                auto apv_channel_low = getBitsInRange(d2, 30, 26);
                current_mpd_hit->apv_samples[0] = getBitsInRange(d2, 12, 0);
                current_mpd_hit->apv_samples[1] = getBitsInRange(d2, 23, 13);

                auto d3 = data_words[++j];
                auto apv_channel_high = getBitsInRange(d3, 30, 26);
                current_mpd_hit->apv_samples[2] = getBitsInRange(d3, 12, 0);
                current_mpd_hit->apv_samples[3] = getBitsInRange(d3, 23, 13);

                current_mpd_hit->apv_channel = (apv_channel_high << 4) | apv_channel_low;

                auto d4 = data_words[++j];
                current_mpd_hit->apv_id = getBitsInRange(d4, 30, 26);
                current_mpd_hit->apv_samples[4] = getBitsInRange(d4, 12, 0);
                current_mpd_hit->apv_samples[5] = getBitsInRange(d4, 23, 13);
            } 
        }
    }
}
