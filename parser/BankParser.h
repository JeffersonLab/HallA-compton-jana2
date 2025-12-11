#ifndef BANKPARSER_H
#define BANKPARSER_H

#include <vector>
#include <memory>
#include "eviocc.h"
#include "EventHits.h"
#include "PhysicsEvent.h"

/**
 * @class BankParser
 * @brief Base class for parsing bank data blocks into PhysicsEvent objects
 *
 * This is the common base type for all user-defined bank parsers. Each
 * concrete parser implements the @ref parse method for a specific hardware
 * format (e.g. FADC250). Helper functions for decoding words and building
 * hit objects are provided as protected utility methods so that all parsers
 * can reuse them.
 */
class BankParser {
public:
    virtual ~BankParser() = default;

    /**
     * @brief Parse a raw data block and extract physics events
     *
     * @param data_block            EVIO data block to parse
     * @param rocid                 ROC ID for this data block
     * @param physics_events        Vector to be filled with PhysicsEvent*.
     *                              The parser is responsible for allocating
     *                              PhysicsEvent objects; ownership is handed
     *                              to the caller.
     * @param block_first_event_num First event number in the EVIO block
     */
    virtual void parse(std::shared_ptr<evio::BaseStructure> data_block,
                       uint32_t rocid,
                       std::vector<PhysicsEvent*>& physics_events,
                       uint64_t block_first_event_num) = 0;

protected:
    /// Extract bits from a 32-bit word (utility shared by derived parsers)
    uint32_t getBitsInRange(uint32_t x, int high, int low) {
        return (x >> low) & ((1u << (high - low + 1)) - 1);
    }
};

#endif // BANKPARSER_H