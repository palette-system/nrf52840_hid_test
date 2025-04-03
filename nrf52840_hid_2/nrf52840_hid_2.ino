/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
#include "src/lib/az_common.h"
#include "src/lib/CustamService.h"

// 共通クラス
AzCommon common_cls = AzCommon();

BLEDis bledis;
BLEHidAdafruit blehid;
BLECustam blecus;


bool hasKeyPressed = false;
int send_index = 0;

void setup() 
{
  /*
  Serial.begin(9600);
  while(!Serial){
      delay(100);
  }
      */

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("XIAO_HID");

  // Configure and Start Device Information Service
  bledis.setManufacturer("xiao_nrf");
  bledis.setModel("nrfA");
  uint8_t sig = 0x02;
  uint16_t hid_vid = 0xe502; 
  uint16_t hid_pid = 0x0200; 
  uint16_t version = 0x0001; 
  uint8_t pnp[] = {
    sig,
    (uint8_t) (hid_vid >> 8), (uint8_t) hid_vid,
    (uint8_t) (hid_pid >> 8), (uint8_t) hid_pid,
    (uint8_t) (version >> 8), (uint8_t) version
  };
  bledis.setPNPID((const char*)pnp, sizeof(pnp));
  // Bluefruit.configAttrTableSize(64);
  // Bluefruit.configPrphBandwidth(BANDWIDTH_NORMAL);
  // Bluefruit.Security.setIOCaps(false, false, false);
  // Bluefruit.Security.setMITM(true);
  bledis.begin();

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  blehid.begin();

  // Set callback for set LED from central
  blehid.setKeyboardLedCallback(set_keyboard_led);

  blecus.begin();

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.Periph.setConnInterval(9, 12); */

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);
  // Bluefruit.Advertising.addService(blecus);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() 
{
  send_index++;
  if (send_index > 15) send_index = 0;
    blehid.keyPress(0x41);
    delay(50);
    blehid.keyRelease();
    delay(100000);
    // blecus.notify(send_index);
    // blehid.sendTest();
    delay(1000);
}

/**
 * Callback invoked when received Set LED from central.
 * Must be set previously with setKeyboardLedCallback()
 *
 * The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
 *    Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
 */
void set_keyboard_led(uint16_t conn_handle, uint8_t led_bitmap)
{
  (void) conn_handle;
  
  // light up Red Led if any bits is set
  if ( led_bitmap & 0x02 )
  {
    ledOn( LED_RED );
  }
  else
  {
    ledOff( LED_RED );
  }
}
