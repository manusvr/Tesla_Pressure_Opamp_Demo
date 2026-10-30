#include "sense.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include <stdbool.h>


#define HVREF_CODE 2048U            /* HVREF = VREF+ / 2 */

static OPAMP_HandleTypeDef *const tia_opamp[4] = { &hopamp1, &hopamp2, &hopamp4, &hopamp6 };
static ADC_HandleTypeDef   *const tia_adc[4]   = { &hadc1, &hadc2, &hadc5, &hadc4 };

/* VDRIVE = 3.3 V * code / 4096: 1.540 / 1.399 / 1.100 / 0.450 / 0.200 V */
static const uint16_t dac_code[SENSE_DAC_SCALES] = { 1911, 1737, 1365, 558, 248 };

void sense_set_scale(uint8_t s)
{
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_code[s]);
}

uint16_t sense_drive(uint8_t s)
{
    return HVREF_CODE - dac_code[s];
}

void sense_init(void)
{
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
        out[t] = LL_ADC_REG_ReadConversionData12(tia_adc[t]->Instance);
    }
}
