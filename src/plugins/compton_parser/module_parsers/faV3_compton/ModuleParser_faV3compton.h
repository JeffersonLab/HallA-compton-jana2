#ifndef MODULEPARSER_faV3compton_H
#define MODULEPARSER_faV3compton_H

#include "ModuleParser.h"
#include "faV3comptonAccumulatorHit.h"
#include "faV3comptonPulseHit.h"

/**
 * @class ModuleParser_faV3compton
 * @brief ModuleParser implementation for faV3compton250 data
 */
class ModuleParser_faV3compton : public ModuleParser {
public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
               uint32_t rocid,
               std::vector<PhysicsEvent*>& physics_events,
               TriggerData& trigger_data) override;

private:
     std::vector<faV3comptonAccumulatorHit> parseAccumulator(
        const std::vector<uint32_t>& data_words,
        size_t& index
     );

     std::vector<faV3comptonPulseHit> parsePulseSum(
         const std::vector<uint32_t>& data_words,
         size_t& index,
         uint32_t pulse_number,
         uint32_t chan,
         uint32_t prescale_factor);
};
#endif // MODULEPARSER_faV3compton_H
