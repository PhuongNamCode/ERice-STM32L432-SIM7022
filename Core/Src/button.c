/*
 * button.c
 *
 *  Created on: Aug 25, 2023
 *      Author: ASUS
 */



#include "button.h"


int button1_flag = 0;

//KeyReg trong qua trinh rung
int KeyReg0_1 = NORMAL_STATE;
int KeyReg1_1 = NORMAL_STATE;
int KeyReg2_1 = NORMAL_STATE;

//KeyReg cua qua trinh on dinh truoc day
int KeyReg3_1 = NORMAL_STATE;


//Thoi gian cua nhan de'
int TimerforKeyPRESS1 = 20;

int isButton1Pressed(){
	if (button1_flag == 1){
		button1_flag = 0;
		return 1;
	}
	else return 0;
}
//
void getKeyInput1(){
	KeyReg0_1 = KeyReg1_1;
	KeyReg1_1 = KeyReg2_1;
	KeyReg2_1 = HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin);

	if (KeyReg0_1 == KeyReg1_1 && KeyReg1_1 == KeyReg2_1){

		if (KeyReg2_1 != KeyReg3_1){					//Nhan doi trang thai
			KeyReg3_1 = KeyReg2_1;
			if (KeyReg2_1 == PRESSED_STATE){			//Neu dang nhan
				button1_flag = 1;
			}
			else{									//Luc truoc thi nhan, bay gio dang tha ra
				button1_flag = 0;
				TimerforKeyPRESS1 = 20;				//khi release thi RESET nhan de 1 giay
													//neu nhan lai trong 1 giay thi tinh la nhan de
			}

		}
		else{										//Nhan de
			if(KeyReg2_1 == PRESSED_STATE){			// Neu dang nhan de
				TimerforKeyPRESS1--;
				if (TimerforKeyPRESS1 == 0){			// Nhan de thanh cong
					TimerforKeyPRESS1 = 10;			// Moi 1s nhan de thi RETURN button_flag 1 lan
					button1_flag = 1;
				}
			}
		}
	}

}

