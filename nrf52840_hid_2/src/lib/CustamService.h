#ifndef BLECUSTAM_H_
#define BLECUSTAM_H_

#include "bluefruit_common.h"

#include "BLECharacteristic.h"
#include "BLEService.h"


#include "ble_callbacks.h"

extern BLECharacteristic *_characteristic_input;
extern BLECharacteristic *_characteristic_output;

class BLECustam : public BLEService
{
  public:
    BLECustam(void);

    virtual err_t begin(void);

    bool write (const char* str);

    bool notify(uint8_t level);
    bool notify(uint16_t conn_hdl, uint8_t level);
    static void onCommandWritten(uint16_t conn_hdl, BLECharacteristic* characteristic, uint8_t* data, uint16_t data_length);
};



#endif /* BLECUSTAM_H_ */
