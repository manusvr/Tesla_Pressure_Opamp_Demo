#ifndef RANGING_H
#define RANGING_H

#include <stdint.h>
#include "app_config.h"
#include "scan.h"

uint8_t ranging_next_scale(const scan_frame_t *f);  

#endif /* RANGING_H */
