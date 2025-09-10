#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

const int mosfetGatePin = 7;
const float cutoffVoltage = 4.10;
const float resumeVoltage = 3.50;
int chargingEnabled = 1;

void setup()
{
  Serial.begin(9600);
  ina219.begin();
  pinMode(mosfetGatePin, OUTPUT);
  digitalWrite(mosfetGatePin, LOW);
  chargingEnabled = 1;

  Serial.println("Battery charging control started...");
}

void loop() {
  float busVoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA()/1000;

  Serial.print("Battery Voltage: ");
  Serial.print(busVoltage, 3);
  Serial.print(" V | Current: ");
  Serial.print(current_mA, 1);
  Serial.println(" A");

  if (busVoltage >= cutoffVoltage && chargingEnabled == 1)
  {
    digitalWrite(mosfetGatePin, HIGH);
    chargingEnabled = 0;
  }
  if(busVoltage <= resumeVoltage && chargingEnabled == 0)
  {
    digitalWrite(mosfetGatePin, LOW);
    chargingEnabled = 1;
  }
}