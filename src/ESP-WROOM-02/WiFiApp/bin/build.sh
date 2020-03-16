#!/bin/bash
set -eu
BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
CURDIR=$(pwd)

AR_DATA=$(arduino-cli config dump --format json | jq -r '.directories.data')
AR_PORT=$(cat ../serial_port)
AR_BAUD=921600
AR_EESZ="4M1M"

MKSPIFFS="$AR_DATA/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506/mkspiffs"
 PYTHON3="$AR_DATA/packages/esp8266/tools/python3/3.7.2-post1/python3"
 ESPTOOL="$AR_DATA/packages/esp8266/hardware/esp8266/2.6.3/tools/esptool/esptool.py"
PYSERIAL="$AR_DATA/packages/esp8266/hardware/esp8266/2.6.3/tools/pyserial"

AR_FQBN="esp8266:esp8266:generic:baud=$AR_BAUD,eesz=$AR_EESZ"

SPIFFS_FILE="$BASEDIR/var/spiffs.bin"

eval $( cat $AR_DATA/packages/esp8266/hardware/esp8266/2.6.3/boards.txt \
	| grep -e '^generic.menu.eesz.'$AR_EESZ'.build.spiffs' \
	| perl -nlpE 's#(?:.+)\.spiffs_(.+)=#export SPIFFS_\U$1=#g' )

echo "  SPIFFS_PAGESIZE:  $SPIFFS_PAGESIZE"
echo "  SPIFFS_BLOCKSIZE: $SPIFFS_BLOCKSIZE"
echo "  SPIFFS_END:       $SPIFFS_END"
echo "  SPIFFS_START:     $SPIFFS_START"

echo
echo "*** コンパイル開始 ***"
cd "$BASEDIR/.."
arduino-cli compile --fqbn $AR_FQBN $BASEDIR
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
set -x
arduino-cli upload -p $AR_PORT --fqbn $AR_FQBN $BASEDIR
set +x
cd $CURDIR

echo
echo "*** SPIFFS アップロード ***"
echo "  プログラムモードにしてください"
set -x
PYTHONPATH="$PYSERIAL" $PYTHON3 $ESPTOOL \
	--baud 921600 \
	--port $(cat ../serial_port) \
	write_flash $SPIFFS_START $SPIFFS_FILE -u
set +x

echo
echo "*** シリアル接続開始 ***"
screen $AR_PORT 115200

