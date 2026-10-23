#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 14, 27, 26, 25, 33);

RTC_DS1307 RTC;

#define NEXT_BUTTON 23
#define INC_BUTTON  4
#define SET_BUTTON  32

#define RED_LED     16
#define YELLOW_LED  17
#define GREEN_LED   18

#define BUZZER 19

#define EEPROM_SIZE 10

#define TIME1_HOUR  0
#define TIME1_MIN   1
#define TIME2_HOUR  2
#define TIME2_MIN   3
#define TIME3_HOUR  4
#define TIME3_MIN   5

int HOUR = 0;
int MINUT = 0;
int SECOND = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);
  EEPROM.begin(EEPROM_SIZE);
  lcd.begin(16, 2);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(INC_BUTTON, INPUT_PULLUP);
  pinMode(SET_BUTTON, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  allOff();
  if (!RTC.begin())
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC ERROR!");
    while (1);
  }
  if(!RTC.isrunning())
  {
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Medicine");
  lcd.setCursor(0, 1);
  lcd.print("Reminder System");

  delay(2000);

  lcd.clear();
}
void loop()
{
  DateTime now = RTC.now();

  HOUR = now.hour();
  MINUT = now.minute();
  SECOND = now.second();
  if (digitalRead(SET_BUTTON) == LOW)
  {
    delay(50);

    if (digitalRead(SET_BUTTON) == LOW)
    {
      setMedicineTimes();

      while (digitalRead(SET_BUTTON) == LOW);
    }
  }
  displayDateTime();
  checkMedicine();

  delay(500);
}

void displayDateTime()
{
  DateTime now = RTC.now();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Time:");

  if (now.hour() < 10)
    lcd.print("0");

  lcd.print(now.hour());
  lcd.print(":");

  if (now.minute() < 10)
    lcd.print("0");

  lcd.print(now.minute());
  lcd.print(":");

  if (now.second() < 10)
    lcd.print("0");

  lcd.print(now.second());

  lcd.setCursor(0, 1);
  lcd.print("Date:");

  if (now.day() < 10)
    lcd.print("0");

  lcd.print(now.day());
  lcd.print("/");

  if (now.month() < 10)
    lcd.print("0");

  lcd.print(now.month());
  lcd.print("/");

  lcd.print(now.year());
}
void setMedicineTimes()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Medicine");
  lcd.setCursor(0, 1);
  lcd.print("Reminder Times");

  delay(2000);
  setTime(1, TIME1_HOUR, TIME1_MIN);
  setTime(2, TIME2_HOUR, TIME2_MIN);
  setTime(3, TIME3_HOUR, TIME3_MIN);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Times Saved!");
  lcd.setCursor(0, 1);
  lcd.print("Reminder Ready");

  delay(2000);
}


void setTime(int medicineNumber, int hourAddress, int minuteAddress)
{
  int hour = 0;
  int minute = 0;
  while (true)
  {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Medicine ");
    lcd.print(medicineNumber);

    lcd.setCursor(0, 1);

    if (hour < 10)
      lcd.print("0");

    lcd.print(hour);
    lcd.print(":");

    if (minute < 10)
      lcd.print("0");

    lcd.print(minute);

    lcd.print("  H");

    if (digitalRead(INC_BUTTON) == LOW)
    {
      hour++;

      if (hour >= 24)
        hour = 0;

      delay(250);
    }
    if (digitalRead(NEXT_BUTTON) == LOW)
    {
      delay(300);
      break;
    }

    delay(50);
  }

  while (true)
  {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Medicine ");
    lcd.print(medicineNumber);

    lcd.setCursor(0, 1);

    if (hour < 10)
      lcd.print("0");

    lcd.print(hour);
    lcd.print(":");

    if (minute < 10)
      lcd.print("0");

    lcd.print(minute);

    lcd.print("  M");
    if (digitalRead(INC_BUTTON) == LOW)
    {
      minute++;

      if (minute >= 60)
        minute = 0;

      delay(250);
    }
    if (digitalRead(NEXT_BUTTON) == LOW)
    {
      delay(300);
      break;
    }

    delay(50);
  }
  EEPROM.write(hourAddress, hour);
  EEPROM.write(minuteAddress, minute);

  EEPROM.commit();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Saved:");

  lcd.setCursor(0, 1);

  if (hour < 10)
    lcd.print("0");

  lcd.print(hour);
  lcd.print(":");

  if (minute < 10)
    lcd.print("0");

  lcd.print(minute);

  delay(1000);
}

void checkMedicine()
{
  int hour1 = EEPROM.read(TIME1_HOUR);
  int min1  = EEPROM.read(TIME1_MIN);

  int hour2 = EEPROM.read(TIME2_HOUR);
  int min2  = EEPROM.read(TIME2_MIN);

  int hour3 = EEPROM.read(TIME3_HOUR);
  int min3  = EEPROM.read(TIME3_MIN);

  if (HOUR == hour1 && MINUT == min1)
  {
    medicineAlert(
      "Take Group One",
      RED_LED
    );
  }

  if (HOUR == hour2 && MINUT == min2)
  {
    medicineAlert(
      "Take Group Two",
      YELLOW_LED
    );
  }

  if (HOUR == hour3 && MINUT == min3)
  {
    medicineAlert(
      "Take Group Three",
      GREEN_LED
    );
  }
}

void medicineAlert(const char *message, int led)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(message);

  lcd.setCursor(0, 1);
  lcd.print("Take Medicine");
  digitalWrite(led, HIGH);
  tone(BUZZER, 2000);
  delay(5000);
  noTone(BUZZER);
  digitalWrite(led, LOW);

  lcd.clear();
}

void allOff()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  noTone(BUZZER);
}