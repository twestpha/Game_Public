// function_helper.cpp
// Trevor Westphal

#include "function_helper.hpp"

FunctionHelper* FunctionHelper::instance;

FunctionHelper* FunctionHelper::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new FunctionHelper();
        return instance;
    }
}

FunctionHelper::FunctionHelper(){
    lookup_table["testFunction"] = &FunctionHelper::testFunction;
    lookup_table["exitProgram"] = &FunctionHelper::exitProgram;
    lookup_table["toggleShadows"] = &FunctionHelper::toggleShadows;
    lookup_table["toggleNormals"] = &FunctionHelper::toggleNormals;
    lookup_table["toggleLighting"] = &FunctionHelper::toggleLighting;
    lookup_table["toggleParticles"] = &FunctionHelper::toggleParticles;
    lookup_table["toggleFramebuffers"] = &FunctionHelper::toggleFramebuffers;
    lookup_table["toggleDepthmap"] = &FunctionHelper::toggleDepthmap;

}

void FunctionHelper::runFunction(std::string function_name){
    std::map<std::string, function_thing>::iterator result = lookup_table.find(function_name);

    if(result != lookup_table.end()){
        auto mem = result->second;  //C++11 only
        (this->*mem)();
    } else {
        Debug::error("Could not run requested function: %s\n", function_name.c_str());
    }
}

void FunctionHelper::testFunction(){
    Profile::getInstance()->toggleVsync();
}

void FunctionHelper::toggleShadows(){
    Profile::getInstance()->toggleShadows();
}

void FunctionHelper::toggleNormals(){
    Profile::getInstance()->toggleNormals();
}

void FunctionHelper::toggleLighting(){
    Profile::getInstance()->toggleLighting();
}

void FunctionHelper::toggleParticles(){
    Profile::getInstance()->toggleParticles();
}

void FunctionHelper::toggleFramebuffers(){
    Profile::getInstance()->toggleFramebuffers();
}

void FunctionHelper::toggleDepthmap(){
    Profile::getInstance()->toggleDepthmap();
}

void FunctionHelper::exitProgram(){
    Window::getInstance()->requestClose();
}
