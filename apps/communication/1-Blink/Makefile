include ../config.mk

SKETCH=blink

all:
	FQBN=$(UNO_FQBN)   PORT=$(UNO_PORT)   $(MAKE) install
	FQBN=$(EVERY_FQBN) PORT=$(EVERY_PORT) $(MAKE) install

install: compile upload

compile:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

list:
	arduino-cli board list
