
#include "bluefruit.h"
#include "CustamService.h"

int write_index;

// ブラウザからデータを受け取った
void onCommandWritten(uint16_t conn_hdl, BLECharacteristic* characteristic, uint8_t* data, uint16_t len) {
    write_index++;
    if (write_index > 3) write_index = 0;
}

  
BLECustam::BLECustam(void) :
  BLEService(0xff01), _characteristic_input(0xff02), _characteristic_output(0xff03)
{

}

err_t BLECustam::begin(void)
{
    write_index = 0;
  // Invoke base class begin()
  VERIFY_STATUS( BLEService::begin() );

  // ブラウザからデータ受け取る用のcharacteristic
  _characteristic_output.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP | CHR_PROPS_NOTIFY);
  _characteristic_output.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  // Input report len is configured, else variable len up to 255
  _characteristic_output.setMaxLen(32);
  // _characteristic_output.setFixedLen(8 );
  // _characteristic_output.setWriteCallback(BLEUart::bleuart_rxd_cb, true);
  _characteristic_output.setWriteCallback(onCommandWritten); // データを受け取った時のイベント登録
  VERIFY_STATUS( _characteristic_output.begin() );


  // XIAOからブラウザにデータを送る用のcharacteristic
  _characteristic_input.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP | CHR_PROPS_NOTIFY);
  _characteristic_input.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  // _characteristic_input.setFixedLen(8);
  _characteristic_input.setMaxLen(32);
  VERIFY_STATUS( _characteristic_input.begin() );

  return ERROR_NONE;
}

bool BLECustam::write(const char* str)
{
  return _characteristic_input.write(str) > 0;
}

bool BLECustam::notify(uint8_t level)
{
    int i;
    uint8_t data[4];
    for (i=0; i<4; i++) {
        data[i] = (write_index << 6) + (i << 4) + level;
    }
  
    return _characteristic_input.notify(data, 4);
}

bool BLECustam::notify(uint16_t conn_hdl, uint8_t level)
{
  return _characteristic_input.notify8(conn_hdl, level);
}
