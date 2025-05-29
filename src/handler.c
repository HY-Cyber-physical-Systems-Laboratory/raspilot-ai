#include "handler.h"

void sigPipeHandler(int s) {
    // TODO: Do some more informal message. Implement something to detect closed pipe
    // and printing some informative message which device crashed.
    lprintf(0, "%s: Warning: SIGPIPE received!\n", PPREFIX());
    lprintf(0, "%s: Core dumped for later debug.\n", PPREFIX()); CORE_DUMP();
}

void pilotInterruptHandler(int sig) {
    static unsigned fatal_error_in_progress = 0;
    char	*ss;

    if (fatal_error_in_progress ++) {
        raise(sig);
        exit(-1);
    }
    
    // try to print info message
    ss = signalInterruptNames[sig] == NULL ? "" : signalInterruptNames[sig];
    
    printf("%s: FATAL ERROR: Signal %s (%d) received. Aborting!\n", PPREFIX(), signalInterruptNames[sig], sig);
    fflush(stdout);

    // TODO: Remove this exception for production, i.e. do safe landing on interrupt
    if (sig == SIGINT || sig == SIGTERM) {
	    // user interrupted (probably during debug), just stop motors and shutdown pilot
	    mainStandardShutdown(NULL);
    } else {
        // unexpected signal (like segmentation fault, floating point exception ...) probably bug.
        // send motors safe-land command and core dump
        motorsEmmergencyLand();
        terminalResume();
        // strangely, abort corrupts my stack.
        signal(SIGSEGV, SIG_DFL);
        DO_SIGSEGV();    
    }

}

__forceinline void zombieHandler(int s) {
    waitpid((pid_t)(-1), 0, WNOHANG);
}
