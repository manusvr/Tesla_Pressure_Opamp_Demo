#include "mux.h"
#include "main.h"

/* Define gpio macro for setting pin states */
#define gpio(name, v) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, (v) ? GPIO_PIN_SET : GPIO_PIN_RESET)

/* B1 address, A1 A0 = DRIVE_A4, DRIVE_A3 */
enum { B1_ROW8 = 0b00, B1_NULL_REF = 0b01, B1_CAL_A = 0b10 };

void mux_off(void)                 /* All Muxes off*/
{
    gpio(DRIVE_EN, 0); gpio(DRIVE_EN2, 0);
}

static void select_b1(uint8_t a)
{
    mux_off();                      /* Every selection starts with all muxes off */
    gpio(DRIVE_A3, a & 1); 
    gpio(DRIVE_A4, (a >> 1) & 1);
    gpio(DRIVE_EN2, 1);
}

void select_row(uint8_t k)          /* k = 0..8: rows 0..7 via ho1/ho2, row 8 via B1 */
{
    if (k == 8)
    {
        select_b1(B1_ROW8);
        return;
    }
    mux_off();
    if (k > 8)
    {
        return;                     
    }
    gpio(DRIVE_A0, k & 1); 
    gpio(DRIVE_A1, (k >> 1) & 1); 
    gpio(DRIVE_A2, (k >> 2) & 1);
    gpio(DRIVE_EN, 1);
}

void select_null_ref(void)
{
    select_b1(B1_NULL_REF);
}

void select_cal_a(void)
{
    select_b1(B1_CAL_A);
}
