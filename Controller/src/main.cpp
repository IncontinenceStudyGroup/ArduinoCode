#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

//  Multi-threading procedures
void TaskSwcReception(void *pvParameters);
void TaskDataReception(void *pvParameters);
void TaskLedUpdate(void *pvParameters);

/** LEDs behaviour status
 * 
 * - Every two bits refers to a LED status
 * - The lowest two bits is one of the led 0 (white led)
 *    0b |  00  |  00  |  00  |  00  |
 *       | led3 | led2 | led1 | led0 |
 *       |green2|green1|green0|white |
 * - Each LED will behaves as follows:
 * -- 00: continuously OFF
 * -- 11: continuously ON
 * -- 01: Blink at slow rate
 * -- 10: Blink at rapid rate 
 *  
 */
byte led_behaviour_code = 0b00000000;

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

  //  Prepare Executable Multi-THread Tasks
  xTaskCreate(
      TaskSwcReception,
      (const portCHAR *)"Swc",
      128,
      NULL,
      2,
      NULL);
  xTaskCreate(
      TaskDataReception,
      (const portCHAR *)"Data",
      128,
      NULL,
      1,
      NULL);
  xTaskCreate(
      TaskLedUpdate,
      (const portCHAR *)"Led",
      128,
      NULL,
      2,
      NULL);
  led_behaviour_code = 0b00000000;
}

void loop() {}

void TaskSwcReception(void *pvParameters)
{
  byte swc[3] = {0};
  byte status = 0;
  byte swccode = 0;

  while (1)
  {
    (void)pvParameters;

    //  Delay the task for 50 ms
    vTaskDelay(80 / portTICK_PERIOD_MS);

    //  Get States of switches
    for (int i = 0; i < 3; i++)
      swc[i] = digitalRead(6 + i);

    //  The status of switches at a moment is saved as a byte
    //  A prev status is saved as a code in swccode
    swccode = status;
    status = 0;
    for (int i = 0; i < 3; i++)
    {
      status += (swc[i] << i);
    }

    // Send the code when the switces get RELEASED
    if (swccode > 0 && status == 0)
      Serial.write(swccode);
      //Serial.println(swccode, BIN);
  }
}

void TaskDataReception(void *pvParameters)
{
  (void)pvParameters;
  int tmp = 0;

  while (1){
    if (Serial.available() > 0){
      tmp = Serial.read();

      //  For debug
      //  following statement could do bad if you use integer>='0' as a code
      //if (tmp >= '0') tmp -= '0';

      led_behaviour_code = tmp;
    }
  }

}
void TaskLedUpdate(void *pvParameters)
{
  (void)pvParameters;

  bool led[4];
  bool flag[4] = {0};

LOOPLEDUPDATE:
  for (int i = 0; i < 4; i++)
  {
    byte tmp = (led_behaviour_code >> (i * 2)) & 0b11;
    switch (tmp)
    {
    case 0b00:
      led[i] = 0;
      break;
    case 0b11:
      led[i] = 1;
      break;
    case 0b01: //led state turns onec every two loop
      if (flag[i])
        led[i] = !led[i];
      flag[i] = !flag[i];
      break;
    case 0b10: //led state turns every loop
      led[i] = !led[i];
      break;
    }
    //Serial.write(tmp);
    digitalWrite(2 + i, led[i]);
  }
  vTaskDelay(250 / portTICK_PERIOD_MS);
  goto LOOPLEDUPDATE;
}