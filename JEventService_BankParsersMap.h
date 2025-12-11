#pragma once

#include <JANA/JService.h>
#include "BankParser.h"
#include <map>
#include <mutex>

/**
 * @class JEventService_BankParsersMap
 * @brief JANA service mapping Bank IDs to BankParser implementations
 */
class JEventService_BankParsersMap : public JService {

    std::map<int, BankParser*> m_bank_parsers;
    std::mutex m_mutex;

public:
    /// Register a parser implementation for a given Bank ID
    void addParser(int bank_id, BankParser* parser) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_bank_parsers[bank_id] = parser;
    }

    /// Look up a parser implementation for a given Bank ID
    BankParser* getParser(int bank_id) {
        auto it = m_bank_parsers.find(bank_id);
        return (it != m_bank_parsers.end()) ? it->second : nullptr;
    }
};
