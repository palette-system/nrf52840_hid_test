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
#include "src/lib/az_common.h"
// BLEキーボード
#include "src/lib/az_keyboard.h"

// 共通クラス
AzCommon common_cls = AzCommon();

// キーボードモードクラス
AzKeyboard azkb = AzKeyboard();


void setup() 
{
  /*
  Serial.begin(9600);
  while(!Serial){
      delay(100);
  }
  */
    // 共通処理の初期化
    common_cls.common_start();

    // 設定jsonの読み込み
    common_cls.load_setting_json();

    // キーボード初期処理
    azkb.begin_keyboard();

    // キーボードとして起動
    azkb.start_keyboard();
}


void loop() 
{

  // blehid.keyPress(0x41);
    // delay(50);
    // blehid.keyRelease();
    // delay(100000);
    // blehid.sendTest();
    delay(1000);
}

