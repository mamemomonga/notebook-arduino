# ------------------------------------------------
# Arduino Nano Every(328エミュレーションオフ)
AR_FQBN=arduino:megaavr:nona4809:mode=off

# Arduino Nano Every ポート
AR_PORT=/dev/cu.usbmodem1211301

# スケッチ
AR_SKETCH=serial1

# 外付けUSB-UART
PORT1=/dev/cu.usbserial-1005
# ------------------------------------------------

upload: build/$(AR_SKETCH).ino.hex
	arduino-cli upload -b $(AR_FQBN) -p $(AR_PORT) --input-dir "$(CURDIR)/build"

build/$(AR_SKETCH).ino.hex: $(AR_SKETCH)/$(AR_SKETCH).ino
	arduino-cli compile -b $(AR_FQBN) --build-path "$(CURDIR)/build" $(AR_SKETCH)

port0:
	screen $(AR_PORT) 115200

port1:
	screen $(PORT1) 115200

