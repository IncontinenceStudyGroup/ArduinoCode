#include <Arduino.h>
#include "SigReciever.h"

SigReciever::SigReciever(){}
SigReciever::~SigReciever(){}

void SigReciever::init(){
    m_serialcom.init();
}

void SigReciever::checkBuffer(){
    if(m_serialcom.available() > 0)
        m_data = m_serialcom.read();
}

bool SigReciever::isPressed(){
    return (m_data > 0);
}

unsigned char SigReciever::getData(){
    unsigned char retval = m_data;
    m_data = 0;
    return retval;
}