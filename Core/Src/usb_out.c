#include "usb_out.h"

#include <stdio.h>

#include "usbd_cdc_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

static char cdc_tx_buf[sizeof(char) * (6 * TAXEL_COUNT + 2) + sizeof(char)] = {0};

void output_sensor_data(const uint16_t sensor_values[TAXEL_COUNT])
{
  /* drop this frame while no host is connected or the previous line is still being sent:
     CDC_Transmit_FS does not copy cdc_tx_buf */
  if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED ||
      ((USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData)->TxState != 0U)
  {
    return;
  }

  size_t idx_char = 0;
  int ret;
  for (size_t i = 0; i < TAXEL_COUNT; i++)
  {
    ret = snprintf(&cdc_tx_buf[idx_char], sizeof(cdc_tx_buf) - idx_char, "%u ", sensor_values[i]);
    if (ret < 0 || (size_t)ret >= sizeof(cdc_tx_buf) - idx_char)
    {
      return;
    }
    idx_char += ret;
  }
  ret = snprintf(&cdc_tx_buf[idx_char], sizeof(cdc_tx_buf) - idx_char, "\r\n");
  if (ret < 0 || (size_t)ret >= sizeof(cdc_tx_buf) - idx_char)
  {
    return;
  }
  idx_char += ret;

  CDC_Transmit_FS((uint8_t *)cdc_tx_buf, (uint16_t)idx_char);
}
