ARDUINO_PORT=/dev/ttyUSB0
ARDUINO_FQBN=esp8266:esp8266:generic

all: compile upload serial

compile:
	cd .. && arduino-cli compile --fqbn $(ARDUINO_FQBN) $(CURDIR)

upload:
	cd .. && arduino-cli upload -p $(ARDUINO_PORT) --fqbn $(ARDUINO_FQBN) $(CURDIR)

serial:
	@echo -e 'エスケープ文字は [$'\e[1m ` \e[0m] 中断は [$'\e[1m `. \e[0m]'
	cu -s 115200 -E '`' -l /dev/ttyUSB0

htmldata:
	bin/htmldata.pl


