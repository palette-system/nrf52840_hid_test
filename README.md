

#環境の作り方＆試した方法
<br>
１．WindowsでArduinoをインストール（使ってるのは2.3.3だった）<br>
<br>
２．Arduino起動→追加のボードマネージャのURLに下記を追加<br>
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json<br>
<br>
３．ボードマネージャから「Seeed nRF52 Boards by Seeed Studio」をインストール（バージョンは1.1.9だった）<br>
<br>
４．nrf52840_hid_2.inoを開いてコンパイル<br>
　　(ボードは「Seeed nRF52 Boards」→「Seeed XIAO nRF52840」を選択)<br>
　　(ReportID1でキーボード、ReportID2でHID_RAWのみのReportMap)<br>
<br>
５．PCとBluetoothでペアリング<br>
<br>
６．AZTOOL　https://palette-system.github.io/aztool/　で接続（すにさんが前に作った　https://yswallow.github.io/javascript/simple/webhid/nrf52_init.html　でも）<br>
<br>
７．HID_RAW自体は認識してコネクトはできてデバイスのオブジェクトは取得できるが、device.sendReportしようとするとJavaScriptの下記エラーが発生してしまいHID_RAWへの書き込みが行えない<br>
「Uncaught (in promise) NotAllowedError: Failed to write the report.」<br>
<br>
８．どこかの権限周りが悪そうな気がして↓ここが怪しかったので自分のソースに AZBLEHidAdafruit::begin_my を持って来てsetPropertiesとsetPermissionの値を変えてみたけどうまく行かず<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/services/BLEHidGeneric.cpp#L184-L185<br>
<br>
↓定数定義はここにあった<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/bluefruit_common.h#L82-L87<br>
<br>
<br>
<br>
#補足
<br>
ただBluetooth　HIDするだけなら結構簡単で下記のサンプルですぐHIDできる。<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/examples/Peripheral/blehid_keyboard/blehid_keyboard.ino<br>
<br>
HIDのクラス<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/services/BLEHidAdafruit.cpp<br>
<br>
ここでReportMap作成<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/services/BLEHidAdafruit.cpp#L49-L54<br>
<br>
ここでINPUT、OUTPUTの数やサイズを設定<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/services/BLEHidAdafruit.cpp#L57<br>
https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/services/BLEHidAdafruit.cpp#L66-L67<br>
<br>
