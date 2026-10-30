#include "ranging.h"
#include "sense.h"

#define STEP_CLIP 1850U             /* PGA at its 0.1 V floor (~1924): TIA may be saturated */
#define STEP_HIGH 1700U             /* above this: one scale down */
#define STEP_LOW  1500U             /* one scale up only if the largest step stays below this */

_Static_assert(OUTPUT_SENSITIVITY < SENSE_DAC_SCALES, "OUTPUT_SENSITIVITY must be 0..4");

uint8_t ranging_next_scale(const scan_frame_t *f)
{
    if (RANGING_ENABLE == false)
    {
        return OUTPUT_SENSITIVITY;
    }

    uint16_t max_step = 0;

    for (uint8_t k = 0; k < SCAN_ROWS; k++)
    {
        for (uint8_t t = 0; t < SCAN_CHANNELS; t++)
        {
            uint16_t step = (f->raw[k][t] < f->null[t]) ? f->null[t] - f->raw[k][t] : 0;
            if (step > max_step)
            {
                max_step = step;
            }
        }
    }

    uint8_t s = f->scale;

    if (max_step > STEP_CLIP)                       /* clipped: back off fully, don't disturb the other readings */
    {
        return 0;
    }
    if (max_step > STEP_HIGH && s > 0)
    {
        return s - 1;
    }
    if (s < SENSE_DAC_SCALES - 1 &&
        (uint32_t)max_step * sense_drive(s + 1) / sense_drive(s) < STEP_LOW)
    {
        return s + 1;
    }
    return s;
}
