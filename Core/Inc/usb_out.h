#ifndef USB_OUT_H
#define USB_OUT_H

#include <stdint.h>
#include "mapping.h"

/* one text line "v0 v1 ... v28 \r\n" over USB CDC; dropped while no host or still sending */
void output_sensor_data(const uint16_t sensor_values[TAXEL_COUNT]);

#endif /* USB_OUT_H */
