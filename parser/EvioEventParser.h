#ifndef EVIOEVENTPARSER_H
#define EVIOEVENTPARSER_H

#include <string>
#include <vector>
#include <memory>

#include <JANA/JApplication.h>
#include <JANA/JEvent.h>

#include "eviocc.h"
#include "BankParser.h"
#include "EventHits.h"
#include "PhysicsEvent.h"
#include "TriggerData.h"

/**
 * Class for processing a single EVIO event
 */
class EvioEventParser {
private:
    
    const JEvent& m_event;
    JApplication* m_app;
    std::vector<PhysicsEvent*> m_physics_events;
    bool m_is_parsed;
    TriggerData trigger_data;
    
    /**
     * Parse trigger bank and extract ROC segments
     * @param trigger_bank The trigger bank to process
     * @return Vector of trigger bank ROC segments
     */
    std::vector<std::shared_ptr<evio::BaseStructure>> 
    parseTriggerBank(std::shared_ptr<evio::BaseStructure> trigger_bank);
    
    /**
     * Parse ROC banks and extract hits
     * @param roc_banks Vector of ROC banks
     * @param trigger_bank_roc_segments Vector of trigger bank ROC segments
     */
    void parseROCBanks(const std::vector<std::shared_ptr<evio::BaseStructure>>& roc_banks,
                       const std::vector<std::shared_ptr<evio::BaseStructure>>& trigger_bank_roc_segments);

public:
    /**
     * Constructor
     * @param event The EVIO event to parse
     */
    EvioEventParser(JApplication* app, const JEvent& event) 
        : m_event(event),
          m_app(app),
          m_physics_events(),
          m_is_parsed(false),
          trigger_data(0) {};
    
    /**
     * Destructor
     */
    ~EvioEventParser() = default;
    
    /**
     * Parse the EVIO event and extract all hits
     */
    void parse();

    
    /**
     * Get the parsed physics events
     * @return Vector of PhysicsEvent pointers containing all parsed physics events
     */
    std::vector<PhysicsEvent*> getPhysicsEvents() const;
    
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
