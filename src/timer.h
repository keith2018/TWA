//
// Created by Keith on 2020/1/14.
//

#pragma once

#include <iostream>
#include <ctime>

namespace TWA {
namespace Time {

static clock_t timer_start;

#define TIMER_BEGIN \
    TWA::Time::timer_start = std::clock();

#define TIMER_END \
    std::cout << "exec cost: " << (double)(std::clock() - TWA::Time::timer_start) / (CLOCKS_PER_SEC / 1000) << " ms\n";

}
}
