#ifndef SENSE_H
#define SENSE_H

#include <stdint.h>

#define SENSE_DAC_NOMINAL 1911U     /* VDRIVE 4096/3.3v=0.0008 0.0008*1911 = 1.540 V */

void sense_init(void);              /* once, before scan_start() */
void sense_read4(uint16_t out[4]);  /* TIA 0..3; safe inside the TIM6 interrupt */

#endif /* SENSE_H */
