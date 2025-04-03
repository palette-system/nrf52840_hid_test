

#include "ble_keyboard_jis.h"



// コンストラクタ
BleKeyboardJIS::BleKeyboardJIS(void)
{
};


// HID report map 設定
// 例： bleKeyboard.set_report_map((uint8_t *)_hidReportDescriptorDefault, sizeof(_hidReportDescriptorDefault));
void BleKeyboardJIS::set_report_map(uint8_t * report_map, unsigned short report_size)
{
};

// BLEキーボードとして処理開始
void BleKeyboardJIS::begin(std::string deviceName, std::string deviceManufacturer)
{
};

// 接続中かどうかを返す
bool BleKeyboardJIS::isConnected(void)
{
    return true;
};


unsigned short BleKeyboardJIS::modifiers_press(unsigned short k) {
  this->setConnInterval(0); // 消費電力モード解除
  if (k & JIS_SHIFT) { // shift
    this->_keyReport.modifiers |= 0x02; // the left shift modifier
    k &= 0xFF;
  }
  if (k == 224) { k = 0; this->_keyReport.modifiers |= 0x01; } // LEFT Ctrl
  if (k == 228) { k = 0; this->_keyReport.modifiers |= 0x10; } // RIGHT Ctrl
  if (k == 225) { k = 0; this->_keyReport.modifiers |= 0x02; } // LEFT Shift
  if (k == 229) { k = 0; this->_keyReport.modifiers |= 0x20; } // RIGHT Shift
  if (k == 226) { k = 0; this->_keyReport.modifiers |= 0x04; } // LEFT Alt
  if (k == 230) { k = 0; this->_keyReport.modifiers |= 0x40; } // RIGHT Alt
  if (k == 227) { k = 0; this->_keyReport.modifiers |= 0x08; } // LEFT GUI
  if (k == 231) { k = 0; this->_keyReport.modifiers |= 0x80; } // RIGHT GUI
  return k;
};


unsigned short BleKeyboardJIS::modifiers_release(unsigned short k) {
  if (k & JIS_SHIFT) { // shift
    this->_keyReport.modifiers &= ~(0x02);  // the left shift modifier
    k &= 0xFF;
  }
  if (k == 224) { k = 0; this->_keyReport.modifiers &= ~(0x01); } // LEFT Ctrl
  if (k == 228) { k = 0; this->_keyReport.modifiers &= ~(0x10); } // RIGHT Ctrl
  if (k == 225) { k = 0; this->_keyReport.modifiers &= ~(0x02); } // LEFT Shift
  if (k == 229) { k = 0; this->_keyReport.modifiers &= ~(0x20); } // RIGHT Shift
  if (k == 226) { k = 0; this->_keyReport.modifiers &= ~(0x04); } // LEFT Alt
  if (k == 230) { k = 0; this->_keyReport.modifiers &= ~(0x40); } // RIGHT Alt
  if (k == 227) { k = 0; this->_keyReport.modifiers &= ~(0x08); } // LEFT GUI
  if (k == 231) { k = 0; this->_keyReport.modifiers &= ~(0x80); } // RIGHT GUI
  return k;
};

// Shiftを離す
void BleKeyboardJIS::shift_release() {
  int i;
  this->_keyReport.modifiers &= ~(0x22);
  for (i=0; i<6; i++) {
    if (this->_keyReport.keys[i] == 225 || this->_keyReport.keys[i] == 229) {
      this->_keyReport.keys[i] = 0;
    }
  }
}

unsigned short BleKeyboardJIS::modifiers_media_press(unsigned short k) {
  this->setConnInterval(0); // 消費電力モード解除
  if (k == 8193) { // Eject
    this->_mediaKeyReport[0] |= 0x01;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8194) { // Media Next
    this->_mediaKeyReport[0] |= 0x02;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8195) { // Media Previous
    this->_mediaKeyReport[0] |= 0x04;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8196) { // Media Stop
    this->_mediaKeyReport[0] |= 0x08;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8197) { // Media play / pause
    this->_mediaKeyReport[0] |= 0x10;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8198) { // Media Mute
    this->_mediaKeyReport[0] |= 0x20;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8199) { // Media volume +
    this->_mediaKeyReport[0] |= 0x40;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8200) { // Media volume -
    this->_mediaKeyReport[0] |= 0x80;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  }
  return 0;
};

unsigned short BleKeyboardJIS::modifiers_media_release(unsigned short k) {
  if (k == 8193) { // Eject
    this->_mediaKeyReport[0] &= ~(0x01);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8194) { // Media Next
    this->_mediaKeyReport[0] &= ~(0x02);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8195) { // Media Previous
    this->_mediaKeyReport[0] &= ~(0x04);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8196) { // Media Stop
    this->_mediaKeyReport[0] &= ~(0x08);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8197) { // Media play / pause
    this->_mediaKeyReport[0] &= ~(0x10);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8198) { // Media Mute
    this->_mediaKeyReport[0] &= ~(0x20);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8199) { // Media volume +
    this->_mediaKeyReport[0] &= ~(0x40);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8200) { // Media volume -
    this->_mediaKeyReport[0] &= ~(0x80);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  }
  return 0;
};

void BleKeyboardJIS::sendReport(KeyReport* keys)
{
  if (this->isConnected())
  {
    // this->pInputCharacteristic->setValue((uint8_t*)keys, sizeof(KeyReport));
    // this->pInputCharacteristic->notify();
  }
};

void BleKeyboardJIS::sendReport(MediaKeyReport* keys)
{
  if (this->isConnected())
  {
    // this->pInputCharacteristic2->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
    // this->pInputCharacteristic2->notify();
  }
};

void BleKeyboardJIS::mouse_click(uint8_t b)
{
    this->_MouseButtons = b;
    this->mouse_move(0,0,0,0);
    delay(10);
    this->_MouseButtons = 0x00;
    this->mouse_move(0,0,0,0);
};


void BleKeyboardJIS::mouse_press(uint8_t b)
{
    this->_MouseButtons |= b;
    this->mouse_move(0,0,0,0);
};

void BleKeyboardJIS::mouse_release(uint8_t b)
{
    this->_MouseButtons &= ~(b);
    this->mouse_move(0,0,0,0);
};

void BleKeyboardJIS::mouse_move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
    if (this->isConnected()) {
        if (x != 0 || y != 0 || wheel != 0 || hWheel != 0 || this->_MouseButtons != 0) {
            this->setConnInterval(0); // 消費電力モード解除
        }
        uint8_t m[5];
        m[0] = this->_MouseButtons;
        m[1] = x;
        m[2] = y;
        m[3] = wheel;
        m[4] = hWheel;
        // this->pInputCharacteristic3->setValue(m, 5);
        // this->pInputCharacteristic3->notify();
    }
};

size_t BleKeyboardJIS::press_raw(unsigned short k)
{
  uint8_t i;
  unsigned short kk;
  this->setConnInterval(0); // 消費電力モード解除
  // メディアキー
  if (modifiers_media_press(k)) return 1;
  kk = this->modifiers_press(k);
  if (this->_keyReport.keys[0] != kk && this->_keyReport.keys[1] != kk &&
    this->_keyReport.keys[2] != kk && this->_keyReport.keys[3] != kk &&
    this->_keyReport.keys[4] != kk && this->_keyReport.keys[5] != kk) {

    for (i=0; i<6; i++) {
      if (this->_keyReport.keys[i] == 0x00) {
        this->_keyReport.keys[i] = kk;
        break;
      }
    }
    if (i == 6) {
      return 0;
    }
  }
  this->sendReport(&_keyReport);
  return 1;
};

size_t BleKeyboardJIS::press_set(uint8_t k)
{
  uint8_t i;
  unsigned short kk;
  this->setConnInterval(0); // 消費電力モード解除
  kk = _asciimap[k];
  if (!kk) {
    return 0;
  }
  this->_keyReport.modifiers = 0x00;
  kk = this->modifiers_press(kk);
  this->_keyReport.keys[0] = kk;
  this->_keyReport.keys[1] = 0x00;
  this->_keyReport.keys[2] = 0x00;
  this->_keyReport.keys[3] = 0x00;
  this->_keyReport.keys[4] = 0x00;
  this->_keyReport.keys[5] = 0x00;

  this->sendReport(&this->_keyReport);
  return 1;
};

size_t BleKeyboardJIS::release_raw(unsigned short k)
{
  uint8_t i;
  unsigned short kk;
  // メディアキー
  if (modifiers_media_release(k)) return 1;
  kk = this->modifiers_release(k);

  // Test the key report to see if k is present.  Clear it if it exists.
  // Check all positions in case the key is present more than once (which it shouldn't be)
  for (i=0; i<6; i++) {
    if (0 != kk && this->_keyReport.keys[i] == kk) {
      this->_keyReport.keys[i] = 0x00;
    }
  }

  this->sendReport(&this->_keyReport);
  return 1;
};

void BleKeyboardJIS::releaseAll(void)
{
  this->_keyReport.keys[0] = 0;
  this->_keyReport.keys[1] = 0;
  this->_keyReport.keys[2] = 0;
  this->_keyReport.keys[3] = 0;
  this->_keyReport.keys[4] = 0;
  this->_keyReport.keys[5] = 0;
  this->_keyReport.modifiers = 0;
  this->_mediaKeyReport[0] = 0;
  this->_mediaKeyReport[1] = 0;
  this->sendReport(&this->_keyReport);
  this->sendReport(&this->_mediaKeyReport);
};

// Shiftが押されている状態かどうか(物理的に)
bool BleKeyboardJIS::onShift()
{
  int i;
  for (i=0; i<PRESS_KEY_MAX; i++) {
    if (press_key_list[i].key_num < 0) continue; // 押されたよデータ無ければ無視
    if (press_key_list[i].unpress_time > 0) continue; // 離したよカウントが始まっていたら押していないので無視
    if (press_key_list[i].key_id == 225 || press_key_list[i].key_id == 229) return true; // ShiftコードならばShiftが押されている
  }
  return false;
}

// コネクションインターバル設定
void BleKeyboardJIS::setConnInterval(int interval_type)
{
  if (hid_power_saving_mode == 0) return; // 通常モードなら何もしない
  hid_state_change_time = millis() + hid_saving_time;
  if (hid_power_saving_state == interval_type) return; // ステータスの変更が無ければ何もしない
  hid_power_saving_state = interval_type;
  if (hid_interval_saving == hid_interval_normal) return; // 省電力モードのインターバルと通常モードのインターバルが一緒なら何もしない
  if (!this->isConnected()) return; // 接続していなければ何もしない
  if (interval_type == 1) {
    // 省電力中
    // this->pServer->updateConnParams(hid_conn_handle, hid_interval_saving - 2, hid_interval_saving + 2, 0, 200);
  } else {
    // 通常
    // this->pServer->updateConnParams(hid_conn_handle, hid_interval_normal - 2, hid_interval_normal + 2, 0, 200);
  }
    
}


