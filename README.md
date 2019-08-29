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
  * 仕様
    * ボタンスイッチ
      * 同時入力を受け付ける
      * ボタン入力はプレス後のリリース時にシグナル送信
      * 通信プロトコル（アプリケーション）
        * UARTシリアルで一度の送信で1byteを送る
        * 1byteのうち下位3bitがペイロード
        * 各ビットが各ボタンに割り当てられている
        * e.g.) ボタン0~2のうちボタン0のみが押された（最後に0がリリースされた）
          →`0b001`が送信
        * e.g.) ボタン0~2のうちボタン1,2が押された（最後にボタン1とボタン2が同時にリリースされた）
          →`0b110`が送信
    * LEDモニタ
      * 現在，ハードウェアとしてはLED0が白，LED1-3が緑
      * 通信プロトコル（アプリケーション）
        * UARTシリアルで一度の受信で1byteを受けつける
        * 1byteのうち8bitすべてがペイロード
        * 各2bitずつが各LEDの状態を表している．最下位2bitがLED0の状態
          * `0b00` ... `off`
          * `0b01` ... `slow blink` (1Hz)
          * `0b10` ... `rapid blink` (0.5Hz)
          * `0b11` ... `constantly on`
        * e.g.) `(0b)00|00|00|00` → ALL `off`
        * e.g.) `(0b)00|00|00|11` → LED0（白）だけ`constantly on`
        * e.g.) `(0b)11|10|01|00` → LED0（白）は`off`，LED1は`slow blink`，LED2は`rapid blink`, LED3は`constantly on`
* MasterBrd
  * yowai_ExternalSwitch_for_nano_20190427をもとにUARTでControllerと通信できるように最小限の変更を加えたもの
  * PlatformIOのプロジェクトになっている．
  * Aruduino IDEで用いるときは
    * src/main.cppの内容を.inoにコピペ
    * `#include <Arduino.h>` を消去
    * src/以下，include/以下のファイルを一緒にビルドできるところに持っていく．
    * LibraryとしてFlexiTimer2をインストール
  * 通信周りについて
    * Controllerとのシリアル通信はUART（ハードウェア）で行っている．
    * このUARTはUSB変換チップにつながっており，PCとの通信にも使われている．
    * Controllerとの通信とPCとの通信を同時に行いたい場合は，SerialComHandler.cpp/.hをSoftwareSerialを使うように書き換え，基板もそれに用にすればよい
      * SoftwareSerial... ソフトウェアでUARTをシミュレートするライブラリ，好きなDigitalIOピンをTX/RXピンとして使えるようにできる．
      

* yowai_ExternalSwitch_for_nano_20190427
    * ArduinoNanoと外部スイッチ，リレーモジュールを使ったコード

## メモ

