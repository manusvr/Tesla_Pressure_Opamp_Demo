#ifndef SCAN_H
#define SCAN_H

#include <stdbool.h>
#include <stdint.h>

#define SCAN_DEBUG_SLOW 0               /* 1 = 30 Hz frames for debugging, 0 = 120 Hz */

#if SCAN_DEBUG_SLOW
#define SCAN_FRAMERATE 30               /* Hz */
#else
#define SCAN_FRAMERATE 120              /* Hz */
#endif
#define  SCAN_ROWS      9                /* rows 0..8 */
#define SCAN_CHANNELS  4                /* TIA 0..3 */
#define SCAN_STEPS     (SCAN_ROWS + 2)  /* rows, null, CAL */

typedef struct
{
    uint16_t raw[SCAN_ROWS][SCAN_CHANNELS];
    uint16_t null[SCAN_CHANNELS];   /* null reference, same path, no row driven */
    uint16_t cal[SCAN_CHANNELS];    /* CAL_A, 9.09 kOhm into every column */
    uint32_t frame;                 /* frame counter */
} scan_frame_t;

void scan_start(void);              /* starts TIM6; frames then run by themselves */
bool scan_frame_ready(void);
void scan_take(scan_frame_t *out);  /* copy the last finished frame */

#endif /* SCAN_H */
