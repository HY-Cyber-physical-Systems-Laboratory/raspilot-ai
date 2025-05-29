
#ifndef NULL
    #define NULL ((void *)0)
#endif


#include "cxx/stdafx.hpp"

#include "test.h"
#include "init.h"


int raspilotPoll() {
    struct timeval 	tv;
    int 		r;

    setCurrentTime();
    timeLineTimeToNextEvent(&tv, 1);
    // execute I/O operations
    r = baioPoll(tv.tv_sec * 1000000 + tv.tv_usec);
    setCurrentTime();
    // execute planned operations
    r += timeLineExecuteScheduledEvents(0);
    return(r);
}

void raspilotBusyWaitUntilTimeoutOrStandby(double sleeptime) {
    double tt;
    
    setCurrentTime();
    tt = currentTime.dtime;
    while (currentTime.dtime < tt + sleeptime && uu->flyStage != FS_STANDBY) raspilotPoll();
}

void mainLoadPreviousFlyTime() {
    FILE * ff;
    uu->previousTotalFlyTime = 0;
    ff = fopen("totalFlyTime.txt", "r");
    if (ff != NULL) {
	fscanf(ff, "%lf", &uu->previousTotalFlyTime);
	fclose(ff);
    }
}

void mainSavePreviousFlyTime() {
    FILE * ff;

    // do not touch if did not fly at all
    if (uu->flyStage < FS_FLY) return;
    
    ff = fopen("totalFlyTime.txt", "w");
    if (ff != NULL) {
	fprintf(ff, "%f\n", uu->previousTotalFlyTime);
	fclose(ff);
    }
}

void mainStatisticsMotors(int action) {
    int 	i;
    double 	sum, max, tt, flightTime;

    flightTime = (currentTime.dtime - uu->flyStartTime);
    // avoid division by zero
    if (flightTime == 0) flightTime = 1;
    
    sum = 0; max = -1;
    if (action == STATISTIC_PRINT) lprintf(0, "%s: Average motor thrusts: ", PPREFIX());
    for(i=0; i<uu->motor_number; i++) {
	if (action == STATISTIC_INIT) {
	    uu->motor[i].totalWork = 0;
	}
	tt = uu->motor[i].totalWork;
	if (action == STATISTIC_PRINT) lprintf(0, "%g ", tt / flightTime);
	sum += tt;
	if (tt > max) max = tt;
    }
    if (action == STATISTIC_PRINT) lprintf(0, "\n");
    
    if (action != STATISTIC_PRINT) return;
    
    lprintf(0, "%s: Total average motor thrust: %g\n", PPREFIX(), sum / uu->motor_number / flightTime);

    if (0) {
	lprintf(0, "%s: Proposed New motor_esc_corrections: ", PPREFIX());
	for(i=0; i<uu->motor_number; i++) {
	    tt = uu->motor[i].totalWork;
	    if (i==0) {
		lprintf(0, "[");
	    } else {
		lprintf(0, ",");
	    }
	    lprintf(0, "%f", tt/max);
	}
	lprintf(0, "],\n");
    }
}

void mainStatisticsSensors(int action) {
    int 			i, j, k;
    struct deviceData		*dd;
    struct deviceStreamData	*ddd;
    char			*sep;
    
    if (action == STATISTIC_PRINT) {
	lprintf(0, "%s:\n", PPREFIX());
	lprintf(0, "%s: Sensors / Devices:\n", PPREFIX());
    }
    for(i=0; i<uu->deviceMax; i++) {
	dd = uu->device[i];
	if (dd != NULL) {
	    if (action == STATISTIC_PRINT) lprintf(0, "%s:  %s\n", PPREFIX(), dd->name);
	    for(j=0; j<dd->ddtMax; j++) {
		ddd = dd->ddt[j];
		if (ddd != NULL) {
		    if (action == STATISTIC_INIT) ddd->totalNumberOfRecordsReceivedForStatistics = 0;
		    if (action == STATISTIC_PRINT) lprintf(0, "%s:   %30s: ", PPREFIX(), ddd->name);
		    switch (ddd->type) {
		    case DT_VOID:
			if (action == STATISTIC_PRINT) lprintf(0, "%d records received", ddd->totalNumberOfRecordsReceivedForStatistics);
			break;
		    case DT_DEBUG:
			// no statistics for debugging info?
			if (action == STATISTIC_PRINT) lprintf(0, "%d records received", ddd->totalNumberOfRecordsReceivedForStatistics);
			break;
		    case DT_PONG:
			if (action == STATISTIC_INIT) ddd->pongTotalTimeForStatistics = 0;
			if (action == STATISTIC_PRINT) lprintf(0, "average round trip latency: %g ms from %d pings", 1000.0*ddd->pongTotalTimeForStatistics/ddd->totalNumberOfRecordsReceivedForStatistics, ddd->totalNumberOfRecordsReceivedForStatistics);
			break;
		    default:
			sep = "average: [";
			for(k=0; k<ddd->outputBuffer.vectorsize; k++) {
			    if (action == STATISTIC_INIT) ddd->outputBuffer.totalSumForStatistics[k]=0;
			    if (action == STATISTIC_PRINT && ddd->outputBuffer.totalElemsForStatistics != 0) lprintf(0, "%s%g", sep, ddd->outputBuffer.totalSumForStatistics[k] / ddd->outputBuffer.totalElemsForStatistics);
			    sep = ", ";
			}
			if (action == STATISTIC_INIT) ddd->outputBuffer.totalElemsForStatistics = 0;
			if (action == STATISTIC_PRINT) lprintf(0, "]");
			break;
		    }
		    if (action == STATISTIC_PRINT) lprintf(0, "\n");
		}
	    }
	}
    }
    if (action == STATISTIC_PRINT) lprintf(0, "%s: End\n", PPREFIX());   
}


void mainStatisticsPids(int action) {
    int 			i, j, k;
    struct deviceData		*dd;
    struct deviceStreamData	*ddd;
    char			*sep;


    if (action == STATISTIC_INIT) {
        pidControllerReset(&uu->pidX, 1/uu->autopilot_loop_Hz);
        pidControllerReset(&uu->pidY, 1/uu->autopilot_loop_Hz);
        pidControllerReset(&uu->pidAltitude, 1.0/uu->stabilization_loop_Hz);
        pidControllerReset(&uu->pidAccAltitude, 1.0/uu->stabilization_loop_Hz);
        pidControllerReset(&uu->pidRoll, 1.0/uu->stabilization_loop_Hz);
        pidControllerReset(&uu->pidPitch, 1.0/uu->stabilization_loop_Hz);
        pidControllerReset(&uu->pidYaw, 1.0/uu->stabilization_loop_Hz);
    }
    
    if (action == STATISTIC_PRINT) {
	lprintf(0, "%s:\n", PPREFIX());
	lprintf(0, "%s: PID Roll:     %s\n", PPREFIX(), pidControllerStatistics(&uu->pidRoll, 1));
	lprintf(0, "%s: PID Pitch:    %s\n", PPREFIX(), pidControllerStatistics(&uu->pidPitch, 1));
	lprintf(0, "%s: PID Yaw:      %s\n", PPREFIX(), pidControllerStatistics(&uu->pidYaw, 1));
	lprintf(0, "%s: PID Altitude: %s\n", PPREFIX(), pidControllerStatistics(&uu->pidAltitude, 1));
	lprintf(0, "%s: PID X:        %s\n", PPREFIX(), pidControllerStatistics(&uu->pidX, 0));
	lprintf(0, "%s: PID Y:        %s\n", PPREFIX(), pidControllerStatistics(&uu->pidY, 0));
	lprintf(0, "%s:\n", PPREFIX());
    }
}

void mainStatistics(int action) {
    if (action == STATISTIC_INIT) {
	uu->flyStartTime = currentTime.dtime;
    }
    if (action == STATISTIC_PRINT) {
	double flyTime;

	if (uu->flyStartTime == 0) {
	    flyTime = 0;
	} else {
	    flyTime = currentTime.dtime - uu->flyStartTime;
	}
	lprintf(0, "%s: \n", PPREFIX());
	lprintf(0, "%s: STATISTICS:\n", PPREFIX());
	lprintf(0, "%s: Mission time: %gs\n", PPREFIX(), flyTime);
	uu->previousTotalFlyTime += flyTime;
	lprintf(0, "%s: Fly time since reset: %gs\n", PPREFIX(), uu->previousTotalFlyTime);
    }
    
    mainStatisticsMotors(action);
    mainStatisticsSensors(action);
    mainStatisticsPids(action);
    
    if (action == STATISTIC_PRINT) {
	lprintf(0, "\n");
    }
}

void mainPrintUsage() {
    printf("Usage:     raspilot [options]\n"
	   "\n"
	   "options:\n"
	   " -c <config_file> : load configuration from config_file\n"
	   " -d <number>      : set verbosity of debug output. Range 0 - 1000\n"
	   " -h               : this help\n"
	   " -p <ip_address>  : during a fly, regularly ping this host. Land if the connection is lost.\n"
	   "\n"
	);
}

void mainExit(void *d) {
    printf("%s: Exiting.\n", PPREFIX());
    fflush(stdout);
    exit(0);
}


int mainProcessCommandLineArgs(int argc, char **argv) {
    int i;

    // task name
    if (argc > 0 && strlen(argv[0]) >= 4 && strcmp(argv[0]+strlen(argv[0])-4, "stop") == 0) {
	// if invoked as stop engine, stop the engine
	lprintf(0, "%s: Stopping engine!\n", PPREFIX());
	motorsEmmergencyShutdown();
	stdbaioClose();
	exit(0);
    } else {
	// otherwise execute mission
	debugLevel = DEFAULT_DEBUG_LEVEL;
	logLevel = DEFAULT_LOG_LEVEL;
    }

    for(i=1; i<argc; i++) {
	// printf("Checking %s\n", argv[i]);
	if (strcmp(argv[i], "-c") == 0) {
	    // cfg file 
	    if (i+1 >= argc) {
		lprintf(0, "%s: Error: Command line: -c shall be followed by configuration file\n", PPREFIX());
	    } else {
		i++;
		uu->cfgFileName = strDuplicate(argv[i]);
		lprintf(0, "%s: Info: Configuration file is %s\n", PPREFIX(), uu->cfgFileName);
	    }
	} else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-dl") == 0) {
	    // debug level
	    if (i+1 >= argc) {
		lprintf(0, "%s: Error: Command line: -d and -dl shall be followed by debug level argument\n", PPREFIX());
	    } else {
		i++;
		debugLevel = atoi(argv[i]);
		lprintf(0, "%s: Info: debug level: %d\n", PPREFIX(), debugLevel);
	    }
	} else if (strcmp(argv[i], "-ll") == 0) {
	    // log level
	    if (i+1 >= argc) {
		lprintf(0, "%s: Error: Command line: -ll shall be followed by log level argument\n", PPREFIX());
	    } else {
		i++;
		logLevel = baseLogLevel = atoi(argv[i]);
		if (logLevel > 0) lprintf(0, "%s: Info: log level: %d\n", PPREFIX(), logLevel);
	    }
	} else if (strcmp(argv[i], "-l") == 0) {
	    // log file
	    if (i+1 >= argc) {
		lprintf(0, "%s: Error: Command line: -l shall be followed by log file name\n", PPREFIX());
	    } else {
		i++;
		uu->logFileName = argv[i];
		lprintf(0, "%s: Info: log file: %s\n", PPREFIX(), uu->logFileName);
	    }
	} else if (strcmp(argv[i], "-p") == 0) {
	    // ping to host
	    // if the connection to this host is broken, return to home or land!
	    // TODO: This -p option and ping config shall be moved to the config file!!
	    // for example openhd & mavlink is not dependent on ssh ping connection.
	    if (i+1 >= argc) {
		lprintf(0, "%s: Error: Command line: -p shall be followed by IP address of host to ping\n", PPREFIX());
	    } else {
		i++;
		uu->pingToHost = argv[i];
		lprintf(0, "%s: Info: ping to host %s\n", PPREFIX(), uu->pingToHost);
	    }
	} else if (strcmp(argv[i], "-auto") == 0) {
	    uu->autostart = 1;
	} else {
	    if (argv[i][1] != 'h') {
		lprintf(0, "Error: unknown option %s\n\n", argv[i]);
	    }
	    mainPrintUsage();
	    exit(0);
	}
    }

    if (argc != i) {
	lprintf(0, "%s: Error: Command line: wrong number of arguments!\n", PPREFIX());
	mainPrintUsage();
	exit(0);
    }

    return(i);
}


void mainStandardShutdown(void *d) {
    int 	i;
    
    lprintf(0, "%s: Raspilot is going down\n", PPREFIX());
    motorsStop(NULL);

    motorsStandby(NULL);
    if (uu->deviceMotors >= 0 && uu->device[uu->deviceMotors] != NULL && uu->device[uu->deviceMotors]->shutdownExit) motorsExit(NULL);

    if (1) mainStatistics(STATISTIC_PRINT);
    // save flight time after statistics where it is updated

    // do not change flight status before savign total flight time
    mainSavePreviousFlyTime();
    uu->flyStage = FS_SHUTDOWN;
    
    
    shutDownInProgress = 1;

    // Deinitialize/close all devices
    for(i=0; i<uu->deviceMax; i++) deviceFinalize(i);

    trajectoryLogClose();
    pingToHostClose();
    logbaioClose();
    stdbaioClose();
    fflush(stdout);
    terminalResume();
    timeLineRemoveAllEvents();
    timeLineInsertEvent(UTIME_AFTER_MSEC(500), mainExit, d);
}


static void pilotModeMotorPwmCalibrationAndExit(int motorIndex) {
    int i, c;

    printf("\n\n\n");
    printf("%s: Make sure that the power for ESCs is turned off !!!\n", PPREFIX());
    printf("%s: If not, motors will launch on max speed  !!!\n", PPREFIX());
    printf("%s: Type 'c' if ESCs are off and we can continue.\n", PPREFIX());
    stdbaioStdinClearBuffer();
    timeLineInsertEvent(UTIME_AFTER_MSEC(1), pilotRegularMotorTestModeTick, NULL);
    
    while ((c=stdbaioStdinMaybeGetPendingChar()) == -1) raspilotPoll();
    if (c != 'c') {
	printf("%s: 'c' not pressed. Exiting calibration.\n", PPREFIX());
	raspilotShutDownAndExit();
    }
    
    printf("%s: OK. Sending max pulses to GPIO.\n", PPREFIX());
    fflush(stdout);
    motorThrustSetAndSend(motorIndex, 1.0);

    printf("%s: Turn ESCs on and press 'c' one more time!\n", PPREFIX());
    stdbaioStdinClearBuffer();
    while ((c=stdbaioStdinMaybeGetPendingChar()) == -1) raspilotPoll();
    if (c != 'c') {
	motorThrustSetAndSend(motorIndex, 0.0);
	printf("%s: 'c' not pressed. Exiting calibration.\n", PPREFIX());
	raspilotShutDownAndExit();
    }

    printf("%s: OK. Sending min pulses for 5 seconds.\n", PPREFIX());
    fflush(stdout);
    motorThrustSetAndSend(motorIndex, 0.0);
    raspilotBusyWaitUntilTimeoutOrStandby(5.0);

    printf("%s: Calibration done. Spinning slowly for 5 seconds\n", PPREFIX());
    fflush(stdout);
    motorThrustSetAndSend(motorIndex, uu->config.motor_thrust_min_spin);
    raspilotBusyWaitUntilTimeoutOrStandby(5.0);

    printf("%s: All done.\n", PPREFIX());
    fflush(stdout);
    raspilotShutDownAndExit();
}

static void pilotSingleMotorTest(int motorIndex) {
    motorsThrustSetAndSend(0);
    lprintf(0, "%s: Warning: testing motor %d\n", PPREFIX(), motorIndex);
    motorThrustSetAndSend(motorIndex, uu->config.motor_thrust_min_spin);
    raspilotBusyWaitUntilTimeoutOrStandby(2.0);
    motorThrustSetAndSend(motorIndex, 0);
    raspilotBusyWaitUntilTimeoutOrStandby(1.0);
}

static void pilotModeMotorTest(int i) {
    timeLineInsertEvent(UTIME_AFTER_MSEC(1), pilotRegularMotorTestModeTick, NULL);
    
    raspilotBusyWaitUntilTimeoutOrStandby(10.0);
    
    if (i < 0) {
    
        pilotSingleMotorTest(0);
        pilotSingleMotorTest(1);
        pilotSingleMotorTest(2);
        pilotSingleMotorTest(3);
    
    } else {
	    pilotSingleMotorTest(i) ;   
    }

    raspilotShutDownAndExit();
}


static void pilotModeManualRc() {

    static char* PY_CODE_CACHE = NULL;

    if(PY_CODE_CACHE == NULL)
    {
        PY_CODE_CACHE = strDuplicate(
            "from raspilot import *\n"
            "from time import sleep\n"
            "import sys\n"
            "\n"
            "def main():\n"
            "    while True:\n"
            "        raspilotPoll()\n"
            "        sleep(0.01)\n"
        );
    }

    Py_Initialize();
    

    // In this mode the drone is controller by the joystick or similar
    manualControlInit(&uu->rc.roll, &uu->config.manual_rc_roll);
    manualControlInit(&uu->rc.pitch, &uu->config.manual_rc_pitch);
    manualControlInit(&uu->rc.yaw, &uu->config.manual_rc_yaw);
    manualControlInit(&uu->rc.altitude, &uu->config.manual_rc_altitude);

    timeLineInsertEvent(UTIME_AFTER_MSEC(10), manualControlRegularCheck, NULL);
    
    uu->flyStage = FS_STANDBY;
    
    timeLineInsertEvent(UTIME_AFTER_MSEC(2), pilotRegularStabilisationTick, NULL);
    // This is the main loop when raspilot is in manual rc mode
    while (uu->flyStage == FS_STANDBY) {
        uu->rc.altitude.value = -9999;
        lprintf(0, "%s: Standby\n", PPREFIX());
        
        mavlinkPrintfStatusTextToListeners("Standby mode");
        usleep(10000);
        pilotLaunchPoseClear(NULL);
        
        while (uu->flyStage == FS_STANDBY) {
            raspilotPoll() ;
        }
        
        if (uu->flyStage == FS_COUNTDOWN) {
            lprintf(0, "%s: Countdown!\n", PPREFIX());
            // mavlinkPrintfStatusTextToListeners("Countdown");
            raspilotPreLaunchSequence(1);
            if (uu->flyStage == FS_STANDBY) {
            lprintf(0, "%s: Info: Launch sequence interrupted.\n", PPREFIX());		
            } else if (uu->rc.altitude.value <= -9999) {
            lprintf(0, "%s: Error: Launch altitude not set during prefly. Interrupting!\n", PPREFIX());
            mavlinkPrintfStatusTextToListeners("Launch altitude not set during prefly. Interrupting!\n");
            uu->flyStage = FS_STANDBY;
            } else {
            lprintf(0, "%s: Info: launched.\n", PPREFIX());
            mavlinkPrintfStatusTextToListeners("Launch");
            uu->flyStage = FS_FLY;
            while (uu->flyStage == FS_FLY) {
                raspilotPoll();
            }
            if (uu->flyStage == FS_EMERGENCY_LANDING) {
                mainEmergencyLanding();
                if (uu->flyStage == FS_EMERGENCY_LANDING) raspilotGotoStandby();
            }
            }
        }
    }
}



int main(int argc, char **argv)
{
    raspilotInit(argc, argv);
    

    switch (uu->config.pilot_main_mode) {
        case MODE_MOTOR_PWM_CALIBRATION:
            pilotModeMotorPwmCalibrationAndExit(1);
        break;

        case MODE_MOTOR_TEST:
            pilotModeMotorTest(-1);
        break;
        
        case MODE_MANUAL_RC:
            
            ANOTATION_LOGIC("DRONE_MODE", "DRONE_MANUAL_RC", {
                /*
                    MODE_MANUAL_RC makes drone controll as RC controller input
                    This is the main mode _for manual control of the drone.
                    It is used _for manual takeoff, landing and other manual operations.
                    It is also used _for manual control of the drone in the flight. 
                */    
            });

            
            pilotModeManualRc();

        break;
        
        case MODE_MANUAL_AI:
            ANOTATION_LOGIC("DRONE_MODE", "DRONE_MANUAL_AI", {
                /*
                    MODE_MANUAL_AI makes drone controll as AI controller input
                    This is the main mode _for manual control of the drone.
                    It is used _for manual takeoff, landing and other manual operations.
                    It is also used _for manual control of the drone in the flight. 
                */    
               if(defined(AI_DRONE))
               {
                    lprintf(0, "%s: Warning: AI_DRONE is defined, but MODE_MANUAL_AI is not implemented yet.\n", PPREFIX());
                    //todo: implement AI_DRONE and implements pilotModeManualAI() function.
               }
            });
            pilotModeManualRc();
        break;

        case MODE_SINGLE_MISSION:
            timeLineInsertEvent(UTIME_AFTER_MSEC(1), pilotRegularMissionModeLoopTick, NULL);
            timeLineInsertEvent(UTIME_AFTER_MSEC(2), pilotRegularStabilisationTick, NULL);
            mission();
        break;
        
        
        default:
            lprintf(0, "%s: Error: unexpected main pilot mode %d\n", PPREFIX(), uu->config.pilot_main_mode);
        break;
    }


    //todo : auto python embedding code generation
    //const char* PYTHON_EMBEDDING_CODE = 
    //#include "./python/video/face_distortion_detect.py" // add your Python code's head and tail R"()"
    //;
    
    const char* test_str = "World응 아이냥";

    #if 0
    const char* test_str = 
    #include "./python/test.py"
    ;
    #endif

    Py_Initialize();

    PyObject *code = PY_COMPILE("def hello(name): print('Hello,', name)");
    PyObject *mod = PY_EXEC(code);
    PyObject *func = PY_GET_FUNC(mod, "hello");

    PyObject *args = PyTuple_Pack(1, PyUnicode_FromString(test_str));
    PY_CALL(func, args);

    Py_XDECREF(args);
    Py_XDECREF(func);
    Py_XDECREF(mod);
    Py_XDECREF(code);

    Py_Finalize();
}