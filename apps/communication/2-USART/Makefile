include ../config.mk

SKETCH=usart
BAUDRATE=115200

PORT=$(EVERY_PORT)
FQBN=$(EVERY_FQBN)

all: install serial

install: compile upload

compile:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

serial:
	screen $(PORT) $(BAUDRATE)

list:
	arduino-cli board list
