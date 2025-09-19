
#ifndef _JEventProcessor_MOLLER_h_
#define _JEventProcessor_MOLLER_h_

#include <fstream>

#include <JANA/JEventProcessor.h>
#include "EventHits.h"

/**
 * @class JEventProcessor_MOLLER
 * @brief Main event processor for FADC250 detector data analysis
 * 
 * This processor receives FADC250 detector hits (both waveform and pulse hits)
 * and outputs detailed information about each hit to a configurable text file.
 * The output file name can be customized via JANA2 parameters.
 */
class JEventProcessor_MOLLER : public JEventProcessor {

private:
    // Declare Inputs
    Input<FADC250WaveformHit> m_waveform_hits_in {this}; 
    Input<FADC250PulseHit> m_pulse_hits_in {this};

    /**
     * @brief Output filename parameter
     * 
     * This parameter allows users to specify the output filename via JANA2 configuration.
     * The parameter constructor takes the following arguments:
     * - owner: Pointer to this component (for parameter registration)
     * - name: "OUTPUT_FILENAME" - the parameter name used in configuration files/command line
     * - default_value: "output.txt" - default filename if not specified
     * - description: "Output file name" - help text for the parameter
     * - is_shared: if true, the parameter name is used as-is;  
     *              if false, the component's prefix (set in the constructor) is prepended to the name.
     * 
     */
    Parameter<std::string> m_out_filename {this, "OUTPUT_FILENAME", "output.txt", "Output file name", true};

    // Output file stream
    std::ofstream m_outfile;

public:

    JEventProcessor_MOLLER();
    virtual ~JEventProcessor_MOLLER() = default;

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};

#endif // _JEventProcessor_MOLLER_h_

