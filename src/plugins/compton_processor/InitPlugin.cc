#include <JANA/JApplication.h>
#include "JEventProcessor_compton.h"

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);
        app->AddPlugin("compton_parser");
        app->Add(new JEventProcessor_compton());
    }
}
