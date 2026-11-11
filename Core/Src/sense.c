#include "sense.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "mux.h"
#include "app_config.h"
#include <stdbool.h>


#define HVREF_CODE  2048U           /* HVREF = VREF+ / 2 */
#define CAL_SAMPLES 64U             /* readings per step when measuring the drive */

static OPAMP_HandleTypeDef *const tia_opamp[4] = { &hopamp1, &hopamp2, &hopamp4, &hopamp6 };
static ADC_HandleTypeDef   *const tia_adc[4]   = { &hadc1, &hadc2, &hadc5, &hadc4 };

/* VDRIVE = 3.3 V * code / 4096: 1.540 / 1.399 / 1.100 / 0.450 / 0.203 V
   252 not 248: DAC buffer output >= 0.2 V (DS12712 Table 69), margin for VDDA down to 3.25 V */
static const uint16_t dac_code[SENSE_DAC_SCALES] = { 1911, 1737, 1365, 558, 252 };

static uint16_t drive[SENSE_DAC_SCALES];   /* relative drive per scale, only ratios are used */

void sense_set_scale(uint8_t s)
{
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_code[s]);
}

uint16_t sense_drive(uint8_t s)
{
    return drive[s];
}

static uint32_t read_sum(void)              /* sum of CAL_SAMPLES readings on all 4 TIAs */
{
    uint16_t v[4];
    uint32_t sum = 0;

    HAL_Delay(2);                           /* TIA settle after a mux or DAC change */
    for (uint8_t n = 0; n < CAL_SAMPLES; n++)
    {
        sense_read4(v);
        sum += v[0] + v[1] + v[2] + v[3];
    }
    return sum;
}

/* Real drive per scale from the CAL step (null - CAL): includes DAC and HVREF errors.
   Keeps the nominal drive if any scale gives no step or a clipped one. */
static void measure_drive(void)
{
    uint16_t meas[SENSE_DAC_SCALES];
    bool ok = true;

    for (uint8_t s = 0; s < SENSE_DAC_SCALES && ok; s++)
    {
        sense_set_scale(s);
        select_null_ref();
        uint32_t null_sum = read_sum();
        select_cal_a();
        uint32_t cal_sum = read_sum();

        /* 8 x average CAL step over 4 TIAs x 64 readings: keeps fractions, fits uint16 */
        uint32_t step = (cal_sum < null_sum) ? (null_sum - cal_sum) / (CAL_SAMPLES * 4U / 8U) : 0;
        if (step == 0 || step > 8U * SENSE_STEP_FULL)
        {
            ok = false;
        }
        meas[s] = (uint16_t)step;
    }

    mux_off();
    sense_set_scale(0);

    if (ok)
    {
        for (uint8_t s = 0; s < SENSE_DAC_SCALES; s++)
        {
            drive[s] = meas[s];
        }
    }
}

void sense_init(void)
{
    for (uint8_t s = 0; s < SENSE_DAC_SCALES; s++)
    {
        drive[s] = HVREF_CODE - dac_code[s];    /* nominal drive in DAC codes */
    }

    sense_set_scale(0);             /* DAC at 1.540 V */
    if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    //HAL_Delay(1);

    for (uint8_t t = 0; t < 4; t++)
    {
        /* calibrate while the ADC is still disabled, then one throw-away
           conversion: leaves the ADC enabled and EOC clear for sense_read4() */
        if (HAL_OPAMP_Start(tia_opamp[t]) != HAL_OK ||
            HAL_ADCEx_Calibration_Start(tia_adc[t], ADC_SINGLE_ENDED) != HAL_OK ||
            HAL_ADC_Start(tia_adc[t]) != HAL_OK ||
            HAL_ADC_PollForConversion(tia_adc[t], 10) != HAL_OK)
        {
            Error_Handler();
        }
        (void)HAL_ADC_GetValue(tia_adc[t]);
    }

    if (CAL_SCALE_ENABLE)           /* ~0.1 s at start-up, before any row is driven */
    {
        measure_drive();
    }
}

void sense_read4(uint16_t out[4])
{
    for (uint8_t t = 0; t < 4; t++)          /* start all four: they convert in parallel */
    {
        LL_ADC_REG_StartConversion(tia_adc[t]->Instance);
    }

    for (uint8_t t = 0; t < 4; t++)
    {
        while (LL_ADC_IsActiveFlag_EOC(tia_adc[t]->Instance)==false)
        {
        }
        out[t] = (uint16_t)LL_ADC_REG_ReadConversionData32(tia_adc[t]->Instance);  
    }
}
