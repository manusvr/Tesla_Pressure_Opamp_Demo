#include "tare.h"
#include <stdbool.h>
#include "app_config.h"

#define QUIET_FRAMES    ((uint32_t)TARE_QUIET_MS * SCAN_FRAMERATE / 1000U)
#define COOLDOWN_FRAMES ((uint32_t)TARE_COOLDOWN_MS * SCAN_FRAMERATE / 1000U)

/* Re-tare groups: every taxel on its own, or all taxels together */
#define GROUP_COUNT     (TARE_PER_TAXEL ? TAXEL_COUNT : 1U)
#define GROUP_OF(i)     (TARE_PER_TAXEL ? (i) : 0U)

_Static_assert(TARE_PER_TAXEL == 0 || TARE_PER_TAXEL == 1, "TARE_PER_TAXEL must be 0 or 1");

static uint32_t baseline[TAXEL_COUNT];               /* zero point per taxel */
static uint32_t window_ref[TAXEL_COUNT];             /* value of each taxel when its rest window started */
static bool     started = false;
static uint32_t quiet_frames[GROUP_COUNT];           /* frames the group stayed at rest */
static uint32_t frames_since_hard[GROUP_COUNT];      /* frames since the group's last hard press */

void tare_apply(uint32_t value[TAXEL_COUNT])
{
    if (started == false)                           /* start-up: first frame is the zero point */
    {
        for (uint8_t i = 0; i < TAXEL_COUNT; i++)
        {
            baseline[i] = value[i];
        }
        for (uint8_t g = 0; g < GROUP_COUNT; g++)
        {
            quiet_frames[g] = 0;
            frames_since_hard[g] = COOLDOWN_FRAMES;
        }
        started = true;
    }

    bool at_rest[GROUP_COUNT];

    for (uint8_t g = 0; g < GROUP_COUNT; g++)
    {
        at_rest[g] = true;
    }

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        uint8_t g = GROUP_OF(i);

        if (value[i] < baseline[i])                 /* follow down: a press only moves the value up */
        {
            baseline[i] = value[i];
        }

        uint32_t tared = value[i] - baseline[i];

        if (tared > TARE_QUIET_LEVEL)
        {
            at_rest[g] = false;
        }
        if (quiet_frames[g] == 0)                   /* rest window starts: remember where the taxel is */
        {
            window_ref[i] = value[i];
        }
        else if (value[i] > window_ref[i] + TARE_STABLE_BAND ||
                 value[i] + TARE_STABLE_BAND < window_ref[i])   /* still moving: not at rest */
        {
            at_rest[g] = false;
        }
        if (tared > TARE_HARD_LEVEL)
        {
            frames_since_hard[g] = 0;
        }
    }

    bool retare[GROUP_COUNT];

    for (uint8_t g = 0; g < GROUP_COUNT; g++)
    {
        if (at_rest[g] == false)
        {
            quiet_frames[g] = 0;
        }
        else
        {
            quiet_frames[g]++;
        }
        if (frames_since_hard[g] < COOLDOWN_FRAMES)
        {
            frames_since_hard[g]++;
        }

        retare[g] = (quiet_frames[g] >= QUIET_FRAMES && frames_since_hard[g] >= COOLDOWN_FRAMES);
        if (retare[g])                              /* at rest long enough: re-tare this group */
        {
            quiet_frames[g] = 0;
        }
    }

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        if (retare[GROUP_OF(i)])
        {
            baseline[i] = value[i];
        }
        value[i] -= baseline[i];
    }
}
