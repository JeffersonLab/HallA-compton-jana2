

#ifndef _JEventSource_EVIO_h_
#define  _JEventSource_EVIO_h_

#include <JANA/JEventSource.h>
#include <JANA/JEventSourceGeneratorT.h>

#include <fstream>
#include <filesystem>

#include "eviocc.h"


class JEventSource_EVIO : public JEventSource {

    /// Add member variables here
    std::unique_ptr<evio::EvioReader> m_evio_reader;
    int m_event_num = 0;

public:
    JEventSource_EVIO();
    virtual ~JEventSource_EVIO() = default;
    
    void Open() override;
    void Close() override;
    Result Emit(JEvent& event) override;
    static std::string GetDescription();

};

template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string);

#endif // _JEventSource_EVIO_h_

