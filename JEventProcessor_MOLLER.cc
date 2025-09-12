
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
    m_output_file.open("output.txt", std::ios::out);
     if (!m_output_file.is_open()) {
        throw JException("Error opening output.txt!");
    }
}

void JEventProcessor_MOLLER::ProcessSequential(const JEvent &event) {
   // Do anything in sequential
        m_output_file<<"Event: "<< event.GetEventNumber()<<"\n";
        for(auto hit: m_hits_in()) {
            m_output_file << "chan: "<<hit->chan<<", q: "<<hit->q<<", t: "<<hit->t<<"\n";
        }
        m_output_file << "\n\n";
   // want to do anything in parallel use ProcessParallel instead
}

void JEventProcessor_MOLLER::Finish() {
    // Close any resources
    if (m_output_file.is_open()) {
        m_output_file.close();
    }
    LOG << "JEventProcessor_MOLLER::Finish" << LOG_END;
}

