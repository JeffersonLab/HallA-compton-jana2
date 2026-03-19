
#include <JANA/JApplication.h>           // Core JANA2 application framework
#include <JANA/JEventSourceGeneratorT.h>     // Event source generator template
#include <JANA/JEventProcessor.h>            // Event processor

// Experiment specific components
#include "JEventSource_EVIO.h"              // EVIO file event source
#include "JEventUnfolder_EVIO.h"            // Event unfolder
#include "JEventService_FilterDB.h"         // Service for ROC/bank filtering
#include "JEventService_BankToModuleMap.h"   // Service for mapping bank IDs to module IDs
#include "JEventService_ModuleParsersMap.h"  // Service for mapping module IDs to parser implementations

// Module parsers
#include "ModuleParser_FADC.h"               // FADC250 module parser implementation
#include "ModuleParser_FADCScaler.h"         // FADC scaler module parser implementation
#include "ModuleParser_TIScaler.h"           // TI scaler module parser implementation
#include "ModuleParser_HelicityDecoder.h"    // Helicity decoder module parser implementation
#include "ModuleParser_MPD.h"                // MPD module parser implementation
#include "ModuleParser_VFTDC.h"             // VFTDC module parser implementation

// Satisfies the topology requirement for an unfolder: at least one processor
// must exist downstream so JANA can wire a complete source→unfolder→processor
// chain. This processor does nothing; any real processor plugin loaded alongside
// evio_source will take precedence and this one simply passes through.
struct JEventProcessor_Passthrough : public JEventProcessor {
    JEventProcessor_Passthrough() { SetTypeName("JEventProcessor_Passthrough"); }
    void ProcessSequential(const JEvent&) override {}
};

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);

        // Register all plugin components
        app->Add(new JEventSourceGeneratorT<JEventSource_EVIO>());
        app->Add(new JEventUnfolder_EVIO());
        app->Add(new JEventProcessor_Passthrough());

        // Register services
        auto filter_svc       = std::make_shared<JEventService_FilterDB>();
        auto bank_to_module_svc = std::make_shared<JEventService_BankToModuleMap>();
        auto module_parsers_svc = std::make_shared<JEventService_ModuleParsersMap>();

        app->ProvideService(filter_svc);
        app->ProvideService(bank_to_module_svc);
        app->ProvideService(module_parsers_svc);

        // Register all parsers directly here with module parsers map
        module_parsers_svc->addParser(250,   new ModuleParser_FADC());
        module_parsers_svc->addParser(9250,  new ModuleParser_FADCScaler());
        module_parsers_svc->addParser(9001,  new ModuleParser_TIScaler());
        module_parsers_svc->addParser(0xdec, new ModuleParser_HelicityDecoder());
        module_parsers_svc->addParser(3561,    new ModuleParser_MPD());
        module_parsers_svc->addParser(9,     new ModuleParser_VFTDC());
    }
}