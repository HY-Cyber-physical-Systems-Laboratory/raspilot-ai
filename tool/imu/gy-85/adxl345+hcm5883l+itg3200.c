#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>



#include "adxl345.h"
#include "hmc5883l.h"
#include "itg3200.h"
#include "Fusion.h"
#include "pi2c.h"

static int aFile = 0;
static int cFile = 0;
static int gFile = 0;


static inline double doubleGetTime() {
  struct timespec tt;
  clock_gettime(CLOCK_REALTIME, &tt);
  return(tt.tv_sec + tt.tv_nsec/1000000000.0);
}

static void taskStop(int signum) {
    exit(0);
}

#define CHECK(X) 
static int aConfigure(int aFile)
{
    CHECK(ADXL345_Init(aFile, ADXL345_ID, 1));

    struct ADXL345_DataFormat confDataFormat = {
        .range = ADXL345_RANGE_2G,
    };
    CHECK(ADXL345_ConfigureDataFormat(aFile, &confDataFormat));

    struct ADXL345_Power confPowerControl = {
        .measurement = 1,
    };
    CHECK(ADXL345_ConfigurePower(aFile, &confPowerControl));

    return 0;
}

static int cConfigure(int cFile)
{
    CHECK(HMC5883L_Init(cFile, HMC5883L_ID, 1));

    struct HMC5883L conf = {
        .gain = HMC5883L_GAIN_1090,
        .measurementMode = HMC5883L_MEASUREMENTMODE_NORMAL,
        .outputRate = HMC5883L_OUTPUTRATE_30,
        .samples = HMC5883L_SAMPLES_2,
    };
    CHECK(HMC5883L_Configure(cFile, &conf));

    CHECK(HMC5883L_SetContinuousMeasurement(cFile));

    return 0;
}

static int gConfigure(int gFile)
{
    CHECK(ITG3200_Init(gFile, ITG3200_ID, 1));
    
    struct ITG3200_Acquisition confAcquisition = {
        .lowPassFilter = ITG3200_LOWPASSFILTER_42,
        .sampleRateDivider = 0,
    };
    CHECK(ITG3200_ConfigureAcquisition(gFile, &confAcquisition));
    
    struct ITG3200_Power confPower = {
        .clockSource = ITG3200_CLOCKSOURCE_PLL_X,
    };
    CHECK(ITG3200_ConfigurePower(gFile, &confPower));
    
    return 0;
}

int main(int argc, char **argv) {
    double 	t0, t1, samplePeriod;
    int		i, usleepTime;
    int		magFd;
    u_int8_t	mm[6];
    int16_t 	aRawX, aRawY, aRawZ, gRawX, gRawY, gRawZ;
    int16_t     mRawX, mRawY, mRawZ;

    int		optSharedI2cFlag = 0;
    char	*optI2cPath = (char*)"/dev/i2c-1";
    double	optRate = 1000.0;

    // 옵션 처리
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            optSharedI2cFlag = 1;
        } else if (strcmp(argv[i], "-r") == 0 && (i + 1) < argc) {
            optRate = strtod(argv[++i], NULL);
        } else {
            optI2cPath = argv[i];
        }
    }

    if (optSharedI2cFlag) pi2cInit(optI2cPath, 1);

    // Fusion 초기화
    const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
    const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
    const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
    const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
    const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

    FusionOffset offset;
    FusionAhrs ahrs;
    FusionOffsetInitialise(&offset, optRate);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &(FusionAhrsSettings){
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .accelerationRejection = 10.0f,
        .magneticRejection = 20.0f,
        .rejectionTimeout = (unsigned)(5 * optRate),
    });

    // 센서 초기화
    int accelFd = open(optI2cPath, O_RDWR);
    int gyroFd  = open(optI2cPath, O_RDWR);
    int compFd  = open(optI2cPath, O_RDWR);

    if (aConfigure(accelFd) || gConfigure(gyroFd) || cConfigure(compFd)) {
        fprintf(stderr, "Sensor init failed\n");
        return -1;
    }

    signal(SIGINT, taskStop);

    // HMC5883L 설정
    magFd = pi2cOpen(optI2cPath, 0x1e);
    if (magFd < 0) {
        fprintf(stderr, "pi2c magnetometer connection failed\n");
        return -1;
    }
    pi2cWriteByteToReg(magFd, 0x00, 0x14);  // 75Hz
    pi2cWriteByteToReg(magFd, 0x02, 0x00);  // continuous mode

    usleepTime = 1000000 / optRate;
    usleep(usleepTime);

    t0 = doubleGetTime();
    while (1) {
        FusionVector gyroscope, accelerometer, magnetometer;

        // 센서 데이터 읽기
        ADXL345_ReadData(accelFd, &aRawX, &aRawY, &aRawZ);
        ITG3200_ReadData(gyroFd, &gRawX, &gRawY, &gRawZ);
        pi2cReadBytes(magFd, 0x03, 6, mm);

        // 데이터 변환
        accelerometer.axis.x = aRawX / 256.0f;  // ADXL345는 ±2g 에서 256 LSB/g
        accelerometer.axis.y = aRawY / 256.0f;
        accelerometer.axis.z = aRawZ / 256.0f;

        gyroscope.axis.x = gRawX / 14.375f;     // ITG3200는 14.375 LSB/(°/s)
        gyroscope.axis.y = gRawY / 14.375f;
        gyroscope.axis.z = gRawZ / 14.375f;

        mRawX = (int16_t)((mm[0] << 8) | mm[1]);
        mRawY = (int16_t)((mm[2] << 8) | mm[3]);
        mRawZ = (int16_t)((mm[4] << 8) | mm[5]);

        magnetometer.axis.x = mRawX;
        magnetometer.axis.y = mRawY;
        magnetometer.axis.z = mRawZ;

        // 타이밍 계산
        t1 = doubleGetTime();
        samplePeriod = t1 - t0;

        // 보정
        gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
        magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

        // 오프셋 보정
        gyroscope = FusionOffsetUpdate(&offset, gyroscope);

        // AHRS 업데이트
        FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, samplePeriod);

        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
        printf("rpy %7.5f %7.5f %7.5f\n",
            euler.angle.pitch * M_PI / 180.0,
            euler.angle.roll * M_PI / 180.0,
            euler.angle.yaw * M_PI / 180.0);

        fflush(stdout);

        t0 = t1;
        if (samplePeriod > 1.0 / optRate && usleepTime > 0) usleepTime--;
        else if (samplePeriod < 1.0 / optRate) usleepTime++;

        usleep(usleepTime);
    }

    taskStop(0);
}
