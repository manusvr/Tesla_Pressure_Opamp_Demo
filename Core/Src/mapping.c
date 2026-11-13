#include "mapping.h"
#include "sense.h"
#include "ranging.h"
#include "tare.h"

_Static_assert(DEADBAND_LEVEL < TARE_QUIET_LEVEL, "DEADBAND_LEVEL must be below TARE_QUIET_LEVEL");

typedef struct
{
    uint8_t row;                    /* row k 0..8 */
    uint8_t tia;                    /* TIA t 0..3 */
} taxel_cell_t;

_Static_assert(GLOVE_WIRING_CASE >= 1 && GLOVE_WIRING_CASE <= 4, "GLOVE_WIRING_CASE must be 1, 2, 3 or 4");

/* Taxel to row and TIA mapping, per glove wiring case. The taxel order (0 = A1 ... 28 = H8)
   is the same in every case; only the cell each taxel is read from changes. */
static const taxel_cell_t taxel_cell[TAXEL_COUNT] =
{
#if GLOVE_WIRING_CASE == 1   /* ROW_0..8 = A..I, column pins 0..8 = 1..9 */
    [0]  = { 0, 0 },                /* A1 */
    [1]  = { 1, 0 },                /* B1 */
    [2]  = { 0, 1 },                /* A2 */
    [3]  = { 1, 1 },                /* B2 */
    [4]  = { 0, 2 },                /* A3 */
    [5]  = { 2, 1 },                /* C2 */
    [6]  = { 2, 0 },                /* C5 */
    [7]  = { 3, 0 },                /* D5 */
    [8]  = { 2, 2 },                /* C3 */
    [9]  = { 3, 2 },                /* D3 */
    [10] = { 3, 3 },                /* D4 */
    [11] = { 2, 3 },                /* C4 */
    [12] = { 1, 3 },                /* B4 */
    [13] = { 1, 2 },                /* B3 */
    [14] = { 4, 1 },                /* E6 */
    [15] = { 4, 0 },                /* E5 */
    [16] = { 5, 1 },                /* F6 */
    [17] = { 5, 0 },                /* F5 */
    [18] = { 4, 3 },                /* E4 */
    [19] = { 7, 2 },                /* H7 */
    [20] = { 7, 1 },                /* H6 */
    [21] = { 6, 2 },                /* G7 */
    [22] = { 6, 1 },                /* G6 */
    [23] = { 6, 3 },                /* G8 */
    [24] = { 8, 2 },                /* I7 */
    [25] = { 8, 0 },                /* I9 */
    [26] = { 7, 0 },                /* H9 */
    [27] = { 8, 3 },                /* I8 */
    [28] = { 7, 3 },                /* H8 */
#elif GLOVE_WIRING_CASE == 2   /* ROW_0..8 = I..A, column pins 0..8 = 9..1 */
    [0]  = { 8, 0 },                /* A1 */
    [1]  = { 7, 0 },                /* B1 */
    [2]  = { 8, 3 },                /* A2 */
    [3]  = { 7, 3 },                /* B2 */
    [4]  = { 8, 2 },                /* A3 */
    [5]  = { 6, 3 },                /* C2 */
    [6]  = { 6, 0 },                /* C5 */
    [7]  = { 5, 0 },                /* D5 */
    [8]  = { 6, 2 },                /* C3 */
    [9]  = { 5, 2 },                /* D3 */
    [10] = { 5, 1 },                /* D4 */
    [11] = { 6, 1 },                /* C4 */
    [12] = { 7, 1 },                /* B4 */
    [13] = { 7, 2 },                /* B3 */
    [14] = { 4, 3 },                /* E6 */
    [15] = { 4, 0 },                /* E5 */
    [16] = { 3, 3 },                /* F6 */
    [17] = { 3, 0 },                /* F5 */
    [18] = { 4, 1 },                /* E4 */
    [19] = { 1, 2 },                /* H7 */
    [20] = { 1, 3 },                /* H6 */
    [21] = { 2, 2 },                /* G7 */
    [22] = { 2, 3 },                /* G6 */
    [23] = { 2, 1 },                /* G8 */
    [24] = { 0, 2 },                /* I7 */
    [25] = { 0, 0 },                /* I9 */
    [26] = { 1, 0 },                /* H9 */
    [27] = { 0, 1 },                /* I8 */
    [28] = { 1, 1 },                /* H8 */
#elif GLOVE_WIRING_CASE == 3   /* ROW_0..8 = 1..9, column pins 0..8 = A..I */
    [0]  = { 0, 0 },                /* A1 */
    [1]  = { 0, 1 },                /* B1 */
    [2]  = { 1, 0 },                /* A2 */
    [3]  = { 1, 1 },                /* B2 */
    [4]  = { 2, 0 },                /* A3 */
    [5]  = { 1, 2 },                /* C2 */
    [6]  = { 4, 2 },                /* C5 */
    [7]  = { 4, 3 },                /* D5 */
    [8]  = { 2, 2 },                /* C3 */
    [9]  = { 2, 3 },                /* D3 */
    [10] = { 3, 3 },                /* D4 */
    [11] = { 3, 2 },                /* C4 */
    [12] = { 3, 1 },                /* B4 */
    [13] = { 2, 1 },                /* B3 */
    [14] = { 5, 0 },                /* E6 */
    [15] = { 4, 0 },                /* E5 */
    [16] = { 5, 1 },                /* F6 */
    [17] = { 4, 1 },                /* F5 */
    [18] = { 3, 0 },                /* E4 */
    [19] = { 6, 3 },                /* H7 */
    [20] = { 5, 3 },                /* H6 */
    [21] = { 6, 2 },                /* G7 */
    [22] = { 5, 2 },                /* G6 */
    [23] = { 7, 2 },                /* G8 */
    [24] = { 6, 0 },                /* I7 */
    [25] = { 8, 0 },                /* I9 */
    [26] = { 8, 3 },                /* H9 */
    [27] = { 7, 0 },                /* I8 */
    [28] = { 7, 3 },                /* H8 */
#elif GLOVE_WIRING_CASE == 4   /* ROW_0..8 = 9..1, column pins 0..8 = I..A */
    [0]  = { 8, 0 },                /* A1 */
    [1]  = { 8, 3 },                /* B1 */
    [2]  = { 7, 0 },                /* A2 */
    [3]  = { 7, 3 },                /* B2 */
    [4]  = { 6, 0 },                /* A3 */
    [5]  = { 7, 2 },                /* C2 */
    [6]  = { 4, 2 },                /* C5 */
    [7]  = { 4, 1 },                /* D5 */
    [8]  = { 6, 2 },                /* C3 */
    [9]  = { 6, 1 },                /* D3 */
    [10] = { 5, 1 },                /* D4 */
    [11] = { 5, 2 },                /* C4 */
    [12] = { 5, 3 },                /* B4 */
    [13] = { 6, 3 },                /* B3 */
    [14] = { 3, 0 },                /* E6 */
    [15] = { 4, 0 },                /* E5 */
    [16] = { 3, 3 },                /* F6 */
    [17] = { 4, 3 },                /* F5 */
    [18] = { 5, 0 },                /* E4 */
    [19] = { 2, 1 },                /* H7 */
    [20] = { 3, 1 },                /* H6 */
    [21] = { 2, 2 },                /* G7 */
    [22] = { 3, 2 },                /* G6 */
    [23] = { 1, 2 },                /* G8 */
    [24] = { 2, 0 },                /* I7 */
    [25] = { 0, 0 },                /* I9 */
    [26] = { 0, 1 },                /* H9 */
    [27] = { 1, 0 },                /* I8 */
    [28] = { 1, 1 },                /* H8 */
#endif
};

void mapping_frame_to_taxels(const scan_frame_t *f, uint16_t out[TAXEL_COUNT])
{
    uint32_t value[TAXEL_COUNT];

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        uint16_t drive = sense_drive(f->scale[taxel_cell[i].row]);   /* DAC scale of this taxel's row */
        uint16_t null = f->null[taxel_cell[i].tia];
        uint16_t raw  = f->raw[taxel_cell[i].row][taxel_cell[i].tia];

        /* pressed pad pulls raw below null; converted to OUTPUT_SENSITIVITY units so every scale gives the same value */
        uint32_t step = (raw < null) ? (uint32_t)(null - raw) * sense_drive(OUTPUT_SENSITIVITY) / drive : 0;
        value[i] = step * 4095U / SENSE_STEP_FULL;                /* largest trusted step = 4095 */
    }

    if (TARE_ENABLE)                                              /* before the cap, so a tared pad still reaches 4095 */
    {
        tare_apply(value);
    }

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        if (DEADBAND_ENABLE && value[i] <= DEADBAND_LEVEL)        /* noise at rest: send 0 */
        {
            value[i] = 0;
        }
        out[i] = (value[i] > 4095U) ? 4095U : value[i];
    }
}
