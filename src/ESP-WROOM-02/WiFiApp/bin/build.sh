#!/bin/bash
set -eu
BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
CURDIR=$(pwd)

# ---------------------------------------------------

AR_DATA=$(arduino-cli config dump --format json | jq -r '.directories.data')
AR_PORT=$(cat ../serial_port)
AR_BAUD=921600
AR_EESZ="4M1M"
AR_FQBN="esp8266:esp8266:generic"
AR_FQBN_WOPT="$AR_FQBN:baud=$AR_BAUD,eesz=$AR_EESZ"

AR_TOOLS=$(arduino-cli board details --format json $AR_FQBN | jq -r '.required_tools[]')

ESP8266_VERSION=$( arduino-cli core search esp8266 | sed '1d' | sed '$d' | awk '{ print $2 }' )
MKSPIFFS_VERSION=$( echo "$AR_TOOLS" | jq -r '.| select(.name == "mkspiffs") | .version' )
PYTHON3_VERSION=$(  echo "$AR_TOOLS" | jq -r '.| select(.name == "python3") | .version' )

MKSPIFFS="$AR_DATA/packages/esp8266/tools/mkspiffs/$MKSPIFFS_VERSION/mkspiffs"
 PYTHON3="$AR_DATA/packages/esp8266/tools/python3/$PYTHON3_VERSION/python3"
 ESPTOOL="$AR_DATA/packages/esp8266/hardware/esp8266/$ESP8266_VERSION/tools/esptool/esptool.py"
PYSERIAL="$AR_DATA/packages/esp8266/hardware/esp8266/$ESP8266_VERSION/tools/pyserial"

SPIFFS_FILE="$BASEDIR/var/spiffs.bin"

eval $( cat $AR_DATA/packages/esp8266/hardware/esp8266/2.6.3/boards.txt \
	| grep -e '^generic.menu.eesz.'$AR_EESZ'.build.spiffs' \
	| perl -nlpE 's#(?:.+)\.spiffs_(.+)=#export SPIFFS_\U$1=#g' )

echo "  SPIFFS_PAGESIZE:  $SPIFFS_PAGESIZE"
echo "  SPIFFS_BLOCKSIZE: $SPIFFS_BLOCKSIZE"
echo "  SPIFFS_END:       $SPIFFS_END"
echo "  SPIFFS_START:     $SPIFFS_START"

# ---------------------------------------------------

echo
echo "*** コンパイル ***"
cd "$BASEDIR/.."
arduino-cli compile --fqbn $AR_FQBN_WOPT $BASEDIR
cd $CURDIR

echo
echo "*** SPIFFS ビルド ***"
mkdir -p $BASEDIR/var/spiffs
cp $BASEDIR/webui/var/build/public/index.html $BASEDIR/var/spiffs

set -x
$MKSPIFFS \
	-p $SPIFFS_PAGESIZE \
	-b $SPIFFS_BLOCKSIZE \
	-s $(($SPIFFS_END - $SPIFFS_START)) \
	-d 5 -c $BASEDIR/var/spiffs/ $SPIFFS_FILE
set +x

echo
echo "*** スケッチアップロード ***"
echo "  プログラムモードにしてください"
cd "$BASEDIR/.."
arduino-cli upload -p $AR_PORT --fqbn $AR_FQBN_WOPT $BASEDIR
cd $CURDIR

echo
echo "*** SPIFFS アップロード ***"
echo "  プログラムモードにしてください"
PYTHONPATH="$PYSERIAL" $PYTHON3 $ESPTOOL \
	--baud 921600 \
	--port $(cat ../serial_port) \
	write_flash $SPIFFS_START $SPIFFS_FILE -u

echo
echo "*** シリアル接続開始 ***"
screen $AR_PORT 115200

