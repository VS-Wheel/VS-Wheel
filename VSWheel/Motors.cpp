#include "Motors.h"

INPUTS * inputs;
USBJoystick * joy;

void MOTORS::init(INPUTS *in, USBJoystick *j, float * m1_p, float * m1_d, bool * m1_r, float * m2_p, float * m2_d, bool * m2_r)
{
	inputs = in;
	joy = j;

	m1_period = m1_p;
	m1_duty = m1_d;
	m1_dir = m1_r;

	m2_period = m2_p;
	m2_duty = m2_d;
	m2_dir = m2_r;
}

bool MOTORS::manageFFBData()
{
	//static int16_t magnitude = 0;
	//magnitude = joy->get_magnitude();
	int16_t magnitude = joy->get_magnitude();

	if(magnitude > 0)
	{
		*m1_period = 0.0001f;
		*m1_duty = ((float)(abs(magnitude)) / 255);
		*m2_period = 0.0001f;
		*m2_duty = ((float)(abs(magnitude)) / 255);
		*m1_dir = 0;
		*m2_dir = 0;
	}
	if(magnitude < 0)
	{
		*m1_period = 0.0001f;
		*m1_duty = ((float)(abs(magnitude)) / 255);
		*m2_period = 0.0001f;
		*m2_duty = ((float)(abs(magnitude)) / 255);
		*m1_dir = 1;
		*m2_dir = 1;
	}
	if(magnitude == 0)
	{
		*m1_period = 0.0f;
		*m1_duty = 0.0f;
		*m2_period = 0.00f;
		*m2_duty = 0.0f;
	}
	return true;
}