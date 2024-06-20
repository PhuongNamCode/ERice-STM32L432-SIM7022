#include "ViettelSDK.h"
#include <string.h>

void sleepMCU(struct ViettelSDK *self, uint16_t period_in_seconds) {
	/* Set the RTC time to 0 seconds */
	self->sTime.Hours = 0x00;
	self->sTime.Minutes = 0x00;
	self->sTime.Seconds = 0x00;
	self->sTime.TimeFormat = RTC_HOURFORMAT_24;
	self->sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	self->sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(self->rtc_timer, &self->sTime, RTC_FORMAT_BCD)
			!= HAL_OK) {
		sprintf(self->log_content, "ERROR IN SLEEPING...\n");
		writeLog(self, LOG_INFO, self->log_content, true);
		Error_Handler();
	}

	/* Parse seconds to hours, minutes and seconds */
	uint8_t hours = (period_in_seconds / 3600);
	uint8_t minutes = (period_in_seconds - (3600 * hours)) / 60;
	uint8_t seconds = (period_in_seconds - (3600 * hours) - (minutes * 60));

	/* Set the RTC alarm to trigger after 100 seconds */
	self->sAlarm.AlarmTime.Hours = hours;
	self->sAlarm.AlarmTime.Minutes = minutes;
	self->sAlarm.AlarmTime.Seconds = seconds;
	self->sAlarm.Alarm = RTC_ALARM_A;
	self->sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
	self->sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	self->sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	self->sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	self->sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;

	sprintf(self->log_content,
			"Triggered timer with period %u hours %u minutes %u seconds.\n",
			hours, minutes, seconds);
	writeLog(self, LOG_INFO, self->log_content, true);

	self->passively_listen = true;

	/* Sleep */
	sprintf(self->log_content, "MCU IS SLEEPING...\n");
	writeLog(self, LOG_INFO, self->log_content, true);

	HAL_IWDG_Refresh(&hiwdg);
	//HAL_TIM_Base_Start_IT(&htim2);

	if (HAL_RTC_SetAlarm_IT(self->rtc_timer, &self->sAlarm, RTC_FORMAT_BIN)
			!= HAL_OK) {
		Error_Handler();
	}

	sprintf(self->log_content, "DEBUG BEFORE SLEEPING....\n\n\n\n\n\n...");
	writeLog(self, LOG_INFO, self->log_content, true);


	// Go to sleep
	HAL_SuspendTick();
	HAL_PWR_EnableSleepOnExit();		//Ngu lai moi khi bi interrupt

	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	//HAL_PWR_EnterSTANDBYMode();



	sprintf(self->log_content, "DEBUG AFTER SLEEPING.....\n");
	writeLog(self, LOG_INFO, self->log_content, true);


	// Wake up
	HAL_ResumeTick();
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_PWR_DisableSleepOnExit();

	sprintf(self->log_content, "MCU IS WAKING...\n");
	writeLog(self, LOG_INFO, self->log_content, true);
}

void resetMCU(struct ViettelSDK *self) {
	getSensorData(self);
	sprintf(self->log_content, "MCU will be reset now.\n\n\n");
	writeLog(self, LOG_INFO, self->log_content, true);
	NVIC_SystemReset();
}

void wakeUpModule(struct ViettelSDK *self) {
	uint8_t count = WAKE_UP_MODULE_PUSH_COUNT;
	while (count--) {
		pullDownPWRpin(self);
		HAL_Delay(MODULE_PWR_PIN_PULL_DOWN_DELAY_MS);
		noPullPWRpin(self);
//		HAL_GPIO_WritePin(self->pwr_pin_GPIOx, self->pwr_pin_GPIO_Pin,
//				GPIO_PIN_SET);
		HAL_Delay(MODULE_PWR_PIN_NO_PULL_DELAY_MS);
	}
	noPullPWRpin(self);

	sprintf(self->log_content, "Woke up module...");
	writeLog(self, LOG_INFO, self->log_content, true);

	HAL_Delay(WAIT_FOR_MODULE_TO_WAKE_MS);
}



void GetUpModule(struct ViettelSDK *self) {
	uint8_t count = 1;
	while (count--) {
		pullDownPWRpin(self);
		HAL_Delay(MODULE_PWR_PIN_PULL_DOWN_DELAY_MS);
		noPullPWRpin(self);
		HAL_Delay(MODULE_PWR_PIN_NO_PULL_DELAY_MS);
	}
	noPullPWRpin(self);

	sprintf(self->log_content, "GET UP module...");
	writeLog(self, LOG_INFO, self->log_content, true);

	HAL_Delay(WAIT_FOR_MODULE_TO_WAKE_MS);
}


void pullDownPWRpin(struct ViettelSDK *self) {
	self->GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(self->pwr_pin_GPIOx, &self->GPIO_InitStruct);
	HAL_GPIO_WritePin(self->pwr_pin_GPIOx, self->pwr_pin_GPIO_Pin,
			GPIO_PIN_RESET);
}

void noPullPWRpin(struct ViettelSDK *self) {
	self->GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(self->pwr_pin_GPIOx, &self->GPIO_InitStruct);
	HAL_GPIO_WritePin(self->pwr_pin_GPIOx, self->pwr_pin_GPIO_Pin,
					GPIO_PIN_SET);
}

//void setupSleepTimer(struct ViettelSDK *self) {
//
//	/* Enable the LSI clock */
//	RCC_OscInitTypeDef RCC_OscInitStruct;
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
//	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
//		Error_Handler();
//	}
//
//	/* Select the LSI as the RTC clock source */
//	RCC_PeriphCLKInitTypeDef RCC_RTCCLKInitStruct;
//	RCC_RTCCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//	RCC_RTCCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
//	if (HAL_RCCEx_PeriphCLKConfig(&RCC_RTCCLKInitStruct) != HAL_OK) {
//		Error_Handler();
//	}
//
//	/* Enable the RTC clock */
//	__HAL_RCC_RTC_ENABLE();
//
//	/* Initialize RTC Only */
//	self->rtc_timer.Instance = RTC;
//	self->rtc_timer.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
//	self->rtc_timer.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
//	if (HAL_RTC_Init(&self->rtc_timer) != HAL_OK) {
//		Error_Handler();
//	}
//
//	sprintf(self->log_content, "Setup sleep timer successfully.");
//	writeLog(self, LOG_INFO, self->log_content, true);
//}

/* Data */
void updateFLASHData(struct ViettelSDK *self) {
	*self->data_list->pair.value.uint32_value = self->previous_pulse;
	sprintf(self->log_content, "Update current flow data: %lu",
			*self->data_list->pair.value.uint32_value);
	writeLog(self, LOG_INFO, self->log_content, true);
}

void readFLASHPreviousFlowData(struct ViettelSDK *self, uint32_t address) {
	self->previous_pulse = *(__IO uint32_t*) (address);
	sprintf(self->log_content, "Read flow data from FLASH memory: %lu",
			self->previous_pulse);
	writeLog(self, LOG_INFO, self->log_content, true);
}

//void writeFLASHFlowData(struct ViettelSDK *self, uint32_t start_page,
//		uint32_t end_page, uint32_t data32) {
//	HAL_FLASH_Unlock();
//	FLASH_EraseInitTypeDef EraseInit;
//	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
//	EraseInit.PageAddress = start_page;
//	EraseInit.NbPages = (end_page - start_page) / FLASH_PAGE_SIZE;
//	uint32_t page_error = 0;
//	HAL_FLASHEx_Erase(&EraseInit, &page_error);
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_page, data32);
//	sprintf(self->log_content, "Write flow data from FLASH memory: %lu\n",
//			data32);
//	writeLog(self, LOG_INFO, self->log_content, true);
//	HAL_FLASH_Lock();
//}

/* Debugger */
bool validateUARTPorts(struct ViettelSDK *self) {
	bool validated = true;
	if (self->debugger_uart == self->module_uart) {
		self->module_uart = 0x00;
		validated = false;
		writeLog(self, LOG_WARNING,
				"Debugger has the same UART port as the Connector. UART port will be used for Debugger.",
				true);
	} else {
		writeLog(self, LOG_INFO, "Module UART port is initialized.", true);
	}
	return validated;
}

void writeLog(struct ViettelSDK *self, enum LogType log_type,
		char log_content[], bool default_log) {

	if ((!default_log) && (!self->detail_log)) {
		return;
	}

	if (strlen(log_content) >= LOG_SIZE) {
		strcpy(self->log_content, (const char*) "Log content is too long!");
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
				(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) self->log_content,
				(uint16_t) strlen(self->log_content),
				LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\n",
				(uint16_t) strlen("\n"), LOG_TIMEOUT_MS);
		return;
	}

	strcpy(self->log_content, log_content);
	if (log_type == LOG_INFO) {
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG INFO: ",
				(uint16_t) strlen("LOG INFO: "), LOG_TIMEOUT_MS);
	} else if (log_type == LOG_WARNING) {
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG WARNING: ",
				(uint16_t) strlen("LOG WARNING: "),
				LOG_TIMEOUT_MS);
	} else if (log_type == LOG_ERROR) {
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
				(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
	} else if (log_type == LOG_DEBUG) {
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG DEBUG: ",
				(uint16_t) strlen("LOG DEBUG: "), LOG_TIMEOUT_MS);
	} else if (log_type == LOG_EMPTY) {
		/* Log nothing */
	} else {
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
				(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "Invalid LOG TYPE!",
				(uint16_t) strlen("Invalid LOG TYPE!"),
				LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\n",
				(uint16_t) strlen("\n"),
				LOG_TIMEOUT_MS);
		return;
	}
	HAL_UART_Transmit(self->debugger_uart, (uint8_t*) self->log_content,
			(uint16_t) strlen(self->log_content), LOG_TIMEOUT_MS);
	HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\r\n",
			(uint16_t) strlen("\r\n"),
			LOG_TIMEOUT_MS);
}

/* SDK */
void logStartBanner(struct ViettelSDK *self) {
	sprintf(self->log_content,
			"\n\n\n==================== STARTING ====================");
	writeLog(self, LOG_EMPTY, self->log_content, true);
}

void initializeSDK(struct ViettelSDK *self, UART_HandleTypeDef *debugger_uart,
		UART_HandleTypeDef *module_uart, DMA_HandleTypeDef *module_uart_hdma,
		GPIO_TypeDef *pwr_pin_GPIOx, uint16_t pwr_pin_GPIO_Pin, RTC_HandleTypeDef* hrtc) {

	HAL_Delay(INITIALIZE_DELAY_MS);

	self->detail_log = DETAIL_LOG_SETUP;
	self->data_list = NULL;
	self->addDataSuccessfully = true;
	self->sleep = false;
	self->mqtt_params.receiveSubcribeTimeout = RECEIVE_SUBSCRIBE_TIMEOUT;
	self->warming_up_counter = WARMING_UP_COUNT;
	self->fail_increment = 0;
	self->passively_listen = false;
	self->successfully_initialized = false;
	self->attempt_count = 1;
	strcpy(self->delimiter, ",\r\n");

	self->debugger_uart = debugger_uart;
	self->module_uart = module_uart;
	self->module_uart_hdma = module_uart_hdma;

	/* Initialize Power Pin Wake Up */
	self->pwr_pin_GPIOx = pwr_pin_GPIOx;
	self->pwr_pin_GPIO_Pin = pwr_pin_GPIO_Pin;
	self->GPIO_InitStruct.Pin = self->pwr_pin_GPIO_Pin;
	self->GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	self->GPIO_InitStruct.Pull = GPIO_NOPULL;
	self->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Print start banner */
	logStartBanner(self);
	readFLASHPreviousFlowData(self, FLASH_USER_START_ADDR);
	HAL_IWDG_Init(&hiwdg);

	if (!validateUARTPorts(self)) {
		writeLog(self, LOG_ERROR, "SDK is NOT initialized.", true);
	} else {
		writeLog(self, LOG_INFO, "SDK is initialized.", true);
		self->successfully_initialized = true;
	}

	HAL_GPIO_Init(self->pwr_pin_GPIOx, &self->GPIO_InitStruct);
	self->period_timer = HAL_GetTick();
	self->successfully_initialized = true;
	self->rtc_timer = hrtc;
	self->run_first_time = true;
	self->stage = 1;

	self->data7022[0] = 0;
	self->data7022[1] = 0;
	self->data7022[2] = 0;
	self->data7022[3] = 0;
	self->data7022[4] = 0;

	strcpy(self->message7022,"");
}

void connectToPlatform(struct ViettelSDK *self) {

	uint32_t interval_time = HAL_GetTick() - self->period_timer;
	sprintf(self->log_content, "Interval: %lu \n",
			interval_time);
	writeLog(self, LOG_INFO, self->log_content, true);



	if (self->run_first_time
			|| (HAL_GetTick() - self->period_timer >= CONNECTING_PERIOD_MS )) {

		/* Header */
		writeLog(self, LOG_EMPTY, "\n", true);
		sprintf(self->log_content, "Start connecting to Platform. Attempt: %u.",
				self->attempt_count);
		writeLog(self, LOG_INFO, self->log_content, true);
		self->period_timer = HAL_GetTick();

		/* Check whether data are properly added */
		if (!self->addDataSuccessfully) {
			resetMCU(self);
		}

		/* Main Flow */
		if (!self->successfully_initialized) {
			writeLog(self, LOG_ERROR,
					"SDK is NOT initialized. MCU will be reset...", true);
			HAL_Delay(3000);
			resetMCU(self);
		} else {
			wakeUpModule(self);


			//*********** Main flow here *****************//

			mainFlow(self);


			//*******************************************//

		}

		/* Footer */
		writeLog(self, LOG_DEBUG, "End Period\n", true);
		self->attempt_count++;
		writeLog(self, LOG_EMPTY, "\n\n", true);
		self->period_timer = HAL_GetTick();
		self->run_first_time = false;

	}



	else {
		sprintf(self->log_content, "Start connecting again to Platform. Attempt: %u.",
						self->attempt_count);
		writeLog(self, LOG_INFO, self->log_content, true);

		/*************** MAIN FLOW *****************/
		mainFlow(self);

		/*******************************************/


		/* Footer */
		writeLog(self, LOG_DEBUG, "End Period\n", true);
		self->attempt_count++;
		writeLog(self, LOG_EMPTY, "\n\n", true);
		self->period_timer = HAL_GetTick();
		self->run_first_time = false;



	}



}

void getData (struct ViettelSDK *self){
    snprintf(self->message7022, sizeof(self->message7022),
             "{\"imei\": \"%s\", \"RSRP\": %d, \"rice_amount\": %d, \"humidity\": %d, \"temperature\": %d}",
             self->imei,
             self->data7022[1],
             self->data7022[2],
             self->data7022[4],
             self->data7022[3]);
}

struct CommandResponse sendCommand(struct ViettelSDK *self, char *command,
		uint8_t count, uint32_t timeout) {

	uint8_t max_count = count;
	count = 0;
	HAL_IWDG_Refresh(&hiwdg);



	while (count++ < max_count) {
		self->command_response.status = STATUS_UNKNOWN;
		clearMainBuffer(self);
		self->passively_listen = false;
		self->psm_timer = HAL_GetTick();

		/* Start the DMA again */
		resetDMAInterrupt_2(self);

		sprintf(self->log_content,
				"\n\n========= SEND COMMAND: %s | Attempt: %u/%u =========",
				command, count, max_count);
		writeLog(self, LOG_EMPTY, self->log_content, true);


		strcpy(self->command_response.response, "");
		HAL_UART_Transmit(self->module_uart, (uint8_t*) command,
				(uint16_t) strlen(command), timeout);
		HAL_UART_Transmit(self->module_uart, (uint8_t*) "\r\n",
				(uint16_t) strlen("\r\n"), timeout);


		self->command_response.status = STATUS_TIMEOUT;
		self->response_received = false;
		self->command_timer = HAL_GetTick();

		while (HAL_GetTick() - self->command_timer <= timeout) {
			if (self->response_received == true) {
				break;
			}
		}

		if (self->command_response.status == STATUS_SUCCESS) {
			break;
		}
		clearMainBuffer(self);
		HAL_Delay(COMMAND_DELAY_MS);

	}
	HAL_Delay(200);
	return self->command_response;
}

void clearCommand(struct ViettelSDK *self) {
	for (int i = 0; i < COMMAND_SIZE; i++) {
		self->command[i] = 0;
	}
}

void clearMainBuffer(struct ViettelSDK *self) {

	for (int i = 0; i < RESPONSE_MAIN_BUFFER_SIZE; i++) {
		self->response_main_buffer[i] = '\0';
	}
	self->old_position = 0;
	self->new_position = 0;
}

void resetDMAInterrupt_2(struct ViettelSDK *self) {

	HAL_UART_AbortReceive(self->module_uart);

	HAL_UARTEx_ReceiveToIdle_DMA(self->module_uart,
				self->response_receive_buffer,
				RESPONSE_RECEIVE_BUFFER_SIZE);

	__HAL_DMA_DISABLE_IT(self->module_uart_hdma, DMA_IT_HT);

//	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, self->response_receive_buffer, RESPONSE_RECEIVE_BUFFER_SIZE);
//	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}


void resetDMAInterrupt(struct ViettelSDK *self) {
//	HAL_UARTEx_ReceiveToIdle_DMA(self->module_uart,
//				self->response_receive_buffer,
//				RESPONSE_RECEIVE_BUFFER_SIZE);
//	__HAL_DMA_DISABLE_IT(self->module_uart_hdma, DMA_IT_HT);
////	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, self->response_receive_buffer, RESPONSE_RECEIVE_BUFFER_SIZE);
////	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void saveResponse(struct ViettelSDK *self, UART_HandleTypeDef *huart,
		uint16_t size) {
	/* Check whether MCU is sleep */



	if (self->module_uart->Instance == USART1) {
		char *ptr;

		// Update the last position before copying new data
		self->old_position = self->new_position;

		/* If the data in large and it is about to exceed the buffer size, Debugger indicates error */
		if (self->old_position + size > RESPONSE_MAIN_BUFFER_SIZE) {
			sprintf(self->log_content,
					"Response too large! Response Receive Buffer and Response Main Buffer will be cleared!");
			writeLog(self, LOG_ERROR, self->log_content, false);
			clearMainBuffer(self);
		}

		/* If the current position + new data size is less than the main buffer
		 * we will simply copy the data into the buffer and update the position
		 */
		else {
			//writeLog(self, LOG_WARNING, self->response_receive_buffer, true);


			memcpy( (uint8_t*)self->response_main_buffer + self->old_position,
					self->response_receive_buffer, size);

			self->new_position = self->old_position + size;
		}

		/* Start thce DMA again */

		//resetDMAInterrupt(self);
		//resetDMAInterrupt_2(self);
		//writeLog(self, LOG_DEBUG, self->response_main_buffer, true);



		/* Checking passively listen */
		if (self->passively_listen) {
			HAL_PWR_DisableSleepOnExit();
			for (int i = 0; i < PASSIVE_RESPONSE_LENGTH; i++) {
				ptr = strstr(self->response_main_buffer,
						PASSIVE_RESPONSE_SIGN[i]);
				if (ptr != NULL) {
					writeLog(self, LOG_EMPTY, self->response_main_buffer,
					true);
					sprintf(self->log_content, "Received an PASSIVE command!");
					writeLog(self, LOG_INFO, self->log_content, true);
					self->command_response.status = STATUS_SUCCESS;
					clearMainBuffer(self);
					self->response_received = true;

					if (i == 0) {
						sprintf(self->log_content, "Power off module!");
					} else if (i == 1) {
						sprintf(self->log_content, "Entered PSM mode!");
						self->sleep = true;
					} else if (i == 2) {
						sprintf(self->log_content, "Exited PSM mode!");
						self->sleep = false;
					} else if (i == 3) {
						sprintf(self->log_content,
								"Received subcribe messages from platform!");
						self->mqtt_params.receiveSubcribe = true;
					}
					writeLog(self, LOG_INFO, self->log_content, true);
					self->passively_listen = false;

					return;
				}
			}
			clearMainBuffer(self);
			return;
		}

		for (int i = 0; i < SUCCESS_RESPONSE_LENGTH; i++) {
			ptr = strstr(self->response_main_buffer, SUCCESS_COMMAND_SIGN[i]);
			if (ptr != NULL) {
				self->command_response.status = STATUS_SUCCESS;
				strcpy(self->command_response.response,
						self->response_main_buffer);
				writeLog(self, LOG_EMPTY, self->response_main_buffer,
				true);
				sprintf(self->log_content, "Received an OK command!");
				writeLog(self, LOG_INFO, self->log_content, false);
				clearMainBuffer(self);
				self->response_received = true;
				return;
			}
		}

		/* Checking completion of command */
		for (int i = 0; i < ERROR_RESPONSE_LENGTH; i++) {
			ptr = strstr(self->response_main_buffer, ERROR_COMMAND_SIGN[i]);
			if (ptr != NULL) {
				self->command_response.status = STATUS_ERROR;
				writeLog(self, LOG_EMPTY, self->response_main_buffer,
				true);
				sprintf(self->log_content, "Received an ERROR command!");
				writeLog(self, LOG_ERROR, self->log_content, false);
				clearMainBuffer(self);
				self->response_received = true;
//				HAL_Delay(ERROR_COMMAND_DELAY_MS);
				return;
			}
		}

		return;
	}
}


StatusType checkModule(struct ViettelSDK *self) {
	/* AT */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	self->command_response = sendCommand(self, "AT",
	RUN_COMMAND_COUNTER_DEFAULT + 6,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT - 400); /* Configured for PSM mode, DO NOT CHANGE */

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Checking Module Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
		GetUpModule(self);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */

		/* Warm up module */
		sprintf(self->log_content, "Warming up module %u times.",
				self->warming_up_counter);
		writeLog(self, LOG_INFO, self->log_content, true);
		while (self->warming_up_counter--) {
			self->command_response = sendCommand(self, "AT", 1,
			RUN_COMMAND_TIMEOUT_MS_DEFAULT);
		}

		self->warming_up_counter = WARMING_UP_COUNT;
	}

	return output_status;
}

StatusType setCommandEchoMode(struct ViettelSDK *self, bool echo_mode) {
	/* ATEx */
	StatusType output_status = STATUS_UNKNOWN;
	uint8_t echo_mode_code;
	if (echo_mode) {
		echo_mode_code = 1;
	} else {
		echo_mode_code = 0;
	}

	sprintf(self->command, "%s%u", COMMAND_ECHO_MODE, echo_mode_code);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	self->echo_mode = echo_mode;
	if (self->echo_mode) {
		sprintf(self->log_content, "Echo mode is ON");
	} else {
		sprintf(self->log_content, "Echo mode is OFF");
	}

	sprintf(self->log_content, "Set Command Echo Mode Status is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


StatusType setLedMode(struct ViettelSDK *self, bool led_mode) {
	/* AT+QCLEDMODE= */
	StatusType output_status = STATUS_UNKNOWN;
	uint8_t led_mode_code;
	if (led_mode) {
		led_mode_code = 1;
	} else {
		led_mode_code = 0;
	}

	sprintf(self->command, "%s=%u", COMMAND_LED_MODE, led_mode_code);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	self->led_mode = led_mode;
	if (self->led_mode) {
		sprintf(self->log_content, "Led mode is ON");
	} else {
		sprintf(self->log_content, "Led mode is OFF");
	}

	sprintf(self->log_content, "Set Command Led Mode Status is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


StatusType requestIMEI(struct ViettelSDK *self) {
    /* AT+CGSN */
    StatusType output_status = STATUS_UNKNOWN;
    sprintf(self->command, "%s=1", REQUEST_IMEI);

    self->command_response = sendCommand(self, self->command,
        RUN_COMMAND_COUNTER_DEFAULT,
        RUN_COMMAND_TIMEOUT_MS_DEFAULT);

    output_status = self->command_response.status;
    sprintf(self->log_content, "Requesting IMEI status is %s.",
            getStatusTypeString(output_status));

    if (output_status != STATUS_SUCCESS) {
        writeLog(self, LOG_WARNING, self->log_content, false);
    } else {
        writeLog(self, LOG_INFO, self->log_content, false);

        // Find the start of the IMEI in the response
        self->token = strstr(self->command_response.response, ": ");
        if (self->token != NULL) {
            self->token += 2; // Move pointer to the start of the IMEI

            // Find the start and end of the IMEI, excluding quotes
            char *start = strchr(self->token, '\"');
            if (start != NULL) {
                start += 1; // Move past the opening quote
                char *end = strchr(start, '\"');
                if (end != NULL) {
                    *end = '\0'; // Null-terminate at the closing quote
                }

                // Copy the IMEI
                strncpy(self->imei, start, IMEI_MAX_LENGTH - 1);
                self->imei[IMEI_MAX_LENGTH - 1] = '\0'; // Ensure null termination

                sprintf(self->log_content, "IMEI: %s.", self->imei);
                writeLog(self, LOG_INFO, self->log_content, false);
            } else {
                // Handle error if quotes are not found
                sprintf(self->log_content, "Failed to parse IMEI from response.");
                writeLog(self, LOG_ERROR, self->log_content, false);
                output_status = STATUS_ERROR;
            }
        } else {
            // Handle error if IMEI not found in response
            sprintf(self->log_content, "Failed to parse IMEI from response.");
            writeLog(self, LOG_ERROR, self->log_content, false);
            output_status = STATUS_ERROR;
        }
    }
    return output_status;
}



StatusType requestTARevisionIdentification(struct ViettelSDK *self) {
	/* AT+CGMR */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_TA_REVISION_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 3,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Requesting TA Revision ID is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		self->token = strstr(self->command_response.response, "\n");
		self->token = self->token + 1;
		self->token = strtok(self->token, self->delimiter);
		strcpy(self->ta_revision_id, self->token);
		sprintf(self->log_content, "Request TA Revision ID: %s.",
				self->ta_revision_id);
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType readSignalQualityReport(struct ViettelSDK *self) {
	/* AT+CSQ */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", SIGNAL_QUALITY_REPORT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);


	//sendCommand(self, "AT+CMNB=2", RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);


	output_status = self->command_response.status;
	sprintf(self->log_content, "Reading Signal Quality Report is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		int number_of_elements = 2;
		char *data_pointer;
		char *token_array[number_of_elements];
		uint8_t i = 0;

		data_pointer = strstr(self->command_response.response, ":");
		data_pointer = data_pointer + 2;

		uint8_t rssi_code;
		uint8_t ber;

		self->token = strtok(data_pointer, self->delimiter);
		while (self->token != NULL && i < number_of_elements) {
			token_array[i] = self->token;
			self->token = strtok(NULL, self->delimiter);
			i++;
		}

		rssi_code = atoi(token_array[0]);
		ber = atoi(token_array[1]);

		sprintf(self->log_content, "RSSI code: %u", rssi_code);
		writeLog(self, LOG_INFO, self->log_content, false);

		if (ber >= 0 && ber <= 7) {
			sprintf(self->log_content, "Bit error rate (ber): %u%%", ber);
		} else if (ber == 99) {
			sprintf(self->log_content,
					"Bit error rate (ber) is Not known or not detectable.");
		} else {
			sprintf(self->log_content, "Invalid Bit error rate (ber): %u",
					rssi_code);
		}
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType setEPSNetworkRegistrationStatus(struct ViettelSDK *self,
		uint8_t code) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u", EPS_NETWORK_REGISTRATION_STATUS, code);
	sprintf(self->command, "%s?", EPS_NETWORK_REGISTRATION_STATUS);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Setting EPS Network Registration Status is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType readEPSNetworkRegistrationStatus(struct ViettelSDK *self, bool save_data) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", EPS_NETWORK_REGISTRATION_STATUS);

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Reading EPS Network Registration Status is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		//int number_of_elements = 2;
		char *data_pointer = { 0 };
		//char *token_array[number_of_elements];


		for (uint8_t i = 0; i < RESPONSE_MAIN_BUFFER_SIZE; i++) {
			if (self->command_response.response[i] == ':') {
				data_pointer = &self->command_response.response[i] + 2;
				break;
			}
		}



//		data_pointer = strchr(self->command_response.response, ':');
		if (!data_pointer) {
			output_status = STATUS_INTERNAL_FAILURE;
			return output_status;
		}
//		data_pointer = data_pointer + 2;

		uint8_t n = atoi(data_pointer);
		uint8_t stat = atoi(data_pointer+2);
		//n = atoi(data_pointer);

		self->n = n;
		self->stat = stat;

//		switch (n) {
//		case 0:
//			self->token = strtok(data_pointer, self->delimiter);
//			while (self->token != NULL && i < number_of_elements) {
//				token_array[i] = self->token;
//				self->token = strtok(NULL, self->delimiter);
//				i++;
//			}
//
//			stat = atoi(token_array[1]);
//
//			if (save_data) {
//				self->n = atoi(token_array[0]);
//				self->stat = atoi(token_array[1]);
//			}
//
//			break;
//
//		case 4:
//			self->token = strtok(data_pointer, self->delimiter);
//			while (self->token != NULL && i < number_of_elements) {
//				token_array[i] = self->token;
//				self->token = strtok(NULL, self->delimiter);
//				i++;
//			}
//
//			stat = atoi(token_array[1]);
//
//			if (save_data) {
//				self->n = atoi(token_array[0]);
//				self->stat = atoi(token_array[1]);
//			}
//
//			break;
//
//		default:
//			break;
//		}

		sprintf(self->log_content, "<n> = %u", n);
		writeLog(self, LOG_INFO, self->log_content, false);

//		switch (stat) {
//		case 0:
//			break;
//		case 1:
//			break;
//		case 2:
//			break;
//		default:
//			;
//		}

		sprintf(self->log_content, "<stat> = %u", stat);
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType setOperatorSelection(struct ViettelSDK *self, uint8_t mode) {
	/* AT+COPS= */
	StatusType output_status = STATUS_UNKNOWN;
	if ((mode == 0) || (mode == 1) || (mode == 2) || (mode == 3)) {
		switch (mode) {
		case 0:
			sprintf(self->command, "%s=%u", OPERATOR_SELECTION, mode);
			break;
		case 1:
			sprintf(self->command, "%s=%u,2,\"45204\"", OPERATOR_SELECTION,
					mode);
			break;
		case 2:
			sprintf(self->command, "%s=%u", OPERATOR_SELECTION, mode);
			break;
		case 3:
			sprintf(self->command, "%s=%u,2", OPERATOR_SELECTION, mode);
			break;
		}
		sprintf(self->log_content, "<mode> = %u", mode);
		writeLog(self, LOG_INFO, self->log_content, false);
	} else {
		sprintf(self->log_content, "<mode> = %u is invalid.", mode);
		writeLog(self, LOG_ERROR, self->log_content, false);
		output_status = STATUS_BAD_PARAMETER;
		return output_status;
	}

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Set Operator Selection is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType setReportMobileEquipmentError(struct ViettelSDK *self, uint8_t mode) {
	/* AT+CMEE= */
	StatusType output_status = STATUS_UNKNOWN;
	if ((mode == 0) || (mode == 1) || (mode == 2)) {
		sprintf(self->command, "%s=%u", REPORT_MOBILE_EQUIPMENT_ERROR, mode);
		self->command_response = sendCommand(self, self->command,
		RUN_COMMAND_COUNTER_DEFAULT,
		RUN_COMMAND_TIMEOUT_MS_DEFAULT);
		output_status = self->command_response.status;
		sprintf(self->log_content, "<n> = %u", mode);
		writeLog(self, LOG_INFO, self->log_content, false);
	} else {
		sprintf(self->log_content, "<n> = %u is invalid.", mode);
		writeLog(self, LOG_ERROR, self->log_content, false);
		output_status = STATUS_BAD_PARAMETER;
	}

	sprintf(self->log_content, "Report Mobile Equipment Error is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType displayProductID(struct ViettelSDK *self) {
	/* ATI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", DISPLAY_PRODUCT_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Display Product ID is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType requestManufacturerID(struct ViettelSDK *self) {
	/* AT+CGMI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_MANUFACTURER_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Request Manufacturer ID is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType getAndSetMobileOperationBand(struct ViettelSDK *self,
		enum AT_QCBAND mode) {
	/* AT+CBAND= */
	StatusType output_status = STATUS_UNKNOWN;

	switch (mode) {
	case EGSM_MODE:
		sprintf(self->command, "%s=0,EGSM_MODE",
		GET_AND_SET_MOBILE_OPERATION_BAND);
		break;
	case DCS_MODE:
		sprintf(self->command, "%s=0,DCS_MODE",
		GET_AND_SET_MOBILE_OPERATION_BAND);
		break;
	case ALL_MODE:
		sprintf(self->command, "%s=0,ALL_MODE",
		GET_AND_SET_MOBILE_OPERATION_BAND);
		break;
	default:
		sprintf(self->command, "%s=0,3",
		GET_AND_SET_MOBILE_OPERATION_BAND);
		break;
	}

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Get And Set Mobile Operation Band is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType requestModelID(struct ViettelSDK *self) {
	/* AT+CGMM */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_MODEL_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Request Model ID is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType requestInternationalMobileSubscriberIdentity(struct ViettelSDK *self) {
	/* AT+CIMI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s",
	REQUEST_INTERNATIONAL_MOBILE_SUBSCRIBER_IDENTITY);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content,
			"Request International Mobile Subscriber Identity is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType readOperatorSelection(struct ViettelSDK *self) {
	/* AT+COPS? */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", OPERATOR_SELECTION);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Read Operator Selection is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType readpreferredOperatorList(struct ViettelSDK *self) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?",
	PREFERRED_OPERATOR_LIST);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Preferred Operator List is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType setDefaultPSDConnection(struct ViettelSDK *self) {
	/* AT*MCGDEFCONT         ---->   AT+CGDCONT           */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1,\"ip\",\"nbiot\"",								//add id =1 , "%s=\"ip\",\"nbiot\"   ---->    "%s=1,\"ip\",\"nbiot\"
	SET_DEFAULT_PSD_CONNECTION);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Set Default PSD Connection is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


StatusType UDP_Connect(struct ViettelSDK *self) {
	/* AT+SMCONF */
	StatusType output_status = STATUS_UNKNOWN;

	sprintf(self->command, "AT+CIPOPEN?");
	self->command_response = sendCommand(self, self->command,
								RUN_COMMAND_COUNTER_DEFAULT + 1, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);
	output_status = self->command_response.status;


	sprintf(self->log_content, "Initialize UDP is %s.", getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType send_UDP(struct ViettelSDK *self, char* addr, int port) {
	/* AT+CASEND */
	StatusType output_status = STATUS_UNKNOWN;


	clearCommand(self);

	// Send AT+CSOSEND .......
	int message_length = strlen(self->message7022);
	sprintf(self->command,"AT+CIPSEND=0,%d,\"%s\",%d",message_length,addr,port);

    sendCommand(self, self->command,1,RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	sprintf(self->command,self->message7022);

	// In ra self->message7022 trước khi gửi
	printf("Message to be sent: %s\n", self->message7022);

	sendCommand(self, self->command,1,RUN_COMMAND_TIMEOUT_MS_DEFAULT);

//	self->command_response = sendCommand(self, self->command,
//			RUN_COMMAND_COUNTER_DEFAULT + 1, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 1000);


	output_status = self->command_response.status;
	sprintf(self->log_content, "Send UDP is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


StatusType receive_UDP(struct ViettelSDK *self) {
	/* AT+CARECV */
	StatusType output_status = STATUS_UNKNOWN;

	clearCommand(self);
	sprintf(self->command, "AT+CIPRXGET=1");
	sprintf(self->command, "AT+CIPRXGET=2,0");


	uint8_t max_count = 10;
	uint8_t count = 0;



	while (count++ < max_count) {
		HAL_IWDG_Refresh(&hiwdg);
		self->command_response.status = STATUS_UNKNOWN;
		clearMainBuffer(self);
		self->passively_listen = false;
		self->psm_timer = HAL_GetTick();

		/* Start the DMA again */
		resetDMAInterrupt_2(self);

		uint8_t* command = self->command;
		sprintf(self->log_content,
				"\n\n========= SEND COMMAND: %s | Attempt: %u/%u =========",
				command, count, max_count);
		writeLog(self, LOG_EMPTY, self->log_content, true);

		strcpy(self->command_response.response, "");
		HAL_UART_Transmit(self->module_uart, (uint8_t*) command,
				(uint16_t) strlen(command), 1000);
		HAL_UART_Transmit(self->module_uart, (uint8_t*) "\r\n",
				(uint16_t) strlen("\r\n"), 1000);

		self->command_response.status = STATUS_TIMEOUT;
		self->response_received = false;
		self->command_timer = HAL_GetTick();

		while (HAL_GetTick() - self->command_timer <= 3000) {
		}

		clearMainBuffer(self);
		HAL_Delay(COMMAND_DELAY_MS);
	}



	return output_status;
}



StatusType UDP_Close(struct ViettelSDK *self) {
	/* AT+CACLOSE */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "AT+NETCLOSE");
	self->command_response = sendCommand(self, self->command,
			RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "UDP disconnect is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


StatusType ActivePDP(struct ViettelSDK *self, int cid, int act) {
	/* AT+CMQDISCON */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "AT+CNACT=%d,%d", cid, act);
	self->command_response = sendCommand(self, self->command,
			RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;

	if (act)
		sprintf(self->log_content, "UDP connect is %s.",
			getStatusTypeString(output_status));
	else
		sprintf(self->log_content, "UDP disconnect is %s.",
			getStatusTypeString(output_status));


	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}



StatusType configWakeupIndication(struct ViettelSDK *self, bool type) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%d",
	REPORT_THE_POWER_SAVING_MODE_STATUS, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Config Wakeup Indication is %s.",
			getStatusTypeString(output_status));
	if (type) {
		sprintf(self->log_content, "Turn ON wakeup indication.");
	} else {
		sprintf(self->log_content, "Turn OFF wakeup indication.");
	}

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configEPSNetworkRegistration(struct ViettelSDK *self, uint8_t type) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u",
	EPS_NETWORK_REGISTRATION_STATUS, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config EPS Network Registration is %s.",
			getStatusTypeString(output_status));
	if (type == 0) {
		sprintf(self->log_content,
				"Disable network registration unsolicited result code.");
	} else if (type == 1) {
		sprintf(self->log_content,
				"Enable network registration unsolicited result code.");
	} else if (type == 2) {
		sprintf(self->log_content,
				"Enable network registration and location information unsolicited result code.");
	} else if (type == 3) {
		sprintf(self->log_content,
				"Enable network registration, location information and EMM cause value information unsolicited result code.");
	} else if (type == 4) {
		sprintf(self->log_content,
				"Enable network registration and location information unsolicited result code for UE wants PSM.");
	} else if (type == 5) {
		sprintf(self->log_content,
				"Enable network registration, location information and EMM cause value information unsolicited result code for UE wants PSM.");
	}

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configSlowClock(struct ViettelSDK *self, uint8_t type) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u",
	CONFIG_SLOW_CLOCK_AT_CMD, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config Slow Clock is %s.",
			getStatusTypeString(output_status));
	if (type == 0) {
		sprintf(self->log_content,
				"Disable slow clock, module will not enter sleep mode.");
	} else if (type == 1) {
		sprintf(self->log_content,
				"Enable slow clock, it is controlled by DTR.");
	} else if (type == 2) {
		sprintf(self->log_content,
				"Enable slow clock automatically. When there is no interrupt, module can enter sleep mode.");
	}
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configPSM(struct ViettelSDK *self, uint8_t type,
		char requestedPeriodicTAU[], char requestedActiveTime[]) {
	StatusType output_status = STATUS_UNKNOWN;
	if (type == 1) {
		sprintf(self->command, "%s=%u,,,\"%s\",\"%s\"",
		PSM_AT_CMD, type, requestedPeriodicTAU, requestedActiveTime);
		strcpy(self->requestedPeriodicTAU, requestedPeriodicTAU);
		strcpy(self->requestedActiveTime, requestedActiveTime);
	} else {
		sprintf(self->command, "%s=%u", PSM_AT_CMD, type);
	}
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config PSM is %s.",
			getStatusTypeString(output_status));
	if (type == 0) {
		sprintf(self->log_content, "Disable PSM.");
	} else if (type == 1) {
		sprintf(self->log_content, "Enable PSM.");
	} else if (type == 2) {
		sprintf(self->log_content,
				"Disable PSM and discard all parameters for PSM.");
	}
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		writeLog(self, LOG_INFO, self->requestedPeriodicTAU, false);
		writeLog(self, LOG_INFO, self->requestedActiveTime, false);
	}
	return output_status;
}


/* Module */
StatusType setPMUMode(struct ViettelSDK *self, bool enable, uint8_t mode) {
	/* AT+QCPMUCFG= */
	StatusType output_status = STATUS_UNKNOWN;
	if (mode > 4) {
		output_status = STATUS_BAD_PARAMETER;
		sprintf(self->log_content, "Set PMU Mode is %s.",
				getStatusTypeString(output_status));
		return output_status;
	}
	if (enable) {
		sprintf(self->command, "%s=1,%u", SET_PMU_MODE, mode);
	} else {
		sprintf(self->command, "%s=0", SET_PMU_MODE);
	}
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Set PMU Mode is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}


void getSensorData(struct ViettelSDK *self) {
	sprintf(self->log_content, "Writing data to FLASH memory.\n");
	writeLog(self, LOG_INFO, self->log_content, true);
	//writeFLASHFlowData(self, FLASH_USER_START_ADDR, FLASH_USER_END_ADDR,*self->data_list->pair.value.uint32_value);
}

StatusType readDynamicParamPDPContext(struct ViettelSDK *self,
bool save_data) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", READ_DYNAMIC_PARAM_PDP_CONTEXT);							// "%s="    ---------> "%s"
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 2,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Read Dynamic Param PDP Context is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType autoTimeUpdate(struct ViettelSDK *self) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", AUTO_TIME_UPDATE);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Auto Time Update is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType enableLocalTimestamp(struct ViettelSDK *self) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", ENABLE_LOCAL_TIMESTAMP);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Enable Local Timestamp is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType getClock(struct ViettelSDK *self, bool save_data) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", GET_CLOCK);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Get Clock is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType checkSIM(struct ViettelSDK *self) {
	/* AT+CPIN? */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	sprintf(self->command, "%s?", ENTER_PIN);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Checking SIM Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */
	}

	return output_status;
}

StatusType setPhoneFunctionality(struct ViettelSDK *self, uint8_t fun) {
	/* AT+CFUN= */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	if ((fun != 0) && (fun != 1) && (fun != 4) && (fun != 7)) {
	}

	/* Send command */
	sprintf(self->command, "%s=%u", PHONE_FUCNTIONALITY, fun);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Set Set Phone Functionality Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */
	}

	return output_status;
}

StatusType checkActivePDPContext(struct ViettelSDK *self) {
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", CHECK_ACTIVE_PDP_CONTEXT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Check Active PDP Context is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS) {
		writeLog(self, LOG_WARNING, self->log_content, false);
	} else {
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

void mainFlow(struct ViettelSDK *self) {
	int8_t try;

	/* Stage 1 -- Check normally operation of module */
	try = 5;
	while (self->stage == 1) {
		try--;

		if (try == 0){
			/* Sleep */
			/* AT+QCPMUCFG */
			setLedMode(self, false);
			if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
				continue;
			}
			//sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}

		/* AT */
		if (checkModule(self) != STATUS_SUCCESS) {
			continue;
		}

		/* ATEx */
		if (setCommandEchoMode(self, false) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CPIN? */
		if (checkSIM(self) != STATUS_SUCCESS) {
			/* AT+QCPMUCFG */
			setLedMode(self, false);
			if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
				continue;
			}
			/* Sleep */
			//sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}

		/* AT+CMEE */
		if (setReportMobileEquipmentError(self, 2) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+QCPMUCFG */
		if (setPMUMode(self, 0, 1) != STATUS_SUCCESS) {
			continue;
		}
		setLedMode(self, true);

		/* ATI */
		if (displayProductID(self) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CGMI */
		if (requestManufacturerID(self) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CGMM */
		if (requestModelID(self) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CGMR */
		if (requestTARevisionIdentification(self) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CGSN */
		if (requestIMEI(self) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CIMI */
		if (requestInternationalMobileSubscriberIdentity(self)
				!= STATUS_SUCCESS) {
			continue;
		}

		/* AT+CBAND */
		if (getAndSetMobileOperationBand(self, 3) != STATUS_SUCCESS) {
			continue;
		}

		/* AT+CSQ */
		if (readSignalQualityReport(self) != STATUS_SUCCESS) {
			continue;
		}

		HAL_Delay(STAGE_DELAY_MS);
		self->stage = 2;
	}






	/* Stage 2 -- Check register to network*/
	try = 3;
	while (self->stage == 2) {


		while (try--) {
			/* AT+CEREG? */
			if (try < 2) {
				//wakeUpModule(self);													// Delete wakeup, need more time for module attach to network
				HAL_Delay(5000);														// Increasing wake up module
			}
			if ((readEPSNetworkRegistrationStatus(self, true) == STATUS_SUCCESS)
					&& (self->stat == 1)) {
				self->stage = 3;
				break;
			}
			HAL_Delay(10000);															// Increase delay when attach to network
		}




		if (try == -1) {
			/* Cannot register automatically. Try to register manually 3 times  */
			try = 3;
			while (try--) {
				sprintf(self->log_content,
						"Cannot register automatically. Try to register manually 3 times. Attempt: %u/3.\n",
						3 - try);
				writeLog(self, LOG_WARNING, self->log_content, true);

				/* AT+CFUN= 0*/
				if (setPhoneFunctionality(self, 0) != STATUS_SUCCESS) {
					continue;
				}

				/* AT*MCGDEFCONT         ---->   AT+CGDCONT           */
				if (setDefaultPSDConnection(self) != STATUS_SUCCESS) {
					continue;
				}

				/* AT+CFUN= */
				if (setPhoneFunctionality(self, 1) != STATUS_SUCCESS) {
					continue;
				}

				HAL_Delay(3000);

				/* AT+CEREG? */
				if ((readEPSNetworkRegistrationStatus(self, true)
						== STATUS_SUCCESS) && (self->stat == 1)) {
					self->stage = 3;
					break;
				}
			}

			if (try == -1) {
				/* Fail to register manually */
				sprintf(self->log_content, "Fail to register manually!");
				writeLog(self, LOG_WARNING, self->log_content, true);

				/* AT+QCPMUCFG */
				setLedMode(self, false);
				if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
					continue;
				}



				/* Sleep */
				sleepMCU(self, SLEEP_INTERVAL);
				resetMCU(self);
			}
		}
	}



	/* Stage 3 */
	while (self->stage == 3) {

		try = 3;
		while (try--) {

			// Shutdown PSM to avoid ERROR
			if (configPSM(self, 0, "01011111", "00001010") != STATUS_SUCCESS) {
				continue;
			}

			/*AT+CGCONTRDP*/
			if (readDynamicParamPDPContext(self, false) != STATUS_SUCCESS) {
				continue;
			}
			/*AT+CTZU=1*/
			if (autoTimeUpdate(self) != STATUS_SUCCESS) {
				continue;
			}
			/*AT+CCLK*/
			if (getClock(self, false) != STATUS_SUCCESS) {
				continue;
			}
//			/*AT+CENG?*/
//			if (readReportNetworkState(self) == STATUS_SUCCESS) {
//				break;
//			}
		}

//		if (try == -1) {
//			sleepMCU(self, SLEEP_INTERVAL);
//			resetMCU(self);
//		}



		/* Register PSM with cell */
		/*AT+CPSMSTATUS*/
		if (configWakeupIndication(self, 1) != STATUS_SUCCESS) {
			continue;
		}

		if (configEPSNetworkRegistration(self, 0) != STATUS_SUCCESS) {
			continue;
		}

		/*AT+CEREG=4*/
		if (configEPSNetworkRegistration(self, 4) != STATUS_SUCCESS) {
			continue;
		}


//		// Set up PSM again
//		if (configPSM(self, 1, "01011111", "00001010") != STATUS_SUCCESS) {
//			continue;
//		}

		HAL_Delay(1000);

		try = 3;
		while (try--) {
			if (readEPSNetworkRegistrationStatus(self, true)
					== STATUS_SUCCESS) {
				break;
			}
		}


		if (try == -1) {
			/* AT+QCPMUCFG */
			setLedMode(self, false);
			if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
				continue;
			}

			sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}



		if (UDP_SELECT){
			self->stage = 4;
		}

//		else{
//			self->stage = 4;
//		}


	}


	/* Stage 4 _ UDP */
	while (self->stage == 4) {


		sendCommand(self, "AT+NETOPEN",
			1,
			RUN_COMMAND_TIMEOUT_MS_DEFAULT);

		sendCommand(self, "AT+CIPOPEN=0,\"UDP\",,,12345",
			1,
			RUN_COMMAND_TIMEOUT_MS_DEFAULT);


		/* AT+CSOCON= .... */
		try = 3;
		while (try--) {

			if (UDP_Connect(self) == STATUS_SUCCESS) {
				break;
			}
		}
		/* Fail to UDP connection */
		if (try == -1) {
			/* AT+QCPMUCFG */
			setLedMode(self, false);
			if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
				continue;
			}

			sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}




		/* Package data to send */
		getData(self);


		/* SEND DATA */
		try = 3;
		int udp_success = 0;
		while (try--) {
			/* AT+CASEND */
			if ( send_UDP(self,"115.78.92.253",12345) != STATUS_SUCCESS) {
				//wakeUpModule(self);
				continue;
			}
			else {
				udp_success = 1;
				HAL_Delay(3000);
				//receive_UDP(self);
			}
		}



		if (try == -1 && udp_success == 0) {
			/* Not receive response from platform after 3 attempts */
			if (readEPSNetworkRegistrationStatus(self, true)
					!= STATUS_SUCCESS) {
				continue;
			}

			if (self->stat == 1) {
				/* Bad signal. Cannot publish messages */
				sprintf(self->log_content,
						"Bad signal. Cannot publish messages");
				writeLog(self, LOG_WARNING, self->log_content, true);
			} else if (self->stat == 2) {
				sprintf(self->log_content, "Cannot register. Clear PSM.");
				writeLog(self, LOG_WARNING, self->log_content, true);

				/* Clear PSM */
				if (configEPSNetworkRegistration(self, 0)
						!= STATUS_SUCCESS) {
					continue;
				}

				if (configPSM(self, 0, "", "") != STATUS_SUCCESS) {
					continue;
				}

				/* AT+QCPMUCFG */
				setLedMode(self, false);
				if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
					continue;
				}

				sleepMCU(self, SLEEP_INTERVAL);
				resetMCU(self);

			} else {
				sprintf(self->log_content,
						"Unsupport code of EPS Network Registration Status: %u",
						self->stat);
				writeLog(self, LOG_INFO, self->log_content, true);
			}
		}

		/* AT+CACLOSE  & Deactive PDP*/
		UDP_Close(self);
		//ActivePDP(self, 0, 0);


		// Set up PSM again

		configEPSNetworkRegistration(self,4);
		configPSM(self, 1, "10100101", "00000010");
		HAL_Delay(5000);
		/* AT+QCPMUCFG */
		setLedMode(self, false);
		if (setPMUMode(self, 1, 4) != STATUS_SUCCESS) {
		continue;
		}

		//sendCommand(self, "AT+CPSMRDP",RUN_COMMAND_COUNTER_DEFAULT,RUN_COMMAND_TIMEOUT_MS_DEFAULT);

		HAL_Delay(2000);

		self->stage = 0;
	}

    /*-----------------------------------------------
		Stage 0: Operate successfully, go to sleep
	--------------------------------------------------*/

	/* Wait module to go to sleep */

	self->passively_listen = true;
	self->sleep = false;
	self->psm_timer = HAL_GetTick();
	clearMainBuffer(self);

	try = 7;
	while(try--){
		self->psm_timer = HAL_GetTick();
		while (HAL_GetTick() - self->psm_timer <= WAIT_FOR_PSM_MODE) {
			if (self->sleep) {
				HAL_IWDG_Refresh(&hiwdg);
				try = 0;
				break;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
	try = 0;
	HAL_Delay(2000);


	/* Sleep MCU after Module enter PSM*/
	/* AT+QCPMUCFG */
	setLedMode(self, false);
	setPMUMode(self, 1, 4);

	sleepMCU(self, SLEEP_INTERVAL);
	HAL_IWDG_Refresh(&hiwdg);




	try = 3;
	GetUpModule(self);
	while (try--) {
		HAL_Delay(2000);
		if (checkModule(self) == STATUS_SUCCESS) {
			setCommandEchoMode(self, false);
			break;
		}
	}
	/* Fail to connect to module */
	if (try == -1) {
		resetMCU(self);
	}



	try = 3;
	while (try--) {
		if (readEPSNetworkRegistrationStatus(self, true) == STATUS_SUCCESS
				|| self->stat == 1) {
			/* Still conenct to cell */
			self->stage = 3;
			HAL_Delay(1000);
			return;
		}

		else HAL_Delay(1000);
	}

	if (try == -1) {
		/* Clear PSM */
		self->stage = 1;
		return;
	}

}
