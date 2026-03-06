#include <Wire.h>
#include "INA3221.h"
#include <Servo.h>

INA3221 ina[2]=
{
	INA3221(0x40,&Wire),
	INA3221(0x41,&Wire)
};

const int switching=26;
const float upper=25.2;
const float lower=19.5;
int charging=1;

Servo pan,tilt;
int panPos=90;
int tiltPos=90;

float angle=1;
float tolerance=200;
float cellVoltage[6];
float cellCurrent[6];

void setup()
{
	Serial.begin(115200);
	Wire.begin();
	Wire.setClock(400000);

	for(int i=0;i<2;i++)
	{
		ina[i].begin();
		ina[i].setShuntR(0,0.1);
		ina[i].setShuntR(1,0.1);
		ina[i].setShuntR(2,0.1);
	}

	pinMode(switching,OUTPUT);
	digitalWrite(switching,LOW);
	pan.attach(18);
	tilt.attach(19);
	pan.write(90);
	tilt.write(90);
	Serial.println("6 Cell BMS + Solar Tracker Started");
}

void loop()
{
	int index=0;
	float packVoltage=0;
	for(int i=0;i<2;i++)
	{
		for(int cell=0;cell<3;cell++)
		{
			float volt=ina[i].getBusVoltage(cell);
			float current=ina[i].getCurrent_mA(cell)/1000.0;
			cellVoltage[index]=volt;
			cellCurrent[index]=current;
			packVoltage+=volt;

			Serial.print("Cell ");
			Serial.print(index+1);
			Serial.print(": ");
			Serial.print(volt,3);
			Serial.print(" V  ");
			Serial.print(current,3);
			Serial.println(" A");
			index++;
		}
	}
	float batteryPercent=(packVoltage/25.2)*100;
	Serial.print("Pack Voltage: ");
	Serial.print(packVoltage,3);
	Serial.println(" V");
	Serial.print("Battery Percentage: ");
	Serial.print(batteryPercent);
	Serial.println(" %");
	Serial.println();

	if(packVoltage>=upper&&charging==1)
	{
		digitalWrite(switching,HIGH);
		charging=0;
	}
	if(packVoltage<=lower&&charging==0)
	{
		digitalWrite(switching,LOW);
		charging=1;
	}
	if(packVoltage>lower)
	{
		DualAxis();
	}
	delay(100);
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
			panPos-=angle;
		else
			panPos+=angle;
		panPos=constrain(panPos,0,180);
		pan.write(panPos);
	}
	if(abs(ldrUp-ldrDown)>tolerance)
	{
		if(ldrUp>ldrDown)
			tiltPos+=angle;
		else
			tiltPos-=angle;
		tiltPos=constrain(tiltPos,0,180);
		tilt.write(tiltPos);
	}
}
