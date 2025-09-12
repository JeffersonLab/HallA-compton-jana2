
#include "JFactory_FADC250.h"

#include <JANA/JEvent.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Please add the following lines to your InitPlugin or similar routine
// in order to register this factory with the system.
//
// #include "JFactory_FADC250.h"
//
//     app->Add( new JFactoryGeneratorT<JFactory_FADC250>() );
//- - - - - - - - - - - - - - - - - - - - - - - - - -JFactory_FADC250

JFactory_FADC250::JFactory_FADC250() {


    // Set the type name of this class, in order to have more informative error messages
    SetTypeName(NAME_OF_THIS);

    // Set the default prefix. This is a unique name for this factory that is used as a prefix for parameters.
    // In ePIC, this is usually overridden by the JOmniFactoryGenerator
    SetPrefix("fadc250_factory");

    // If you declare an input as optional, JANA2 won't throw an exception if it can't find it. When you try to access
    // the missing input, it will present as an empty vector.
    //   m_hits_in.SetOptional(true);

    // Set factory flags here, as needed.
    //   m_hits_out.SetNotOwnerFlag(true);
    // Note that when there are multiple outputs, the NOT_OBJECT_OWNER flag should be set on the Output, not on the factory itself.
    // Setting NOT_OBJECT_OWNER on a factory really means setting NOT_OBJECT_OWNER on the factory's _fiJFactory_FADC250d JFactory_FADC250::Init() {
}

void JFactory_FADC250::Init() {
    // Init() is called sequentially before any processing starts. It is safe to modify state
    // that is not a member variable of the factory itself.

    // By the time Init() is called,
    // - Logger has been configured
    // - Declared Parameter values have been fetched
    // - Declared Services have been fetched

    LOG_DEBUG(GetLogger()) << "Inside Init()";

    // Unlike v1, you don't have to fetch any parameters or services in here, as this is done automatically now.
    // However, you may wish to fetch data from your JServices, if it isn't keyed off of the run number.
    // This is also where you should initialize your algorithm, if necessary.
}

void JFactory_FADC250::ChangeRun(const JEvent& event) {
    LOG_DEBUG(GetLogger()) << "Inside ChangeRun() with run_number=" << event.GetRunNumber();

    // This is where you should fetch any data from your JServices that IS keyed off of the run number
}

void JFactory_FADC250::Process(const JEvent& event) {
    LOG_DEBUG(GetLogger()) << "Inside Execute() with run_number=" << event.GetRunNumber()
                          << ", event_number=" << event.GetEventNumber();

    // The Input helpers will already have been filled by the time Execute() gets called. You can access
    // the data using the () operator. Parameter values may also be accessed using the () operator.
    std::shared_ptr<evio::EvioEvent> evio_event = m_hits_in().at(0)->evio_event;
    std::vector<std::shared_ptr<evio::BaseStructure>> &children = evio_event->getChildren();
    std::shared_ptr<evio::BaseStructure> &second_child = children[1];
    std::vector<std::shared_ptr<evio::BaseStructure>> &second_subchilds = second_child->getChildren();
    uint32_t chan, q, t;
    std::vector<FADC250*> fadc_out_hits;

    for(auto subchild: second_subchilds) {
        if(subchild->getHeader()->getDataType() == evio::DataType::UINT32) {
            std::vector<uint32_t> data_points = subchild->getUIntData();
            for(auto data: data_points) {
                chan = (data>>13) & 0x000F;
                q    = (data>>0 ) & 0x1FFF;
                t    = ((data>>17) & 0x3FFF)*4;
                std::cout<<"\t chan: "<<chan<<", q: "<<q<<", t: "<<t<<std::endl;

                FADC250* hit = new FADC250(chan, q, t);
                fadc_out_hits.push_back(hit);  
            }
        }
    }

    // While you are inside Execute(), you can populate your output databundles however you like. Once Execute()
    // returns, JANA2 will store and retrieve them automatically.
     m_hits_out() = fadc_out_hits;
}

void JFactory_FADC250::Finish() {
    LOG_DEBUG(GetLogger()) << "Inside Finish()";
}
