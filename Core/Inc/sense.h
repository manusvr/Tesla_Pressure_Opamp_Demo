#ifndef SENSE_H
#define SENSE_H

#include <stdint.h>

void sense_read4(uint16_t out[4]);  /* start all four ADCs, return TIA 0..3 */

#endif /* SENSE_H */
