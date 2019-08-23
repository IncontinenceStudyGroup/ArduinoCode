#include <Arduino.h>

bool flag = false;
bool sflag = false;
int t = 0;
int swc[3] = {0};
int pressed = 0;
int lastprs;
int released = 0;
int led[4];
int cmd = 0;

void setup()
{
  // put your setup code here, to run once:
  // LEDs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  // Switches
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  Serial.begin(9600);
}

void loop()
{
  delay(2);
  for (int i = 0; i < 3; i++)
    swc[i] = digitalRead(6 + i);
  lastprs = pressed;
  pressed = 0;
  for (int i = 0; i < 3; i++)
    pressed += (swc[i] << i);

  if (lastprs > 0 && pressed == 0)
    released = 1;
  else
    released = 0;

  if (released > 0)
  {
    Serial.write(lastprs);
  }

  if (Serial.available() > 0)
  {
    cmd = Serial.read();
    for (int i = 0; i < 3; i++)
    {
      led[i] = ((cmd >> i) & 0b1);
      digitalWrite(2 + i, led[i]);
    }
  }

  /*
  if (cmd == 'a')
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
  */
}