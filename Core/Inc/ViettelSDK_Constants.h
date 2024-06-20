#ifndef VIETTELSDK_CONSTANTS_H_
#define VIETTELSDK_CONSTANTS_H_

/* Initialize parameter constants */
static const char *SUCCESS_COMMAND_SIGN[] = { "OK", "+CSONMI","+SMSTATE"}; // intentionally left unused, Add SUCCESS_STATE for CNACT=0,1

static const char *ERROR_COMMAND_SIGN[] = { "ERROR\r\n", "ERROR" }; // intentionally left unused

static const char *PASSIVE_RESPONSE_SIGN[] = { "NORMAL POWER DOWN",
		"+CPSMSTATUS: \"EN", "+CPSMSTATUS: \"EX", "+SMSUB:" }; // intentionally left unused

#ifndef COMMAND_DELAY_MS
#define COMMAND_DELAY_MS (500U)
#endif

#ifndef COMMAND_SIZE
#define COMMAND_SIZE (600U)
#endif

#ifndef CONNECTING_PERIOD_MS
#define CONNECTING_PERIOD_MS (5000U)
#endif

#ifndef DELIMITER_SIZE
#define DELIMITER_SIZE (4U)
#endif

#ifndef DETAIL_LOG_SETUP
#define DETAIL_LOG_SETUP true
#endif

#ifndef ERROR_RESPONSE_DELAY_MS
#define ERROR_RESPONSE_DELAY_MS (2000U)
#endif

#ifndef ERROR_RESPONSE_LENGTH
#define ERROR_RESPONSE_LENGTH (sizeof(ERROR_COMMAND_SIGN) / sizeof(ERROR_COMMAND_SIGN[0]))
#endif

#ifndef INITIALIZE_DELAY_MS
#define INITIALIZE_DELAY_MS (1500U)
#endif

#ifndef MAX_FAIL_INCREMENT
#define MAX_FAIL_INCREMENT (5U)
#endif

#ifndef MODULE_PWR_PIN_NO_PULL_DELAY_MS
#define MODULE_PWR_PIN_NO_PULL_DELAY_MS (300U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef MODULE_PWR_PIN_PULL_DOWN_DELAY_MS
#define MODULE_PWR_PIN_PULL_DOWN_DELAY_MS (1100U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef PASSIVE_RESPONSE_LENGTH
#define PASSIVE_RESPONSE_LENGTH (sizeof(PASSIVE_RESPONSE_SIGN) / sizeof(PASSIVE_RESPONSE_SIGN[0]))
#endif

#ifndef RECEIVE_SUBSCRIBE_TIMEOUT
#define RECEIVE_SUBSCRIBE_TIMEOUT (10000U)
#endif

#ifndef RESPONSE_MAIN_BUFFER_SIZE
#define RESPONSE_MAIN_BUFFER_SIZE (800U)
#endif

#ifndef RESPONSE_RECEIVE_BUFFER_SIZE
#define RESPONSE_RECEIVE_BUFFER_SIZE (300U)
#endif

#ifndef SLEEP_INTERVAL
#define SLEEP_INTERVAL (80U)
#endif

#ifndef STAGE_DELAY_MS
#define STAGE_DELAY_MS (2000U)
#endif

#ifndef SUCCESS_COMMAND_LENGTH
#define SUCCESS_COMMAND_LENGTH (200U)
#endif

#ifndef SUCCESS_RESPONSE_LENGTH
#define SUCCESS_RESPONSE_LENGTH (sizeof(SUCCESS_COMMAND_SIGN) / sizeof(SUCCESS_COMMAND_SIGN[0]))
#endif

#ifndef WAIT_FOR_MODULE_TO_WAKE_MS
#define WAIT_FOR_MODULE_TO_WAKE_MS (9000U)
#endif

#ifndef WAIT_FOR_PSM_MODE
#define WAIT_FOR_PSM_MODE (10000U)						// Decrease delay from 20 to 10, but we will reset WDG_timer because we set T3324 is 32 seconds
#endif

#ifndef WAKE_UP_MODULE_PUSH_COUNT
#define WAKE_UP_MODULE_PUSH_COUNT (2U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef WARMING_UP_COUNT
#define WARMING_UP_COUNT (4U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

/* Debugger */
#ifndef LOG_SIZE
#define LOG_SIZE (600U)
#endif

#ifndef LOG_TIMEOUT_MS
#define LOG_TIMEOUT_MS (600U)
#endif

/* Command attribute constants */
#ifndef CCID_MAX_LENGTH
#define CCID_MAX_LENGTH (20U)
#endif

#ifndef CLIENT_MAX_LENGTH
#define CLIENT_MAX_LENGTH (120U)
#endif

#ifndef IMEI_MAX_LENGTH
#define IMEI_MAX_LENGTH (16U)
#endif

#ifndef MAXIMUM_NEIGHBOR_CELL
#define MAXIMUM_NEIGHBOR_CELL (4U) /* Do not change this constant for SIMCom7020 */
#endif

#ifndef MESSAGE_SIZE
#define MESSAGE_SIZE (200U)
#endif

#ifndef MQTT_CONNECT_CLEAN_SESSION
#define MQTT_CONNECT_CLEAN_SESSION (1)
#endif

#ifndef MQTT_CONNECT_WILL_FLAG
#define MQTT_CONNECT_WILL_FLAG (0U)
#endif

#ifndef MQTT_KEEP_ALIVE_INTERVAL
#define MQTT_KEEP_ALIVE_INTERVAL (64800U)
#endif

#ifndef MQTT_PASS_MAX_LENGTH
#define MQTT_PASS_MAX_LENGTH (100U)
#endif

#ifndef MQTT_SERVER_MAX_LENGTH
#define MQTT_SERVER_MAX_LENGTH (51U)
#endif

#ifndef MQTT_SERVER_PORT_MAX_LENGTH
#define MQTT_SERVER_PORT_MAX_LENGTH (6U)
#endif

#ifndef MQTT_USER_MAX_LENGTH
#define MQTT_USER_MAX_LENGTH (100U)
#endif

#ifndef NEIGHBOR_CELL_ELEMENT_COUNT
#define NEIGHBOR_CELL_ELEMENT_COUNT (4U)
#endif

#ifndef RUN_COMMAND_COUNTER_DEFAULT
#define RUN_COMMAND_COUNTER_DEFAULT (4U)
#endif

#ifndef RUN_COMMAND_TIMEOUT_MS_DEFAULT
#define RUN_COMMAND_TIMEOUT_MS_DEFAULT (1000U)
#endif

#ifndef SERVING_CELL_ELEMENT_COUNT
#define SERVING_CELL_ELEMENT_COUNT (13U)
#endif

#ifndef TA_REVISION_ID_MAX_LENGTH
#define TA_REVISION_ID_MAX_LENGTH (18U)
#endif

#ifndef UDP_SELECT
#define UDP_SELECT (1)
#endif

/* Data Constants */
#ifndef FLASH_ADDR_PAGE_126
#define FLASH_ADDR_PAGE_126 ((uint32_t)0x0801F800)
#endif

#ifndef FLASH_ADDR_PAGE_127
#define FLASH_ADDR_PAGE_127 ((uint32_t)0x0801FC00)
#endif

#ifndef FLASH_USER_START_ADDR
#define FLASH_USER_START_ADDR (FLASH_ADDR_PAGE_126)
#endif

#ifndef FLASH_USER_END_ADDR
#define FLASH_USER_END_ADDR (FLASH_ADDR_PAGE_127 + FLASH_PAGE_SIZE)
#endif

#ifndef KEY_MAX_LENGTH
#define KEY_MAX_LENGTH (30U)
#endif

/* Utilities Constants */
#ifndef MAXIMUM_TOKEN_COUNT
#define MAXIMUM_TOKEN_COUNT (15U)
#endif

#endif /* VIETTELSDK_CONSTANTS_H_ */
