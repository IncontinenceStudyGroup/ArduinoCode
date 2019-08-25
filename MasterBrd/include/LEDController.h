#ifndef _LEDCONTROLLER_H_ 
#define _LEDCONTROLLER_H_
#include "CtrlComHandler.h"

extern boolean collect_flag;

enum stateUrea{
  eRESET = 0,	        // 4からはじめる
                      //      起動直後 / リセットボタンを押して、開始ボタンが押されるのを待っている状態
  eAIR_SUPPLYING,     // : 1  開始ボタンが押さた後 エアー送り中
  eWATER_SUPPLYING,   // : 2  水送り中
  eWATER_COLLECTING,  // : 3  水回収中
  eEND                // : 4  水送りが終わって、押しボタンによる水回収開始を待っている状態 
}; 

class LEDController {
  private:
  CtrlComHandler m_serialcom;

  public:
  LEDController();
  ~LEDController();
  void init();
  void update(bool _pauseFlag, enum stateUrea _sUrea);
};


#endif
