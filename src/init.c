#include "init.h"


void initTask() {
    int 	i, r;
    cpu_set_t 	set;

    // the very first initializations of the system

    memset(uu, 0, sizeof(*uu));
    uu->flyStage = FS_START;
    enumNamesInit();
    setCurrentTime();
    uu->pilotStartingTime = currentTime.dtime;

    // pre-init vector length konstants. Some values depend on configuration, so it will be "reinitialized"
    // from config.c one more time.
    mainInitDeviceDataStreamVectorLengths(0);

    baioLibraryInit(0);
    strtodninit() ;
    // Hardware i2c may be shared between processes
    pi2cInit("/dev/i2c-1", 0);
    
    signal(SIGABRT, pilotInterruptHandler);
    signal(SIGFPE,  pilotInterruptHandler);
    signal(SIGILL,  pilotInterruptHandler);
    signal(SIGINT,  pilotInterruptHandler);
    signal(SIGSEGV, pilotInterruptHandler);
    signal(SIGTERM, pilotInterruptHandler);

    signal(SIGPIPE, sigPipeHandler);				// ignore SIGPIPE Signals
    signal(SIGCHLD, zombieHandler);				// avoid system of keeping child zombies

}

void initConfiguredPilot() {
    int			i;
    struct baio 	*bb;
    struct deviceData	*dd;
    char		*mem;
    
    uu->pilotLaunchTime = currentTime.dtime;
    regressionBufferInit(&uu->longBufferPosition, 3, uu->config.long_buffer_seconds * uu->autopilot_loop_Hz + 0.5, "long buffer pose");
    regressionBufferInit(&uu->longBufferRpy, 3, uu->config.long_buffer_seconds * uu->autopilot_loop_Hz + 0.5, "long buffer orientation");
    regressionBufferInit(&uu->shortBufferPosition, 3, uu->config.short_buffer_seconds * uu->autopilot_loop_Hz + 0.5, "short buffer pose");
    regressionBufferInit(&uu->shortBufferRpy, 3, uu->config.short_buffer_seconds * uu->autopilot_loop_Hz + 0.5, "short buffer orientation");
    regressionBufferInit(&uu->shortBufferAcceleration, 3, uu->config.short_buffer_seconds * uu->autopilot_loop_Hz + 0.5, "short buffer acceleration");
    // hold non regression history of [x,y,z,r,p,y] for 5 seconds. At hight rate it is too big. Store 1 second.
    ALLOCC(mem, RASPILOT_RING_BUFFER_SIZE(1 * uu->autopilot_loop_Hz + 0.5, 6), char);
    uu->historyPose = (struct raspilotRingBuffer *) mem;
    raspilotRingBufferInit(uu->historyPose, 6, 1 * uu->autopilot_loop_Hz + 0.5, "historyPose");
    for(i=0; i<uu->motor_number; i++) {
	uu->motor[i].thrust = 0.0;
    }

    
    // The main launch/initialization of all configured devices.
    for(i=0; i<uu->deviceMax; i++) {
	deviceInitiate(i);
    }

    motorsSendStreamThrustFactor(NULL);

}


int raspilotInit(int argc, char **argv) {
    double 	tt;

    stdbaioInit();
    initTask();
    mainProcessCommandLineArgs(argc, argv);
    logbaioInit();
    configloadFile();

    if (uu->autostart && uu->config.pilot_main_mode != MODE_MANUAL_RC) {
	printf("%s: Raspilot is not auto starting, because the main mode is not MODE_MANUAL_RC.\n", PPREFIX());
	exit(1);
    }
    
    initConfiguredPilot();

    trajectoryLogInit();
    if (uu->pingToHost != NULL) pingToHostInit();
    
    // the first ping wakes up motors. ? Is this still true?
    timeLineInsertEvent(UTIME_AFTER_MSEC(5000), pilotRegularSendPings, NULL);
    // read standard input for interactive commands
    timeLineInsertEvent(UTIME_AFTER_MSEC(10), pilotInteractiveInputRegularCheck, NULL);
    // start saving trajectory
    timeLineInsertEvent(UTIME_AFTER_MSEC(30), pilotRegularSaveTrajectory, NULL);
    // TODO: only if gimbal configured
    timeLineInsertEvent(UTIME_AFTER_MSEC(20), pilotRegularSendGimbalPwm, NULL);
    
    // check that ping to master host is still alive, give him some time before the first check
    if (uu->pingToHost != NULL) timeLineInsertEvent(UTIME_AFTER_SECONDS(50), pingToHostRegularCheck, NULL);

    // Sleep at leat 1 second. It is the usual time for killing subprocesses and restarting new one in config
    // file
    // raspilotBusyWait(1.10);
    return(0);
}
