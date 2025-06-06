#include <stdint.h>
#include <semaphore.h>


void pi2cInit(char *path, int multiProcessSharingFlag) ;
void pi2cClose(int fd) ;
int pi2cOpen(char *path, int devAddr) ;
int pi2cReadBytes(int fd, uint8_t regAddr, uint8_t length, uint8_t *data) ;
int pi2cWrite(int fd, uint8_t* data, int length) ;
int pi2cWriteBytesToReg(int fd, uint8_t regAddr, uint8_t length, uint8_t* data) ;
int pi2cWriteByteToReg(int ifd, uint8_t regAddr, uint8_t data) ;
int pi2cWriteWordsToReg(int fd, uint8_t regAddr, uint8_t length, uint16_t* data) ;

