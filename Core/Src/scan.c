#include "scan.h"
#include "main.h"
#include "mux.h"
#include "sense.h"

#define SETTLE_US 300U              /* TIA settle time with the X5R feedback capacitors */

void dwt_init(void)                 /* start the Cortex-M4 cycle counter */
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < cycles)
    {
    }
}

void scan_frame(scan_frame_t *f)    /* 11 steps: rows 0..8, null, CAL */
{
    for (uint8_t k = 0; k < SCAN_ROWS; k++)
    {
        select_row(k);
        delay_us(SETTLE_US);
        sense_read4(f->raw[k]);
    }

    select_null_ref();
    delay_us(SETTLE_US);
    sense_read4(f->null);

    select_cal_a();
    delay_us(SETTLE_US);
    sense_read4(f->cal);

    mux_off();                      /* nothing driven between frames */
    f->frame++;
}
