// function_helper.h
// Trevor Westphal

#ifndef FunctionHelper_h
#define FunctionHelper_h

#include <string>
#include <map>

#include "debug.hpp"
#include "profile.hpp"

class FunctionHelper;
typedef void (FunctionHelper::*function_thing)();

class FunctionHelper {
public:
    static FunctionHelper* getInstance();
    void runFunction(std::string function_name);
private:
    static FunctionHelper* instance;
    FunctionHelper();

    void testFunction();
    void exitProgram();
    void toggleShadows();
    void toggleNormals();
    void toggleLighting();
    void toggleParticles();
    void toggleFramebuffers();
    void toggleDepthmap();

    std::map<std::string, function_thing> lookup_table;
};

#endif
