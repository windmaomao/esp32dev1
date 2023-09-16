#include <Arduino.h>
#include <Button2.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)

Button2 key;

void onPress(Button2 &btn)
{
  digitalWrite(PIN_LED, HIGH);
}

void onRelease(Button2 &btn)
{
  digitalWrite(PIN_LED, LOW);
}

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  key.begin(PIN_KEY);
  key.setPressedHandler(onPress);
  key.setReleasedHandler(onRelease);
}

void loop()
{
  key.loop();
}
