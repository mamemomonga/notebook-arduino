ARDUINO_PORT=/dev/ttyUSB0
ARDUINO_FQBN=esp8266:esp8266:generic

all: compile upload

compile:
	cd .. && arduino-cli compile --fqbn $(ARDUINO_FQBN) $(CURDIR)

upload:
	cd .. && arduino-cli upload -p $(ARDUINO_PORT) --fqbn $(ARDUINO_FQBN) $(CURDIR)

