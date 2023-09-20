#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>
#include <BleMouse.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)
#define TIME_INACTIVITY (1000 * 60)

Button2 key;
volatile int count = 0;
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);
BleMouse bleMouse;
unsigned long lastActivityTime = 0;

void keepActive()
{
  lastActivityTime = millis();
}

bool isIdle()
{
  return millis() - lastActivityTime >= TIME_INACTIVITY;
}

IRAM_ATTR void onPress(Button2 &btn)
{
  digitalWrite(PIN_LED, HIGH);
}

IRAM_ATTR void onRelease(Button2 &btn)
{
  digitalWrite(PIN_LED, LOW);
  count++;
  Serial.println(count);
  bleMouse.click(MOUSE_LEFT);
}

void IRAM_ATTR readEncoderISR()
{
  rotary.readEncoder_ISR();
}

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);

  key.begin(PIN_KEY);
  key.setPressedHandler(onPress);
  key.setReleasedHandler(onRelease);
  attachInterrupt(digitalPinToInterrupt(PIN_KEY), keepActive, CHANGE);

  pinMode(PIN_ROTARY_LEFT, INPUT_PULLUP);
  pinMode(PIN_ROTARY_RIGHT, INPUT_PULLUP);
  rotary.begin();
  rotary.setup(readEncoderISR);
  rotary.areEncoderPinsPulldownforEsp32 = false;
  rotary.setAcceleration(0);

  bleMouse.begin();
}

void loop()
{
  if (!bleMouse.isConnected())
  {
    Serial.println("Bluetooth disconnected. Attempting to reconnect...");
    bleMouse.begin();
    delay(5000);
    return;
  }

  if (isIdle())
  {
    Serial.println("Device is idle. Disconnecting...");
    bleMouse.end();
    delay(5000);
    return;
  }

  key.loop();

  int res = rotary.encoderChanged();
  if (res != 0)
  {
    keepActive();
    bleMouse.move(0, 0, res);
  }
}
