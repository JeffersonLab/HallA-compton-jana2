#include <JANA/JApplication.h>
#include <JANA/JService.h>
#include <memory>
#include "JEventService_ModuleParsersMap.h"

// Module parsers
#include "ModuleParser_faV3compton.h"

class JEventService_ComptonModuleParsers final : public JService {
public:
    Service<JEventService_ModuleParsersMap> parsers {this};

    void Init() override {
        add(253, std::make_shared<ModuleParser_faV3compton>());
    }

private:
    void add(int id, std::shared_ptr<ModuleParser> parser) {
        parsers->addParser(id, std::move(parser));
    }
};

void RegisterComptonModuleParsers(JApplication* app) {
    app->ProvideService(std::make_shared<JEventService_ComptonModuleParsers>());
}
