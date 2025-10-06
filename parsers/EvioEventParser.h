#ifndef EVIOEVENTPARSER_H
#define EVIOEVENTPARSER_H

#include <string>
#include <vector>
#include <memory>

#include "eviocc.h"
#include "RawDataParser.h"
#include "EventHits.h"

/**
 * Class for processing a single EVIO event
 */
class EvioEventParser {
private:
    
    std::shared_ptr<evio::EvioEvent> m_event;
    std::shared_ptr<EventHits> m_hits;
    uint64_t m_event_num;
    bool m_is_parsed;
    
    /**
     * Parse trigger bank and extract ROC segments
     * @param trigger_bank The trigger bank to process
     * @return Vector of trigger bank ROC segments
     */
    std::vector<std::shared_ptr<evio::BaseStructure>> 
    parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank);
    
    /**
     * Parse data banks and extract hits
     * @param data_banks Vector of data banks
     * @param trigger_bank_roc_segments Vector of trigger bank ROC segments
     */
    void parseDataBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& data_banks,
                       const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments);

public:
    /**
     * Constructor
     * @param event The EVIO event to parse
     */
    EvioEventParser(std::shared_ptr<evio::EvioEvent> event) 
        : m_event(event), m_event_num(0), m_hits(new EventHits()), m_is_parsed(false) {};
    
    /**
     * Destructor
     */
    ~EvioEventParser() = default;
    
    /**
     * Parse the EVIO event and extract all hits
     */
    void parse();
    
    /**
     * Get the event number
     * @return Event number
     */
    uint64_t getEventNumber() const;
    
    /**
     * Get the parsed hits
     * @return EventHits structure containing all parsed hits
     */
    std::shared_ptr<EventHits> getHits() const;
    
     /**
     * Identifies run control events and extracts run number from prestart events
     * 
     * Run control events have tags in the range 0xFFD0-0xFFDF. When a prestart event 
     * (tag 0xFFD1) is detected, the run number is extracted from the event data and 
     * stored in the run_number parameter.
     * 
     * @param event The EVIO event to examine
     * @param run_number Reference to run number (updated if prestart event found)
     * @return true if this was any run control event, false otherwise
     */
    static bool isRunControlEvent(std::shared_ptr<evio::EvioEvent> event, int& run_number);
   
};

#endif // EVIOEVENTPARSER_H
