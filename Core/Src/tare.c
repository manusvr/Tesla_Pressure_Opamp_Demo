#include "tare.h"
#include <stdbool.h>
#include "app_config.h"

#define QUIET_FRAMES    ((uint32_t)TARE_QUIET_MS * SCAN_FRAMERATE / 1000U)
#define COOLDOWN_FRAMES ((uint32_t)TARE_COOLDOWN_MS * SCAN_FRAMERATE / 1000U)

static uint32_t baseline[TAXEL_COUNT];               /* zero point per taxel */
static bool     started = false;
static uint32_t quiet_frames = 0;                    /* frames all taxels stayed untouched */
static uint32_t frames_since_hard = COOLDOWN_FRAMES; /* frames since the last hard press */

void tare_apply(uint32_t value[TAXEL_COUNT])
{
    if (started == false)                           /* start-up: first frame is the zero point */
    {
        for (uint8_t i = 0; i < TAXEL_COUNT; i++)
        {
            baseline[i] = value[i];
        }
        started = true;
    }

    bool quiet = true;

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        if (value[i] < baseline[i])                 /* follow down: a press only moves the value up */
        {
            baseline[i] = value[i];
        }

        uint32_t tared = value[i] - baseline[i];

        if (tared > TARE_QUIET_LEVEL)
        {
            quiet = false;
        }
        if (tared > TARE_HARD_LEVEL)
        {
            frames_since_hard = 0;
        }
    }

    if (quiet == false)
    {
        quiet_frames = 0;
    }
    else
    {
        quiet_frames++;
    }
    if (frames_since_hard < COOLDOWN_FRAMES)
    {
        frames_since_hard++;
    }

    if (quiet_frames >= QUIET_FRAMES && frames_since_hard >= COOLDOWN_FRAMES)   /* hand quiet: re-tare */
    {
        for (uint8_t i = 0; i < TAXEL_COUNT; i++)
        {
            baseline[i] = value[i];
        }
        quiet_frames = 0;
    }

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        value[i] -= baseline[i];
    }
}
