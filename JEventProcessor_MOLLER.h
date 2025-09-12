
#ifndef _JEventProcessor_MOLLER_h_
#define _JEventProcessor_MOLLER_h_

#include <fstream>

#include <JANA/JEventProcessor.h>
#include "FADC250.h"

class JEventProcessor_MOLLER : public JEventProcessor {

    Input<FADC250> m_hits_in {this};
    std::ofstream m_output_file;

public:

    JEventProcessor_MOLLER();
    virtual ~JEventProcessor_MOLLER() = default;

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};


#endif // _JEventProcessor_MOLLER_h_

