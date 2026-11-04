#ifndef TARE_H
#define TARE_H

#include <stdint.h>
#include "mapping.h"

void tare_apply(uint32_t value[TAXEL_COUNT]);  /* subtract the zero point per taxel, in place */

#endif /* TARE_H */
