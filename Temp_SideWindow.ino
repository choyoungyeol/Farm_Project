#include <Sensirion.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define Relay1 8
#define Relay2 9
#define Relay3 10
#define Relay4 11

const uint8_t dataPin  =  2;
const uint8_t clockPin =  3;

float temperature;
float humidity;
float dewpoint;

const int LowTemp = 20;
const int HighTemp = 30;

Sensirion tempSensor = Sensirion(dataPin, clockPin);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Jeju Nat'l Univ.");
  lcd.setCursor(0, 1);
  lcd.print("Vegetables Lab.");
  delay(5000);
}

void loop()
{
  tempSensor.measure(&temperature, &humidity, &dewpoint);

  //  Serial.print("Temperature: ");
  //  Serial.print(temperature);
  //  Serial.print(" C, Humidity: ");
  //  Serial.print(humidity);
  //  Serial.print(" %, Dewpoint: ");
  //  Serial.print(dewpoint);
  //  Serial.println(" C");
  //  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" oc");
  lcd.setCursor(0, 1);
  lcd.print("Humi: ");
  lcd.print(humidity);
  lcd.print(" %");
  delay(5000);

  if (temperature > HighTemp) {
    digitalWrite(Relay1, HIGH);
    delay(5000);
    digitalWrite(Relay2, LOW);
    digitalWrite(Relay3, HIGH);
    delay(5000);
    digitalWrite(Relay4, LOW);
  }

  if (temperature < LowTemp) {
    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, HIGH);
    delay(5000);
    digitalWrite(Relay3, LOW);
    digitalWrite(Relay4, HIGH);
    delay(5000);
  }
}
