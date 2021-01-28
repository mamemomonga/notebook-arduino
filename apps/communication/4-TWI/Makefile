include ../config.mk

BAUDRATE=115200

all: install

install:
	SKETCH=master FQBN=$(INO1_FQBN) PORT=$(INO1_PORT) $(MAKE) upload
	SKETCH=slave  FQBN=$(INO2_FQBN) PORT=$(INO2_PORT) $(MAKE) upload

upload:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

ino1:
	screen $(INO1_PORT) $(BAUDRATE)

ino2:
	screen $(INO2_PORT) $(BAUDRATE)

list:
	arduino-cli board list

