#ifndef USB_OUT_H
#define USB_OUT_H

#include <stdint.h>
#include "mapping.h"


void output_sensor_data(const uint16_t sensor_values[TAXEL_COUNT]);

#endif /* USB_OUT_H */
