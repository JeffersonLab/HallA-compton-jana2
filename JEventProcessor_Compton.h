
#ifndef _JEventProcessor_Compton_h_
#define _JEventProcessor_Compton_h_

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <JANA/JEventProcessor.h>
#include "EventHits.h"

/**
 * @struct WaveformTreeRow
 * @brief Data structure representing one row in the waveform ROOT TTree
 * 
 * Contains the waveform hit information to be stored in ROOT Tree:
 * - slot: FADC250 slot number
 * - chan: Channel number within the slot
 * - waveform: Vector of ADC sample values
 */
struct WaveformTreeRow {
    uint32_t slot;
    uint32_t chan;
    std::vector<uint32_t> waveform;
};

/**
 * @class JEventProcessor_Compton
 * @brief Main event processor for FADC250 detector data analysis
 * 
 * This processor receives FADC250 detector hits (both waveform and pulse hits)
 * and outputs the data to a ROOT file containing:
 * - Waveform TTree: Channel-by-channel waveform data
 * - Pulse integral histogram: Distribution of pulse integral sums
 * 
 * The output filename can be customized via JANA2 parameters.
 */
class JEventProcessor_Compton : public JEventProcessor {

private:
    // Declare Inputs
    Input<FADC250WaveformHit> m_waveform_hits_in {this}; 
    Input<FADC250PulseHit> m_pulse_hits_in {this};

    /**
     * @brief ROOT output filename parameter
     * 
     * This parameter allows users to specify the ROOT output filename via JANA2 configuration.
     * The parameter constructor takes the following arguments:
     * - owner: Pointer to this component (for parameter registration)
     * - name: "ROOT_OUT_FILENAME" - the parameter name used in configuration files/command line
     * - default_value: "compton.root" - default filename if not specified
     * - description: "Output file name for root data" - help text for the parameter
     * - is_shared: if true, the parameter name is used as-is;  
     *              if false (default), the component's prefix (set in the constructor) is prepended to the name.
     */
    Parameter<std::string> m_root_output_filename {this, "ROOT_OUT_FILENAME", "compton.root", "Output file name for ROOT data", true};

    // ROOT Tree variables 
    std::vector<uint32_t> ev_slot;
    std::vector<uint32_t> ev_chan;
    std::vector<uint32_t> ev_waveform;


    // ROOT output objects
    TFile *m_root_output_file;                ///< ROOT file for histogram and tree storage
    WaveformTreeRow m_waveform_tree_row;      ///< Data structure holding the current row for TTree filling
    TTree *m_waveform_tree;                   ///< ROOT tree for waveform data
    TTree *m_tree;                            ///< ROOT tree for physics event
    TH1I *m_pulse_integral_hist;              ///< Histogram of pulse integral sums

public:

    JEventProcessor_Compton();
    virtual ~JEventProcessor_Compton() = default;

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};

#endif // _JEventProcessor_Compton_h_

