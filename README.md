# ArduinoCode
## About 
失禁体験装置の制御コード

## 梱包物
* Controller 
  * 3つのボタン，4つのLEDを配したコントローラのファームウェア
  * PlatformIO でコンパイル
  * Aruduino IDEでもちいるときは
    * src/main.cppの内容を.inoにコピペ
    * `#include <Arduino.h>` を消去
    * LibraryとしてFreeRTOS (by Richard, Phillips)をインストール
* MasterBrd
  * yowai_ExternalSwitch_for_nano_20190427をもとにUARTでControllerと通信できるように最小限の変更を加えたもの
  * PlatformIOのプロジェクトになっている．
  * Aruduino IDEで用いるときは
    * src/main.cppの内容を.inoにコピペ
    * `#include <Arduino.h>` を消去
    * src/以下，include/以下のファイルを一緒にビルドできるところに持っていく．
    * LibraryとしてFlexiTimer2をインストール

* yowai_ExternalSwitch_for_nano_20190427
    * ArduinoNanoと外部スイッチ，リレーモジュールを使ったコード

## メモ

