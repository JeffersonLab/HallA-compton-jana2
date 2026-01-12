#include "JEventProcessor_Compton.h"
#include <JANA/JLogger.h>

/**
 * @brief Constructor for JEventProcessor_Compton
 * 
 * Initialize the processor with the appropriate type name, prefix, and callback style.
 */
JEventProcessor_Compton::JEventProcessor_Compton() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with this class's name
    SetPrefix("jeventprocessor_compton");          // Set unique prefix for parameters
    SetCallbackStyle(CallbackStyle::ExpertMode);  // Use expert mode for full control

    // All of these are optional because not all events will have these hits
    m_fadc_scaler_hits_in.SetOptional(true);
    m_pulse_hits_in.SetOptional(true);
    m_waveform_hits_in.SetOptional(true);
    m_ti_scaler_hits_in.SetOptional(true);
}

/**
 * @brief Initialize the processor
 * 
 * Called once at the start of processing. Open the output files and set up
 * any necessary resources for event processing.
 */
void JEventProcessor_Compton::Init() {
    LOG << "JEventProcessor_Compton::Init" << LOG_END;
    
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

    // Optionally: Text output file for human-readable hit summaries
    m_txt_output_file.open(m_txt_output_filename().c_str());

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
void JEventProcessor_Compton::ProcessSequential(const JEvent &event) {
   
    ev_slot.clear();
    ev_chan.clear();
    ev_waveform.clear();

    // FADC250 waveform hits
    for (const auto& waveform_hit : m_waveform_hits_in()) {
        // Fill ROOT tree with waveform data
        m_waveform_tree_row.slot = waveform_hit->slot;
        m_waveform_tree_row.chan = waveform_hit->chan;
        m_waveform_tree_row.waveform = waveform_hit->waveform;

	size_t waveform_sample_number = m_waveform_tree_row.waveform.size();

	ev_slot.insert(ev_slot.end(), waveform_sample_number, m_waveform_tree_row.slot);
	ev_chan.insert(ev_chan.end(), waveform_sample_number, m_waveform_tree_row.chan);
	ev_waveform.insert(ev_waveform.end(),  m_waveform_tree_row.waveform.begin(), m_waveform_tree_row.waveform.end());
    }
    m_waveform_tree->Fill();

    // FADC250 pulse hits
    for (const auto& pulse_hit : m_pulse_hits_in()) {
        // Fill histogram with pulse integral values
        m_pulse_integral_hist->Fill(pulse_hit->integral_sum);
    }

    // ------------------------------------------------------------------
    // Optional text dump of hits for this event (waveforms, pulses, scalers)
    // ------------------------------------------------------------------
    if (m_txt_output_file.is_open()) {
        const auto& waveform_hits      = m_waveform_hits_in();
        const auto& pulse_hits         = m_pulse_hits_in();
        const auto& fadc_scaler_hits   = m_fadc_scaler_hits_in();
        const auto& ti_scaler_hits     = m_ti_scaler_hits_in();

        bool have_waveforms       = !waveform_hits.empty();
        bool have_pulses          = !pulse_hits.empty();
        bool have_fadc_scalers    = !fadc_scaler_hits.empty();
        bool have_ti_scalers      = !ti_scaler_hits.empty();

        // Only write anything if we have at least one type of hit
        if (have_waveforms || have_pulses || have_fadc_scalers || have_ti_scalers) {
            auto event_number = event.GetEventNumber();

            m_txt_output_file << "Event " << event_number << "\n";

            // Waveform summary
            if (have_waveforms) {
                m_txt_output_file << "  Waveform hits: " << waveform_hits.size() << "\n";
                for (const auto& hit : waveform_hits) {
                    m_txt_output_file
                        << "    WF slot=" << hit->slot
                        << " chan=" << hit->chan
                        << " nsamples=" << hit->waveform.size()
                        << "\n";
                }
            } else {
                m_txt_output_file << "  No FADC250 waveform hits in this event\n";
            }

            // Pulse summary
            if (have_pulses) {
                m_txt_output_file << "  Pulse hits: " << pulse_hits.size() << "\n";
                for (const auto& hit : pulse_hits) {
                    m_txt_output_file
                        << "    PULSE slot=" << hit->slot
                        << " chan=" << hit->chan
                        << " integral_sum=" << hit->integral_sum
                        << "\n";
                }
            } else {
                m_txt_output_file << "  No FADC250 pulse hits in this event\n";
            }

            // FADC scaler summary
            if (have_fadc_scalers) {
                m_txt_output_file << "  FADC scaler hits: " << fadc_scaler_hits.size() << "\n";
                for (const auto& hit : fadc_scaler_hits) {
                    m_txt_output_file
                        << "    SCALER rocid=" << hit->rocid
                        << " slot=" << hit->slot
                        << " ncounts=" << hit->ncounts
                        << " counts=";
                    for (uint32_t i = 0; i < hit->ncounts && i < 16u; ++i) {
                        m_txt_output_file << hit->counts[i];
                        if (i + 1 < hit->ncounts && i + 1 < 16u) {
                            m_txt_output_file << ",";
                        }
                    }
                    m_txt_output_file << "\n";
                }
            } else {
                m_txt_output_file << "  No FADCScalerHit objects in this event\n";
            }

            // TI scaler summary
            if (have_ti_scalers) {
                m_txt_output_file << "  TI scaler hits: " << ti_scaler_hits.size() << "\n";
                for (const auto& hit : ti_scaler_hits) {
                    m_txt_output_file
                        << "    TISCALER rocid=" << hit->rocid
                        << " slot=" << hit->slot
                        << " nwords=" << hit->nscalerwords
                        << " live_time=" << hit->live_time
                        << " busy_time=" << hit->busy_time
                        << " ts_inputs_before_busy=" << hit->ts_inputs_before_busy
                        << "\n";
                }
            } else {
                m_txt_output_file << "  No TIScalerHit objects in this event\n";
            }

            m_txt_output_file << "\n";
        }
    }
}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_Compton::Finish() {
    LOG << "JEventProcessor_Compton::Finish" << LOG_END;

    // Write ROOT objects and close ROOT file
    if (m_root_output_file) {
        m_waveform_tree->Write();        // Save waveform tree to file
        m_pulse_integral_hist->Write();  // Save integral histogram to file
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }

    // Close text output file if open
    if (m_txt_output_file.is_open()) {
        m_txt_output_file.close();
    }
}

