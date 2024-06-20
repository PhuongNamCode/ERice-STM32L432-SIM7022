/*
 * Software_Timer.c
 *
 *  Created on: Oct 1, 2022
 *      Author: ASUS
 */

#include "Software_Timer.h"
//
int timer1_counter = 0;
int timer2_counter = 0;
int timer3_counter = 0;
int timer4_counter = 0;
int timer5_counter = 0;
int timer6_counter = 0;
int timer7_counter = 0;
int timer8_counter = 0;
int timer9_counter = 0;
int timer10_counter = 0;
//


///
int timer1_flag = 0;
int timer2_flag = 0;
int timer3_flag = 0;
int timer4_flag = 0;
int timer5_flag = 0;
int timer6_flag = 0;
int timer7_flag = 0;
int timer8_flag = 0;
int timer9_flag = 0;
int timer10_flag = 0;
///


//*******
void setTimer1(int duration){
	timer1_counter = duration/CLOCK_TIMER;
	timer1_flag = 0;
}

void setTimer2(int duration){
	timer2_counter = duration/CLOCK_TIMER;
	timer2_flag = 0;
}

void setTimer3(int duration){
	timer3_counter = duration/CLOCK_TIMER;
	timer3_flag = 0;
}

void setTimer4(int duration){
	timer4_counter = duration/CLOCK_TIMER;
	timer4_flag = 0;
}

void setTimer5(int duration){
	timer5_counter = duration/CLOCK_TIMER;
	timer5_flag = 0;
}

void setTimer6(int duration){
	timer6_counter = duration/CLOCK_TIMER;
	timer6_flag = 0;
}


void setTimer7(int duration){
	timer7_counter = duration/CLOCK_TIMER;
	timer7_flag = 0;
}

void setTimer8(int duration){
	timer8_counter = duration/CLOCK_TIMER;
	timer8_flag = 0;
}

void setTimer9(int duration){
	timer9_counter = duration/CLOCK_TIMER;
	timer9_flag = 0;
}

void setTimer10(int duration){
	timer10_counter = duration/CLOCK_TIMER;
	timer10_flag = 0;
}
//**********



void timerRun(){
	if (timer1_counter == 1 ){
		//BEGIN-TODO
		timer1_flag = 1;
		//END-TODO
	}
	else timer1_counter--;

	if (timer2_counter == 1 ) timer2_flag = 1;
	else timer2_counter--;

	if (timer3_counter == 1 ) timer3_flag = 1;
	else timer3_counter--;

	if (timer4_counter == 1 ) timer4_flag = 1;
	else timer4_counter--;

	if (timer5_counter == 1 ) timer5_flag = 1;
	else timer5_counter--;

	if (timer6_counter == 1 ) timer6_flag = 1;
	else timer6_counter--;


	if (timer7_counter == 1 ) timer7_flag = 1;
	else timer7_counter--;

	if (timer8_counter == 1 ) timer8_flag = 1;
	else timer8_counter--;

	if (timer9_counter == 1 ) timer9_flag = 1;
	else timer9_counter--;

	if (timer10_counter == 1 ) timer10_flag = 1;
	else timer10_counter--;


}
