//
// Created by Keith on 2020/1/26.
//

#pragma once


#include "store.h"
#include "runtime.h"

#define ARG_FORMAT_FOR_TEST

namespace TWA {

class Application {
public:
    Application();
    void reset();
    bool loadModule(const char *path);
    int callFunction(TypedValue &result, const char* name, int argc, const char* argv[]);
    int callFunction(TypedValue &result, const char* name, std::vector<Value> &params);

private:
    std::string getModuleName(std::string &path);

    void addModuleInstance(const char *name, Ptr<ModuleInstance> &&mi) {
        moduleIns_.push_back(mi);
        moduleIdx_[name] = moduleIns_.size() - 1;
    }

private:
    Runtime rt;
    std::vector<Ptr<ModuleInstance>> moduleIns_;
    std::map<std::string, U32> moduleIdx_;
};

}
