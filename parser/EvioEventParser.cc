#include "EvioEventParser.h"
#include <JANA/JException.h>
#include "JEventService_BankParsersMap.h"
#include "EvioEventWrapper.h"

/**
 * @brief Parse the EVIO event and extract all detector hits
 * 
 * This method orchestrates the parsing of an EVIO event by:
 * 1. Validating the event structure
 * 2. Parsing the trigger bank to extract ROC segments and event number
 * 3. Parsing data banks to extract detector hits using BankParser
 */
void EvioEventParser::parse(const JEvent& event, std::vector<PhysicsEvent*>& physics_events) {
    // Get all child structures (Trigger Bank + ROC Banks)
    auto evio_data_block = event.Get<EvioEventWrapper>().at(0)->evio_event;
    auto& children = evio_data_block->getChildren();
    
    // Parse the trigger bank to extract ROC segments and event number
    TriggerData trigger_data(0);
    auto trigger_bank_roc_segments = parseTriggerBank(children[0], trigger_data);
    
    // Parse the data banks while using the trigger bank ROC segments for validation
    std::vector<std::shared_ptr<evio::BaseStructure>> roc_banks(children.begin() + 1, children.end());
    parseROCBanks(roc_banks, trigger_bank_roc_segments, trigger_data, physics_events);
}

/**
 * @brief Parse the trigger bank and extract ROC segments
 * 
 * This method parses the trigger bank structure to:
 * 1. Extract the event number from the EB1 segment
 * 2. Collect all ROC segments (UINT32 data type)
 * 3. Validate the number of ROC segments matches the header
 * 
 * @param trigger_bank  The trigger bank structure to parse
 * @param trigger_data  Output trigger metadata (first event number, etc.)
 * @return Vector of trigger bank ROC segments
 */
std::vector<std::shared_ptr<evio::BaseStructure>> EvioEventParser::parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank,
                                                                                    TriggerData& trigger_data) {
    // Get the number of ROC segments from the header
    int trigger_bank_rocs_count = static_cast<int>(trigger_bank->getHeader()->getNumber());
    auto trigger_bank_children = trigger_bank->getChildren();
    
    // Extract event number from the first segment (EB1)
    auto eb1_segment = trigger_bank_children.at(0);
    std::vector<uint64_t> eb1_data = eb1_segment->getULongData();
    trigger_data.first_event_number = static_cast<uint64_t>(eb1_data[0]);
    
    // Collect all ROC segments (UINT32 data type)
    std::vector<std::shared_ptr<evio::BaseStructure>> trigger_bank_rocs_data;
    
    for (auto& child : trigger_bank_children) {
        auto dtype = child->getHeader()->getDataType();
        if (dtype == evio::DataType::UINT32) {
            trigger_bank_rocs_data.push_back(child);
        }
    }
    
    // Validate that the number of ROC segments matches the header
    if(trigger_bank_rocs_count != trigger_bank_rocs_data.size()) {
        throw JException("EvioEventParser::parseTriggerBank: #ROC segments != header #ROCS -- %d != %d", trigger_bank_rocs_count, trigger_bank_rocs_data.size());
    }

    return trigger_bank_rocs_data;
}

/**
 * @brief Parse data banks and extract hits
 * 
 * This method processes the data banks by:
 * 1. Validating that the number of data banks matches trigger bank ROC segments
 * 2. Matching ROC IDs between trigger and data banks
 * 3. Parsing each data block using BankParser
 * 
 * @param data_banks                 Vector of data banks to parse
 * @param trigger_bank_roc_segments  Vector of trigger bank ROC segments for validation
 * @param trigger_data               Trigger metadata for this EVIO block
 * @param physics_events             Output vector which will be filled with PhysicsEvent*
 */
void EvioEventParser::parseROCBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& data_banks,
                                     const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments,
                                     TriggerData& trigger_data,
                                     std::vector<PhysicsEvent*>& physics_events) {
    
    // Validate that the number of data banks matches the number of ROC segments
    // Expected: #ROCs == #TriggerBankROCsegments == #RemainingBanksAfterTriggerBank 
    if(data_banks.size() != trigger_bank_roc_segments.size()) {
        throw JException("EvioEventParser::parseROCBanks: #ROC databanks != #ROC segments in trigger bank -- %d != %d", data_banks.size(), trigger_bank_roc_segments.size());
    }
    
    // Process each data bank
    for (size_t i = 0; i < data_banks.size(); i++) {
        auto db = data_banks[i];
        auto tb = trigger_bank_roc_segments[i];
        
        // Extract ROC IDs for validation
        uint32_t tb_rocid = tb->getHeader()->getTag();
        uint32_t db_rocid = (db->getHeader()->getTag()) & 0x0FFF;  // Mask to get ROC ID
        
        // Validate that ROC IDs match between trigger and data banks
        if(tb_rocid != db_rocid) {
            throw JException("EvioEventParser::parseROCBanks: Trigger bank roc segment rocid != Data bank rocid -- %d != %d", tb_rocid, db_rocid);
        }
        
        // Parse one or more DMA banks within this ROC bank using the registered BankParsers
        auto dma_blocks = db->getChildren();
        for (auto dma : dma_blocks) {
            auto bank_id = dma->getHeader()->getTag();
            auto bank_parser = m_app->GetService<JEventService_BankParsersMap>()->getParser(bank_id);
            if (bank_parser == nullptr) {
                throw JException("EvioEventParser::parseROCBanks: No parser found for bank tag %d", bank_id);
            }
            bank_parser->parse(dma, db_rocid, physics_events, trigger_data);
        }
    }
}

