#ifndef VIETTELSDK_UTILITIES_H_
#define VIETTELSDK_UTILITIES_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "ViettelSDK.h"
#include "ViettelSDK_Constants.h"

struct ViettelSDK;
enum ValueType;
union Value;

char** tokenizeString(struct ViettelSDK *self, char *str, const char *delim,
		uint8_t *num_tokens);

void removeSubstring(char *str, const char *sub);

void removeChars(char *str, const char *sub);

void floatToString(float num, char *str, uint8_t decimal_digit);

char* castToString(union Value data, enum ValueType value_type);

#endif /* VIETTELSDK_UTILITIES_H_ */
