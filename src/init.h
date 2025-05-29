#ifndef __INIT_H__
#define __INIT_H__

#include "cxx/stdafx.hpp"

#include "main.h"
#include "handler.h"


ANOTATION_LOGIC("INIT", "INIT", {
    /*
        This module is used to initialize the main program.
        It is used to initialize the main program and its components.
    */
})

void initTask();

void initConfiguredPilot();

int raspilotInit(int argc, char **argv);


#endif