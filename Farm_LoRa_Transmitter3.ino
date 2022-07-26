#include <LoRa.h>
#include <Sensirion.h>
#include <SoftwareSerial.h>
SoftwareSerial K_30_Serial(5, 6);

const uint8_t dataPin  =  3;
const uint8_t clockPin =  4;

float temperature;
float humidity;
float dewpoint;

Sensirion tempSensor = Sensirion(dataPin, clockPin);

String outString = "";
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
byte response[] = {0, 0, 0, 0, 0, 0, 0};
int valMultiplier = 1;


void sendRequest(byte packet[])
{
  while (!K_30_Serial.available())
  {
    K_30_Serial.write(readCO2, 7);
    delay(50);
  }

  int timeout = 0;
  while (K_30_Serial.available() < 7 )
  {
    timeout++;
    if (timeout > 10)
    {
      while (K_30_Serial.available())
        K_30_Serial.read();
      break;
    }
    delay(50);
  }

  for (int i = 0; i < 7; i++)
  {
    response[i] = K_30_Serial.read();
  }
}

unsigned long getValue(byte packet[])
{
  int high = packet[3];
  int low = packet[4];
  unsigned long val = high * 256 + low;
  return val * valMultiplier;
}


void setup()
{
  Serial.begin(9600);
  K_30_Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop()
{
  tempSensor.measure(&temperature, &humidity, &dewpoint);
  sendRequest(readCO2);
  unsigned long valCO2 = getValue(response);
  float AH =  ((6.112 * exp((17.67 * temperature) / (temperature + 245.5)) * humidity * 18.02) / ((273.15 + temperature) * 100 * 0.08314));
  float Psat =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5))) / 10;
  float P =  (6.112 * exp((17.67 * temperature) / (temperature + 243.5)) * (humidity / 100)) / 10;
  float VPD = (Psat - P);


  String data = "{\"Temperature\":" + String(temperature) + ",\"Humidity\":" + String(humidity) + ",\"Dew_Temperature\":" + String(dewpoint) + ",\"Absolute Humidity\":" + String(AH) + ",\"Vapor pressure\":" + String(P) + ",\"VPD\":" + String(VPD) + ",\"CO2\":" + String(valCO2) +"}";
  Serial.println(data);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  delay(5000);
}
