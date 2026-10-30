#include "mapping.h"
#include "sense.h"
#include "ranging.h"

typedef struct
{
    uint8_t row;                    /* row k 0..8 */
    uint8_t tia;                    /* TIA t 0..3 */
} taxel_cell_t;

/* Taxel to row and TIA mapping */
static const taxel_cell_t taxel_cell[TAXEL_COUNT] =
{
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
};

void mapping_frame_to_taxels(const scan_frame_t *f, uint16_t out[TAXEL_COUNT])
{
    uint16_t drive = sense_drive(f->scale);

    for (uint8_t i = 0; i < TAXEL_COUNT; i++)
    {
        uint16_t null = f->null[taxel_cell[i].tia];
        uint16_t raw  = f->raw[taxel_cell[i].row][taxel_cell[i].tia];

        /* pressed pad pulls raw below null; converted to OUTPUT_SENSITIVITY units so every scale gives the same value */
        uint32_t step  = (raw < null) ? (uint32_t)(null - raw) * sense_drive(OUTPUT_SENSITIVITY) / drive : 0;
        uint32_t value = step * 4095U / SENSE_STEP_FULL;          /* largest trusted step = 4095 */

        out[i] = (value > 4095U) ? 4095U : value;
    }
}
