#include <Arduino.h>

#define PIN_LED LED_BUILTIN
#define PIN_BUTTON 4

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(PIN_LED, LOW);
  Serial.println(digitalRead(PIN_BUTTON));
}
