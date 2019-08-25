#ifndef _SIGRECIEVER_H_
#define _SIGRECIEVER_H_
#include "CtrlComHandler.h"

class SigReciever {
    public:
    SigReciever();
    ~SigReciever();
    void init();
    //  Todo: make chakeBuffer an another therad transaction
    void checkBuffer();
    bool isPressed();
    unsigned char getData();

    private:
    unsigned char m_data;
    CtrlComHandler m_serialcom;
};

#endif