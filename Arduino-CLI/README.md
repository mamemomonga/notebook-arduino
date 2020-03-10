# Arduino CLI

* [Arduino Pro/Arduino CLI](https://www.arduino.cc/pro/cli)
* [Arduino CLI](https://github.com/arduino/arduino-cli)
* [Arduino CLI Documentation](https://arduino.github.io/arduino-cli/)

# セットアップ

## Homebrew

	$ brew update
	$ brew install arduino-cli
	
## Install Script

	$ curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

パスの設定(zshの場合)

	$ echo 'export PATH=$HOME/bin:$PATH' >> $HOME/.zshrc
	$ exec $SHELL
	
## 設定

実行の確認

	$ arduino-cli help core

configの作成

	$ arduino-cli config init

コア一覧の更新

	$ arduino-cli core update-index

インストールされているボードリストの表示

	$ arduino-cli board listall
	
インストールされているコアの表示

	$ arduino-cli core list

## アップロード時にPermission Deniedがでる場合

/dev/tty??? のグループに自分自身を追加しましょう。

	$ ls -al /dev/ttyACM0
	crw-rw---- 1 root dialout 166, 0 Mar 11 03:58 /dev/ttyACM0

	$ sudo usermod -aG dialout $USER

再起動または再ログインが必要


# [Arduino UNOでの利用](./UNO)
# [ESP-WROOM-02 モジュールでの利用](./ESP-WROOM-02)
