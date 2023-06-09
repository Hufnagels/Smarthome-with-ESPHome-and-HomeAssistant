/**************************************************************************/
/*!
    @file     I2CFram.h
    @author   AGE

    @section  HISTORY

    v0.1 - fIRST TEST RUN

    iNTERFACE for the MB85RC I2C FRAM from Fujitsu.

    @section LICENSE

    Software License Agreement (BSD License)

*/
/**************************************************************************/

#include "esphome.h"
#include <FRAM.h>

static const char *const TAG = "I2C-FRAM";

int DeviceID, rv;
FRAM fram;

class MyFram : public Component
{
public:
  MyFram(int device_id)
  {
    DeviceID = device_id;
  }

  void setup() override
  {
    rv = fram.begin(DeviceID);
  }
  void loop() override
  {
    // ESP_LOGI("custom", "Running ...");
  }

  void dump_config()
  {
    ESP_LOGI(TAG, "Starting I2C FRAM on device is %x ", DeviceID);
    if (rv != 0)
    {
      ESP_LOGD(TAG, "No memory module found %d ", rv);
    }
    else
    {
      ESP_LOGI(TAG, "Memory module found");
      uint16_t Size = fram.getSize();
      ESP_LOGI(TAG, "Memory size %d kbyte", Size);
    }
  }
};

float readfromfram(uint32_t memaddr)
{
  float value;
  memaddr = memaddr * 4;
  value = fram.readFloat(memaddr);
  ESP_LOGI(TAG, "read value %f from memory addres %d", value, memaddr);
  return (value);
}

void writetofram(uint32_t memaddr, float value)
{
  memaddr = memaddr * 4;
  fram.writeFloat(memaddr, value);
  ESP_LOGI(TAG, "write value %f to memory addres %d ", value, memaddr);
}

void readcounters()
{
  // counter on meter
  if (id(initial_gas_value) > 0)
  {
    id(initial_gas_value) = readfromfram(0x01);
  }
  // count of last readed impulse
  id(gas_impulses) = readfromfram(0x02);

  id(counter_3) = readfromfram(0x03);
  id(counter_4) = readfromfram(0x03);
}