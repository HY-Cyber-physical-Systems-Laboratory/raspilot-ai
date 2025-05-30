#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>



#include "Fusion.h"
#include "pi2c.h"

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#include "adxl345.h"

#define READ(N) \
	if (read(file, buffer, N) != N) \
		return -1;

#define WRITE(N) \
	if (write(file, buffer, N) != N) \
		return -1;

int ADXL345_Init(int file, unsigned char id, bool check)
{
    if (ioctl(file, I2C_SLAVE, id) < 0)
        return -1;

    if (check)
    {
        unsigned char buffer[1];

        buffer[0] = 0x00;
        WRITE(1);

        READ(1);
        if (buffer[0] != 0xE5)
            return -1;
    }

    return 0;
}

int ADXL345_ConfigureActivity(int file, const struct ADXL345_Activity *conf)
{
    unsigned char buffer[5];

    buffer[0] = 0x24;
    buffer[1] = conf->activityThreshold;
    buffer[2] = conf->inactivityThreshold;
    buffer[3] = conf->inactivityTime;
    buffer[4] = ((conf->activityACDC & 0x01) << 7) |
        ((conf->activityAxes & 0x07) << 4) |
        ((conf->inactivityACDC & 0x01) << 3) |
        (conf->inactivityAxes & 0x07);
    WRITE(5);

    return 0;
}

int ADXL345_ConfigureDataFormat(int file, const struct ADXL345_DataFormat *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x31;
    buffer[1] = (conf->selfTest ? 0x80 : 0x00) |
        (conf->SPI & 0x40) |
        (conf->intActive & 0x20) |
        (conf->resolution & 0x08) |
        (conf->justify & 0x04) |
        (conf->range & 0x03);
    WRITE(2);

    return 0;
}

int ADXL345_ConfigureInterrupts(int file, const struct ADXL345_Interrupts *conf)
{
    unsigned char buffer[3];

    buffer[0] = 0x2E;
    buffer[1] = conf->enable;
    buffer[2] = conf->map;
    WRITE(3);

    return 0;
}

int ADXL345_ConfigureFIFO(int file, const struct ADXL345_FIFO *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x38;
    buffer[1] = (conf->mode & 0xC0) |
        (conf->triggerMap ? 0x20 : 0x00) |
        (conf->samples & 0x1F);
    WRITE(2);

    return 0;
}

int ADXL345_ConfigureFreeFall(int file, const struct ADXL345_FreeFall *conf)
{
    unsigned char buffer[3];

    buffer[0] = 0x28;
    buffer[1] = conf->threshold;
    buffer[2] = conf->time;
    WRITE(3);

    return 0;
}

int ADXL345_ConfigureOutputRate(int file, const struct ADXL345_OutputRate *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x2C;
    buffer[1] = (conf->lowPower ? 0x10 : 0x00) |
        (conf->rate & 0x0F);
    WRITE(2);

    return 0;
}

int ADXL345_ConfigurePower(int file, const struct ADXL345_Power *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x2D;
    buffer[1] = (conf->linkActivity ? 0x20 : 0x00) |
        (conf->autoSleep ? 0x10 : 0x00) |
        (conf->measurement ? 0x08 : 0x00) |
        (conf->sleep ? 0x04 : 0x00) |
        (conf->wakeup & 0x03);
    WRITE(2);

    return 0;
}

int ADXL345_ConfigureTap(int file, const struct ADXL345_Tap *conf)
{
    unsigned char buffer[4];

    buffer[0] = 0x1D;
    buffer[1] = conf->threshold;
    WRITE(2);

    buffer[0] = 0x21;
    buffer[1] = conf->duration;
    buffer[2] = conf->latent;
    buffer[3] = conf->window;
    WRITE(4);

    buffer[0] = 0x2A;
    buffer[1] = (conf->suppress ? 0x80 : 0x00) |
        (conf->axes & 0x07);
    WRITE(2);

    return 0;
}

int ADXL345_ReadActivityTapSources(int file, enum ADXL345_Source *sources)
{
    unsigned char buffer[1];

    buffer[0] = 0x2B;
    WRITE(1);

    READ(1);
    *sources = buffer[0];

    return 0;
}

int ADXL345_ReadData(int file, short *x, short *y, short *z)
{
    unsigned char buffer[6];

    buffer[0] = 0x32;
    WRITE(1);

    READ(6);
    *x = ((buffer[1]) << 8) | buffer[0];
    *y = ((buffer[3]) << 8) | buffer[2];
    *z = ((buffer[5]) << 8) | buffer[4];

    return 0;
}

int ADXL345_ReadFIFOStatus(int file, struct ADXL345_FIFOStatus *status)
{
    unsigned char buffer[1];

    buffer[0] = 0x2B;
    WRITE(1);

    READ(1);
    status->triggered = ((buffer[0] & 0x80) != 0);
    status->entries = (buffer[0] & 0x3F);

    return 0;
}

int ADXL345_ReadInterruptSources(int file, enum ADXL345_Interrupt *sources)
{
    unsigned char buffer[1];

    buffer[0] = 0x30;
    WRITE(1);

    READ(1);
    *sources = buffer[0];

    return 0;
}

int ADXL345_WriteOffset(int file, char x, char y, char z)
{
    unsigned char buffer[4];

    buffer[0] = 0x1E;
    buffer[1] = x;
    buffer[2] = y;
    buffer[3] = z;
    WRITE(4);

    return 0;
}

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#include "hmc5883l.h"

#define READ(N) \
	if (read(file, buffer, N) != N) \
		return -1;

#define WRITE(N) \
	if (write(file, buffer, N) != N) \
		return -1;

int HMC5883L_Init(int file, unsigned char id, bool check)
{
    if (ioctl(file, I2C_SLAVE, id) < 0)
        return -1;

    if (check)
    {
        unsigned char buffer[3];

        buffer[0] = 0x0A;
        WRITE(1);

        READ(3);
        if (buffer[0] != 0x48 ||
            buffer[1] != 0x34 ||
            buffer[2] != 0x33)
            return -1;
    }

    return 0;
}

int HMC5883L_Configure(int file, const struct HMC5883L *device)
{
    unsigned char buffer[3];

    buffer[0] = 0x00;
    buffer[1] = (device->samples & 0x60) |
        (device->outputRate & 0x1C) |
        (device->measurementMode & 0x03);
    buffer[2] = (device->gain & 0xE0);
    WRITE(3);

    return 0;
}

int HMC5883L_SetContinuousMeasurement(int file)
{
    unsigned char buffer[2];

    buffer[0] = 0x02;
    buffer[1] = 0x00;
    WRITE(2);

    return 0;
}

int HMC5883L_SetIdle(int file)
{
    unsigned char buffer[2];

    buffer[0] = 0x02;
    buffer[1] = 0x03;
    WRITE(2);

    return 0;
}

int HMC5883L_SetSingleMeasurement(int file)
{
    unsigned char buffer[2];

    buffer[0] = 0x02;
    buffer[1] = 0x01;
    WRITE(2);

    return 0;
}

int HMC5883L_ReadData(int file, short *x, short *y, short *z)
{
    unsigned char buffer[6];

    buffer[0] = 0x03;
    WRITE(1);

    READ(6);
    *x = ((buffer[0]) << 8) | buffer[1];
    *y = ((buffer[4]) << 8) | buffer[5];
    *z = ((buffer[2]) << 8) | buffer[3];

    return 0;
}

int HMC5883L_ReadDataReady(int file, bool *ready)
{
    unsigned char buffer[1];

    buffer[0] = 0x09;
    WRITE(1);

    READ(1);
    *ready = (buffer[0] & 0x01);

    return 0;
}

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#include "itg3200.h"

#define READ(N) \
	if (read(file, buffer, N) != N) \
		return -1;

#define WRITE(N) \
	if (write(file, buffer, N) != N) \
		return -1;

int ITG3200_Init(int file, unsigned char id, bool check)
{
    if (ioctl(file, I2C_SLAVE, id) < 0)
        return -1;

    if (check)
    {
        unsigned char buffer[1];

        buffer[0] = 0x00;
        WRITE(1);

        READ(1);
        if (buffer[0] != id)
            return -1;
    }

    return 0;
}

int ITG3200_ConfigureAcquisition(int file, const struct ITG3200_Acquisition *conf)
{
    unsigned char buffer[3];

    buffer[0] = 0x15;
    buffer[1] = conf->sampleRateDivider;
    buffer[2] = 0x18 | (conf->lowPassFilter & 0x07);
    WRITE(3);

    return 0;
}

int ITG3200_ConfigureInterrupt(int file, const struct ITG3200_Interrupts *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x17;
    buffer[1] = (conf->active & 0x80) |
        (conf->drive & 0x40) |
        (conf->latch & 0x20) |
        (conf->clear & 0x10) |
        (conf->enable & 0x05);
    WRITE(2);

    return 0;
}

int ITG3200_ConfigurePower(int file, const struct ITG3200_Power *conf)
{
    unsigned char buffer[2];

    buffer[0] = 0x3E;
    buffer[1] = (conf->sleep ? 0x40 : 0x00) |
        (conf->standby & 0x38) |
        (conf->clockSource & 0x07);
    WRITE(2);

    return 0;
}

int ITG3200_ReadData(int file, short *x, short *y, short *z)
{
    unsigned char buffer[6];

    buffer[0] = 0x1D;
    WRITE(1);

    READ(6);
    *x = ((buffer[2] << 8) | buffer[3]);
    *y = ((buffer[0] << 8) | buffer[1]);
    *z = ((buffer[4] << 8) | buffer[5]);

    return 0;
}

int ITG3200_ReadInterruptSources(int file, enum ITG3200_Interrupt *sources)
{
    unsigned char buffer[1];

    buffer[0] = 0x1A;
    WRITE(1);

    READ(1);
    *sources = buffer[0];

    return 0;
}

int ITG3200_ReadTemperature(int file, short *temperature)
{
    unsigned char buffer[2];

    buffer[0] = 0x1B;
    WRITE(1);

    READ(2);
    *temperature = ((buffer[0] << 8) | buffer[1]);

    return 0;
}

float ITG3200_ConvertTemperature(float rawTemperature)
{
    return 35.0f + (rawTemperature + 13200.0f) / 280.0f;
}

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
