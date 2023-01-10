AR_PROGRAMMER :=
AR_PORT       := /dev/cu.usbmodem24101
AR_SKETCH     := app
AR_FQBN       := esp32:esp32:XIAO_ESP32C3:DebugLevel=info

SRCS=$(shell find $(AR_SKETCH) -type f)

upload: build/$(AR_SKETCH).ino.hex
	arduino-cli upload -b $(AR_FQBN) -p $(AR_PORT) $(AR_PROGRAMMER) --input-dir "$(CURDIR)/build"

compile: build/$(AR_SKETCH).ino.hex

build/$(AR_SKETCH).ino.hex: $(SRCS)
	rm -rf build
	arduino-cli compile -b $(AR_FQBN) --build-path "$(CURDIR)/build" $(AR_SKETCH)

clean:
	rm -rf build

setup:
	arduino-cli core update-index --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
	arduino-cli core install esp32:esp32

monitor:
	arduino-cli monitor -p $(AR_PORT)

details:
	arduino-cli board details --fqbn $(AR_FQBN)

list:
	arduino-cli board list

.PHONY: upload compile serial clean setup details list
