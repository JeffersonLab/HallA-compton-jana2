#include "EvioEventParser.h"
#include <JANA/JException.h>


void EvioEventParser::parse() {
    std::shared_ptr<evio::BaseStructureHeader> header = m_event->getHeader();
    
    uint8_t nblock_events = header->getNumber();
    assert(nblock_events == 1);

    // Get Trigger Bank + Data Banks
    auto& children = m_event->getChildren();
    
    // Parse trigger bank
    auto trigger_bank_roc_segments = parseTriggerBank(children[0]);
    
    // Parse data banks
    std::vector<std::shared_ptr<evio::BaseStructure>> data_banks(children.begin() + 1, children.end());
    parseDataBanks(data_banks, trigger_bank_roc_segments);
    
    m_is_parsed = true;
}

std::vector<std::shared_ptr<evio::BaseStructure>> EvioEventParser::parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank) {
    int trigger_bank_rocs_count = static_cast<int>(trigger_bank->getHeader()->getNumber());
    auto trigger_bank_children = trigger_bank->getChildren();
    auto eb1_segment = trigger_bank_children.at(0);
    std::vector<uint64_t> eb1_data = eb1_segment->getULongData();
    m_event_num = eb1_data[0];
    
    std::vector<std::shared_ptr<evio::BaseStructure>> trigger_bank_rocs_data;
    
    for (auto& child : trigger_bank_children) {
        auto dtype = child->getHeader()->getDataType();
        if (dtype == evio::DataType::UINT32) {
            trigger_bank_rocs_data.push_back(child);
        }
    }
    
    assert(trigger_bank_rocs_count == trigger_bank_rocs_data.size());
    
    return trigger_bank_rocs_data;
}

void EvioEventParser::parseDataBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& data_banks,
                                     const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments) {
    
    // Validate data banks count
    // #ROCs == #TriggerBankROCsegments == #RemainingBanksAfterTriggerBank 
    assert(data_banks.size() == trigger_bank_roc_segments.size());
    
    // Parse data banks using RawDataParser
    for (size_t i = 0; i < data_banks.size(); i++) {
        auto db = data_banks[i];
        auto tb = trigger_bank_roc_segments[i];
        uint32_t tb_rocid = tb->getHeader()->getTag();
        uint32_t db_rocid = (db->getHeader()->getTag()) & 0x0FFF;
        
        assert(tb_rocid == db_rocid);
        
        auto data_blocks = db->getChildren();
        for (auto dbb : data_blocks) {
            RawDataParser::parseRawData(dbb, db_rocid, m_hits);
        }
    }
}

uint64_t EvioEventParser::getEventNumber() const {
    assert(m_is_parsed);
    return m_event_num;
}

std::shared_ptr<EventHits> EvioEventParser::getHits() const {
    assert(m_is_parsed);
    return m_hits;
}


bool EvioEventParser::isRunControlEvent(std::shared_ptr<evio::EvioEvent> event, int& run_number) {
    std::shared_ptr<evio::BaseStructureHeader> header = event->getHeader();
    uint16_t tag = header->getTag();
    
    // Run control events are identified by tags in the range 0xFFD0-0xFFDF
    if (tag >= 0xFFD0 && tag <= 0xFFDF) {
        // Prestart events (tag 0xFFD1) contain the run number in the second data word
        if (tag == 0xFFD1) {
            std::vector<uint32_t> data = event->getUIntData();
            if (data.size() > 1) {
                run_number = data[1];  // Run number is stored at index 1
            } else {
                throw JException("Prestart event has no data");
            }
        }
        return true; // Event was identified as a run control event
    }
    
    return false; // Event is a regular physics event
}
