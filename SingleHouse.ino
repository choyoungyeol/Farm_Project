#include <Sensirion.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define WindowR_O 22
#define WindowR_C 24
#define WindowL_O 26
#define WindowL_C 28
#define Fan 30
#define Ventilation 32

const uint8_t dataPin  =  2;
const uint8_t clockPin =  3;

float temperature;
float humidity;
float dewpoint;
float Low_VPD = 0.4;
float High_VPD = 1.4;
float Low_Temp = 12;
float High_Temp = 40;

int WindowR_O_Value = 0;
int WindowR_C_Value = 0;
int WindowL_O_Value = 0;
int WindowL_C_Value = 0;
int Fan_Value = 0;
int Ventilation_Value = 0;

Sensirion tempSensor = Sensirion(dataPin, clockPin);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  pinMode(WindowR_O, OUTPUT);
  pinMode(WindowR_C, OUTPUT);
  pinMode(WindowL_O, OUTPUT);
  pinMode(WindowL_C, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(Ventilation, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Jeju Nat'l Univ.");
  lcd.setCursor(0, 1);
  lcd.print("Hortic. Sci. Major");
  lcd.setCursor(0, 2);
  lcd.print("Vegetables Lab.");
  lcd.setCursor(0, 3);
  lcd.print("By Young-Yeol Cho");
  delay(5000);
}

void loop()
{
  tempSensor.measure(&temperature, &humidity, &dewpoint);

  float AH =  ((6.112 * exp((17.67 * temperature) / (temperature + 245.5)) * humidity * 18.02) / ((273.15 + temperature) * 100 * 0.08314));   //절대습도
  float Psat =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5))) / 10;  //포화수증기압 (단위 : kPa)
  float P =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5)) * (humidity / 100)) / 10;  //수증기압 (단위 : kPa)
  float VPD = (Psat - P); //수증기압차 (단위 : kPa)

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp : ");
  lcd.print(temperature);
  lcd.print("  oC");
  lcd.setCursor(0, 1);
  lcd.print("R H  : ");
  lcd.print(humidity);
  lcd.print("  %");
  lcd.setCursor(0, 2);
  lcd.print("A H  : ");
  lcd.print(AH);
  lcd.print("  g/m3");
  lcd.setCursor(0, 3);
  lcd.print("VPD  : ");
  lcd.print(VPD);
  lcd.print("  kPa");
  delay(5000);

  if (temperature < Low_Temp) {
    digitalWrite(WindowR_O, HIGH);
    WindowR_O_Value = 0;
    delay(1000);
    digitalWrite(WindowR_C, LOW);
    WindowR_C_Value = 1;
    delay(1000);
    digitalWrite(WindowL_O, HIGH);
    WindowL_O_Value = 0;
    delay(1000);
    digitalWrite(WindowL_C, LOW);
    WindowL_C_Value = 1;
    delay(1000);
    digitalWrite(Fan, HIGH);
    Fan_Value = 0;
    delay(1000);
    digitalWrite(Ventilation, HIGH);
    Ventilation_Value = 0;
    delay(1000);
  }
  if ((temperature >= Low_Temp) || (temperature <= High_Temp)) {
    if ((VPD < Low_VPD) || (VPD > High_VPD)) {
      digitalWrite(WindowR_O, LOW);
      WindowR_O_Value = 1;
      delay(1000);
      digitalWrite(WindowR_C, HIGH);
      WindowR_C_Value = 0;
      delay(1000);
      digitalWrite(WindowL_O, LOW);
      WindowL_O_Value = 1;
      delay(1000);
      digitalWrite(WindowL_C, HIGH);
      WindowL_C_Value = 0;
      delay(1000);
      digitalWrite(Fan, LOW);
      Fan_Value = 1;
      delay(1000);
      digitalWrite(Ventilation, LOW);
      Ventilation_Value = 1;
      delay(1000);
    }
  }
  if (temperature > High_Temp) {
    digitalWrite(WindowR_O, LOW);
    WindowR_O_Value = 1;
    delay(1000);
    digitalWrite(WindowR_C, HIGH);
    WindowR_C_Value = 0;
    delay(1000);
    digitalWrite(WindowL_O, LOW);
    WindowL_O_Value = 1;
    delay(1000);
    digitalWrite(WindowL_C, HIGH);
    WindowL_C_Value = 0;
    delay(1000);
    digitalWrite(Fan, LOW);
    Fan_Value = 1;
    delay(1000);
    digitalWrite(Ventilation, LOW);
    Ventilation_Value = 1;
    delay(1000);
  }

  String data = String(temperature) + "," + String(humidity) + "," + String(AH) + "," + String(VPD) + "," + String(WindowR_O_Value) + "," + String(WindowR_C_Value) + "," + String(WindowL_O_Value) + "," + String(WindowL_C_Value)+ "," + String(Fan_Value)+ "," + String(Ventilation_Value);
  Serial.println(data);
}
