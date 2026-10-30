#ifndef RANGING_H
#define RANGING_H

#include <stdint.h>
#include "scan.h"

#define RANGING_ENABLE 1           

/* Output sensitivity 0..4: higher = a lighter press reaches 4095; harder presses read 4095.
 * Forces are rough, from the Loomia pad data; check on the glove.
 *
 *   sensitivity | DAC code | 4095 at about | light touch vs 0
 *   ------------+----------+---------------+-----------------
 *        0      |   1911   |   > 60 N      |   1x
 *        1      |   1737   |   ~35 N       |   2.3x
 *        2      |   1365   |   ~15 N       |   5x
 *        3      |    558   |   ~8 N        |   11x
 *        4      |    248   |   ~6 N        |   13x
 */
#define OUTPUT_SENSITIVITY 0

uint8_t ranging_next_scale(const scan_frame_t *f);  

#endif /* RANGING_H */
