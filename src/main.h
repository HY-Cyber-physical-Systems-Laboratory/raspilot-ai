#ifndef __MAIN_H__
#define __MAIN_H__

#include "cxx/stdafx.hpp"

enum statisticsActionEnum {
    STATISTIC_NONE,
    STATISTIC_INIT,
    STATISTIC_PRINT,
    STATISTIC_MAX,
};

void mainExit(void *d);
void mainStandardShutdown(void *d);
int mainProcessCommandLineArgs(int argc, char **argv);

void shutdown();

#endif