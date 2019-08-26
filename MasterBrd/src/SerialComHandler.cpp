#include "Arduino.h"
#include "SerialComHandler.h"

SerialComHandler::SerialComHandler() {}
SerialComHandler::~SerialComHandler() {}

void SerialComHandler::init(int baudrate)
{
    if (!init_f)
        Serial.begin(baudrate);
    init_f = true;
}

bool SerialComHandler::isInitiated()
{
    return init_f;
}

int SerialComHandler::available()
{
    return Serial.available();
}

unsigned char SerialComHandler::read()
{
    return Serial.read();
}

void SerialComHandler::write(unsigned char data)
{
    Serial.write(data);
}