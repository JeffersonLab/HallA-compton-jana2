#pragma once

#include <JANA/JService.h>
#include "BankParser.h"
#include <map>
#include <mutex>

/**
 * @class JEventService_ModelParsersMap
 * @brief JANA service mapping model IDs to parser implementations
 */
class JEventService_ModelParsersMap : public JService {

    std::map<int, BankParser*> m_model_parsers;
    std::mutex m_mutex;

public:
    /// Register a parser implementation for a given model ID
    void addParser(int model_id, BankParser* parser) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_model_parsers[model_id] = parser;
    }

    /// Look up a parser implementation for a given model ID
    BankParser* getParser(int model_id) {
        auto it = m_model_parsers.find(model_id);
        return (it != m_model_parsers.end()) ? it->second : nullptr;
    }

    /// Destructor to clean up parser instances
    ~JEventService_ModelParsersMap() override {
        for (auto& entry : m_model_parsers) {
            delete entry.second;
        }
    }
};
