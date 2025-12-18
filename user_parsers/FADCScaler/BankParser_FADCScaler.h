#ifndef BANKPARSER_FADCSCALER_H
#define BANKPARSER_FADCSCALER_H

#include "BankParser.h"
#include "FADCScalerHit.h"
#include "EventHits_FADCScaler.h"

/**
 * @class BankParser_FADCScaler
 * @brief BankParser implementation for FADC scaler data
 *
 * This parser decodes scaler words from an EVIO bank and fills
 * an EventHits_FADCScaler container with FADCScalerHit objects.
 */
class BankParser_FADCScaler : public BankParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& block_first_event_data) override;
};

#endif // BANKPARSER_FADCSCALER_H