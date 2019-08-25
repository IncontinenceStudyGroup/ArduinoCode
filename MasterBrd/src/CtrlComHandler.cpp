#include "Arduino.h"
#include "CtrlComHandler.h"

CtrlComHandler::CtrlComHandler(){}
CtrlComHandler::~CtrlComHandler(){}

void CtrlComHandler::init(int baudrate){
    if(!init_f) Serial.begin(baudrate);
    init_f = true;
}

bool CtrlComHandler::isInitiated(){
    return init_f;
}

int CtrlComHandler::available(){
    return Serial.available();
}

unsigned char CtrlComHandler::read(){
    return Serial.read();
}

void CtrlComHandler::write(unsigned char data){
    Serial.write(data);
}