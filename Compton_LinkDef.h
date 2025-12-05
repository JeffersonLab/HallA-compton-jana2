#ifndef LINKDEF_H
#define LINKDEF_H

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::vector<unsigned int>+;
#pragma link C++ class std::vector<std::vector<unsigned int> >+;

#endif
