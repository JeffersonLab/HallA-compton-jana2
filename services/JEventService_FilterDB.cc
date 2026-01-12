#include "JEventService_FilterDB.h"

/**
 * @brief Print a formatted summary table of the filter database
 * 
 * Calculates column widths dynamically based on the data and prints
 * a formatted table showing all allowed ROC_ID, SLOT_ID, MODEL, and BANK_ID
 * combinations from the filter database.
 * 
 * @param os Output stream to write the table to
 */
void JEventService_FilterDB::printSummaryTable(std::ostream& os) const {
    // Column headers in the specified order
    const std::string h_roc = "ROC_ID";
    const std::string h_slots = "SLOT_ID";
    const std::string h_models = "MODEL";
    const std::string h_banks = "BANK_ID";

    // Initialize column widths with header sizes
    size_t w_roc = h_roc.size();
    size_t w_slots = h_slots.size();
    size_t w_models = h_models.size();
    size_t w_banks = h_banks.size();

    // Calculate maximum column widths based on actual values
    for (const auto& [rocid, entry] : data) {
        w_roc = std::max(w_roc, std::to_string(rocid).size());
        
        // Find max width for slots
        for (const auto& slot : entry.at("slots")) {
            w_slots = std::max(w_slots, std::to_string(slot).size());
        }
        
        // Find max width for models
        for (const auto& model : entry.at("models")) {
            w_models = std::max(w_models, std::to_string(model).size());
        }
        
        // Find max width for banks
        for (const auto& bank : entry.at("banks")) {
            w_banks = std::max(w_banks, std::to_string(bank).size());
        }
    }

    // Print heading before the table
    os << "ALLOWED ROCs CONFIGURATIONS:" << std::endl;
    
    // Print table header in specified order: ROC_ID, SLOT_ID, MODEL, BANK_ID
    os << std::left
       << std::setw(static_cast<int>(w_roc)) << h_roc << "  "
       << std::setw(static_cast<int>(w_slots)) << h_slots << "  "
       << std::setw(static_cast<int>(w_models)) << h_models << "  "
       << std::setw(static_cast<int>(w_banks)) << h_banks
       << std::endl;

    // Print separator line
    os << std::string(w_roc, '-') << "  "
       << std::string(w_slots, '-') << "  "
       << std::string(w_models, '-') << "  "
       << std::string(w_banks, '-')
       << std::endl;

    // Print data rows - one row per (rocid, slot, model, bank) combination
    // Since the data structure stores them separately, we print all combinations
    for (const auto& [rocid, entry] : data) {
        const auto& slots = entry.at("slots");
        const auto& models = entry.at("models");
        const auto& banks = entry.at("banks");
        
        // Print one row for each combination
        // Use the maximum size to determine how many rows we need
        size_t max_size = std::max({slots.size(), models.size(), banks.size()});
        
        for (size_t i = 0; i < max_size; ++i) {
            os << std::left
               << std::setw(static_cast<int>(w_roc)) << rocid << "  "
               << std::setw(static_cast<int>(w_slots)) 
                  << (i < slots.size() ? std::to_string(slots[i]) : "") << "  "
               << std::setw(static_cast<int>(w_models)) 
                  << (i < models.size() ? std::to_string(models[i]) : "") << "  "
               << std::setw(static_cast<int>(w_banks)) 
                  << (i < banks.size() ? std::to_string(banks[i]) : "")
               << std::endl;
        }
    }
}

/**
 * @brief Load filter database from a text file
 * 
 * Reads the filter file line by line and parses each non-comment line
 * as four integers: rocid, slot, model, bank. Each parsed entry is
 * added to the internal filter database structure.
 * 
 * File format:
 *   - Lines starting with '#' are treated as comments and skipped
 *   - Empty lines are skipped
 *   - Each data line contains: rocid slot model bank
 * 
 * @param filename Path to the filter database file
 * @throws JException if the file cannot be opened or contains malformed lines
 */
void JEventService_FilterDB::fillDB(const std::string& filename) {
    // Open the filter database file
    std::ifstream file(filename);
    if (!file) {
        throw JException("Failed to open filter DB file: " + filename);
    }

    // Process each line in the file
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments (lines starting with '#') and empty lines
        if (line.empty() || line[0] == '#')
            continue;
        
        // Parse the line as four integers: rocid slot model bank
        std::istringstream iss(line);
        int rocid, slot, model, bank;
        if (!(iss >> rocid >> slot >> model >> bank)) {
            throw JException("Malformed line: " + line);
        }

        // Add the parsed values to the filter database
        auto& entry = data[rocid];
        entry["banks"].push_back(bank);
        entry["slots"].push_back(slot);
        entry["models"].push_back(model);
    }

    // Print summary of loaded filter database
    printSummaryTable(std::cout);
}

/**
 * @brief Initialize the service and optionally load filter database
 * 
 * Called by JANA2 during service initialization. Reads the FILTER:ENABLE
 * and FILTER:FILE parameters. If filtering is enabled, loads the filter
 * database from the specified file.
 */
void JEventService_FilterDB::Init() {
    // Get parameter values
    bool enable = m_filter_enable();
    std::string filename = m_filter_file();
    
    // Load filter database if filtering is enabled
    if (enable) {
        fillDB(filename);
    }
}

/**
 * @brief Check if a ROC ID is allowed
 * 
 * If no filter database has been loaded (data.empty()), filtering is
 * treated as disabled and all ROCs are allowed. Otherwise, checks if
 * the ROC ID exists in the filter database.
 * 
 * @param rocid ROC ID to check
 * @return true if ROC is allowed or filtering is disabled, false otherwise
 */
bool JEventService_FilterDB::isROCAllowed(int rocid) {
    // If no entries have been loaded, treat filtering as disabled (allow all)
    if (data.empty()) return true;
    
    // Check if ROC ID exists in the filter database
    return data.find(rocid) != data.end();
}

/**
 * @brief Check if a bank ID is allowed for a given ROC
 * 
 * If filtering is disabled, all banks are allowed. Otherwise, checks if
 * the bank ID is in the allow-list for the specified ROC.
 * 
 * @param rocid ROC ID
 * @param bank Bank ID to check
 * @return true if bank is allowed or filtering is disabled, false otherwise
 */
bool JEventService_FilterDB::isBankAllowed(int rocid, int bank) {
    // If filtering is disabled, allow all
    if (data.empty()) return true;
    
    // Find the ROC entry
    auto it_roc = data.find(rocid);
    if (it_roc == data.end()) return false;
    
    // Find the banks vector for this ROC
    auto it_vec = it_roc->second.find("banks");
    if (it_vec == it_roc->second.end()) return false;
    
    // Check if the bank ID is in the allow-list
    const auto& vec = it_vec->second;
    return std::find(vec.begin(), vec.end(), bank) != vec.end();
}

/**
 * @brief Check if a slot is allowed for a given ROC
 * 
 * If filtering is disabled, all slots are allowed. Otherwise, checks if
 * the slot is in the allow-list for the specified ROC.
 * 
 * @param rocid ROC ID
 * @param slot Slot number to check
 * @return true if slot is allowed or filtering is disabled, false otherwise
 */
bool JEventService_FilterDB::isSlotAllowed(int rocid, int slot) {
    // If filtering is disabled, allow all
    if (data.empty()) return true;
    
    // Find the ROC entry
    auto it_roc = data.find(rocid);
    if (it_roc == data.end()) return false;
    
    // Find the slots vector for this ROC
    auto it_vec = it_roc->second.find("slots");
    if (it_vec == it_roc->second.end()) return false;
    
    // Check if the slot is in the allow-list
    const auto& vec = it_vec->second;
    return std::find(vec.begin(), vec.end(), slot) != vec.end();
}

/**
 * @brief Check if a model is allowed for a given ROC
 * 
 * If filtering is disabled, all models are allowed. Otherwise, checks if
 * the model is in the allow-list for the specified ROC.
 * 
 * @param rocid ROC ID
 * @param model Model number to check
 * @return true if model is allowed or filtering is disabled, false otherwise
 */
bool JEventService_FilterDB::isModelAllowed(int rocid, int model) {
    // If filtering is disabled, allow all
    if (data.empty()) return true;
    
    // Find the ROC entry
    auto it_roc = data.find(rocid);
    if (it_roc == data.end()) return false;
    
    // Find the models vector for this ROC
    auto it_vec = it_roc->second.find("models");
    if (it_vec == it_roc->second.end()) return false;
    
    // Check if the model is in the allow-list
    const auto& vec = it_vec->second;
    return std::find(vec.begin(), vec.end(), model) != vec.end();
}

