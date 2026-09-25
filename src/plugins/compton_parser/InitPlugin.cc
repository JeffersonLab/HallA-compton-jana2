#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include <memory>

void RegisterComptonModuleParsers(JApplication* app);

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_parser");
    RegisterComptonModuleParsers(app);
}
