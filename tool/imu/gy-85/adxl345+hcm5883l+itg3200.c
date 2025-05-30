#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>

#include "Fusion.h"
#include "pi2c.h"
#include "adxl345.h"
#include "itg3200.h"
#include "hmc5883l.h"

static int aFile = 0, gFile = 0, cFile = 0;

static inline double doubleGetTime() {
    struct timespec tt;
    clock_gettime(CLOCK_REALTIME, &tt);
    return tt.tv_sec + tt.tv_nsec / 1000000000.0;
}

static void taskStop(int sig) {
    exit(0);
}

static int aConfigure(int fd) {
    if (ADXL345_ConfigureDataFormat(fd, &(struct ADXL345_DataFormat){ .range = ADXL345_RANGE_2G }) != 0) return -1;
    if (ADXL345_ConfigurePower(fd, &(struct ADXL345_Power){ .measurement = 1 }) != 0) return -1;
    return 0;
}

static int gConfigure(int fd) {
    if (ITG3200_ConfigureAcquisition(fd, &(struct ITG3200_Acquisition){ .lowPassFilter = ITG3200_LOWPASSFILTER_42, .sampleRateDivider = 0 }) != 0) return -1;
    if (ITG3200_ConfigurePower(fd, &(struct ITG3200_Power){ .clockSource = ITG3200_CLOCKSOURCE_PLL_X }) != 0) return -1;
    return 0;
}

static int cConfigure(int fd) {
    struct HMC5883L conf = {
        .gain = HMC5883L_GAIN_1090,
        .measurementMode = HMC5883L_MEASUREMENTMODE_NORMAL,
        .outputRate = HMC5883L_OUTPUTRATE_30,
        .samples = HMC5883L_SAMPLES_2,
    };
    if (HMC5883L_Configure(fd, &conf) != 0) return -1;
    if (HMC5883L_SetContinuousMeasurement(fd) != 0) return -1;
    return 0;
}

int main(int argc, char **argv) {
    char *i2cPath = "/dev/i2c-1";
    double rate = 1000.0;
    int usleepTime, i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0 && (i + 1) < argc)
            rate = strtod(argv[++i], NULL);
        else
            i2cPath = argv[i];
    }

    aFile = pi2cOpen(i2cPath, ADXL345_ID);
    gFile = pi2cOpen(i2cPath, ITG3200_ID);
    cFile = pi2cOpen(i2cPath, HMC5883L_ID);

    if (aFile < 0 || gFile < 0 || cFile < 0) {
        fprintf(stderr, "Failed to open one or more devices\n");
        return -1;
    }

    if (aConfigure(aFile) || gConfigure(gFile) || cConfigure(cFile)) {
        fprintf(stderr, "Sensor configuration failed\n");
        return -1;
    }

    signal(SIGINT, taskStop);

    FusionAhrs ahrs;
    FusionOffset offset;
    FusionAhrsInitialise(&ahrs);
    FusionOffsetInitialise(&offset, rate);
    FusionAhrsSetSettings(&ahrs, &(FusionAhrsSettings){
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .accelerationRejection = 10.0f,
        .magneticRejection = 20.0f,
        .rejectionTimeout = (unsigned)(5 * rate),
    });

    double t0 = doubleGetTime();
    usleepTime = 1000000 / rate;
    uint8_t mm[6];
    int16_t ax, ay, az, gx, gy, gz;
    int16_t mx, my, mz;

    while (1) {
        ADXL345_ReadData(aFile, &ax, &ay, &az);
        ITG3200_ReadData(gFile, &gx, &gy, &gz);
        pi2cReadBytes(cFile, 0x03, 6, mm);

        mx = (int16_t)((mm[0] << 8) | mm[1]);
        mz = (int16_t)((mm[2] << 8) | mm[3]);
        my = (int16_t)((mm[4] << 8) | mm[5]);

        FusionVector accel = {
            .axis.x = ax / 256.0f,
            .axis.y = ay / 256.0f,
            .axis.z = az / 256.0f,
        };

        FusionVector gyro = {
            .axis.x = gx / 14.375f,
            .axis.y = gy / 14.375f,
            .axis.z = gz / 14.375f,
        };

        FusionVector mag = {
            .axis.x = mx,
            .axis.y = my,
            .axis.z = mz,
        };

        double t1 = doubleGetTime();
        double dt = t1 - t0;
        t0 = t1;

        gyro = FusionOffsetUpdate(&offset, gyro);
        FusionAhrsUpdate(&ahrs, gyro, accel, mag, dt);

        FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
        printf("rpy %7.5f %7.5f %7.5f\n",
            euler.angle.pitch * M_PI / 180.0,
            euler.angle.roll * M_PI / 180.0,
            euler.angle.yaw * M_PI / 180.0);
        fflush(stdout);

        usleep(usleepTime);
    }

    return 0;
}
