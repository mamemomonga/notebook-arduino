include ../config.mk

BAUDRATE=115200

SKETCH=blink-serial

all: install

install:
	FQBN=$(INO1_FQBN) PORT=$(INO1_PORT) $(MAKE) compile upload
	FQBN=$(INO2_FQBN) PORT=$(INO2_PORT) $(MAKE) compile upload

compile:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

ino1:
	screen $(INO1_PORT) $(BAUDRATE)

ino2:
	screen $(INO2_PORT) $(BAUDRATE)

list:
	arduino-cli board list

