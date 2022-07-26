#include <LoRa.h>
#include <Sensirion.h>
#include <K30_I2C.h>
#include <LiquidCrystal_I2C.h>

const uint8_t dataPin  =  3;
const uint8_t clockPin =  4;

float temperature;
float humidity;
float dewpoint;

int co2 = 0;
int rc  = 1;

Sensirion tempSensor = Sensirion(dataPin, clockPin);
K30_I2C k30_i2c = K30_I2C(0x68);
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{
  Serial.begin(9600);
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

  for (int i = 0; i < 100; i++) {            //평균온도  100*0.01 = 1초
    tempSensor.measure(&temperature, &humidity, &dewpoint);
    sum_temperature = sum_temperature + temperature;
    sum_humidity = sum_humidity + humidity;
    sum_dewpoint = sum_dewpoint + dewpoint;
    delay(10);
  }
  temperature = sum_temperature / 100;
  humidity = sum_humidity / 100;
  dewpoint = sum_dewpoint / 100;

  rc = k30_i2c.readCO2(co2);

  float AH =  ((6.112 * exp((17.67 * temperature) / (temperature + 245.5)) * humidity * 18.02) / ((273.15 + temperature) * 100 * 0.08314));
  float Psat =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5))) / 10;
  float P =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5)) * (humidity / 100)) / 10;
  float VPD = (Psat - P);

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

  String data = "{\"Temperature\":" + String(temperature) + ",\"Humidity\":" + String(humidity) + ",\"Dew_Temperature\":" + String(dewpoint) + ",\"Absolute Humidity\":" + String(AH) + ",\"Vapor pressure\":" + String(P) + ",\"VPD\":" + String(VPD) + ",\"CO2\":" + String(co2) + "}";
  Serial.println(data);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  //delay(5000);
}
