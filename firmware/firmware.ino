#include <Mouse.h>

int CLICK_NUMBER = 0x1337;

int MOUSE_DELAY = 10;
int SET_DELAY_NUMBER = 0x1338;
int SET_DELAY = 0;

void setup()
{
  Mouse.begin();
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0)
  {
    String checkStr = Serial.readStringUntil('\0');
    int checkNumber = checkStr.toInt();

    if (SET_DELAY == 1)
    {
      MOUSE_DELAY = checkNumber;
      SET_DELAY = 0;
      return;
    }

    if (checkNumber == SET_DELAY_NUMBER)
    {
      SET_DELAY = 1;
      return;
    }

    if (checkNumber == CLICK_NUMBER)
    {
      delay(MOUSE_DELAY);
      Mouse.press(MOUSE_LEFT);
      delay(3);
      Mouse.release(MOUSE_LEFT);
      delay(1);
    }
  }
}
