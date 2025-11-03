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
    
    // Open the ROOT output file
    m_root_output_file = new TFile(m_root_output_filename().c_str(), "RECREATE");
    if (m_root_output_file == nullptr || m_root_output_file->IsZombie()) {
        throw JException("Failed to open ROOT output file: " + m_root_output_filename());  
    }

    // Initialize the waveform tree row data structure
    m_waveform_tree_row = WaveformTreeRow();

    // Create ROOT tree for waveform data
    m_waveform_tree = new TTree("waveform_tree", "FADC250 Waveform Data (slot, channel, waveform)");
    m_waveform_tree->Branch("slot", &ev_slot);
    m_waveform_tree->Branch("chan", &ev_chan);
    m_waveform_tree->Branch("waveform", &ev_waveform);

    // Create histogram for pulse integral distribution
    m_pulse_integral_hist = new TH1I("h_integral", "Pulse Integral Distribution;Integral Sum;Counts", 100, 0, 1);
    m_pulse_integral_hist->SetCanExtend(TH1::kAllAxes);  // Allow ROOT to automatically extend bins
}

/**
 * @brief Process a single event sequentially
 * 
 * Processes FADC250 detector data for a single event. Fills ROOT tree with
 * waveform data and histogram with pulse integral values. This method is 
 * called for each event in the processing pipeline.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JEventProcessor_MOLLER::ProcessSequential(const JEvent &event) {
   
    ev_slot.clear();
    ev_chan.clear();
    ev_waveform.clear();

    // FADC250 waveform hits
    for (const auto& waveform_hit : m_waveform_hits_in()) {
        // Fill ROOT tree with waveform data
        m_waveform_tree_row.slot = waveform_hit->slot;
        m_waveform_tree_row.chan = waveform_hit->chan;
        m_waveform_tree_row.waveform = waveform_hit->waveform;

	ev_slot.push_back(m_waveform_tree_row.slot);
	ev_chan.push_back(m_waveform_tree_row.chan);
	ev_waveform.push_back(m_waveform_tree_row.waveform);
    }
    m_waveform_tree->Fill();

    // FADC250 pulse hits
    for (const auto& pulse_hit : m_pulse_hits_in()) {
        // Fill histogram with pulse integral values
        m_pulse_integral_hist->Fill(pulse_hit->integral_sum);
    }
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_MOLLER::Finish() {
    LOG << "JEventProcessor_MOLLER::Finish" << LOG_END;

    // Write ROOT objects and close ROOT file
    if (m_root_output_file) {
        m_waveform_tree->Write();        // Save waveform tree to file
        m_pulse_integral_hist->Write();  // Save integral histogram to file
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }
}

