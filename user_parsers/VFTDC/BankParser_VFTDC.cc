#include "BankParser_VFTDC.h"
#include "EventHits_VFTDC.h"
#include <JANA/JException.h>

/**
 * @brief Parse a raw data block and extract hits
 * 
 * This method parses the raw VFTDC data block by:
 * 1. Processing data words sequentially
 * 2. Identifying different word types (headers, trailers, data)
 * 3. Extracting VFTDC hits
 * 4. Adding hits to the event hits container
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger metadata for this EVIO block
 */
void BankParser_VFTDC::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
    // Get all data words from the block
    std::vector<uint32_t> data_words = data_block->getUIntData();
    
    // Variables to track block and event information
    uint32_t block_slot = 0;
    uint32_t block_board_id = 0;
    uint64_t event_timestamp = 0;
    uint32_t event_number = 0;
    int block_nevents = -1;  // -1 indicates no block header processed yet
    
    // Map from event_number to EventHits_VFTDC, used to merge hits from
    // multiple blocks within the same data bank into a single PhysicsEvent.
    std::map<uint32_t, std::shared_ptr<EventHits_VFTDC>> event_hits_map;

    // Process each data word sequentially
    for (size_t j = 0; j < data_words.size(); j++) {
        auto d = data_words[j];
        uint32_t word_type = getBitsInRange(d, 31, 31);
        
        // Process data type defining words (word_type == 1)
        if (word_type == 1) {
            uint32_t data_type = getBitsInRange(d, 30, 27);
            
            if (data_type == 0) { // Block header
                block_slot = getBitsInRange(d, 26, 22);
                block_board_id = getBitsInRange(d, 21, 18);
                block_nevents = getBitsInRange(d, 7, 0);
            } else if (data_type == 1) { // Block trailer              
                if (block_nevents != 0) {
                    throw JException(
                        "BankParser_VFTDC::parse: Invalid data format — block trailer word before reading in all events"
                    );
                }
                block_nevents = -1;
            } else if (data_type == 2) { // Event header
                if (block_nevents <= 0) {
                    throw JException(
                        "BankParser_VFTDC::parse: Invalid data format — event header before block header"
                    );
                }
                block_nevents--;

                auto event_slot = getBitsInRange(d, 26, 22);
                if (event_slot != block_slot) {
                    throw JException(
                        "BankParser_VFTDC::parse: Invalid data format — event slot mismatch with block slot"
                    );
                }
                event_number = getBitsInRange(d, 21, 0);
                if (event_hits_map.find(event_number) == event_hits_map.end()) {
                    event_hits_map[event_number] = std::make_shared<EventHits_VFTDC>();
                }
            } else if (data_type == 3) { // Trigger time
                if (block_nevents < 0) {
                    throw JException(
                        "BankParser_VFTDC::parse: Invalid data format — trigger time word before block & event header"
                    );
                }
                auto timestamp_low = getBitsInRange(d, 23, 0);
                auto d2 = data_words[++j];
                auto timestamp_high = getBitsInRange(d2, 23, 0);
                event_timestamp = (timestamp_high << 24) | timestamp_low;
            } else if (data_type == 7) { // Data word
                if (block_nevents < 0) {
                    throw JException(
                        "BankParser_VFTDC::parse: Invalid data format — data word before block & event header"
                    );
                }

                auto vftdc_hit = new VFTDCHit();
                vftdc_hit->rocid = rocid;
                vftdc_hit->slot = block_slot;
                vftdc_hit->board_id = block_board_id;
                vftdc_hit->timestamp = event_timestamp;
                vftdc_hit->group_num = getBitsInRange(d, 26, 24);
                vftdc_hit->channel_num = getBitsInRange(d, 23, 19);
                vftdc_hit->edge_type = getBitsInRange(d, 18, 18);
                vftdc_hit->coarse_time = getBitsInRange(d, 17, 8);
                vftdc_hit->two_ns = getBitsInRange(d, 7, 7);
                vftdc_hit->fine_time = getBitsInRange(d, 6, 0);
                event_hits_map[event_number]->vftdc_hits.push_back(vftdc_hit);
            } 
        }
    }

    for (auto& event_hit : event_hits_map) {
        PhysicsEvent* physics_event = new PhysicsEvent(event_hit.first, event_hit.second);
        physics_events.push_back(physics_event);
    }
}
