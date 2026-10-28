#ifndef SCAN_H
#define SCAN_H

#include <stdint.h>

#define SCAN_ROWS     9             /* rows 0..8 */
#define SCAN_CHANNELS 4             /* TIA 0..3 */

typedef struct
{
    uint16_t raw[SCAN_ROWS][SCAN_CHANNELS];
    uint16_t null[SCAN_CHANNELS];   /* null reference, same path, no row driven */
    uint16_t cal[SCAN_CHANNELS];    /* CAL_A, 9.09 kOhm into every column */
    uint32_t frame;                 /* frame counter */
} scan_frame_t;

void dwt_init(void);                /* call once after SystemClock_Config(), before delay_us() */
void delay_us(uint32_t us);
void scan_frame(scan_frame_t *f);

#endif /* SCAN_H */
