#include <SimpleTimer.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
SimpleTimer timer;

#define DESK_HEIGHT 90
#define STAND_TIME_SECONDS 45 * 60
#define SIT_TIME_SECONDS 15 * 60

#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define DEBOUNCE_DELAY_MS 2000

#define DOWN false
#define UP true

bool deskStatus = DOWN;
bool lastDeskStatus = DOWN;
int countdownSeconds = 0;
long lastCheckingTime = 0;
bool lastCheckStatus = DOWN;

void setup() {
  lcd.begin();

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  timer.setInterval(1000, countdown);

  lcd.backlight();
}

void countdown() {
  if (countdownSeconds > 0) {
    countdownSeconds--;
  }
}

bool isDeskUp() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  int duration = pulseIn(ECHO_PIN, HIGH);
  int distanceCm = duration*0.034/2;

  return distanceCm > DESK_HEIGHT;
}

void checkDeskStatus() {
  bool currentStatus = isDeskUp();
  // debounce, ultrasonic sensor is pretty sensitive
  if (currentStatus != lastCheckStatus) {
    lastCheckingTime = millis();
  }

  if ((millis() - lastCheckingTime) > DEBOUNCE_DELAY_MS) {
    if (currentStatus != deskStatus) {
      deskStatus = currentStatus;
    }
  }

  lastCheckStatus = currentStatus;
}

int getCountdownMax() {
  if (lastDeskStatus) {
    return STAND_TIME_SECONDS;
  }
  return SIT_TIME_SECONDS;
}

void loop() {
  timer.run();
  checkDeskStatus();

  lcd.setCursor(0,0); 
  lcd.print("Status: "); 
  if (deskStatus == UP) {
    lcd.print("up  ");  
  } else {
    lcd.print("down");
  }

  if (deskStatus != lastDeskStatus) {
    lastDeskStatus = deskStatus;
    countdownSeconds = getCountdownMax();
  }
  
  lcd.setCursor(0,1); 
  lcd.print("Countdown: ");
  lcd.print(countdownSeconds);
  lcd.print("   "); 
}
