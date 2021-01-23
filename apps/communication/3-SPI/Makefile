include ../config.mk

BAUDRATE=115200

all: install serial

install:
	SKETCH=master FQBN=$(EVERY_FQBN) PORT=$(EVERY_PORT) $(MAKE) compile upload
	SKETCH=slave  FQBN=$(UNO_FQBN)   PORT=$(UNO_PORT)   $(MAKE) compile upload
	make serial-master
	# make serial-slave

compile:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

serial-master:
	screen $(EVERY_PORT) $(BAUDRATE)

serial-slave:
	screen $(UNO_PORT) $(BAUDRATE)

list:
	arduino-cli board list
