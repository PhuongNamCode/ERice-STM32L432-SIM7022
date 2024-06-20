#include "ViettelSDK_Data.h"

void addData(struct ViettelSDK *self, char key[], void *value,
		enum ValueType value_type) {

	/* Fail add previous data, then not add more data */
	if (!self->addDataSuccessfully) {
		return;
	}

	DataNode *new_node = malloc(sizeof(DataNode));

	if (!new_node || (strlen(key) > KEY_MAX_LENGTH)) {
		self->addDataSuccessfully = false;
		return;
	}

	strcpy(new_node->pair.key, key);

	switch (value_type) {
	case VALUE_BOOL:
		new_node->pair.value.bool_value = (bool*) value;
		new_node->pair.value_type = VALUE_BOOL;
		break;
	case VALUE_CHAR:
		new_node->pair.value.char_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR;
		break;
	case VALUE_UNSIGNED_CHAR:
		new_node->pair.value.unsigned_char_value = (unsigned char*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_CHAR;
		break;
	case VALUE_UINT8_T:
		new_node->pair.value.uint8_value = (uint8_t*) value;
		new_node->pair.value_type = VALUE_UINT8_T;
		break;
	case VALUE_SHORT:
		new_node->pair.value.short_value = (short*) value;
		new_node->pair.value_type = VALUE_SHORT;
		break;
	case VALUE_UNSIGNED_SHORT:
		new_node->pair.value.unsigned_short_value = (unsigned short*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_SHORT;
		break;
	case VALUE_UINT16_T:
		new_node->pair.value.uint16_value = (uint16_t*) value;
		new_node->pair.value_type = VALUE_UINT16_T;
		break;
	case VALUE_INT:
		new_node->pair.value.int_value = (int*) value;
		new_node->pair.value_type = VALUE_INT;
		break;
	case VALUE_LONG:
		new_node->pair.value.long_value = (long*) value;
		new_node->pair.value_type = VALUE_LONG;
		break;
	case VALUE_UINT32_T:
		new_node->pair.value.uint32_value = (uint32_t*) value;
		new_node->pair.value_type = VALUE_UINT32_T;
		break;
	case VALUE_FLOAT:
		new_node->pair.value.float_value = (float*) value;
		new_node->pair.value_type = VALUE_FLOAT;
		break;
	case VALUE_CHAR_ARRAY:
		new_node->pair.value.char_array_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR_ARRAY;
		break;
	default:
		return;
	}

	/* Set the next node to NULL */
	new_node->next = NULL;

	/* If the list is empty, set the new node as the head */
	if (!self->data_list) {
		self->data_list = new_node;
	}
	/* Otherwise, traverse the list to the end and append the new node */
	else {
		DataNode *current_node = self->data_list;
		while (current_node->next) {
			current_node = current_node->next;
		}
		current_node->next = new_node;
	}

}

void packData(struct ViettelSDK *self) {
	if (!self->data_list) {
		/* Not add data yet, use sample data */
		char sample_data[] = "sample_data";
		addData(self, "data", &sample_data, VALUE_CHAR_ARRAY);
	}

	char buffer[MESSAGE_SIZE] = { 0 };
	DataNode *current_node = self->data_list;

	strcat(buffer, "{");

	while (current_node) {
		/* add key */
		strcat(buffer, "\"");
		strcat(buffer, current_node->pair.key);
		strcat(buffer, "\"");

		strcat(buffer, ":");

		/* add value */
		strcat(buffer,
				castToString(current_node->pair.value,
						current_node->pair.value_type));

		if (current_node->next) {
			strcat(buffer, ",");
		}
		current_node = current_node->next;
	}

	strcat(buffer, "}");

	writeLog(self, LOG_INFO, buffer, true);

	/* Change to hexa string */
	for (int i = 0; i < strlen(buffer); i++) {
		sprintf(self->mqtt_params.message + 2 * i, "%02X", buffer[i]); // Convert each character to its hex value
	}
}
