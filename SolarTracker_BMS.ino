#include <Wire.h>
#include <Adafruit_INA219.h>
#include<Servo.h>

Adafruit_INA219 ina219;

const int switching = 26;
const float upper = 8.00;
const float lower = 6.50;
int charging = 1;

Servo pan, tilt;
int panPos=90;
int tiltPos=90;
float angle=1;

float tolerance=200;

void setup()
{
  Serial.begin(9600);
  ina219.begin();
  pinMode(switching, OUTPUT);
  digitalWrite(switching, LOW);
  charging = 1;

  Serial.println("Battery charging control started......");

  pan.attach(18);
  tilt.attach(19);
  pan.write(90);
  tilt.write(90);

  Serial.println("Pan & Tilt Started......");
}

void loop()
{
  BMS();
}

void BMS()
{
  float volt = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA()/1000;
  float batteryPercent=(volt/100)*8.4;

  Serial.print("Battery Voltage: ");
  Serial.print(volt, 3);
  Serial.print("V  Current: ");
  Serial.print(current, 1);
  Serial.println("A");
  Serial.print("Battery Percentage: ");
  Serial.println("%");

  if (volt >= upper && charging == 1)
  {
    digitalWrite(switching, HIGH);
    charging = 0;
  }
  if(volt <= lower && charging == 0)
  {
    digitalWrite(switching, LOW);
    charging = 1;
  }
  if(volt>lower)
  {
    DualAxis();
  }
}

void DualAxis()
{
  float ldrUp=analogRead(34);
  float ldrDown=analogRead(35);
  float ldrRight=analogRead(32);
  float ldrLeft=analogRead(33);

  if(abs(ldrLeft-ldrRight)>tolerance)
  {
    if(ldrLeft>ldrRight)
    {
      panPos-=angle;
    }
    else
    {
      panPos+=angle;
    }
    panPos = constrain(panPos, 0, 180);
    pan.write(panPos);
  }

  if(abs(ldrUp-ldrDown)>tolerance)
  {
    if(ldrUp>ldrDown)
    {
      tiltPos+=angle;
    }
    else
    {
      tiltPos-=angle;
    }
    tiltPos = constrain(tiltPos, 0, 180);
    tilt.write(tiltPos);
  }
}