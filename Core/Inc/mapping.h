#ifndef MAPPING_H
#define MAPPING_H

#include <stdint.h>
#include "scan.h"

#define TAXEL_COUNT 29


void mapping_frame_to_taxels(const scan_frame_t *f, uint16_t out[TAXEL_COUNT]);

#endif /* MAPPING_H */
