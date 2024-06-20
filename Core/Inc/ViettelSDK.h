#ifndef VIETTELSDK_H_
#define VIETTELSDK_H_

#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "time.h"

#include "ViettelSDK_Constants.h"
#include "ViettelSDK_Collections.h"
#include "ViettelSDK_ATCommandList.h"
#include "ViettelSDK_Utils.h"
#include "ViettelSDK_Data.h"

/* Static variable */
static char static_null_string[1] = { 0 }; // intentionally left unused

/* Extern variables */
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;

struct ViettelSDK {

	/* Sleep */
	bool sleep;

	/* Stage */
	uint8_t stage;

	/* Delay */
	uint32_t general_delay_timer;

	/* Power Pin Wake Up */
	uint8_t warming_up_counter;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *pwr_pin_GPIOx;
	uint16_t pwr_pin_GPIO_Pin;
	RTC_HandleTypeDef* rtc_timer;
	RTC_TimeTypeDef sTime;
	RTC_AlarmTypeDef sAlarm;

	/* Passive response */
	uint32_t receiver_subscribe_timer;
	uint32_t psm_timer;
	bool passively_listen;

	/* Header/Footer */
	bool run_first_time;
	uint16_t attempt_count;
	uint32_t period_timer;
	bool successfully_initialized;
	uint8_t fail_increment;

	/* Commands */
	enum StatusType return_status_type;
	struct CommandResponse command_response;
	char response_receive_buffer[RESPONSE_RECEIVE_BUFFER_SIZE];
	char response_main_buffer[RESPONSE_MAIN_BUFFER_SIZE];
	uint16_t old_position;
	uint16_t new_position;
	uint32_t command_timer;
	bool response_received;
	char command[COMMAND_SIZE];
	char *token;
	char delimiter[DELIMITER_SIZE];

	/* Command Attributes */
	Cell cell_data;
	char ccid[CCID_MAX_LENGTH + 1];
	char imei[IMEI_MAX_LENGTH + 1];
	char ta_revision_id[TA_REVISION_ID_MAX_LENGTH + 1];
	uint8_t n;
	uint8_t stat;
	bool echo_mode;
	bool led_mode;
	bool psm_mode, eDRX_mode;
	char requestedPeriodicTAU[9], requestedActiveTime[9], requestedeDRXCycle[9],
			requestedPTW[9];
	char returnedPeriodicTAU[9], returnedActiveTime[9], returnedeDRXCycle[9],
			returnedPTW[9];
	MQTTParams mqtt_params;

	/* Data */
	bool addDataSuccessfully;
	uint32_t previous_pulse;
	struct DataNode *data_list;
	int data7022[5];
	char message7022[MESSAGE_SIZE];

	/* Debugger */
	bool detail_log;
	char log_content[LOG_SIZE];

	/* Peripherals */
	UART_HandleTypeDef *debugger_uart;
	UART_HandleTypeDef *module_uart;
	DMA_HandleTypeDef *module_uart_hdma;
};

/* Peripheral */
void sleepMCU(struct ViettelSDK *self, uint16_t period_in_seconds);

void wakeUpMCU(struct ViettelSDK *self);

void resetMCU(struct ViettelSDK *self);

void wakeUpModule(struct ViettelSDK *self);

void powerOffModule(struct ViettelSDK *self, bool urgent);

void pullDownPWRpin(struct ViettelSDK *self);

void noPullPWRpin(struct ViettelSDK *self);

void setupSleepTimer(struct ViettelSDK *self);

void startSleepTimer(struct ViettelSDK *self, uint16_t period_ms);

/* Data */
void getSensorData(struct ViettelSDK *self);

void updateFLASHData(struct ViettelSDK *self);

void readFLASHPreviousFlowData(struct ViettelSDK *self, uint32_t address);

void writeFLASHFlowData(struct ViettelSDK *self, uint32_t start_page,
		uint32_t end_page, uint32_t data32);

/* Debugger */
void writeLog(struct ViettelSDK *self, enum LogType log_type,
		char log_content[], bool default_log);

/* SDK */
void logStartBanner(struct ViettelSDK *self);

void initializeSDK(struct ViettelSDK *self, UART_HandleTypeDef *debugger_uart,
		UART_HandleTypeDef *module_uart, DMA_HandleTypeDef *module_uart_hdma,
		GPIO_TypeDef *pwr_pin_GPIOx, uint16_t pwr_pin_GPIO_Pin, RTC_HandleTypeDef* hrtc);

void connectToPlatform(struct ViettelSDK *self);

void setupMQTTParameters(struct ViettelSDK *self, char server[], char port[],
		uint16_t command_timeout_ms, uint16_t bufsize, uint8_t cid,
		char client_id[], uint8_t version, char mqtt_user[], char mqtt_pass[]);

void mainFlow(struct ViettelSDK *self);

bool validateUARTPorts(struct ViettelSDK *self);

struct CommandResponse sendCommand(struct ViettelSDK *self, char *command,
		uint8_t count, uint32_t timeout);

void clearCommand(struct ViettelSDK *self);

void clearMainBuffer(struct ViettelSDK *self);

void resetDMAInterrupt(struct ViettelSDK *self);

void saveResponse(struct ViettelSDK *self, UART_HandleTypeDef *huart,
		uint16_t size);

void display(struct ViettelSDK *self, char data[]);

StatusType checkModule(struct ViettelSDK *self);

StatusType setCommandEchoMode(struct ViettelSDK *self, bool echo_mode);

StatusType readCCID(struct ViettelSDK *self);

StatusType requestIMEI(struct ViettelSDK *self);

StatusType requestTARevisionIdentification(struct ViettelSDK *self);

StatusType readSignalQualityReport(struct ViettelSDK *self);

StatusType setEPSNetworkRegistrationStatus(struct ViettelSDK *self,
		uint8_t code);

StatusType readEPSNetworkRegistrationStatus(struct ViettelSDK *self,
bool save_data);

StatusType setOperatorSelection(struct ViettelSDK *self, uint8_t mode);

StatusType readOperatorSelection(struct ViettelSDK *self);

StatusType setReportMobileEquipmentError(struct ViettelSDK *self, uint8_t mode);

StatusType displayProductID(struct ViettelSDK *self);

StatusType requestManufacturerID(struct ViettelSDK *self);

StatusType getAndSetMobileOperationBand(struct ViettelSDK *self,
		enum AT_QCBAND mode);

StatusType readReportNetworkState(struct ViettelSDK *self);

StatusType requestModelID(struct ViettelSDK *self);

StatusType requestInternationalMobileSubscriberIdentity(struct ViettelSDK *self);

StatusType readpreferredOperatorList(struct ViettelSDK *self);

StatusType setDefaultPSDConnection(struct ViettelSDK *self);

StatusType configWakeupIndication(struct ViettelSDK *self, bool type);

StatusType configEPSNetworkRegistration(struct ViettelSDK *self, uint8_t type);

StatusType enableReleaseAssistanceIndication(struct ViettelSDK *self,
		uint8_t type);

StatusType configCIoTOptimization(struct ViettelSDK *self, uint8_t type,
		uint8_t supportedUEOpt, uint8_t preferredUEOpt);

StatusType configSlowClock(struct ViettelSDK *self, uint8_t type);

StatusType configSocketRetention(struct ViettelSDK *self, bool type);

StatusType configPSM(struct ViettelSDK *self, uint8_t type,
		char requestedPeriodicTAU[], char requestedActiveTime[]);

StatusType queryPSM(struct ViettelSDK *self, bool save_data);

StatusType configeDRX(struct ViettelSDK *self, uint8_t type,
		char requestedeDRXCycle[], char requestedPTW[]);

StatusType queryeDRX(struct ViettelSDK *self, bool save_data);

StatusType checkMQTTConnection(struct ViettelSDK *self);

StatusType newMQTT(struct ViettelSDK *self, uint8_t cid);

StatusType sendMQTTConnect(struct ViettelSDK *self);

StatusType sendMQTTPub(struct ViettelSDK *self, char topic[], uint8_t qos,
bool retained, bool dup, char message[]);

StatusType sendMQTTSub(struct ViettelSDK *self, char topic[], uint8_t qos);

StatusType sendMQTTUnsub(struct ViettelSDK *self, char topic[]);

StatusType MQTTDisconnect(struct ViettelSDK *self);

StatusType checkSIM(struct ViettelSDK *self);

StatusType setPhoneFunctionality(struct ViettelSDK *self, uint8_t fun);

StatusType readDynamicParamPDPContext(struct ViettelSDK *self, bool save_data);

StatusType autoTimeUpdate(struct ViettelSDK *self);

StatusType enableLocalTimestamp(struct ViettelSDK *self);

StatusType getClock(struct ViettelSDK *self, bool save_data);

StatusType checkActivePDPContext(struct ViettelSDK *self);

#endif /* VIETTELSDK_H_ */
