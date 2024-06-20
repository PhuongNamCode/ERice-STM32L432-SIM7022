#ifndef VIETTELSDK_DATA_H_
#define VIETTELSDK_DATA_H_

#include "stdint.h"
#include "math.h"

#include "ViettelSDK_Constants.h"
#include "ViettelSDK_Collections.h"
#include "ViettelSDK.h"

// Forward declaration of ViettelSDK struct
struct ViettelSDK;

typedef enum ValueType {
	/* bool */
	VALUE_BOOL,

	/* 1 byte */
	VALUE_CHAR, VALUE_UNSIGNED_CHAR, VALUE_UINT8_T,

	/* 2 bytes */
	VALUE_SHORT, VALUE_UNSIGNED_SHORT, VALUE_UINT16_T,

	/* 4 bytes */
	VALUE_INT, VALUE_UINT32_T,

	/* 4 or 8 bytes */
	VALUE_LONG,

	/* Decimal point */
	VALUE_FLOAT,

	/* String */
	VALUE_CHAR_ARRAY

} ValueType;

// Union to store different types of values
typedef union Value {
	bool *bool_value;
	char *char_value;
	unsigned char *unsigned_char_value;
	uint8_t *uint8_value;
	short *short_value;
	unsigned short *unsigned_short_value;
	uint16_t *uint16_value;
	int *int_value;
	long *long_value;
	uint32_t *uint32_value;
	float *float_value;
	char *char_array_value;
} Value;

// Struct to represent a key-value pair with a variable value datatype
typedef struct Pair {
	char key[KEY_MAX_LENGTH];
	Value value;
	enum ValueType value_type;
} Pair;

typedef struct DataNode {
	struct Pair pair;
	struct DataNode *next;
} DataNode;

void addData(struct ViettelSDK *self, char key[], void *value,
		enum ValueType value_type);

void packData(struct ViettelSDK *self);

#endif /* VIETTELSDK_DATA_H_ */
