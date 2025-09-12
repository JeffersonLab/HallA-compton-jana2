
#ifndef _JFactory_FADC250_h_
#define _JFactory_FADC250_h_

#include <JANA/Components/JOmniFactory.h>

#include "FADC250.h"
#include "EvioEventWrapper.h"

class JFactory_FADC250 : public JFactory {

    Input<EvioEventWrapper> m_hits_in {this};
    Output<FADC250> m_hits_out{this};

public:
    JFactory_FADC250();
    
    void Init() override;
    void ChangeRun(const JEvent& event) override;
    void Process(const JEvent& event) override;
    void Finish() override;
};

#endif // _JFactory_FADC250_h_
