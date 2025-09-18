
#include "JEventProcessor_MOLLER.h"
#include <JANA/JLogger.h>

JEventProcessor_MOLLER::JEventProcessor_MOLLER() {
    SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name
    SetPrefix("jeventprocessor_moller");
    SetCallbackStyle(CallbackStyle::ExpertMode);
}

void JEventProcessor_MOLLER::Init() {
    LOG << "JEventProcessor_MOLLER::Init" << LOG_END;
    // Open TFiles, set up TTree branches, etc
    m_outfile.open("output.txt", std::ios::out);
     if (!m_outfile.is_open()) {
        throw JException("Error opening output.txt!");
    }
}

void JEventProcessor_MOLLER::ProcessSequential(const JEvent &event) {
   // Do anything in sequential
        m_outfile <<"Event: "<< event.GetEventNumber()<<"\n";
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
        m_outfile << "\n\n";
   // want to do anything in parallel use ProcessParallel instead
}

void JEventProcessor_MOLLER::Finish() {
    // Close any resources
    if (m_outfile.is_open()) {
        m_outfile.close();
    }
    LOG << "JEventProcessor_MOLLER::Finish" << LOG_END;
}

