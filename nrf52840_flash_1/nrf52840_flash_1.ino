// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <SdFat.h>
#include <Adafruit_SPIFlash.h>
// #include <LSM6DS3.h>

// Flash Chip declerations, made up from Data Sheets.
//  https://files.seeedstudio.com/wiki/github_weiruanexample/Flash_P25Q16H-UXH-IR_Datasheet.pdf
SPIFlash_Device_t const p25q16h{
  .total_size = (1UL << 21),  // 2MiB
  .start_up_time_us = 10000,
  .manufacturer_id = 0x85,
  .memory_type = 0x60,
  .capacity = 0x15,
  .max_clock_speed_mhz = 55,
  .quad_enable_bit_mask = 0x02,
  .has_sector_protection = 1,
  .supports_fast_read = 1,
  .supports_qspi = 1,
  .supports_qspi_writes = 1,
  .write_status_register_split = 1,
  .single_status_byte = 0,
  .is_fram = 0,
};

//Create a instance of class LSM6DS3
// LSM6DS3 myIMU(I2C_MODE, 0x6A);
// #define int2Pin PIN_LSM6DS3TR_C_INT1

// #define SS_SPI1 25  // Defaul SS or CS for the Onboard QSPI Flash Chip

SPIClass SPI_2(NRF_SPIM0, PIN_QSPI_IO1, PIN_QSPI_SCK, PIN_QSPI_IO0);  // Onboard QSPI Flash chip
Adafruit_FlashTransport_SPI QflashTransport(PIN_QSPI_CS, SPI_2);      // CS for QSPI Flash
Adafruit_SPIFlash Qflash(&QflashTransport);



void setup() {
  int i, j;
  Serial.begin(9600);
  while (!Serial) {
    delay(100);  // waits for serial
  }
  Serial.println("Starting up");

  if (!Qflash.begin(&p25q16h, 1)) {
    Serial.println(F("Error, failed to initialize QSPI flash chip!"));
    while (1)
      ;
  }
  Serial.println(Qflash.getJEDECID(), HEX);

  Serial.print(Qflash.size() / 1024);
  Serial.println(F(" KB"));

  Serial.println(Qflash.read8(0x000500));
  Serial.println("-----------");
  for (j=512; j<640; j++) {
    Serial.printf("j=%02x ", j);
    for (i=0; i<64; i++) {
      Serial.printf("%02x ", Qflash.read8( (j * 64) + i));
      if (i == 15 || i == 31 || i == 47) Serial.printf(" ");
    }
    Serial.println("-");
    if ((j % 16) == 15) Serial.println("-----------");
  }
  Serial.print("page size ");
  //256
  Serial.println(Qflash.pageSize());
  Serial.print("num page ");
  Serial.println(Qflash.numPages());
  Serial.print("sector count ");
  Serial.print(Qflash.sectorCount());
}

void loop() {
}