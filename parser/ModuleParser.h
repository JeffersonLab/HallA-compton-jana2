#ifndef MODULEPARSER_H
#define MODULEPARSER_H

#include <vector>
#include <memory>
#include "eviocc.h"
#include "EventHits.h"
#include "PhysicsEvent.h"
#include "TriggerData.h"

/**
 * @class ModuleParser
 * @brief Base class for parsing module data blocks into PhysicsEvent objects
 *
 * This is the common base type for all user-defined module parsers. Each
 * concrete parser implements the @ref parse method for a specific hardware
 * module (e.g. FADC250). Helper functions for decoding words and building
 * hit objects are provided as protected utility methods so that all parsers
 * can reuse them.
 */
class ModuleParser {

private:
    JLogger* m_logger = nullptr;  ///< Pointer to the jana::JLogger (set via SetLogger)

protected:

    /// Extract bits from a 32-bit word (utility shared by derived parsers)
    uint32_t getBitsInRange(uint32_t x, int high, int low) {
        return (x >> low) & ((1u << (high - low + 1)) - 1);
    }

public:
    virtual ~ModuleParser() = default;

    /**
     * @brief Parse a raw data block and extract physics events
     *
     * @param data_block             EVIO data block to parse
     * @param rocid                  ROC ID for this data block
     * @param physics_events         Vector to be filled with PhysicsEvent*.
     *                               The parser is responsible for allocating
     *                               PhysicsEvent objects; ownership is handed
     *                               to the caller.
     * @param trigger_data         Metadata (event number, etc.)
     *                             from the trigger bank.
     */
    virtual void parse(std::shared_ptr<evio::BaseStructure> data_block,
                       uint32_t rocid,
                       std::vector<PhysicsEvent*>& physics_events,
                       TriggerData& trigger_data) = 0;

    /**
     * @brief Set the logger
     *
     * @param logger Reference to the jana::JLogger
     */
    void SetLogger(JLogger& logger) { m_logger = &logger; }

    /**
     * @brief Get the logger
     *
     * @return Reference to the jana::JLogger
     */
    JLogger& GetLogger() { return *m_logger; }
};

#endif // MODULEPARSER_H