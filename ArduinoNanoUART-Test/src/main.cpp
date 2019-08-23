#include <Arduino.h>

bool flag = false;
bool sflag = false;
int t = 0;
void setup()
{
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);

  Serial.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  int c = Serial.read();
  int s = digitalRead(3);

  if (c == 'a')
  {
    if (flag)
    {
      digitalWrite(2, LOW);
      Serial.println("let it LOW");
      flag = false;
    }
    else
    {
      digitalWrite(2, HIGH);
      Serial.println("let it HIGH");
      flag = true;
    }
  }
  if (s == HIGH)
  {
    if (sflag)
    {
      Serial.print('a');
      sflag = false;
    }
  }
  else
  {
    t++;
    if (t == 1000)
    {
      sflag = true;
      t = 0;
    }
  }
}