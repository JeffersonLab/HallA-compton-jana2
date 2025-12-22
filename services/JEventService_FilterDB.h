#pragma once

#include <JANA/JService.h>
#include <JANA/JException.h>

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

/**
 * @class JEventService_FilterDB
 * @brief JANA service providing a simple ROC/slot/model/bank allow-list from a text file
 */
class JEventService_FilterDB : public JService {

    /// Map: rocid -> ("banks" | "slots" | "models") -> list of allowed values
    std::map<int, std::map<std::string, std::vector<int>>> data;

public:

    /**
     * @brief Filtering control parameters
     *
     * These parameters follow the same pattern as in JEventProcessor_Compton:
     * - owner: this service (for automatic registration)
     * - name: shared parameter name used on the command line / config
     * - default_value: default if not specified
     * - description: help text
     * - is_shared: true so the name is used as-is (no prefix added)
     *
     * Usage from the command line:
     *   -PFILTER:ENABLE=1 -PFILTER:FILE=my_filter.db
     */
    Parameter<bool> m_filter_enable {this, "FILTER:ENABLE", false,
                                     "Enable ROC/bank filtering using FILTER:FILE (true/false, default false)", true};
    Parameter<std::string> m_filter_file {this, "FILTER:FILE", "config/filter.db",
                                          "Filter DB filename with lines: 'rocid slot model bank'", true};

    /// Configure and optionally load the filter DB using JANA parameters
    void Init() override {
        bool enable = m_filter_enable();
        std::string filename = m_filter_file();
        if (enable) {
            fillDB(filename);
        }
    }

    void fillDB(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw JException("Failed to open filter DB file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#')
                continue;
            std::istringstream iss(line);
            int rocid, slot, model, bank;
            if (!(iss >> rocid >> slot >> model >> bank)) {
                throw JException("Malformed line: " + line);
            }

            auto& entry = data[rocid];
            entry["banks"].push_back(bank);
            entry["slots"].push_back(slot);
            entry["models"].push_back(model);
        }
    }
   
    bool isROCAllowed(int rocid) {
        // If no entries have been loaded, treat filtering as disabled (allow all)
        if (data.empty()) return true;
        return data.find(rocid) != data.end();
    }

    bool isBankAllowed(int rocid, int bank) {
        if (data.empty()) return true;
        auto it_roc = data.find(rocid);
        if (it_roc == data.end()) return false;
        auto it_vec = it_roc->second.find("banks");
        if (it_vec == it_roc->second.end()) return false;
        const auto& vec = it_vec->second;
        return std::find(vec.begin(), vec.end(), bank) != vec.end();
    }

    bool isSlotAllowed(int rocid, int slot) {
        if (data.empty()) return true;
        auto it_roc = data.find(rocid);
        if (it_roc == data.end()) return false;
        auto it_vec = it_roc->second.find("slots");
        if (it_vec == it_roc->second.end()) return false;
        const auto& vec = it_vec->second;
        return std::find(vec.begin(), vec.end(), slot) != vec.end();
    }

    bool isModelAllowed(int rocid, int model) {
        if (data.empty()) return true;
        auto it_roc = data.find(rocid);
        if (it_roc == data.end()) return false;
        auto it_vec = it_roc->second.find("models");
        if (it_vec == it_roc->second.end()) return false;
        const auto& vec = it_vec->second;
        return std::find(vec.begin(), vec.end(), model) != vec.end();
    }
};
