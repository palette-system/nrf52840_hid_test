

# 下記ファイルの BLE_GATT_ATT_MTU_DEFAULT がデフォルト 23 を 35 にしないと 送信する時 20 で通知が行ってしまう

```
C:\Users\user\AppData\Local\Arduino15\packages\Seeeduino\hardware\nrf52\1.1.10\cores\nRF5\nordic\softdevice\s140_nrf52_7.3.0_API\include\ble_gatt.h
```


# hexから

```
python.exe uf2conv.py nrf52840_hid_2.ino.hex --family 0xADA52840 --convert --output out.uf2
```


