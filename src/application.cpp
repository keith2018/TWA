//
// Created by Keith on 2020/1/26.
//

#include "application.h"
#include "decoder.h"
#include "instantiation.h"
#include "bind/spec_test_module.h"
#include "bind/wasi_module.h"


namespace TWA {

Application::Application() : rt(MAX_STACK_SIZE) {
    reset();
}

void Application::reset() {
    rt.store_ = Store();
    moduleIns_.clear();

    addModuleInstance("spectest", SpecTestModule::registerModule(rt));
    addModuleInstance("wasi_unstable", WASI::WASIModule::registerModule(rt));
}

bool Application::loadModule(const char *path) {
    std::string pathStr(path);
    std::string name = getModuleName(pathStr);
    if (moduleIdx_.find(name) != moduleIdx_.end()) {
        std::cout << "module loaded already, skip\n";
        return true;
    }

    Module module;
    module.name = name;

    // decode binary
    Decoder decoder(module);
    bool decodeOk = decoder.decodeFile(pathStr);
    if (!decodeOk) {
        std::cout << "decode module failed\n";
        return false;
    }

    // imports
    std::vector<ExternalValAddr> extVal;
    U32 idx = 0;
    bool importOk = true;
    while (idx < module.imports.size()) {
        SegImport &si = module.imports[idx];
        auto mIdx = moduleIdx_.find(si.module);
        if (mIdx == moduleIdx_.end()) {
            std::string importModuleFile = si.module + ".wasm";
            std::cout << "import module not loaded: " << si.module << ", try load from file: " << importModuleFile << "\n";
            bool loadOK = loadModule(importModuleFile.c_str());
            if (loadOK) {
                extVal.clear();
                idx = 0;
                continue;
            } else {
                std::cout << "import module not found: " << si.module << "\n";
                importOk = false;
            }
        }

        Ptr<ModuleInstance> &mi = moduleIns_[mIdx->second];
        auto ev = mi->exports.find(si.name);
        if (ev == mi->exports.end()) {
            std::cout << "import module component not exist: " << si.module << "::" << si.name << "\n";
            importOk = false;
        }

        if (importOk) {
            extVal.push_back(ev->second.extVal);
        }

        idx++;
    }

    if (!importOk) {
        std::cout << "imports not satisfied\n";
        return false;
    }

    // instantiate
    Ptr<ModuleInstance> moduleInst = Ptr<ModuleInstance>(new ModuleInstance());
    bool instantiateOk = Instantiation::instantiate(moduleInst, rt, module, extVal);
    if (!instantiateOk) {
        std::cout << "instantiate module failed\n";
        return false;
    }

    addModuleInstance(module.name.c_str(), std::move(moduleInst));
    return true;
}

int Application::callFunction(TypedValue &result, const char *name, int argc, const char **argv) {
    if (name == nullptr) {
        std::cout << "try call default 'main'\n";
        name = "main";
    }

    // find export function
    bool funcExist = false;
    Addr funcAddr;
    for (auto mi = moduleIns_.rbegin(); mi != moduleIns_.rend(); mi++) {
        auto ev = (*mi)->exports.find(name);
        if (ev != (*mi)->exports.end() && ev->second.extVal.type == ExtTypeFunc) {
            funcAddr = ev->second.extVal.addr;
            funcExist = true;
            break;
        }
    }

    if (!funcExist) {
        std::cout << "function not found: " << name << "\n";
        return -1;
    }

    // params
    Ptr<FuncInstance> &fi = rt.store().funcs[funcAddr];
    if (fi->type.params.size != argc) {
        std::cout << "arguments count invalid, expect " << fi->type.params.size << " but got " << argc << "\n";
        return -1;
    }

    std::vector<Value> params;
    for (U32 i = 0; i < argc; ++i){
        const char *str = argv[i];
        Value val;
        switch (fi->type.params.types[i]) {
#ifdef ARG_FORMAT_FOR_TEST
            case ValueTypeI32:  val.i32 = strtoul(str, NULL, 10);  break;
            case ValueTypeI64:  val.i64 = strtoull(str, NULL, 10); break;
            case ValueTypeF32:  val.i32 = strtoul(str, NULL, 10);  break;
            case ValueTypeF64:  val.i64 = strtoull(str, NULL, 10); break;
#else
            case ValueTypeI32:  val.i32 = atol(str);  break;
            case ValueTypeI64:  val.i64 = atoll(str); break;
            case ValueTypeF32:  val.f32 = atof(str);  break;
            case ValueTypeF64:  val.f64 = atof(str);  break;
#endif
            default:
                std::cout << "function param type invalid\n";
                break;
        }
        params.push_back(val);
    }

    rt.invoke(result, *fi, params);
    return rt.exitCode_;
}

int Application::callFunction(TypedValue &result, const char *name, std::vector<Value> &params) {
    if (name == nullptr) {
        std::cout << "try call default 'main'\n";
        name = "main";
    }

    // find export function
    bool funcExist = false;
    Addr funcAddr;
    for (auto mi = moduleIns_.rbegin(); mi != moduleIns_.rend(); mi++) {
        auto ev = (*mi)->exports.find(name);
        if (ev != (*mi)->exports.end() && ev->second.extVal.type == ExtTypeFunc) {
            funcAddr = ev->second.extVal.addr;
            funcExist = true;
            break;
        }
    }

    if (!funcExist) {
        std::cout << "function not found: " << name << "\n";
        return -1;
    }

    // params
    Ptr<FuncInstance> &fi = rt.store().funcs[funcAddr];
    if (fi->type.params.size != params.size()) {
        std::cout << "arguments count invalid, expect " << fi->type.params.size << " but got " << params.size() << "\n";
        return -1;
    }

    rt.invoke(result, *fi, params);
    return rt.exitCode_;
}

std::string Application::getModuleName(std::string &path) {
    int start = path.find_last_of('/');
    if (start < 0) {
        start = 0;
    }

    int end = path.find_last_of('.');
    if (end < start) {
        end = path.length() - 1;
    }

    return path.substr(start, end - start);
}

}