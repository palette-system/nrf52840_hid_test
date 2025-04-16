


# arduinoIDE設定に追加するJSON
```
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
```


# ボード
```
Seed nRF52 Boards by Seeed Studio 1.1.10
Seeed XIAO nRF52840
```


# 下記ファイルの BLE_GATT_ATT_MTU_DEFAULT がデフォルト 23 を 35 にしないと 送信する時 20 で通知が行ってしまう

```
C:\Users\user\AppData\Local\Arduino15\packages\Seeeduino\hardware\nrf52\1.1.10\cores\nRF5\nordic\softdevice\s140_nrf52_7.3.0_API\include\ble_gatt.h
```


# ライブラリ
```
ArduinoJson by Benoit Blanchon version 7.4.1
Adafruit SSD 1306 by Adafruit version 2.5.13
Adafruit NeoPixel by Adafruit version 1.12.5
Adafruit MCP23017 Arduino Library by Adafruit version 2.3.2
```


# hexからuf2出力

```
python.exe uf2conv.py nrf52840_hid_2.ino.hex --family 0xADA52840 --convert --output out.uf2
```


