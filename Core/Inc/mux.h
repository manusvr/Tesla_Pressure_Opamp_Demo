#ifndef MUX_H
#define MUX_H

#include <stdint.h>

void mux_off(void);
void select_row(uint8_t k);         /* 0..8 */
void select_null_ref(void);
void select_cal_a(void);

#endif /* MUX_H */
