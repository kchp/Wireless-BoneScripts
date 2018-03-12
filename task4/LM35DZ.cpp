
#include "LM35DZ.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <fstream>

using namespace std;

double readTemp()
{
	double adc_val = 0;
	double temp = 0;
	double temp_cal = 0;

	fstream tempFile (ADC_PATH, fstream::in);
	tempFile >> adc_val;
	tempFile.close();

	//	Calibrate sensorvalue:
	temp = (adc_val*1.8)/4095;
	temp = (temp-0.5)*100;
	temp_cal = temp+49;

	return temp_cal;
}

void pwmDuty(double tempInput)
{
	int index = 0;
	int duty = 0;

	while(1)
	{
		//	Calculate the duty cycle based on the desired temperature:
		duty = index;	//	Increment by 100 until sensor reads the desired value

		if(readTemp() > tempInput+5 || readTemp() < tempInput-5)	//	Tolerance is [+-] 5
		{
			ofstream DUTY (PWM_DUTY);
			DUTY << duty;		//	Duty cycle value
			DUTY.close();
			break;
		}

		else
		{
			sleep(2);	//	Wait until the sensor arrives at desired value
			continue;	//	If conditions were not met, skip to the next iteration in the loop
		}
		index += 100;
	}
}

void pwmPeriod()
{
	ofstream PERIOD (PWM_PERIOD);
	PERIOD << "1000000000";	//	1*10⁹ns=1s period of PWM signal
	PERIOD.close();
}

void pwmEnable()
{
	ofstream ENABLE (PWM_ENABLE);
	ENABLE << "1";
	ENABLE.close();
}
