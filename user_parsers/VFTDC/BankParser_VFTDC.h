#ifndef BANKPARSER_VFTDC_H
#define BANKPARSER_VFTDC_H

#include "BankParser.h"
#include "VFTDCHit.h"
#include "EventHits_VFTDC.h"
/**
 * @class BankParser_VFTDC
 * @brief BankParser implementation for VFTDC data
 */
class BankParser_VFTDC : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
    
};

#endif // BANKPARSER_VFTDC_H