#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>
#include <BleMouse.h>
#include <RGBLed.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)
#define TIME_INACTIVITY (1000 * 60 * 5)
#define PIN_RED (26)
#define PIN_GREEN (27)
#define PIN_BLUE (25)
#define PIN_KEY2 (23)

Button2 clickButton;
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);
BleMouse bleMouse;
unsigned long lastActivityTime = 0;
RGBLed rgbLed(PIN_RED, PIN_GREEN, PIN_BLUE, RGBLed::COMMON_CATHODE);
hw_timer_t *timer = NULL;
Button2 scrollToggler;
volatile bool scrollOn = false;

void keepActive()
{
  lastActivityTime = millis();
  bleMouse.move(0, 0, 0, 1);
  rgbLed.fadeOut(RGBLed::BLUE, 100, 100);
}

bool isIdle()
{
  return millis() - lastActivityTime >= TIME_INACTIVITY;
}

void IRAM_ATTR readEncoderISR()
{
  rotary.readEncoder_ISR();
}

void timerISR()
{
  if (scrollOn)
  {
    bleMouse.move(0, 0, 1);
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);

  clickButton.begin(PIN_KEY);
  clickButton.setClickHandler([](Button2 &btn)
                              { bleMouse.click(MOUSE_LEFT); });

  pinMode(PIN_ROTARY_LEFT, INPUT_PULLUP);
  pinMode(PIN_ROTARY_RIGHT, INPUT_PULLUP);
  rotary.begin();
  rotary.setup(readEncoderISR);
  rotary.areEncoderPinsPulldownforEsp32 = false;
  rotary.setAcceleration(0);

  bleMouse.begin();

  rgbLed.off();

  timer = timerBegin(2, 80, true);
  timerAttachInterrupt(timer, &timerISR, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  scrollToggler.begin(PIN_KEY2);
  scrollToggler.setClickHandler([](Button2 &btn)
                                { scrollOn = !scrollOn; });
}

void loop()
{
  if (!bleMouse.isConnected())
  {
    rgbLed.fadeOut(RGBLed::RED, 5, 100);
    Serial.print(".");
    // Note: no delay can be used here, otherwise
    // the device will not be able to connect.
    return;
  }

  if (isIdle())
  {
    Serial.println("Device is idle. Keeping it alive...");
    keepActive();
    return;
  }

  clickButton.loop();
  scrollToggler.loop();

  int res = rotary.encoderChanged();
  if (res != 0)
  {
    bleMouse.move(0, 0, res);
  }
}
