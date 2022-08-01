#include <LoRa.h>
#include <Sensirion.h>
#include <K30_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define Relay1 5
#define Relay2 6
#define Relay3 7
#define Relay4 8

SoftwareSerial BT(0, 1);

const uint8_t dataPin  =  3;
const uint8_t clockPin =  4;

float temperature;
float humidity;
float dewpoint;

int co2 = 0;
int rc  = 1;
int Relay1_value = 0;
int Relay2_value = 0;
int Relay3_value = 0;
int Relay4_value = 0;

String readString;

Sensirion tempSensor = Sensirion(dataPin, clockPin);
K30_I2C k30_i2c = K30_I2C(0x68);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay2, LOW);
  digitalWrite(Relay3, LOW);
  digitalWrite(Relay4, LOW);

  while (!Serial);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  lcd.init();
  lcd.backlight();
}

void loop()
{
  float sum_temperature = 0;
  float sum_humidity = 0;
  float sum_dewpoint = 0;
  float sum_co2 = 0;

  for (int i = 0; i < 100; i++) {            //평균온도  100*0.01 = 1초
    tempSensor.measure(&temperature, &humidity, &dewpoint);
    rc = k30_i2c.readCO2(co2);
    sum_temperature = sum_temperature + temperature;
    sum_humidity = sum_humidity + humidity;
    sum_dewpoint = sum_dewpoint + dewpoint;
    sum_co2 = sum_co2 + co2;
    delay(10);
  }
  temperature = sum_temperature / 100;
  humidity = sum_humidity / 100;
  dewpoint = sum_dewpoint / 100;
  co2 = sum_co2 / 100;

  //rc = k30_i2c.readCO2(co2);

  float AH =  ((6.112 * exp((17.67 * temperature) / (temperature + 245.5)) * humidity * 18.02) / ((273.15 + temperature) * 100 * 0.08314));
  float Psat =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5))) / 10;
  float P =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5)) * (humidity / 100)) / 10;
  float VPD = (Psat - P);

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'a') {
      digitalWrite(Relay1, HIGH);
      Relay1_value = 1;
    }
    if (c == 'b') {
      digitalWrite(Relay1, LOW);
      Relay1_value = 0;
    }
    if (c == 'c') {
      digitalWrite(Relay2, HIGH);
      Relay2_value = 1;
    }
    if (c == 'd') {
      digitalWrite(Relay2, LOW);
      Relay2_value = 0;
    }
    if (c == 'e') {
      digitalWrite(Relay3, HIGH);
      Relay3_value = 1;
    }
    if (c == 'f') {
      digitalWrite(Relay3, LOW);
      Relay3_value = 0;
    }
    if (c == 'g') {
      digitalWrite(Relay4, HIGH);
      Relay4_value = 1;
    }
    if (c == 'h') {
      digitalWrite(Relay4, LOW);
      Relay4_value = 0;
    }
  }

  if (BT.available()) {
    char c = BT.read();
    if (c == 'a') {
      digitalWrite(Relay1, HIGH);
      Relay1_value = 1;
    }
    if (c == 'b') {
      digitalWrite(Relay1, LOW);
      Relay1_value = 0;
    }
    if (c == 'c') {
      digitalWrite(Relay2, HIGH);
      Relay2_value = 1;
    }
    if (c == 'd') {
      digitalWrite(Relay2, LOW);
      Relay2_value = 0;
    }
    if (c == 'e') {
      digitalWrite(Relay3, HIGH);
      Relay3_value = 1;
    }
    if (c == 'f') {
      digitalWrite(Relay3, LOW);
      Relay3_value = 0;
    }
    if (c == 'g') {
      digitalWrite(Relay4, HIGH);
      Relay4_value = 1;
    }
    if (c == 'h') {
      digitalWrite(Relay4, LOW);
      Relay4_value = 0;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0); // set the cursor to column 0, line 0
  lcd.print("Temp : ");
  lcd.print(temperature);
  lcd.print(" oC");
  lcd.setCursor(0, 1); // set the cursor to column 0, line 1
  lcd.print("Humi : ");
  lcd.print(humidity);
  lcd.print(" %");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0); // set the cursor to column 0, line 0
  lcd.print("VPD : ");
  lcd.print(VPD);
  lcd.print(" kPa");
  lcd.setCursor(0, 1); // set the cursor to column 0, line 1
  lcd.print("CO2 : ");
  lcd.print(co2);
  lcd.print(" ppm");
  delay(3000);

  String data = "{\"Temperature\":" + String(temperature) + ",\"Humidity\":" + String(humidity) + ",\"Dew_Temperature\":" + String(dewpoint) + ",\"Absolute_Humidity\":" + String(AH) + ",\"Vapor_pressure\":" + String(P) + ",\"VPD\":" + String(VPD) + ",\"CO2\":" + String(co2) + ",\"Relay1\":" + String(Relay1_value) + ",\"Relay2\":" + String(Relay2_value) + ",\"Relay3\":" + String(Relay3_value) + ",\"Relay4\":" + String(Relay4_value) + "}";
  Serial.println(data);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  //delay(5000);
  while (Serial.available()) {
    delay(3);
    char c = Serial.read();
    readString += c;
  }
}
