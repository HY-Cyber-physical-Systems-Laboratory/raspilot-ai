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
#include <semaphore.h>




#include "Fusion.h"
#include "pi2c.h"

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>  

#include "adxl345.h"
#include "hmc5883l.h"
#include "itg3200.h"

struct fdStr {
    int 	fd;
    sem_t	*sem;
};

#define MAX_OPEN_DEV 256
extern struct fdStr fdTab[MAX_OPEN_DEV];

int ADXL345_Init(int ifd, unsigned char id, bool check)
{

	////sem_wait(fdTab[ifd].sem);
    if (ioctl(fdTab[ifd].fd, I2C_SLAVE, id) < 0)
        return -1;

    if (check)
    {
        unsigned char val = 0;
        if (pi2cReadBytes(ifd, 0x00, 1, &val) != 1 || val != 0xE5)
            return -1;
    }

    return 0;
}

int ADXL345_ConfigureActivity(int ifd, const struct ADXL345_Activity *conf)
{
    unsigned char data[4] = {
        conf->activityThreshold,
        conf->inactivityThreshold,
        conf->inactivityTime,
        ((conf->activityACDC & 0x01) << 7) |
        ((conf->activityAxes & 0x07) << 4) |
        ((conf->inactivityACDC & 0x01) << 3) |
        (conf->inactivityAxes & 0x07)
    };
    return pi2cWriteBytesToReg(ifd, 0x24, 4, data);
}

int ADXL345_ConfigureDataFormat(int ifd, const struct ADXL345_DataFormat *conf)
{
    unsigned char val = (conf->selfTest ? 0x80 : 0x00) |
        (conf->SPI & 0x40) |
        (conf->intActive & 0x20) |
        (conf->resolution & 0x08) |
        (conf->justify & 0x04) |
        (conf->range & 0x03);
    return pi2cWriteByteToReg(ifd, 0x31, val);
}

int ADXL345_ConfigureInterrupts(int ifd, const struct ADXL345_Interrupts *conf)
{
    unsigned char data[2] = { conf->enable, conf->map };
    return pi2cWriteBytesToReg(ifd, 0x2E, 2, data);
}

int ADXL345_ConfigureFIFO(int ifd, const struct ADXL345_FIFO *conf)
{
    unsigned char val = (conf->mode & 0xC0) |
        (conf->triggerMap ? 0x20 : 0x00) |
        (conf->samples & 0x1F);
    return pi2cWriteByteToReg(ifd, 0x38, val);
}

int ADXL345_ConfigureFreeFall(int ifd, const struct ADXL345_FreeFall *conf)
{
    unsigned char data[2] = { conf->threshold, conf->time };
    return pi2cWriteBytesToReg(ifd, 0x28, 2, data);
}

int ADXL345_ConfigureOutputRate(int ifd, const struct ADXL345_OutputRate *conf)
{
    unsigned char val = (conf->lowPower ? 0x10 : 0x00) | (conf->rate & 0x0F);
    return pi2cWriteByteToReg(ifd, 0x2C, val);
}

int ADXL345_ConfigurePower(int ifd, const struct ADXL345_Power *conf)
{
    unsigned char val = (conf->linkActivity ? 0x20 : 0x00) |
        (conf->autoSleep ? 0x10 : 0x00) |
        (conf->measurement ? 0x08 : 0x00) |
        (conf->sleep ? 0x04 : 0x00) |
        (conf->wakeup & 0x03);
    return pi2cWriteByteToReg(ifd, 0x2D, val);
}

int ADXL345_ConfigureTap(int ifd, const struct ADXL345_Tap *conf)
{
    unsigned char data1[1] = { conf->threshold };
    unsigned char data2[3] = { conf->duration, conf->latent, conf->window };
    unsigned char data3[1] = { (conf->suppress ? 0x80 : 0x00) | (conf->axes & 0x07) };

    if (pi2cWriteBytesToReg(ifd, 0x1D, 1, data1) != 2) return -1;
    if (pi2cWriteBytesToReg(ifd, 0x21, 3, data2) != 4) return -1;
    if (pi2cWriteBytesToReg(ifd, 0x2A, 1, data3) != 2) return -1;

    return 0;
}

int ADXL345_ReadActivityTapSources(int ifd, enum ADXL345_Source *sources)
{
    unsigned char val = 0;
    if (pi2cReadBytes(ifd, 0x2B, 1, &val) != 1)
        return -1;

    *sources = val;
    return 0;
}

int ADXL345_ReadData(int ifd, short *x, short *y, short *z)
{
    unsigned char data[6];

    if (pi2cReadBytes(ifd, 0x32, 6, data) != 6)
        return -1;

    *x = ((data[1]) << 8) | data[0];
    *y = ((data[3]) << 8) | data[2];
    *z = ((data[5]) << 8) | data[4];

    return 0;
}

int ADXL345_ReadFIFOStatus(int ifd, struct ADXL345_FIFOStatus *status)
{
    unsigned char val = 0;
    if (pi2cReadBytes(ifd, 0x2B, 1, &val) != 1)
        return -1;

    status->triggered = ((val & 0x80) != 0);
    status->entries = (val & 0x3F);
    return 0;
}

int ADXL345_ReadInterruptSources(int ifd, enum ADXL345_Interrupt *sources)
{
    unsigned char val = 0;
    if (pi2cReadBytes(ifd, 0x30, 1, &val) != 1)
        return -1;

    *sources = val;
    return 0;
}

int ADXL345_WriteOffset(int ifd, char x, char y, char z)
{
    unsigned char data[3] = { x, y, z };
    return pi2cWriteBytesToReg(ifd, 0x1E, 3, (uint8_t*)data);
}

int HMC5883L_Init(int ifd, unsigned char id, bool check)
{
	////sem_wait(fdTab[ifd].sem);
    if (ioctl(fdTab[ifd].fd, I2C_SLAVE, id) < 0)
        return -1;

    if (check)
    {
        unsigned char buffer[3];
        if (pi2cReadBytes(ifd, 0x0A, 3, buffer) != 3)
            return -1;

        if (buffer[0] != 0x48 || buffer[1] != 0x34 || buffer[2] != 0x33)
            return -1;
    }

    return 0;
}

int HMC5883L_Configure(int ifd, struct HMC5883L *device)
{
    unsigned char data[2];

    // Configuration Register A: 0x00
    data[0] = (device->samples & 0x60) |
              (device->outputRate & 0x1C) |
              (device->measurementMode & 0x03);
    if (pi2cWriteBytesToReg(ifd, 0x00, 1, data) != 2)
        return -1;

    // Configuration Register B: 0x01
    data[0] = (device->gain & 0xE0);
    if (pi2cWriteBytesToReg(ifd, 0x01, 1, data) != 2)
        return -1;

    return 0;
}

int HMC5883L_SetContinuousMeasurement(int ifd)
{
    unsigned char mode = 0x00;
    return pi2cWriteByteToReg(ifd, 0x02, mode);
}

int HMC5883L_SetIdle(int ifd)
{
    unsigned char mode = 0x03;
    return pi2cWriteByteToReg(ifd, 0x02, mode);
}

int HMC5883L_SetSingleMeasurement(int ifd)
{
    unsigned char mode = 0x01;
    return pi2cWriteByteToReg(ifd, 0x02, mode);
}

int HMC5883L_ReadData(int ifd, short *x, short *y, short *z)
{
    unsigned char buffer[6];
    if (pi2cReadBytes(ifd, 0x03, 6, buffer) != 6)
        return -1;

    *x = ((buffer[0]) << 8) | buffer[1];
    *z = ((buffer[2]) << 8) | buffer[3];
    *y = ((buffer[4]) << 8) | buffer[5];

    return 0;
}

int HMC5883L_ReadDataReady(int ifd, bool *ready)
{
    unsigned char val = 0;
    if (pi2cReadBytes(ifd, 0x09, 1, &val) != 1)
        return -1;

    *ready = (val & 0x01);
    return 0;
}


int ITG3200_Init(int ifd, unsigned char id, bool check)
{

    printf("[DEBUG] ITG3200_Init: ifd=%d, id=0x%02X\n", ifd, id);

    if (!fdTab[ifd].sem) {
        fprintf(stderr, "[ERROR] fdTab[%d].sem is NULL\n", ifd);
        return -1;
    }

    if (ioctl(fdTab[ifd].fd, I2C_SLAVE, id) < 0) {
        perror("[ERROR] ioctl I2C_SLAVE failed");
        return -1;
    }

    if (check) {
        unsigned char val = 0;
        int rc = pi2cReadBytes(ifd, 0x00, 1, &val);
        printf("[DEBUG] pi2cReadBytes returned %d, val=0x%02X\n", rc, val);
        if (rc != 1 || 0x69 != id) {
            fprintf(stderr, "[ERROR] ITG3200 check failed: expected 0x%02X, got 0x%02X\n", id, val);
            return -1;
        }
    }

    return 0;
}

int ITG3200_ConfigureAcquisition(int ifd, const struct ITG3200_Acquisition *conf)
{
    unsigned char data[2];
    data[0] = conf->sampleRateDivider;
    data[1] = 0x18 | (conf->lowPassFilter & 0x07);

    return pi2cWriteBytesToReg(ifd, 0x15, 2, data);
}

int ITG3200_ConfigureInterrupt(int ifd, const struct ITG3200_Interrupts *conf)
{
    unsigned char val = (conf->active & 0x80) |
                        (conf->drive & 0x40) |
                        (conf->latch & 0x20) |
                        (conf->clear & 0x10) |
                        (conf->enable & 0x05);

    return pi2cWriteByteToReg(ifd, 0x17, val);
}

int ITG3200_ConfigurePower(int ifd, const struct ITG3200_Power *conf)
{
    unsigned char val = (conf->sleep ? 0x40 : 0x00) |
                        (conf->standby & 0x38) |
                        (conf->clockSource & 0x07);

    return pi2cWriteByteToReg(ifd, 0x3E, val);
}

int ITG3200_ReadData(int ifd, short *x, short *y, short *z)
{
    unsigned char data[6];
    if (pi2cReadBytes(ifd, 0x1D, 6, data) != 6)
        return -1;

    *y = ((data[0] << 8) | data[1]);
    *x = ((data[2] << 8) | data[3]);
    *z = ((data[4] << 8) | data[5]);

    return 0;
}

int ITG3200_ReadInterruptSources(int ifd, enum ITG3200_Interrupt *sources)
{
    unsigned char val = 0;
    if (pi2cReadBytes(ifd, 0x1A, 1, &val) != 1)
        return -1;

    *sources = val;
    return 0;
}

int ITG3200_ReadTemperature(int ifd, short *temperature)
{
    unsigned char data[2];
    if (pi2cReadBytes(ifd, 0x1B, 2, data) != 2)
        return -1;

    *temperature = (data[0] << 8) | data[1];
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
#define CHECK(X) do { if ((X) < 0) { fprintf(stderr, "%s failed at %s:%d\n", #X, __FILE__, __LINE__); return -1; } } while (0)

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
    setbuf(stdout, NULL); 
    double 	t0, t1, samplePeriod;
    int		i, usleepTime;
    int		magFd;
    u_int8_t	mm[6];
    int16_t 	aRawX, aRawY, aRawZ, gRawX, gRawY, gRawZ;
    int16_t     mRawX, mRawY, mRawZ;

    int		optSharedI2cFlag = 0;
    char	*optI2cPath = (char*)"/dev/i2c-1";
    double	optRate = 1000.0;

    printf("sibal1");

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

    printf("sibal2");

    if (optSharedI2cFlag) pi2cInit(optI2cPath, 1);

    printf("sibal3");
    // Fusion 초기화
    const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
    const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
    const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
    const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
    const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};;
    const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

    printf("sibal4");
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

    printf("sibal5");
    // 센서 초기화
    printf("sibal6");
    int accelFd = pi2cOpen(optI2cPath, 0x53);
    int gyroFd  = pi2cOpen(optI2cPath, 0x68);
    //int compFd  = pi2cOpen(optI2cPath, O_RDWR);

    magFd = pi2cOpen(optI2cPath, 0x1e);
    
    printf("sibal7");

    if (aConfigure(accelFd)) {
        fprintf(stderr, "Sensor init failed\n");
        return -1;
    }

    if (gConfigure(gyroFd)) {
        fprintf(stderr, "Sensor init failed\n");
        return -1;
    }

    if (cConfigure(magFd)) {
        fprintf(stderr, "Sensor init failed\n");
        return -1;
    }


    printf("sibal8");
    signal(SIGINT, taskStop);

    // HMC5883L 설정
    if (magFd < 0) {
        fprintf(stderr, "pi2c magnetometer connection failed\n");
        return -1;
    }
    printf("sibal9");
    pi2cWriteByteToReg(magFd, 0x00, 0x14);  // 75Hz
    pi2cWriteByteToReg(magFd, 0x02, 0x00);  // continuous mode

    usleepTime = 1000000 / optRate;
    usleep(usleepTime);

    printf("sibal123123123");
    t0 = doubleGetTime();
    while (1) {

        printf("s32313ibal123123123");
        FusionVector gyroscope, accelerometer, magnetometer;

        printf("s32313ibal123123123");
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


        t0 = t1;
        if (samplePeriod > 1.0 / optRate && usleepTime > 0) usleepTime--;
        else if (samplePeriod < 1.0 / optRate) usleepTime++;
        fflush(stdout);
        usleep(usleepTime);
    }

    taskStop(0);
}
