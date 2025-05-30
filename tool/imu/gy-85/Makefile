LIBI2C=$(RASPILOT_HOME)/lib/pi2c
LIBFUSION=$(RASPILOT_HOME)/lib/FussionMagwick/Fusion
LIBMPU=.


all: adxl345+hcm5883l+itg3200 



adxl345+hcm5883l+itg3200: $(LIBMPU)/adxl345+hcm5883l+itg3200.c $(LIBI2C)/pi2c.c $(LIBI2C)/pi2c.h
	g++ -o adxl345+hcm5883l+itg3200 -g adxl345+hcm5883l+itg3200.c -I$(LIBFUSION) -I$(LIBI2C)  $(LIBI2C)/pi2c.c -L$(LIBFUSION) -lFusion  -lm -pthread -fpermissive


clean: always
	rm -f ./adxl345+hcm5883l+itg3200 

.PHONY: always

