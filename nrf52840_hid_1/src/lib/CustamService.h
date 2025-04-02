#ifndef BLECUSTAM_H_
#define BLECUSTAM_H_

#include "bluefruit_common.h"

#include "BLECharacteristic.h"
#include "BLEService.h"

class BLECustam : public BLEService
{
  protected:
  BLECharacteristic _characteristic_input;
  BLECharacteristic _characteristic_output;

  public:
    BLECustam(void);

    virtual err_t begin(void);

    bool write (const char* str);

    bool notify(uint8_t level);
    bool notify(uint16_t conn_hdl, uint8_t level);
};



#endif /* BLECUSTAM_H_ */
