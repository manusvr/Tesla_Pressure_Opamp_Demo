#ifndef SCAN_H
#define SCAN_H

#include <stdbool.h>
#include <stdint.h>
#include "app_config.h"

#define  SCAN_ROWS      9
#define SCAN_CHANNELS  4                /* TIA 0..3 */
#define SCAN_STEPS     (SCAN_ROWS + 2)  /* Rows + null step + CAll */

typedef struct
{
    uint16_t raw[SCAN_ROWS][SCAN_CHANNELS];
    uint16_t null[SCAN_CHANNELS];   /* null reference, same path, no row driven */
    uint16_t cal[SCAN_CHANNELS];    /* CAL_A, 9.09 kOhm on every column */
    uint8_t  scale[SCAN_ROWS];      /* DAC scale per row of this frame, 0..4 (all equal unless RANGING_PER_ROW) */
} scan_frame_t;

void scan_start(void);              /* starts TIM6 Triggers frame steps */
bool scan_frame_ready(void);
void scan_take(scan_frame_t *out);  /* copy the last finished frame */

#endif /* SCAN_H */
