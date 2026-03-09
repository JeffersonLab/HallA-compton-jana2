#pragma once

#include <JANA/JService.h>
#include <map>
#include <mutex>

/**
 * @class JEventService_BankToModelMap
 * @brief JANA service mapping bank IDs to model IDs
 */
class JEventService_BankToModelMap : public JService {

    std::map<int, int> m_bank_to_model;
    Parameter<std::string> m_bank_to_model_file {this, "BANKMAP:FILE", "config/mapping.db",
        "Mapping file with lines: 'model_id bank_id'", true};
    std::mutex m_mutex;

public:
    void Init() override {
        if (!m_bank_to_model_file().empty()) {
            loadMappingFile(m_bank_to_model_file());
        }
    }

    void loadMappingFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw JException("JEventService_BankToModelMap: Failed to open mapping file: %s", filename.c_str());
        }
        // Load the mappings from the file.
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            int model_id, bank_id;
            if (iss >> model_id >> bank_id) {
                m_bank_to_model[bank_id] = model_id;
            }
        }
        // If nothing after the header, throw an error.
        if (m_bank_to_model.empty()) {
            throw JException("JEventService_BankToModelMap: No data in mapping file: %s", filename.c_str());
        }
    }

    /// Look up model ID for a bank ID. Returns true if found.
    int getModelId(int bank_id) {
        auto it = m_bank_to_model.find(bank_id);
        if (it == m_bank_to_model.end()) return -1;
        return it->second;
    }
};
