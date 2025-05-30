LIBI2C=$(RASPILOT_HOME)/lib/pi2c
LIBFUSION=$(RASPILOT_HOME)/lib/FussionMagwick/Fusion
LIBMPU=.
all: adxl345+hcm5883l+itg3200 adxl345



adxl345+hcm5883l+itg3200: $(LIBMPU)/adxl345+hcm5883l+itg3200.c $(LIBI2C)/pi2c.c $(LIBI2C)/pi2c.h
	g++ -o adxl345+hcm5883l+itg3200 -g adxl345+hcm5883l+itg3200.c -I$(LIBFUSION) -I$(LIBI2C)  $(LIBI2C)/pi2c.c -L$(LIBFUSION) -lFusion  -lm -pthread -fpermissive

adxl345: adxl345.c $(LIBI2C)/pi2c.c $(LIBI2C)/pi2c.h  $(LIBMS5611)/MS5611.cpp 
	g++ -o adxl345 -g adxl345.c -lm -pthread

itg3200: itg3200.c $(LIBI2C)/pi2c.c $(LIBMPU)/MPU6050.cpp $(LIBMPU)/I2Cdev.cpp $(LIBI2C)/pi2c.h 
	g++ -o itg3200 -g itg3200.c -I$(LIBMPU) -I$(LIBI2C) $(LIBMPU)/MPU6050.cpp $(LIBMPU)/I2Cdev.cpp $(LIBI2C)/pi2c.c -lm -pthread

hmc5883l: hmc5883l.c $(LIBI2C)/pi2c.c $(LIBMPU)/MPU6050.cpp $(LIBMPU)/I2Cdev.cpp $(LIBI2C)/pi2c.h 
	g++ -o hmc5883l -g hmc5883l.c -I$(LIBMS5611) -I$(LIBMPU) -I$(LIBI2C) $(LIBMPU)/MPU6050.cpp $(LIBMPU)/I2Cdev.cpp $(LIBI2C)/pi2c.c -lm -pthread


clean: always
	rm -f mpu6050+hmc5883l ms5611 hmc5883l *~

.PHONY: always

