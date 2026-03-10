#pragma once

#include <JANA/JService.h>
#include <map>
#include <mutex>

/**
 * @class JEventService_BankToModuleMap
 * @brief JANA service mapping bank IDs to module IDs
 */
class JEventService_BankToModuleMap : public JService {

    std::map<int, int> m_bank_to_module;
    Parameter<std::string> m_bank_to_module_file {this, "BANKMAP:FILE", "config/mapping.db",
        "Mapping file with lines: 'module_id bank_id'", true};
    std::mutex m_mutex;

public:
    void Init() override {
        if (!m_bank_to_module_file().empty()) {
            loadMappingFile(m_bank_to_module_file());
        }
    }

    void loadMappingFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw JException("JEventService_BankToModuleMap: Failed to open mapping file: %s", filename.c_str());
        }
        // Load the mappings from the file.
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            int module_id, bank_id;
            if (iss >> module_id >> bank_id) {
                m_bank_to_module[bank_id] = module_id;
            }
        }
        // If nothing after the header, throw an error.
        if (m_bank_to_module.empty()) {
            throw JException("JEventService_BankToModuleMap: No data in mapping file: %s", filename.c_str());
        }
    }

    /// Look up module ID for a bank ID. Returns true if found.
    int getModuleId(int bank_id) {
        auto it = m_bank_to_module.find(bank_id);
        if (it == m_bank_to_module.end()) return -1;
        return it->second;
    }
};
