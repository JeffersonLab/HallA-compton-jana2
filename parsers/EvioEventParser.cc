#include "EvioEventParser.h"
#include <JANA/JException.h>

/**
 * @brief Parse the EVIO event and extract all detector hits
 * 
 * This method orchestrates the parsing of an EVIO event by:
 * 1. Validating the event structure
 * 2. Parsing the trigger bank to extract ROC segments and event number
 * 3. Parsing data banks to extract detector hits
 */
void EvioEventParser::parse() {
    // Check that the block contains exactly one event (>1 events in  an evio_event are NOT_SUPPORTED yet)
    std::shared_ptr<evio::BaseStructureHeader> header = m_event->getHeader();
    uint8_t nblock_events = header->getNumber();
    if(nblock_events > 1) {
        throw JException("EvioEventParser::parse: block has more than one events - NOT SUPPORTED YET");
    }

    // Get all child structures (Trigger Bank + Data Banks)
    auto& children = m_event->getChildren();
    
    // Parse the trigger bank to extract ROC segments and event number
    auto trigger_bank_roc_segments = parseTriggerBank(children[0]);
    
    // Parse the data banks while using the trigger bank ROC segments for validation
    std::vector<std::shared_ptr<evio::BaseStructure>> data_banks(children.begin() + 1, children.end());
    parseDataBanks(data_banks, trigger_bank_roc_segments);
    
    // Mark the event as successfully parsed
    m_is_parsed = true;
}

/**
 * @brief Parse the trigger bank and extract ROC segments
 * 
 * This method parses the trigger bank structure to:
 * 1. Extract the event number from the EB1 segment
 * 2. Collect all ROC segments (UINT32 data type)
 * 3. Validate the number of ROC segments matches the header
 * 
 * @param trigger_bank The trigger bank structure to parse
 * @return Vector of trigger bank ROC segments
 */
std::vector<std::shared_ptr<evio::BaseStructure>> EvioEventParser::parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank) {
    // Get the number of ROC segments from the header
    int trigger_bank_rocs_count = static_cast<int>(trigger_bank->getHeader()->getNumber());
    auto trigger_bank_children = trigger_bank->getChildren();
    
    // Extract event number from the first segment (EB1)
    auto eb1_segment = trigger_bank_children.at(0);
    std::vector<uint64_t> eb1_data = eb1_segment->getULongData();
    m_event_num = eb1_data[0];
    
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
 * 3. Parsing each data block using RawDataParser
 * 
 * @param data_banks Vector of data banks to parse
 * @param trigger_bank_roc_segments Vector of trigger bank ROC segments for validation
 */
void EvioEventParser::parseDataBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& data_banks,
                                     const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments) {
    
    // Validate that the number of data banks matches the number of ROC segments
    // Expected: #ROCs == #TriggerBankROCsegments == #RemainingBanksAfterTriggerBank 
    if(data_banks.size() != trigger_bank_roc_segments.size()) {
        throw JException("EvioEventParser::parseDataBanks: #ROC databanks != #ROC segments in trigger bank -- %d != %d", data_banks.size(), trigger_bank_roc_segments.size());
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
            throw JException("EvioEventParser::parseDataBanks: Trigger bank roc segment rocid != Data bank rocid -- %d != %d", tb_rocid, db_rocid);
        }
        
        // Parse all data blocks within this data bank
        auto data_blocks = db->getChildren();
        for (auto dbb : data_blocks) {
            RawDataParser::parseRawData(dbb, db_rocid, m_hits);
        }
    }
}

/**
 * @brief Get the event number
 * 
 * Returns the event number extracted from the trigger bank during parsing.
 * 
 * @return Event number
 * @throws JException if called before the event is parsed
 */
uint64_t EvioEventParser::getEventNumber() const {
    if(!m_is_parsed) {
        throw JException("EvioEventParser::getEventNumber: Trying to get event_num before the event is parsed");
    }
    return m_event_num;
}

/**
 * @brief Get the parsed hits
 * 
 * Returns the EventHits structure containing all parsed detector hits.
 * 
 * @return Shared pointer to EventHits containing all parsed hits
 * @throws JException if called before the event is parsed
 */
std::shared_ptr<EventHits> EvioEventParser::getHits() const {
    if(!m_is_parsed) {
        throw JException("EvioEventParser::getHits: Trying to get hits before the event is parsed");
    }
    return m_hits;
}

/**
 * @brief Identifies run control events and extracts run number from prestart events
 * 
 * Run control events have tags in the range 0xFFD0-0xFFDF. When a prestart event 
 * (tag 0xFFD1) is detected, the run number is extracted from the event data and 
 * stored in the run_number parameter.
 * 
 * @param event The EVIO event to examine
 * @param run_number Reference to run number (updated if prestart event found)
 * @return true if this was a run control event, false otherwise
 */
bool EvioEventParser::isRunControlEvent(std::shared_ptr<evio::EvioEvent> event, int& run_number) {
    std::shared_ptr<evio::BaseStructureHeader> header = event->getHeader();
    uint16_t tag = header->getTag();
    
    if (tag >= 0xFFD0 && tag <= 0xFFDF) {
        if (tag == 0xFFD1) { // prestart event
            std::vector<uint32_t> data = event->getUIntData();
            if (data.size() > 1) {
                run_number = data[1];  // Run number is stored at index 1
            } else {
                throw JException("Prestart event has no data");
            }
        }
        return true;
    }
    
    return false; 
}
