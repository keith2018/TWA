//
// Created by Keith on 2020/1/26.
//


#include "spec_test_module.h"
#include "../native.h"


namespace TWA {


#define REG_PRINT_VALUE(nameType, ParamType, format)    \
REG_NATIVE_FUNCTION_BEGIN(print_##nameType) {           \
        Value val = rt.stack_.popValue();               \
        printf(format, val.nameType);                   \
        return true;                                    \
    }                                                   \
REG_NATIVE_FUNCTION_END(print_##nameType)               \


Ptr<ModuleInstance> SpecTestModule::registerModule(Runtime &rt) {
    Ptr<ModuleInstance> moduleInst = Ptr<ModuleInstance>(new ModuleInstance());
    std::vector<Ptr<FuncInstance>> &funcs = rt.store().funcs;

    // export functions

    // print
    REG_NATIVE_FUNCTION_BEGIN(print) {
            printf("WebAssembly\n");
            return true;
        }
    REG_NATIVE_FUNCTION_END(print)

    // print_i32
    REG_PRINT_VALUE(i32, I32, "%d\n")

    // print_i64
    REG_PRINT_VALUE(i64, I64, "%lld\n")

    // print_f32
    REG_PRINT_VALUE(f32, F32, "%f\n")

    // print_f64
    REG_PRINT_VALUE(f64, F64, "%f\n")

    // print_i32_f32
    REG_NATIVE_FUNCTION_BEGIN(print_i32_f32) {
            Value val1 = rt.stack_.popValue();
            Value val2 = rt.stack_.popValue();
            printf("%d %f\n", val1.i32, val2.f32);
            return true;
        }
    REG_NATIVE_FUNCTION_END(print_i32_f32)

    // print_i64_f64
    REG_NATIVE_FUNCTION_BEGIN(print_i64_f64) {
            Value val1 = rt.stack_.popValue();
            Value val2 = rt.stack_.popValue();
            printf("%lld %f\n", val1.i64, val2.f64);
            return true;
        }
    REG_NATIVE_FUNCTION_END(print_i64_f64)

    // print_f64_f64
    REG_NATIVE_FUNCTION_BEGIN(print_f64_f64) {
            Value val1 = rt.stack_.popValue();
            Value val2 = rt.stack_.popValue();
            printf("%f %f\n", val1.f64, val2.f64);
            return true;
        }
    REG_NATIVE_FUNCTION_END(print_f64_f64)

    // export tables
    TableType tt;
    tt.elemType = ValueTypeFuncRef;
    tt.limit.min = 10;
    tt.limit.max = 0;

    std::vector<Ptr<TableInstance>> &tables = rt.store().tables;
    tables.push_back(Ptr<TableInstance>(new TableInstance(tt)));
    moduleInst->tableAddrs.emplace_back(tables.size() - 1);
    moduleInst->exports["table"] = ExportInstance("table", ExtTypeTable, moduleInst->tableAddrs.back());

    return moduleInst;
}

}