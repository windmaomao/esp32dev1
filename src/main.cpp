#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>
#include <BleMouse.h>
#include <RGBLed.h>
#include <arduino-timer.h>

#define PIN_LED (2)
#define PIN_ROTARY_KEY (4)
#define PIN_ROTARY_LEFT (23)
#define PIN_ROTARY_RIGHT (22)
#define PIN_TOUCH (32)
#define PIN_RED (26)
#define PIN_GREEN (27)
#define PIN_BLUE (25)
#define PIN_KEY2 (21)

#define uS_TO_S_FACTOR 1000000
#define mS_TO_S_FACTOR 1000
#define IDLE_TIME 20

AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);
BleMouse bleMouse;
RGBLed rgbLed(PIN_RED, PIN_GREEN, PIN_BLUE, RGBLed::COMMON_CATHODE);
Button2 scrollToggler;
volatile bool scrollOn = false;
Button2 focusButton;
Timer<10> timer;
bool bluetoothOn = true;
Timer<>::Task idleTask;

bool onDisconnecting(void *arguments)
{
  rgbLed.flash(RGBLed::YELLOW, 20, 50);
  return !bluetoothOn;
}

bool checkBleStatus()
{
  bool on = bleMouse.isConnected();
  if (on == bluetoothOn)
    return on;

  bluetoothOn = on;
  if (!bluetoothOn)
  {
    timer.every(1000, &onDisconnecting);
  }

  return bluetoothOn;
}

bool onIdle(void *argument)
{
  Serial.println("Idle");
  rgbLed.flash(RGBLed::RED, 20);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_ROTARY_KEY, LOW);
  delay(1000);
  esp_deep_sleep_start();
  return false;
}

void heartBeat()
{
  if (idleTask != nullptr)
  {
    timer.cancel(idleTask);
  }

  idleTask = timer.in(IDLE_TIME * mS_TO_S_FACTOR, &onIdle);
}

bool flashHeartBeat(void *arguments)
{
  digitalWrite(PIN_LED, HIGH);
  timer.in(500, [](void *arguments)
           { digitalWrite(PIN_LED, LOW); return false; });
  return true;
}

bool touched()
{
  return digitalRead(PIN_TOUCH) == 1;
}

byte myButtonStateHandler()
{
  return digitalRead(PIN_TOUCH);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Start");

  pinMode(PIN_LED, OUTPUT);
  timer.every(1000, &flashHeartBeat);
  pinMode(PIN_TOUCH, INPUT);

  pinMode(PIN_ROTARY_KEY, INPUT_PULLUP);
  pinMode(PIN_ROTARY_LEFT, INPUT_PULLUP);
  pinMode(PIN_ROTARY_RIGHT, INPUT_PULLUP);
  rotary.begin();
  rotary.setup([]()
               { rotary.readEncoder_ISR(); });
  rotary.areEncoderPinsPulldownforEsp32 = false;
  rotary.setAcceleration(0);

  bleMouse.begin();

  rgbLed.off();

  focusButton.setButtonStateFunction(myButtonStateHandler);
  focusButton.setTapHandler([](Button2 &btn)
                            { bleMouse.click(MOUSE_BACK); });
  focusButton.begin(VIRTUAL_PIN);

  heartBeat();
}

void loop()
{
  timer.tick();

  if (touched())
  {
    heartBeat();
  }

  if (!checkBleStatus())
    return;

  scrollToggler.loop();
  focusButton.loop();

  int res = rotary.encoderChanged();
  if (res != 0)
  {
    bleMouse.move(0, 0, res);
    heartBeat();
  }
}
