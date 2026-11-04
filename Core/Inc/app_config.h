#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Settings to tune. Hardware facts stay in their own module. */

#define SCAN_FRAMERATE      120     /* Hz */

#define RANGING_ENABLE      1       /* 1: DAC ranging, 0: fixed DAC at OUTPUT_SENSITIVITY */

/* Output sensitivity 0..4: higher = a lighter press reaches 4095.
 * Works in both modes; with ranging off it also sets the DAC.
 *
 *   sensitivity | DAC code | DAC voltage
 *   ------------+----------+------------
 *        0      |   1911   |   1.540 V
 *        1      |   1737   |   1.399 V
 *        2      |   1365   |   1.100 V
 *        3      |    558   |   0.450 V
 *        4      |    252   |   0.203 V
 */
#define OUTPUT_SENSITIVITY  0

/* Tare: zero point per taxel, set at start-up and when the hand is quiet. */
#define TARE_ENABLE         1       /* 1: on, 0: raw values */
#define TARE_QUIET_LEVEL    40      /* output counts: below this a taxel counts as untouched */
#define TARE_QUIET_MS       10000   /* all taxels untouched this long: re-tare */
#define TARE_HARD_LEVEL     400     /* output counts: a press this hard starts the cooldown */
#define TARE_COOLDOWN_MS    45000   /* no re-tare this long after a hard press */

#endif /* APP_CONFIG_H */
