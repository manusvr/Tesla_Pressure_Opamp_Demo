#include "ranging.h"
#include "sense.h"

#define STEP_CLIP SENSE_STEP_FULL                  /* PGA at its 0.1 V floor (~1924 LSB): TIA may be saturated */
#define STEP_HIGH (1700U * SENSE_COUNTS_PER_LSB)   /* above this: one scale down */
#define STEP_LOW  (1500U * SENSE_COUNTS_PER_LSB)   /* one scale up only if the largest step stays below this */

_Static_assert(OUTPUT_SENSITIVITY < SENSE_DAC_SCALES, "OUTPUT_SENSITIVITY must be 0..4");
_Static_assert(RANGING_PER_ROW == 0 || RANGING_PER_ROW == 1, "RANGING_PER_ROW must be 0 or 1");

static uint16_t row_max_step(const scan_frame_t *f, uint8_t k)   /* largest step of row k over its 4 TIAs */
{
    uint16_t max_step = 0;

    for (uint8_t t = 0; t < SCAN_CHANNELS; t++)
    {
        uint16_t step = (f->raw[k][t] < f->null[t]) ? f->null[t] - f->raw[k][t] : 0;
        if (step > max_step)
        {
            max_step = step;
        }
    }
    return max_step;
}

static uint8_t next_scale(uint8_t s, uint16_t max_step)        /* scale s gave max_step: scale for the next frame */
{
    if (max_step > STEP_CLIP)
    {
        return 0;
    }
    if (max_step > STEP_HIGH && s > 0)
    {
        return s - 1;
    }
    if (s < SENSE_DAC_SCALES - 1 && (uint32_t)max_step * sense_drive(s + 1) / sense_drive(s) < STEP_LOW)
    {
        return s + 1;
    }
    return s;
}

void ranging_next_scales(const scan_frame_t *f, uint8_t next[SCAN_ROWS])
{
    if (RANGING_ENABLE == false)
    {
        for (uint8_t k = 0; k < SCAN_ROWS; k++)
        {
            next[k] = OUTPUT_SENSITIVITY;
        }
        return;
    }

    if (RANGING_PER_ROW)                            /* every row on its own */
    {
        for (uint8_t k = 0; k < SCAN_ROWS; k++)
        {
            next[k] = next_scale(f->scale[k], row_max_step(f, k));
        }
        return;
    }

    uint16_t max_step = 0;                          /* whole frame: the largest step of all rows decides */

    for (uint8_t k = 0; k < SCAN_ROWS; k++)
    {
        uint16_t step = row_max_step(f, k);
        if (step > max_step)
        {
            max_step = step;
        }
    }

    uint8_t s = next_scale(f->scale[0], max_step);

    for (uint8_t k = 0; k < SCAN_ROWS; k++)
    {
        next[k] = s;
    }
}
