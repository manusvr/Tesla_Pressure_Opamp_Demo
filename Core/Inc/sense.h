#ifndef SENSE_H
#define SENSE_H

#include <stdint.h>

#define SENSE_DAC_SCALES 5          /* DAC ranging: scale 0 (least drive) .. 4 (most drive) */

void sense_init(void);              /* once, before scan_start(); starts at scale 0 */
void sense_set_scale(uint8_t s);    /* DAC drive level; safe inside the TIM6 interrupt */
uint16_t sense_drive(uint8_t s);    /* |dV| of scale s in DAC codes: HVREF - DAC code */
void sense_read4(uint16_t out[4]);  /* Read TIA 0..3;  */

#endif /* SENSE_H */
