#ifndef BANKPARSER_ITSCALER_H
#define BANKPARSER_ITSCALER_H

#include "BankParser.h"
#include "ITScalerHit.h"
#include "EventHits_ITScaler.h"

/**
 * @class BankParser_ITScaler
 * @brief BankParser implementation for IT scaler data
 *
 * This parser decodes IT scaler words from an EVIO bank and fills
 * an EventHits_ITScaler container with ITScalerHit objects.
 */
class BankParser_ITScaler : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;
};

#endif // BANKPARSER_ITSCALER_H