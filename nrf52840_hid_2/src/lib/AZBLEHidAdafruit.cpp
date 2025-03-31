/**************************************************************************/
/*!
    @file     BLEHidAdafruit.cpp
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "bluefruit.h"

#include "AZBLEHidAdafruit.h"

// For using USB HID descriptor template
#include "class/hid/hid_device.h"

enum
{
    REPORT_ID_KEYBOARD = 1,
//  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_AZTOOL
//  REPORT_ID_MOUSE,
};

enum {
    REPORT_TYPE_INPUT = 1,
    REPORT_TYPE_OUTPUT,
    REPORT_TYPE_FEATURE
};

#define REPORT_KEYBOARD_ID 0x01
#define REPORT_MEDIA_KEYS_ID 0x02
#define REPORT_MOUSE_ID 0x03
#define REPORT_AZTOOL_ID 0x02

#define INPUT_REPORT_RAW_MAX_LEN 32
#define OUTPUT_REPORT_RAW_MAX_LEN 32

/* */
#define AZ_HID_VERSION_1_11    (0x0111)

/* HID Class */
#define AZ_HID_CLASS           (3)
#define AZ_HID_SUBCLASS_NONE   (0)
#define AZ_HID_PROTOCOL_NONE   (0)

/* Descriptors */
#define AZ_HID_DESCRIPTOR          (33)
#define AZ_HID_DESCRIPTOR_LENGTH   (0x09)
#define AZ_REPORT_DESCRIPTOR       (34)

/* Class requests */
#define AZ_GET_REPORT (0x1)
#define AZ_GET_IDLE   (0x2)
#define AZ_SET_REPORT (0x9)
#define AZ_SET_IDLE   (0xa)

/* Main items */
#define AZ_HIDINPUT(size)             (0x80 | size)
#define AZ_HIDOUTPUT(size)            (0x90 | size)
#define AZ_FEATURE(size)           (0xb0 | size)
#define AZ_COLLECTION(size)        (0xa0 | size)
#define AZ_END_COLLECTION(size)    (0xc0 | size)

/* Global items */
#define AZ_USAGE_PAGE(size)        (0x04 | size)
#define AZ_LOGICAL_MINIMUM(size)   (0x14 | size)
#define AZ_LOGICAL_MAXIMUM(size)   (0x24 | size)
#define AZ_PHYSICAL_MINIMUM(size)  (0x34 | size)
#define AZ_PHYSICAL_MAXIMUM(size)  (0x44 | size)
#define AZ_UNIT_EXPONENT(size)     (0x54 | size)
#define AZ_UNIT(size)              (0x64 | size)
#define AZ_REPORT_SIZE(size)       (0x74 | size)  //bits
#define AZ_REPORT_ID(size)         (0x84 | size)
#define AZ_REPORT_COUNT(size)      (0x94 | size)  //bytes
#define AZ_PUSH(size)              (0xa4 | size)
#define AZ_POP(size)               (0xb4 | size)

/* Local items */
#define AZ_USAGE(size)                 (0x08 | size)
#define AZ_USAGE_MINIMUM(size)         (0x18 | size)
#define AZ_USAGE_MAXIMUM(size)         (0x28 | size)
#define AZ_DESIGNATOR_INDEX(size)      (0x38 | size)
#define AZ_DESIGNATOR_MINIMUM(size)    (0x48 | size)
#define AZ_DESIGNATOR_MAXIMUM(size)    (0x58 | size)
#define AZ_STRING_INDEX(size)          (0x78 | size)
#define AZ_STRING_MINIMUM(size)        (0x88 | size)
#define AZ_STRING_MAXIMUM(size)        (0x98 | size)
#define AZ_DELIMITER(size)             (0xa8 | size)

uint8_t const hid_report_descriptor[] =
{

    AZ_USAGE_PAGE(1),      0x01,          // USAGE_PAGE (Generic Desktop Ctrls)
    AZ_USAGE(1),           0x06,          // USAGE (Keyboard)
    AZ_COLLECTION(1),      0x01,          // COLLECTION (Application)
    // ------------------------------------------------- Keyboard
    AZ_REPORT_ID(1),       REPORT_KEYBOARD_ID,   //   REPORT_ID (1)
    AZ_USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  
      // モデファイヤキー(修飾キー)
      AZ_USAGE_MINIMUM(1),   0xE0,          //   USAGE_MINIMUM (0xE0)(左CTRLが0xe0)
    AZ_USAGE_MAXIMUM(1),   0xE7,          //   USAGE_MAXIMUM (0xE7)(右GUIが0xe7)
    AZ_LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
    AZ_LOGICAL_MAXIMUM(1), 0x01,          //   Logical Maximum (1)
    AZ_REPORT_COUNT(1),    0x08,          //   REPORT_COUNT (8)全部で8つ(左右4つずつ)。
      AZ_REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
    AZ_HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  
      // 予約フィールド
      AZ_REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 1 byte (Reserved)
    AZ_REPORT_SIZE(1),     0x08,          //   REPORT_SIZE (8)1ビットが8つ。
    AZ_HIDINPUT(1),        0x01,          //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  
      // LED状態のアウトプット
      AZ_REPORT_COUNT(1),    0x05,          //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)全部で5つ。
    AZ_REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)LEDにつき1ビット
    AZ_USAGE_PAGE(1),      0x08,          //   USAGE_PAGE (LEDs)
    AZ_USAGE_MINIMUM(1),   0x01,          //   USAGE_MINIMUM (0x01) ; Num Lock(NumLock LEDが1)
    AZ_USAGE_MAXIMUM(1),   0x05,          //   USAGE_MAXIMUM (0x05) ; Kana(KANA LEDが5)
    AZ_HIDOUTPUT(1),       0x02,          //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)LED report
  
      // LEDレポートのパディング
      AZ_REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 3 bits (Padding)
    AZ_REPORT_SIZE(1),     0x03,          //   REPORT_SIZE (3)残りの3ビットを埋める。
    AZ_HIDOUTPUT(1),       0x03,          //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  
      // 入力キーのインプット
      AZ_REPORT_COUNT(1),    0x06,          //   REPORT_COUNT (6) ; 6 bytes (Keys)全部で6つ。
    AZ_REPORT_SIZE(1),     0x08,          //   REPORT_SIZE(8)おのおの8ビットで表現
    AZ_LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM(0)キーコードの範囲 開始
    AZ_LOGICAL_MAXIMUM(1), 0x65,          //   LOGICAL_MAXIMUM(0x65) ; 101 keys キーコードの範囲 終了
  
      AZ_USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
    AZ_USAGE_MINIMUM(1),   0x00,          //   USAGE_MINIMUM (0)
    AZ_USAGE_MAXIMUM(1),   0xEF,          //   USAGE_MAXIMUM (0x65)
    AZ_HIDINPUT(1),        0x00,          //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  
      AZ_END_COLLECTION(0),                 // END_COLLECTION
      // ------------------------------------------------- remap
      0x06, 0x60, 0xFF,
      0x09, 0x61,
      0xa1, 0x01,
      0x85, REPORT_AZTOOL_ID,
      
      0x09, 0x62, 
      0x15, 0x00, 
      0x26, 0xFF, 0x00, 
      0x95, INPUT_REPORT_RAW_MAX_LEN,
      0x75, 0x08, 
      0x81, 0x06, 
    
      0x09, 0x63, 
      0x15, 0x00, 
      0x26, 0xFF, 0x00, 
      0x95, OUTPUT_REPORT_RAW_MAX_LEN, //REPORT_COUNT(32)
      0x75, 0x08, //REPORT_SIZE(8)
      0x91, 0x83, 
      0xC0             // End Collection (Application)
  
  };

AZBLEHidAdafruit::AZBLEHidAdafruit(void)
//  : BLEHidGeneric(4, 2, 0)
  : BLEHidGeneric(2, 2, 0)
{
  _mse_buttons = 0;
  _kbd_led_cb = NULL;
}

err_t AZBLEHidAdafruit::begin_my(void)
{
  int i;
  for (i=0; i<32; i++) {
      aztool_data[i] = 50 + i;
  }

  VERIFY ( (_report_map != NULL) && _report_map_len, NRF_ERROR_INVALID_PARAM);

  // Invoke base class begin()
  VERIFY_STATUS( BLEService::begin() );
  BLEService::setPermission(SECMODE_OPEN, SECMODE_OPEN);

  _chr_protocol = new BLECharacteristic(UUID16_CHR_PROTOCOL_MODE);
  VERIFY(_chr_protocol, NRF_ERROR_NO_MEM);
  _chr_protocol->setTempMemory();
  _chr_protocol->setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  _chr_protocol->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  _chr_protocol->setFixedLen(1);
  _chr_protocol->setWriteCallback(BLEHidGeneric::blehid_generic_protocol_mode_cb);
  VERIFY_STATUS( _chr_protocol->begin() );
  _chr_protocol->write8(_report_mode);

  // Input reports
  for(uint8_t i=0; i<_num_input; i++)
  {
    _chr_inputs[i].setUuid(UUID16_CHR_REPORT);
    _chr_inputs[i].setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
    _chr_inputs[i].setPermission(SECMODE_OPEN , SECMODE_NO_ACCESS );
    _chr_inputs[i].setReportRefDescriptor(i+1, REPORT_TYPE_INPUT);

    // Input report len is configured, else variable len up to 255
    _chr_inputs[i].setFixedLen( _input_len[i] );

    VERIFY_STATUS( _chr_inputs[i].begin() );
  }

  // Output reports
  uint16_t output_len_my[][2] = { {REPORT_ID_KEYBOARD, 1}, {REPORT_ID_AZTOOL, OUTPUT_REPORT_RAW_MAX_LEN} };
    _chr_outputs[0].setUuid(UUID16_CHR_REPORT);
    _chr_outputs[0].setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
    _chr_outputs[0].setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _chr_outputs[0].setReportRefDescriptor(1, REPORT_TYPE_OUTPUT);
    _chr_outputs[0].setFixedLen( 1 );
    VERIFY_STATUS( _chr_outputs[0].begin() );
    _chr_outputs[0].write8(0);

    _chr_outputs[1].setUuid(UUID16_CHR_REPORT);
    _chr_outputs[1].setProperties( CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
    _chr_outputs[1].setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _chr_outputs[1].setReportRefDescriptor(2, REPORT_TYPE_OUTPUT);
    _chr_outputs[1].setFixedLen(OUTPUT_REPORT_RAW_MAX_LEN);
    VERIFY_STATUS( _chr_outputs[1].begin() );
    _chr_outputs[1].write(aztool_data, sizeof(aztool_data));
    // _chr_outputs[0].write8(0);
    _chr_outputs[1].setWriteCallback(setAztoolOutputCallback);

    
  // HID Info
  _hid_info[0] = 0x11;
  _hid_info[1] = 0x01;
  _hid_info[2] = 0x00;
  _hid_info[3] = 0x01;
  BLECharacteristic hid_info(UUID16_CHR_HID_INFORMATION);
  hid_info.setTempMemory();
  hid_info.setProperties(CHR_PROPS_READ);
  hid_info.setPermission(SECMODE_ENC_NO_MITM , SECMODE_NO_ACCESS );
  hid_info.setFixedLen(sizeof(_hid_info));
  VERIFY_STATUS( hid_info.begin() );
  hid_info.write(_hid_info, sizeof(_hid_info));

  // Report Map (HID Report Descriptor)
  BLECharacteristic report_map(UUID16_CHR_REPORT_MAP);
  report_map.setTempMemory();
  report_map.setProperties(CHR_PROPS_READ);
  report_map.setPermission(SECMODE_ENC_NO_MITM , SECMODE_ENC_NO_MITM );
  report_map.setFixedLen(sizeof(hid_report_descriptor));
  VERIFY_STATUS( report_map.begin() );
  report_map.write(hid_report_descriptor, sizeof(hid_report_descriptor));


  // HID Control Point
  _chr_control.setProperties(CHR_PROPS_WRITE_WO_RESP);
  _chr_control.setPermission(SECMODE_NO_ACCESS , SECMODE_ENC_NO_MITM );
  _chr_control.setFixedLen(1);
  VERIFY_STATUS( _chr_control.begin() );
  _chr_control.write8(0);

  return ERROR_NONE;
}


err_t AZBLEHidAdafruit::begin(void)
{
  // keyboard, consumer, mouse
//  uint16_t input_len [] = { INPUT_REPORT_RAW_MAX_LEN, sizeof(hid_keyboard_report_t), 2, sizeof(hid_mouse_report_t) };
//  uint16_t output_len[] = { OUTPUT_REPORT_RAW_MAX_LEN, 1 };
  uint16_t input_len [] = { sizeof(hid_keyboard_report_t), INPUT_REPORT_RAW_MAX_LEN };
  uint16_t output_len[] = { 1, OUTPUT_REPORT_RAW_MAX_LEN };

  setReportLen(input_len, output_len, NULL);
  enableKeyboard(false);
  enableMouse(false);
  setReportMap(hid_report_descriptor, sizeof(hid_report_descriptor));

  // VERIFY_STATUS( BLEHidGeneric::begin() );
  VERIFY_STATUS( AZBLEHidAdafruit::begin_my() );

  // this->setOutputReportCallback(2, setAztoolOutputCallback);

  // Attempt to change the connection interval to 11.25-15 ms when starting HID
  Bluefruit.Periph.setConnInterval(9, 12);

  return ERROR_NONE;
}

/*------------------------------------------------------------------*/
/* Keyboard Multiple Connections
 *------------------------------------------------------------------*/
void AZBLEHidAdafruit::blehid_ada_keyboard_output_cb(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  LOG_LV2("HID", "Keyboard LED : 0x%02X", data[0]);

  AZBLEHidAdafruit& svc = (AZBLEHidAdafruit&) chr->parentService();
  if ( svc._kbd_led_cb ) svc._kbd_led_cb(conn_hdl, data[0]);
}

void AZBLEHidAdafruit::setKeyboardLedCallback(kbd_led_cb_t fp)
{
  _kbd_led_cb = fp;

  // Report mode
  this->setOutputReportCallback(REPORT_ID_KEYBOARD, fp ? blehid_ada_keyboard_output_cb : NULL);

  // Boot mode
  // _chr_boot_keyboard_output->setWriteCallback(fp ? blehid_ada_keyboard_output_cb : NULL);
}


bool AZBLEHidAdafruit::keyboardReport(uint16_t conn_hdl, hid_keyboard_report_t* report)
{
  if ( isBootMode() )
  {
    return bootKeyboardReport(conn_hdl, report, sizeof(hid_keyboard_report_t));
  }else
  {
    return inputReport(conn_hdl, REPORT_ID_KEYBOARD, report, sizeof(hid_keyboard_report_t));
  }
}

bool AZBLEHidAdafruit::keyboardReport(uint16_t conn_hdl, uint8_t modifier, uint8_t keycode[6])
{
  hid_keyboard_report_t report =
  {
      .modifier = modifier,
  };
  memcpy(report.keycode, keycode, 6);

  return keyboardReport(conn_hdl, &report);
}

bool AZBLEHidAdafruit::keyPress(uint16_t conn_hdl, char ch)
{
  hid_keyboard_report_t report;
  varclr(&report);

  report.modifier = ( hid_ascii_to_keycode[(uint8_t)ch][0] ) ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
  report.keycode[0] = hid_ascii_to_keycode[(uint8_t)ch][1];

  return keyboardReport(conn_hdl, &report);
}

bool AZBLEHidAdafruit::keyRelease(uint16_t conn_hdl)
{
  hid_keyboard_report_t report;
  varclr(&report);

  return keyboardReport(conn_hdl, &report);
}

bool AZBLEHidAdafruit::keySequence(uint16_t conn_hdl, const char* str, int interval)
{
  // Send each key in sequence
  char ch;
  while( (ch = *str++) != 0 )
  {
    char lookahead = *str;

    keyPress(conn_hdl, ch);
    delay(interval);

    /* Only need to empty report if the next character is NULL or the same with
     * the current one, else no need to send */
    if ( lookahead == ch || lookahead == 0 )
    {
      keyRelease(conn_hdl);
      delay(interval);
    }
  }

  return true;
}

/*------------------------------------------------------------------*/
/* Consumer Media Key
 *------------------------------------------------------------------*/
bool AZBLEHidAdafruit::consumerReport(uint16_t conn_hdl, uint16_t usage_code)
{
//    return inputReport(conn_hdl, REPORT_ID_CONSUMER_CONTROL, &usage_code, sizeof(usage_code));
    return inputReport(conn_hdl, 2, &usage_code, sizeof(usage_code));
}

bool AZBLEHidAdafruit::consumerKeyPress(uint16_t conn_hdl, uint16_t usage_code)
{
  return consumerReport(conn_hdl, usage_code);
}

bool AZBLEHidAdafruit::consumerKeyRelease(uint16_t conn_hdl)
{
  uint16_t usage = 0;
  return consumerReport(conn_hdl, usage);
}

/*------------------------------------------------------------------*/
/* Mouse
 *------------------------------------------------------------------*/
bool AZBLEHidAdafruit::mouseReport(uint16_t conn_hdl, hid_mouse_report_t* report)
{
  if ( isBootMode() )
  {
    return bootMouseReport(conn_hdl, report, sizeof(hid_mouse_report_t));
  }else
  {
//     return inputReport(conn_hdl, REPORT_ID_MOUSE, report, sizeof(hid_mouse_report_t));
    return inputReport(conn_hdl, 3, report, sizeof(hid_mouse_report_t));
  }
}

bool AZBLEHidAdafruit::mouseReport(uint16_t conn_hdl, uint8_t buttons, int8_t x, int8_t y, int8_t wheel, int8_t pan)
{
  hid_mouse_report_t report =
  {
      .buttons = buttons,
      .x       = x,
      .y       = y,
      .wheel   = wheel,
//      .pan     = pan
  };

  _mse_buttons = buttons;

  return mouseReport(conn_hdl, &report);
}

bool AZBLEHidAdafruit::mouseButtonPress(uint16_t conn_hdl, uint8_t buttons)
{
  _mse_buttons = buttons;
  return mouseReport(conn_hdl, buttons, 0, 0, 0, 0);
}

bool AZBLEHidAdafruit::mouseButtonRelease(uint16_t conn_hdl)
{
  return mouseReport(conn_hdl, 0, 0, 0, 0, 0);
}

bool AZBLEHidAdafruit::mouseMove(uint16_t conn_hdl, int8_t x, int8_t y)
{
  return mouseReport(conn_hdl, _mse_buttons, x, y, 0, 0);
}

bool AZBLEHidAdafruit::mouseScroll(uint16_t conn_hdl, int8_t scroll)
{
  return mouseReport(conn_hdl, _mse_buttons, 0, 0, scroll, 0);
}

bool AZBLEHidAdafruit::mousePan(uint16_t conn_hdl, int8_t pan)
{
  return mouseReport(conn_hdl, _mse_buttons, 0, 0, 0, pan);
}

/*------------------------------------------------------------------*/
/* Single Connections
 *------------------------------------------------------------------*/

//------------- Keyboard -------------//
bool AZBLEHidAdafruit::keyboardReport(hid_keyboard_report_t* report)
{
  return keyboardReport(BLE_CONN_HANDLE_INVALID, report);
}

bool AZBLEHidAdafruit::keyboardReport(uint8_t modifier, uint8_t keycode[6])
{
  return keyboardReport(BLE_CONN_HANDLE_INVALID, modifier, keycode);
}

bool AZBLEHidAdafruit::keyPress(char ch)
{
  return keyPress(BLE_CONN_HANDLE_INVALID, ch);
}

bool AZBLEHidAdafruit::keyRelease(void)
{
  return keyRelease(BLE_CONN_HANDLE_INVALID);
}

bool AZBLEHidAdafruit::keySequence(const char* str, int interval)
{
  return keySequence(BLE_CONN_HANDLE_INVALID, str, interval);
}

//------------- Consumer Media Keys -------------//
bool AZBLEHidAdafruit::consumerReport(uint16_t usage_code)
{
  return consumerReport(BLE_CONN_HANDLE_INVALID, usage_code);
}

bool AZBLEHidAdafruit::consumerKeyPress(uint16_t usage_code)
{
  return consumerKeyPress(BLE_CONN_HANDLE_INVALID, usage_code);
}

bool AZBLEHidAdafruit::consumerKeyRelease(void)
{
  return consumerKeyRelease(BLE_CONN_HANDLE_INVALID);
}

//------------- Mouse -------------//
bool AZBLEHidAdafruit::mouseReport(hid_mouse_report_t* report)
{
  return mouseReport(BLE_CONN_HANDLE_INVALID, report);
}

bool AZBLEHidAdafruit::mouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel, int8_t pan)
{
  return mouseReport(BLE_CONN_HANDLE_INVALID, buttons, x, y, wheel, pan);
}

bool AZBLEHidAdafruit::mouseButtonPress(uint8_t buttons)
{
  return mouseButtonPress(BLE_CONN_HANDLE_INVALID, buttons);
}

bool AZBLEHidAdafruit::mouseButtonRelease(void)
{
  return mouseButtonRelease(BLE_CONN_HANDLE_INVALID);
}

bool AZBLEHidAdafruit::mouseMove(int8_t x, int8_t y)
{
  return mouseMove(BLE_CONN_HANDLE_INVALID, x, y);
}

bool AZBLEHidAdafruit::mouseScroll(int8_t scroll)
{
  return mouseScroll(BLE_CONN_HANDLE_INVALID, scroll);
}

bool AZBLEHidAdafruit::mousePan(int8_t pan)
{
  return mousePan(BLE_CONN_HANDLE_INVALID, pan);
}

void AZBLEHidAdafruit::sendTest()
{
    int i;
    for (i=0; i<32; i++) {
        aztool_data[i] = 10 + i;
    }
    inputReport(2, aztool_data, sizeof(aztool_data));
}


static void setAztoolOutputCallback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{

}
