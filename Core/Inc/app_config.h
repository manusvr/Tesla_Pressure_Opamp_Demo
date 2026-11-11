#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Settings to tune. Hardware facts stay in their own module. */

#define SCAN_FRAMERATE      120     /* Hz */

#define RANGING_ENABLE    1       /* 1: DAC ranging, 0: fixed DAC at OUTPUT_SENSITIVITY */

/* Output sensitivity 0..4: higher = a lighter press reaches 4095.
 * Works in both modes; with ranging off it also sets the DAC.
 *
 *   sensitivity | DAC code | DAC voltage
 *   ------------+----------+------------
 *        0      |   1911   |   1.540 V
 *        1      |   1737   |   1.399 V
 *        2      |   1365   |   1.100 V
 *        3      |    558   |   0.450 V
 *        4      |    252   |   0.203 Vcant we do it fast
 */
#define OUTPUT_SENSITIVITY  1

/* Tare: zero point per taxel, set at start-up and when the hand is at rest.
 * At rest = below TARE_QUIET_LEVEL and not moving more than TARE_STABLE_BAND, for TARE_QUIET_MS. */
#define TARE_ENABLE         1      /* 1: on, 0: raw values */
#define TARE_PER_TAXEL      1       /* 1: each taxel re-tares on its own, 0: only when all taxels are at rest */
#define TARE_QUIET_LEVEL    22     /* output counts: below this a taxel counts as untouched */
#define TARE_STABLE_BAND    1      /* output counts: a taxel that moves more than this is not at rest (rest noise ~ +-3, a finger hold moves more) */
#define TARE_QUIET_MS       1000    /* at rest this long: re-tare */
#define TARE_HARD_LEVEL     1000     /* output counts: a press this hard starts the cooldown */
#define TARE_COOLDOWN_MS    2000    /* no re-tare this long after a hard press */
 
/* Deadband: values at or below this are sent as 0, hides noise at rest.
 * Keep it below TARE_QUIET_LEVEL; set it from the noise test (A8). */
#define DEADBAND_ENABLE     1      /* 1: on, 0: off */
#define DEADBAND_LEVEL      5       /* output counts */

/* CAL scale factors: at start-up measure the real drive of every DAC scale
 * with the CAL resistor, so a scale switch gives no jump in the output. */
#define CAL_SCALE_ENABLE    1       /* 1: measured drive, 0: nominal from the DAC codes */

/* Glove wiring case (hardware plan section 5): how the Loomia breakout is plugged into P2.
 * The taxel order of the output is the same in every case (taxel 0 = A1 ... 28 = H8).
 *   1: ROW_0..8 = A..I, column pins 0..8 = 1..9   ("1 A" end at P2 pins 1/2)
 *   2: ROW_0..8 = I..A, column pins 0..8 = 9..1
 *   3: ROW_0..8 = 1..9, column pins 0..8 = A..I
 *   4: ROW_0..8 = 9..1, column pins 0..8 = I..A
 * Find it once: press C3, then G7; only the case where both light up their own taxel is right. */
#define GLOVE_WIRING_CASE   2

#endif /* APP_CONFIG_H */
