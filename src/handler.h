#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "cxx/stdafx.hpp"


ANOTATION_LOGIC("HANDLER", "HANDLER", {
    /*
        This module is used to handle signals and interrupts.
        It is used to handle signals and interrupts in the main loop.

        ZOMBIE_HANDLER is used to handle zombie processes.
        SIGPIPE_HANDLER is used to handle SIGPIPE signals.
        PILOT_INTERRUPT_HANDLER is used to handle pilot interrupts.
    */
})

void zombieHandler(int s);

void sigPipeHandler(int s);

void pilotInterruptHandler(int sig);


#endif