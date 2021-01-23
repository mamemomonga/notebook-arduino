ARDUINO_PORT=/dev/ttyACM0
ARDUINO_FQBN=arduino:avr:uno

all: compile upload

compile:
	cd .. && arduino-cli compile --fqbn $(ARDUINO_FQBN) $(CURDIR)

upload:
	cd .. && arduino-cli upload -p $(ARDUINO_PORT) --fqbn $(ARDUINO_FQBN) $(CURDIR)

