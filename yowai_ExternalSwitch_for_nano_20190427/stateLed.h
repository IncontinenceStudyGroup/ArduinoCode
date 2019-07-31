#ifndef _STATELED_H_ // 多重インクルード防止 
#define _STATELED_H_

#define LED_NUM 4

const int led_pin[ LED_NUM ] = {A0, A1, A2, A3};// A0 - A3

extern boolean collect_flag;

enum stateUrea{
  eRESET = 0,        // 0からはじめる
                     //      起動直後 / リセットボタンを押して、開始ボタンが押されるのを待っている状態
  eAIR_SUPPLYING,    // : 1  開始ボタンが押さた後 エアー送り中
  eWATER_SUPPLYING,  // : 2  水送り中
  eWATER_COLLECTING, // : 3  水回収中
  eEND               // : 4  水送りが終わって、押しボタンによる水回収開始を待っている状態 
};
enum stateUrea sUrea = eRESET;

void ledInit(){
  for(int i=0; i<LED_NUM; i++){
    pinMode(led_pin[i], OUTPUT);
    digitalWrite(led_pin[i], LOW);
  }

  sUrea = eRESET;
}


// ledUpdate( urea_flag , sUrea);
void ledUpdate(boolean _pauseFlag, enum stateUrea _sUrea)
{
  int blinkTime = blinkTime = millis() % 1000; //0;
  boolean pauseFlag = !_pauseFlag;//urea_flag


  if( _sUrea == eEND ){// 終わったら、水回収LEDを高速点灯
    // 他を消す
    for(int i=0; i<LED_NUM; i++){
      if( i != eWATER_COLLECTING ) digitalWrite(led_pin[ i ], LOW);
    }//↑消さずに残しておくのもよいかも？
    
    if( blinkTime < 250
    || (500 <= blinkTime && blinkTime < 750) 
    ){
      digitalWrite(led_pin[ eWATER_COLLECTING ], HIGH); // 点灯
    }else{
      digitalWrite(led_pin[ eWATER_COLLECTING ], LOW); // 消灯
    }
  }
  else
  {
    // 実行中 --------------------------------------------------------------------
    //    その状態に対応するLEDを点灯させる
    if( !pauseFlag
     || _sUrea == eRESET                            // RESETのときは、pauseFlagが下りてるので例外 「resetを実行中」という意味
     || collect_flag && _sUrea == eWATER_COLLECTING // 水回収ボタンから呼ばれたときはcollect_flagが立つのでそれで判定
    ){
      digitalWrite(led_pin[ _sUrea ], HIGH); // 点灯
      // 他を消す
      for(int i=0; i<LED_NUM; i++){
        if( i != _sUrea ) digitalWrite(led_pin[ i ], LOW);
      }
    }
    // 一時停止中 -------------------------------------
    //    その状態に対応するLEDを点灯させる
    else{
      // 他を消す
      for(int i=0; i<LED_NUM; i++){
        if( i != _sUrea ) digitalWrite(led_pin[ i ], LOW);
      }
      
    //    blinkTime = millis() % 1000;
      if(blinkTime < 500){
        digitalWrite(led_pin[ _sUrea ], HIGH); // 点灯
      }else{
        digitalWrite(led_pin[ _sUrea ], LOW); // 消灯
      }
      
    }
  }
}
/*
!pause_flag && eRESET　　　　　待機中
!pause_flag && eAIR_SUPPLYING　
!pause_flag && eWATER_SUPPLYING
!pause_flag && eWATER_COLLECTING

pause_flag && eRESET            あり得ない
pause_flag && eAIR_SUPPLYING　　エアー送り途中
pause_flag && eWATER_SUPPLYING　水送り途中　じらし
pause_flag && eWATER_COLLECTING　水回収途中　止める必要ある？

水回収し終わったらどうなる？
最初に戻すようにする？

*/

/*

//enum stateMain{
//  eRESET,
//  ePAUSE
//};
//boolean pauseFlag = false;//UreaFlagと一緒
#define STATE_LED_NUM 4
enum stateUrea{
  eRESET,
  eAIR_SUPPLYING,
  eWATER_SUPPLYING,
  eWATER_COLLECTING
};
const int stateLedPin[STATE_LED_NUM] = {14,15,16,17};
void ledDrive(boolean sPause, enum stateUrea sUrea){
  unsigned int blinkTime = 0;
  
  if( ! sPause )
  {
    stateLedOn( sUrea );
  }else
  {
    blinkTime = millis() % 1000;
    
    if( blinkTime < 500 )
    {
      stateLedOn( sUrea );
    }else // blinkTime >= 500
    {
      stateLedOff( sUrea );
    }
  }
}
void stateLedOn(enum stateUrea id){// 0 ～ STATE_LED_NUM-1
  digitalWrite( stateLedPin[ id ], HIGH );
}
void stateLedOff(enum stateUrea id){// 0 ～ STATE_LED_NUM-1
  digitalWrite( stateLedPin[ id ], LOW );
}
//void stateLedOn(int _id){// 0 ～ STATE_LED_NUM-1
//  int id = constrain( _id , 0, STATE_LED_NUM-1 );
//  digitalWrite( stateLedPin[ id ], HIGH );
//}
//void stateLedOff(int _id){// 0 ～ STATE_LED_NUM-1
//  int id = constrain( _id , 0, STATE_LED_NUM-1 );
//  digitalWrite( stateLedPin[ id ], LOW );
//}

*/

#endif
