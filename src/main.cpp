#include <Arduino.h>
#include <JC_Button.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY_1 (4)

Button key1(PIN_KEY_1);

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  key1.begin();
}

void loop()
{
  key1.read();
  if (key1.wasPressed())
  {
    digitalWrite(PIN_LED, HIGH);
  }
  if (key1.wasReleased())
  {
    digitalWrite(PIN_LED, LOW);
  }
}
