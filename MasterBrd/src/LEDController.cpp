#include <Arduino.h>
#include "LEDController.h"
extern stateUrea sUrea;

LEDController::LEDController() {}

LEDController::~LEDController() {}

void LEDController::init()
{
  //  Extinguish the leds
  m_serialcom.init();
  m_serialcom.write(0b00000000);
  sUrea = eRESET;
}

void LEDController::update(bool _pauseFlag, enum stateUrea _sUrea)
{
  bool pauseFlag = !_pauseFlag; //urea_flag

  // 終わったら、水回収LEDを高速点灯
  if (_sUrea == eEND)
  {
    // Make a Watercollecting LED blink at 0.5 Hz
    m_serialcom.write(0b10000000);
    return;
  }

  // 実行中 --------------------------------------------------------------------
  //    その状態に対応するLEDを点灯させる
  if (!pauseFlag || _sUrea == eRESET                   // RESETのときは、pauseFlagが下りてるので例外 「resetを実行中」という意味
      || (collect_flag && _sUrea == eWATER_COLLECTING) // 水回収ボタンから呼ばれたときはcollect_flagが立つのでそれで判定
  )
  {
    byte data = (0b11 << _sUrea * 2);
    m_serialcom.write(data);
  }
  // 一時停止中 -------------------------------------
  //    その状態に対応するLEDを点滅させる
  else
  {
    byte data = (0b10 << _sUrea * 2);
    m_serialcom.write(data);
  }
}