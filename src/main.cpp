/*
 * @Author: Keith 
 * @Date: 2019-12-18 15:19:27 
 * @Last Modified by: Keith
 * @Last Modified time: 2019-12-18 15:20:47
 */

#include <iostream>
#include "application.h"
#include "timer.h"

void printResults(TWA::TypedValue &ret) {
    printf("Result: ");

    switch (ret.type) {
#ifdef ARG_FORMAT_FOR_TEST
        case TWA::ValueTypeI32:  printf("%u", ret.val.i32);     break;
        case TWA::ValueTypeI64:  printf("%llu", ret.val.i64);   break;
        case TWA::ValueTypeF32:  printf("%u", ret.val.i32);     break;
        case TWA::ValueTypeF64:  printf("%llu", ret.val.i64);   break;
#else
        case TWA::ValueTypeI32:  printf("%u", ret.val.i32);     break;
        case TWA::ValueTypeI64:  printf("%llu", ret.val.i64);   break;
        case TWA::ValueTypeF32:  printf("%f", ret.val.f32);     break;
        case TWA::ValueTypeF64:  printf("%lf", ret.val.f64);    break;
#endif
        default:
            std::cout << "result type invalid\n";
            break;
    }

    printf("\n");
}

void printVersion() {
    printf("TWA v" TWA_VERSION " (" __DATE__ " " __TIME__ ", " __VERSION__ ")\n");
}

void printUsage() {
    printf("Usage:\n");
    printf("  twa version\n");
    printf("  twa run <file>\n");
    printf("  twa run <file> --func <function> [args...]\n");
    printf("  twa cmd\n");
}

int run(const char* file, const char* func, int argc, const char* argv[]) {
    bool runOk = false;
    TWA::Application app;
    runOk = app.loadModule(file);
    if (!runOk) {
        printf("loadModule failed\n");
        return -1;
    }

    TWA::TypedValue result;
    int exitCode = app.callFunction(result, func, argc, argv);
    if (exitCode != 0) {
        printf("call function failed\n");
        return exitCode;
    }

    if (result.type == TWA::ValueTypeVoid) {
        printf("Result: <Empty Stack>\n");
    } else {
        printResults(result);
    }

    return 0;
}

int splitCmds(char *str, char** argv) {
    int result = 0;
    char *curr = str;
    int len = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (strchr(" \n\r\t", str[i])) {
            if (len) {
                str[i] = '\0';
                argv[result++] = curr;
                len = 0;
            }
        } else {
            if (!len) {
                curr = &str[i];
            }
            len++;
        }
    }
    argv[result] = nullptr;
    return result;
}

void unescape(char* buff) {
    char* outp = buff;
    while (*buff) {
        if (*buff == '\\') {
            switch (*(buff+1)) {
                case '0':  *outp++ = '\0'; break;
                case 'b':  *outp++ = '\b'; break;
                case 'n':  *outp++ = '\n'; break;
                case 'r':  *outp++ = '\r'; break;
                case 't':  *outp++ = '\t'; break;
                case 'x': {
                    char hex[3] = { *(buff+2), *(buff+3), '\0' };
                    *outp++ = strtol(hex, NULL, 16);
                    buff += 2;
                    break;
                }

                default: *outp++ = *(buff+1); break;
            }
            buff += 2;
        } else {
            *outp++ = *buff++;
        }
    }
    *outp = '\0';
}

#define NEXT_ARG \
    argc--;      \
    argv++;      \


int main(int argc, char *argv[]) {
    int exitCode = 0;

    NEXT_ARG
    if (argc <= 0) {
        goto badArgs;
    } else if (!strcmp(*argv, "version")) {
        printVersion();
        goto exit;
    } else if (!strcmp(*argv, "run")) {
        // run mode
        NEXT_ARG
        if (argc <= 0) {
            goto badArgs;
        }

        const char *file = *argv;
        const char *func = nullptr;
        NEXT_ARG
        if (argc >= 1 && !strcmp(*argv, "--func")) {
            NEXT_ARG
            if (argc >= 1) {
                func = *argv;
                NEXT_ARG
            }
        }

        TIMER_BEGIN
        exitCode = run(file, func, argc, (const char **) argv);
        TIMER_END
        goto exit;
    } else if (!strcmp(*argv, "cmd")) {
        TWA::Application app;
        while(true) {
            char cmdBuff[1024] = {0,};
            char *cmdList[32] = {0,};
            printf("twa> ");
            fflush(stdout);
            if (!fgets(cmdBuff, sizeof(cmdBuff), stdin)) {
                goto exit;
            }
            int cmdSize = splitCmds(cmdBuff, cmdList);
            if (cmdSize <= 0) {
                continue;
            }

            if (!strcmp(cmdList[0], ":init")) {
                app.reset();
            } else if (!strcmp(cmdList[0], ":version")) {
                printVersion();
            } else if (!strcmp(cmdList[0], ":exit")) {
                goto exit;
            } else if (!strcmp(cmdList[0], ":load")) {
                app.loadModule(cmdList[1]);
            } else if (cmdList[0][0] == ':') {
                printf("no such command");
            } else {
                unescape(cmdList[0]);
                TWA::TypedValue result;
                exitCode = app.callFunction(result, cmdList[0], cmdSize - 1, (const char **) (cmdList + 1));
                if (exitCode == 0) {
                    if (result.type == TWA::ValueTypeVoid) {
                        printf("Result: <Empty Stack>\n");
                    } else {
                        printResults(result);
                    }
                }
            }
        }
    } else {
        goto badArgs;
    }

badArgs:
    printUsage();

exit:
    return exitCode;
}
