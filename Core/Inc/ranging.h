#ifndef RANGING_H
#define RANGING_H

#include <stdint.h>
#include "app_config.h"
#include "scan.h"

void ranging_next_scales(const scan_frame_t *f, uint8_t next[SCAN_ROWS]);  /* DAC scale per row for the next frame */

#endif /* RANGING_H */
