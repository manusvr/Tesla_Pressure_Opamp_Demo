#ifndef SENSE_H
#define SENSE_H

#include <stdint.h>

#define SENSE_DAC_SCALES 5          /* DAC ranging: scale 0 (least drive) .. 4 (most drive) */
#define SENSE_COUNTS_PER_LSB 4U     /* ADC oversampling 16x, readings 0..16380 */
#define SENSE_STEP_FULL (1850U * SENSE_COUNTS_PER_LSB)   /* largest trusted step: PGA near its 0.1 V floor above this */

void sense_init(void);              /* once, before scan_start(); starts at scale 0 */
void sense_set_scale(uint8_t s);    /* DAC drive level; safe inside the TIM6 interrupt */
uint16_t sense_drive(uint8_t s);    /* relative drive of scale s, only ratios are used: nominal or measured (CAL_SCALE_ENABLE) */
void sense_read4(uint16_t out[4]);  /* Read TIA 0..3;  */

#endif /* SENSE_H */
