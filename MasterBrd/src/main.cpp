/*
Pelter削除
冷却水の制御を追加
Arduino Nano用にピン番号を修正

LED
LED0,1,2,3
  A0,1,2,3へ接続
それぞれ
　0 リセット、待機状態
  1 エアー送り中 （点滅：一時停止中）
　2 水送り中     （点滅：一時停止中）
　3 水回収中     （点滅：一時停止中、
                   高速点滅：水送りが終わって、押しボタンによる水回収開始待ち状態 ）
*/
/*

D 0 シリアル通信
D 1 シリアル通信
D 2 エアーモーター
D 3 電磁弁
D 4 回収用ポンプ
D 5 お水を流す用ポンプ
D 6 冷却水回収用ポンプ
D 7 冷却水用ポンプ
D 8 ファン
D 9 振動子
D10 開始スイッチ
D11 リセットスイッチ
D12 コレクトスイッチ
A 0 表示LED 0
A 1 表示LED 1
A 2 表示LED 2
A 3 表示LED 3
A 4 
A 5 
A 6 
A 7 

*/

#include <Arduino.h>
#include <FlexiTimer2.h>
#include "LEDController.h"
#include "SigReciever.h"

//ピン番号-----------------------------------------------------------------------------------------
#define ELE_NUM 9                   //繋げる素子（element）の数
const int air_motor = 2;            //エアーモーター用のピン
const int solenoid_valve = 3;       //電磁弁のピン
const int collect_pump = 4;         //回収用ポンプのピン
const int water_pump = 5;           //水を流す用のポンプのピン
const int collect_cooling_pump = 6; //冷却水を回収する用のポンプのピン
const int cooling_pump = 7;         //冷却水を流す用のポンプのピン
const int fan = 8;                  //ファン用のピン
const int vibration = 9;            //振動子用のピン
//A7 気圧の読み取り

//外部ボタン関連
#define SW_NUM 3                   //スイッチの数
byte s_pin[SW_NUM] = {10, 11, 12}; //Urea()入力スイッチ,リセットスイッチ,コレクトスイッチ
LEDController ledcon;
stateUrea sUrea;

//add----------------------------------------------
/*
黄 入力       : 開始/一時停止/再開
赤 リセット   : 緊急停止
青 コレクト   : お湯回収
*/
enum
{
  swInputID = 0b001,
  swResetID = 0b010,
  swCollectID = 0b100
};
int swInputPressedNum = 0; //
SigReciever button;

//add----------------------------------------------

#define PUSH_SHORT 3               //スイッチゲージ Serial.print()を使うときは処理が重くなるので5~10くらい。高速なループの時は700くらい
#define PUSH_LIMIT PUSH_SHORT + 10 //スイッチゲージ限界

//エアー関係--------------------------------------------------------------------------------------------
int air_max = 220;   //エアーの最大気圧値
int air_target = 40; //airの段階的制御に使う
int ini;             //気圧の初期値(最初に読み取った値)
//時間関係----------------------------------------------------------------------------------------------
const float air_pump_time = 15000; //namco//30000;
const float solenoid_valve_time = 10000;
const float vibration_time = 3000;
const float fan_time = 0;
const float neck_time = 3000;
const float collect_water_time = 10000;
const float water_time = 7000;
const float blank_time = 3000;
const float blank_time2 = 10000;
const float cooling_water_time = 500;
const float collect_delay = 0;
//フラグ関係----------------------------------------------------------------------------------------------------
bool read_ini_flag = false; //気圧の初期値を取るために使う
bool air_max_flag = true;   //気圧が最大値かどうか
bool urea_flag = false;     //全体通しで実行中かどうか
bool release_flag = false;  //airの段階制御のフラグ
bool collect_flag = false;  //collectのフラグ

//コマンド関係の変数---------------------------------------------------------------------------------------------------
char input[30]; //コマンド分割一時格納用
int len;        //コマンドの長さ格納用
int command[5]; //','で区切った値が入る
String str;     //読み込んだ文字列を格納
//各素子の状態--------------------------------------------------------------------------------------------------
bool air_output = LOW;
bool solenoid_output = LOW;
bool water_output = LOW;
bool collect_output = LOW;
bool neck_output = LOW;
bool cooling_water_output = LOW;
bool collect_cooling_water_output = LOW;
//------------------------------------------------------------------------------------------------------

//時間カウント--------------------------------------------------------------------------------------------------
float times = 0; //経過時間
float t = 0;
void time_loop()
{
  t++;
  times = (float(t)) * 1;
}

void time_start()
{ //時間計測を開始する
  t = 0;
  times = 0;
  FlexiTimer2::set(1, time_loop); //1msごとに割り込み
  FlexiTimer2::start();
}

//エアーテスト　秒数指定 穴があいた時とかに使えるかも---------------------------------------------------------------------------------------
void pumpTest()
{
  digitalWrite(air_motor, HIGH);
  digitalWrite(solenoid_valve, LOW);
  delay(air_pump_time);
  digitalWrite(air_motor, LOW);
  digitalWrite(solenoid_valve, HIGH);
  delay(solenoid_valve_time);
  digitalWrite(solenoid_valve, LOW);
}

//エアーモーターON、電磁弁閉じる---------------------------------------------------------------------------------------------
void air_supply()
{
  digitalWrite(air_motor, HIGH);
  digitalWrite(solenoid_valve, LOW);
}
//電磁弁を一瞬開閉-----------------------------------------------------------------------------------------------
void air_release()
{
  digitalWrite(solenoid_valve, HIGH);
  delay(100);
  digitalWrite(solenoid_valve, LOW);
}
//気圧の最大値を設定----------------------------------------------------------------------------------------------
void air_value(int value)
{
  air_max = value;
  //Serial.println(air_max);
}
void Urea()
{

  if (times == 0)
  {
    time_start(); //時間カウント開始
                  //    sUrea = eRESET;
  }

  if (times < air_pump_time)
  {
    //エアーon
    sUrea = eAIR_SUPPLYING;
    air_output = HIGH;
  }
  else if (times < air_pump_time + blank_time)
  {
    //エアーoff
    air_output = LOW;
  }
  else if (times < air_pump_time + blank_time + water_time)
  {
    //電磁弁open
    solenoid_output = HIGH;
    //お湯送る
    sUrea = eWATER_SUPPLYING;
    water_output = HIGH;
    //お湯回収する
    collect_output = HIGH;
  }
  else if (times < air_pump_time + blank_time + water_time + blank_time2)
  {
    //お湯OFF
    water_output = LOW;
  }
  else if (times < air_pump_time + blank_time + water_time + blank_time2 + cooling_water_time)
  {
    //お湯回収停止
    collect_output = LOW;
    //冷却水ON
    cooling_water_output = HIGH;
    //冷却水回収ON
    collect_cooling_water_output = HIGH;
  }
  else if (times < air_pump_time + blank_time + water_time + +blank_time2 + cooling_water_time + collect_delay)
  {
    //    //お湯回収停止
    //    collect_output=LOW;
    //冷却水OFF
    cooling_water_output = LOW;
  }
  else
  {
    //電磁弁close
    solenoid_output = LOW;
    //冷却水回収停止
    collect_cooling_water_output = LOW;

    //    sUrea = eRESET;//ここでリセットにもっていくと、水回収してプログラム先頭に戻ったかどうかわからない
    sUrea = eEND;
    urea_flag = false;
    FlexiTimer2::stop();
    times = 0;
  }
}

//各素子の状態を出力
void apply()
{
  digitalWrite(air_motor, air_output);
  digitalWrite(solenoid_valve, solenoid_output);
  digitalWrite(water_pump, water_output);
  digitalWrite(collect_pump, collect_output);
  //  digitalWrite(fan, neck_output);
  //  digitalWrite(vibration, neck_output); //振動on
  digitalWrite(cooling_pump, cooling_water_output);
  digitalWrite(collect_cooling_pump, collect_cooling_water_output);
}

//回収用ポンプのテスト--------------------------------------------------------------------------------------------
void CollectWaterTest()
{
  collect_output = HIGH;
  collect_cooling_water_output = HIGH;
  digitalWrite(collect_pump, collect_output);                       //停止時はｒコマンドを入力
  digitalWrite(collect_cooling_pump, collect_cooling_water_output); //停止時はｒコマンドを入力
}

//水を送るポンプのテスト---------------------------------------------------------------------------------------------
void WaterTest()
{
  digitalWrite(water_pump, HIGH);
  delay(water_time);
  digitalWrite(water_pump, LOW);
}

//冷却水を送るポンプのテスト---------------------------------------------------------------------------------------------
void CoolingWaterTest()
{
  digitalWrite(cooling_pump, HIGH);
  delay(cooling_water_time);
  digitalWrite(cooling_pump, LOW);
}

//振動子のテスト--------------------------------------------------------------------------------------------
void VibrationTest()
{
  digitalWrite(vibration, HIGH); //振動on
  delay(vibration_time);         //ファンと同じ時間
  digitalWrite(vibration, LOW);  //振動off
}

//ファンのテスト--------------------------------------------------------------------------------------------
void FanTest()
{
  digitalWrite(fan, HIGH);
  delay(fan_time);
  digitalWrite(fan, LOW);
}

//首のテスト-------------------------------------------------------------------------------------------
void NeckTest()
{
  digitalWrite(vibration, HIGH);
  digitalWrite(fan, HIGH);
  delay(neck_time);
  digitalWrite(vibration, LOW);
  digitalWrite(fan, LOW);
}

//強制リセット--------------------------------------------------------------------------------------------
void reset()
{
  t = 0;
  times = 0;
  FlexiTimer2::stop();

  air_output = LOW;
  solenoid_output = LOW;
  water_output = LOW;
  collect_output = LOW;
  neck_output = LOW;
  cooling_water_output = LOW;
  collect_cooling_water_output = LOW;

  for (int i = 0; i <= ELE_NUM; i++)
    digitalWrite(i, LOW);

  urea_flag = false;
  collect_flag = false;

  swInputPressedNum = 0;

  ledcon.init();
}
//add--------------------
//一時停止
void pause()
{

  FlexiTimer2::stop();

  for (int i = 0; i <= 10; i++)
    digitalWrite(i, LOW);

  urea_flag = false;
  collect_flag = false;
}

void reUrea()
{ //resume
  FlexiTimer2::start();
  urea_flag = true;
}
//add--------------------

//セットアップ--------------------------------------------------------------------------------------------
void setup()
{

  //Acutuators pin setup
  for (int i = 2; i <= ELE_NUM; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  ledcon.init();
  button.init();

  Serial.begin(9600);
}

//main部分--------------------------------------------------------------------------------------------------------

void loop()
{

  //スイッチ入力取得 -------------------------------------------------------
  button.checkBuffer();
  if (button.isPressed())
  {
    switch (button.getData())
    {
    case swInputID: // 開始、一時停止、再開
    case swInputID + '0':
      swInputPressedNum++; //0スタート.  押された回数 : swInputPressedNum

      if (swInputPressedNum == 1) //開始
      {
        //失禁開始
        //Serial.println("Urea switch ON");
        urea_flag = true;
      }
      else if (swInputPressedNum == 2) //一時停止
      {
        //Serial.println("\n---- 一時停止 ----"); //debug
        pause();
      }
      else if (swInputPressedNum == 3) //再開
      {
        //Serial.println("\n---- 再開 ----"); //debug
        reUrea();
        swInputPressedNum = 1; // 次押されたら一時停止(2)へ
      }
      else
      {
      }

      break;

    case swResetID: // リセット
    case swResetID + '0':
      //Serial.println("Reset");
      reset();
      break;

    case swCollectID:
    case swCollectID + '0':
      //Serial.println("Collect");
      sUrea = eWATER_COLLECTING;
      collect_flag = true; //停止時はｒコマンド入力
      CollectWaterTest();
      break;
    }
  }

  if (urea_flag)
  {
    Urea();  // 各素子の状態を変更
    apply(); // 各素子の状態を適用
  }

  //LED
  ledcon.update(urea_flag, sUrea);
}
#if 0
  Serial.print(t);
  Serial.print("    ");
  Serial.print(String(times, 4));
  Serial.print("/");
  Serial.print(float(air_pump_time + blank_time + water_time + +blank_time2 + cooling_water_time + collect_delay));
  Serial.print("    ");
  Serial.print("エアー：");
  Serial.print(air_output);
  Serial.print("  電磁弁：");
  Serial.print(solenoid_output);
  Serial.print("  お湯：");
  Serial.print(water_output);
  Serial.print("  お湯回収ポンプ");
  Serial.print(collect_output);
  Serial.print("  冷却水：");
  Serial.print(cooling_water_output);
  Serial.print("  冷却水回収ポンプ");
  Serial.println(collect_cooling_water_output);
#endif
#if 0
  // シリアル解析-------------------------------------------------------
  if (Serial.available() > 0)
  {                                     //シリアルモニタに文字が入力されていたら
    str = Serial.readStringUntil('\r'); //コマンドをstrに格納

    len = str.length();            //コマンド長さを取得
    str.toCharArray(input, ++len); //strの中身をchar型の配列inputに格納

    str = String(strtok(input, ","));     //inputの中身を','で分ける
    command[0] = atoi(strtok(NULL, ",")); //同じように書けば引数はもっと増やせる　現状１個しか使ってないので
    Serial.println();
    Serial.println(str);
    if (str == "air")
    { //エアー

      air_max_flag = false;
      read_ini_flag = false;
      release_flag = false;
    }
    else if (str == "U")
    { //全体通し
      urea_flag = true;
      Urea();
      //air_max_flag = false;
      //read_ini_flag = false;
      //release_flag = false;
    }
    else if (str == "airtime")
    { //旧エアー
      pumpTest();
    }
    else if (str == "r")
    { //リセット
      reset();
      //exit(1);
    }
    else if (str == "p")
    { //エアーの最大値を設定 [p,設定したい値]でOK
      air_value(command[0]);
    }
    else if (str == "collect")
    {                      //回収用水袋のテスト
      collect_flag = true; //停止時はｒコマンド入力
      digitalWrite(led_pin[ _sUrVa ], HIGH); // 点灯
      // 他を消す
      for(int i=0; i<LED_NUM; i++){
        if( i != _sUrea ) digitalWrite(led_pin[ i ], LOW);
      }
      CollectWaterTest();
    }
    else if (str == "water")
    { //水のテスト
      WaterTest();
    }
    else if (str == "cooling_water")
    { //冷却水のテスト
      CoolingWaterTest();
    }
    else if (str == "vibration")
    { //振動子のテスト
      VibrationTest();
    }
    else if (str == "fan")
    { //ファンのテスト
      FanTest();
    }
    else if (str == "neck")
    { //首のテスト
      NeckTest();
    }
    else
    {
      Serial.print("> error [");
      Serial.print(str);
      Serial.println("]");
    }
  }
#endif
//---------------------------------------------------
////気圧の読み取り------------------------------------------------------------------------------------------------
//float air_read() {
//  int input0 = analogRead(7);
//  if (read_ini_flag == false) {
//    ini = input0;
//    read_ini_flag = true;
//  }
//  int input1 = analogRead(7);
//  int dif = input1 - ini;
//
//  float mV = float(dif) * 5000.0 / 1024.0; //センサ値をボルトに変換
//
//  Serial.println(mV);
//  return mV;
//}
////気圧を下げる----------------------------------------------------------------------------------------------
//void air_check() {
//  float air_value = air_read();
//  if (air_value < air_target && release_flag == true && air_max_flag == false) {
//    delay(1000);
//    //digitalWrite(solenoid_valve,HIGH);
//
//    digitalWrite(air_motor, LOW);
//    air_target += 40;
//    release_flag = false;
//
//  }
//}
////気圧の制御---------------------------------------------------------------------------------------------
//void air_control() {
//  float air_value = air_read();
//
//  if (air_max_flag == false)
//    air_supply();
//
//  if (air_value >= air_target && air_max_flag == false) {
//    //digitalWrite(solenoid_valve,LOW);
//    digitalWrite(air_motor, LOW);
//    release_flag = true;
//  }
//  if (air_value >= air_max) {
//    air_max_flag = true;
//    digitalWrite(air_motor, LOW);
//    air_target = 40;
//    if (urea_flag == true) {
//      delay(1000);
//      digitalWrite(solenoid_valve, HIGH);
//      Urea();
//    }
//    delay(1000);
//  }
//  if (air_max_flag == true) {
//
//    digitalWrite(solenoid_valve, HIGH);
//  }
//
//
//}

//----------------------------------------------------------------------------------------------