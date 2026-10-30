#include "scan.h"
#include "tim.h"                                    /* htim6: 1 MHz tick */
#include "mux.h"
#include "sense.h"
#include "ranging.h"

#define SETTLE_US 300U                              /* minimum TIA settle time per step */
#define FRAME_US  (1000000U / SCAN_FRAMERATE)       /* 8333 us at 120 Hz */
#define STEP_US   (FRAME_US / SCAN_STEPS)           /* steps spread over the frame: 757 us at 120 Hz */
_Static_assert(STEP_US >= SETTLE_US, "SCAN_FRAMERATE too high: step shorter than TIA settle time");
_Static_assert(STEP_US <= 65536U, "TIM6 is 16-bit: SCAN_FRAMERATE too low");

static scan_frame_t buffer[2];                         /* ISR fills one, main reads the other */
static uint8_t fill=0;
static uint8_t step;                                /* step being settled, 0..SCAN_STEPS-1 */
static volatile uint8_t done;
static volatile bool ready;

static void select_step(uint8_t s)
{
    if (s < SCAN_ROWS)
    {
        select_row(s);
    }
    else if (s == SCAN_ROWS)
    {
        select_null_ref();
    }
    else
    {
        select_cal_a();
    }
}

static uint16_t *step_dest(scan_frame_t *f, uint8_t s)
{
    if (s < SCAN_ROWS)
    {
        return f->raw[s];
    }
    return (s == SCAN_ROWS) ? f->null : f->cal;
}

static void scan_tick(void)                         /* runs every TIM6 tick which is Step_US  */
{
    sense_read4(step_dest(&buffer[fill], step));      
    step++;

    if (step == SCAN_STEPS)                         /* frame complete switch to other buffer for filling */
    {
        uint8_t s = ranging_next_scale(&buffer[fill]);  /* decide from the frame just finished */

        done = fill;
        ready = true;
        fill = !fill;
        step = 0;

        sense_set_scale(s);                         /* DAC changes only between frames: null and CAL match the rows */
        buffer[fill].scale = s;
    }

    select_step(step);                               /* select next step for the upcoming tick */
}

void scan_start(void)
{
    step = 0;
    select_step(0);                                 
    __HAL_TIM_SET_AUTORELOAD(&htim6, STEP_US - 1U);
    HAL_TIM_Base_Start_IT(&htim6);
}

bool scan_frame_ready(void)
{
    return ready;
}

void scan_take(scan_frame_t *out)
{
    __disable_irq();                                
    *out = buffer[done];
    ready = false;
    __enable_irq();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        scan_tick();
    }
}
