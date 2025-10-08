

#ifndef _JEventSource_EVIO_h_
#define  _JEventSource_EVIO_h_

#include <JANA/JEventSource.h>
#include <JANA/JEventSourceGeneratorT.h>

#include <fstream>
#include <filesystem>

#include "eviocc.h"

/**
 * @class JEventSource_EVIO
 * @brief JANA2 event source for reading EVIO format data files
 * 
 * This class handles the reading of EVIO files and creation of events
 * for the MOLLER experiment data processing pipeline.
 */
class JEventSource_EVIO : public JEventSource {

private:
    std::unique_ptr<evio::EvioReader> m_evio_reader;  ///< EVIO file reader instance
    int m_run_number = 0;                             ///< Current run number

public:
    JEventSource_EVIO();
    virtual ~JEventSource_EVIO() = default;
    
    void Open() override;
    void Close() override;
    void ProcessParallel(JEvent& event) const override;
    Result Emit(JEvent& event) override;
    static std::string GetDescription();

};

template <>
double JEventSourceGeneratorT<JEventSource_EVIO>::CheckOpenable(std::string);

#endif // _JEventSource_EVIO_h_

