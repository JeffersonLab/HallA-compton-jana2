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
 * Called once at the start of processing. Open the output files and set up
 * any necessary resources for event processing.
 */
void JEventProcessor_MOLLER::Init() {
    LOG << "JEventProcessor_MOLLER::Init" << LOG_END;
    
    // Open the text output file for writing hit information
    m_text_output_file.open(m_text_output_filename(), std::ios::out);
    if (!m_text_output_file.is_open()) {
        throw JException("Failed to open text output file: " + m_text_output_filename());
    }
    
    // Open the ROOT output file
    m_root_output_file = new TFile(m_root_output_filename().c_str(), "RECREATE");
    if (m_root_output_file == nullptr || m_root_output_file->IsZombie()) {
        throw JException("Failed to open ROOT output file: " + m_root_output_filename());  
    }

    // Initialize the waveform tree row data structure
    m_waveform_tree_row = WaveformTreeRow();

    // Create ROOT tree for waveform data
    m_waveform_tree = new TTree("waveform_tree", "FADC250 Waveform Data (slot, channel, waveform)");
    m_waveform_tree->Branch("slot", &m_waveform_tree_row.slot, "slot/i");
    m_waveform_tree->Branch("chan", &m_waveform_tree_row.chan, "chan/i");
    m_waveform_tree->Branch("waveform", &m_waveform_tree_row.waveform);

    // Create histogram for pulse integral distribution
    m_pulse_integral_hist = new TH1I("h_integral", "Pulse Integral Distribution;Integral Sum;Counts", 100, 0, 1);
    m_pulse_integral_hist->SetCanExtend(TH1::kAllAxes);  // Allow ROOT to automatically extend bins
}

/**
 * @brief Process a single event sequentially
 * 
 * Processes FADC250 detector data for a single event. Writes detailed hit 
 * information to text file and fills ROOT tree and histograms with waveform 
 * and pulse data. This method is called for each event in the processing pipeline.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JEventProcessor_MOLLER::ProcessSequential(const JEvent &event) {
    
    // Write event header to text file
    m_text_output_file << "Event: " << event.GetEventNumber() << "\n";
    
    // Process and output FADC250 waveform hits
    m_text_output_file << "[Waveform Hits]\n";
    m_text_output_file << "rocid slot module_id chan timestamp1 timestamp2 nWaveformSamples\n";
    for (const auto& waveform_hit : m_waveform_hits_in()) {
        // Write waveform hit data to text file
        m_text_output_file << waveform_hit->rocid << " "
                << waveform_hit->slot << " "
                << waveform_hit->module_id << " "
                << waveform_hit->chan << " "
                << waveform_hit->timestamp1 << " "
                << waveform_hit->timestamp2 << " "
                << waveform_hit->getWaveformSize() << "\n";
        
        // Fill ROOT tree with waveform data
        m_waveform_tree_row.slot = waveform_hit->slot;
        m_waveform_tree_row.chan = waveform_hit->chan;
        m_waveform_tree_row.waveform = waveform_hit->waveform;
        m_waveform_tree->Fill();
    }

    // Output FADC250 pulse hits
    m_text_output_file << "[Pulse Hits]\n";
    m_text_output_file << "rocid slot module_id chan timestamp1 timestamp2 pedestalSum pedestalQual integralSum integralQual coarseTime fineTime pulsePeak\n";
    
    for (const auto& pulse_hit : m_pulse_hits_in()) {
        // Write pulse hit data to text file
        m_text_output_file << pulse_hit->rocid << " "
                << pulse_hit->slot << " "
                << pulse_hit->module_id << " "
                << pulse_hit->chan << " "
                << pulse_hit->timestamp1 << " "
                << pulse_hit->timestamp2 << " "
                << pulse_hit->pedestal_sum << " "
                << pulse_hit->pedestal_quality << " "
                << pulse_hit->integral_sum << " "
                << pulse_hit->integral_quality << " "
                << pulse_hit->coarse_time << " "
                << pulse_hit->fine_time << " "
                << pulse_hit->pulse_peak << "\n";
        
        // Fill histogram with pulse integral values
        m_pulse_integral_hist->Fill(pulse_hit->integral_sum);
    }
    
    // Add spacing between events for readability
    m_text_output_file << "\n\n";
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_MOLLER::Finish() {
    LOG << "JEventProcessor_MOLLER::Finish" << LOG_END;
    
    // Close the text output file
    if (m_text_output_file.is_open()) {
        m_text_output_file.close();
    }

    // Write ROOT objects and close ROOT file
    if (m_root_output_file) {
        m_waveform_tree->Write();        // Save waveform tree to file
        m_pulse_integral_hist->Write();  // Save integral histogram to file
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }
}

