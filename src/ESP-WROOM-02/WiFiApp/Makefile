ARDUINO_PORT=$(shell cat ../serial_port)
ARDUINO_FQBN=esp8266:esp8266:generic

all: webdata compile upload serial

compile:
	cd .. && arduino-cli compile --fqbn $(ARDUINO_FQBN) $(CURDIR)

upload:
	cd .. && arduino-cli upload -p $(ARDUINO_PORT) --fqbn $(ARDUINO_FQBN) $(CURDIR)

serial:
	screen $(ARDUINO_PORT) 115200

webui/var/build/public/index.html:
	cd webui && make

webdata: webui/var/build/public/index.html
	bin/webdata.pl

