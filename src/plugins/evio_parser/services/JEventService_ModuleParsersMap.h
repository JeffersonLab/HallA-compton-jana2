#pragma once

#include <JANA/JService.h>
#include "ModuleParser.h"
#include <map>
#include <mutex>

/**
 * @class JEventService_ModuleParsersMap
 * @brief JANA service mapping module IDs to parser implementations
 */
class JEventService_ModuleParsersMap : public JService {

    std::map<int, ModuleParser*> m_module_parsers;
    std::mutex m_mutex;

public:
    /// Register a parser implementation for a given module ID
    void addParser(int module_id, ModuleParser* parser) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_module_parsers[module_id] = parser;
    }

    /// Look up a parser implementation for a given module ID
    ModuleParser* getParser(int module_id) {
        auto it = m_module_parsers.find(module_id);
        return (it != m_module_parsers.end()) ? it->second : nullptr;
    }

    /// Destructor to clean up parser instances
    ~JEventService_ModuleParsersMap() override {
        for (auto& entry : m_module_parsers) {
            delete entry.second;
        }
    }
};
