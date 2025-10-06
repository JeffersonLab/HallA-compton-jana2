
#include "JEventProcessor_MOLLER.h"
#include <JANA/JLogger.h>

/**
 * @brief Constructor for JEventProcessor_MOLLER
 * 
 * Initialize the processor with the appropriate type name, prefix, and callback style.
 */
JEventProcessor_MOLLER::JEventProcessor_MOLLER() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with this class's name
    SetPrefix("jeventprocessor_moller");          // Set unique prefix for parameters
    SetCallbackStyle(CallbackStyle::ExpertMode);  // Use expert mode for full control
}

/**
 * @brief Initialize the processor
 * 
 * Called once at the start of processing. Open the output file and set up
 * any necessary resources for event processing.
 */
void JEventProcessor_MOLLER::Init() {
    LOG << "JEventProcessor_MOLLER::Init" << LOG_END;
    
    // Open the output file for writing hit information
    m_outfile.open(m_out_filename(), std::ios::out);
    if (!m_outfile.is_open()) {
        throw JException("Error opening output.txt!");
    }
}

/**
 * @brief Process a single event sequentially
 * 
 * Processes the event data and outputs detailed hit information to the file.
 * This method is called for each event in the processing pipeline.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JEventProcessor_MOLLER::ProcessSequential(const JEvent &event) {
    
    // Output event header
    m_outfile << "Event: " << event.GetEventNumber() << "\n";
    
    // Output waveform hits
    m_outfile << "[Waveform Hits]\n";
    m_outfile << "rocid slot module_id chan timestamp1 timestamp2 nWaveformSamples\n";
    for (const auto& w_hit : m_waveform_hits_in()) {
        m_outfile << w_hit->rocid << " "
                << w_hit->slot << " "
                << w_hit->module_id << " "
                << w_hit->chan << " "
                << w_hit->timestamp1 << " "
                << w_hit->timestamp2 << " "
                << w_hit->getWaveformSize() << "\n";
    }

    // Output pulse hits
    m_outfile << "[Pulse Hits]\n";
    m_outfile << "rocid slot module_id chan timestamp1 timestamp2 pedestalSum pedestalQual integralSum integralQual coarseTime fineTime pulsePeak\n";
    
    for (const auto& p_hit : m_pulse_hits_in()) {
        m_outfile << p_hit->rocid << " "
                << p_hit->slot << " "
                << p_hit->module_id << " "
                << p_hit->chan << " "
                << p_hit->timestamp1 << " "
                << p_hit->timestamp2 << " "
                << p_hit->pedestal_sum << " "
                << p_hit->pedestal_quality << " "
                << p_hit->integral_sum << " "
                << p_hit->integral_quality << " "
                << p_hit->coarse_time << " "
                << p_hit->fine_time << " "
                << p_hit->pulse_peak << "\n";
    }
    
    // Add spacing between events
    m_outfile << "\n\n";
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_MOLLER::Finish() {
    // Close the output file
    if (m_outfile.is_open()) {
        m_outfile.close();
    }
    LOG << "JEventProcessor_MOLLER::Finish" << LOG_END;
}

