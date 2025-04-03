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

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h> // for Serial

using namespace Adafruit_LittleFS_Namespace;


File file(InternalFS);


// the setup function runs once when you press reset or power the board
void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("InternalFS Format Example");
  Serial.println();

  // Wait for user input to run.
  Serial.println("This sketch will destroy all of your data in External Flash!");
  Serial.print("Enter any keys to continue:");
  while ( !Serial.available() ) delay(1);
  Serial.println();
  Serial.println();

  // Initialize Internal File System
  InternalFS.begin();

  Serial.print("Formating ... ");
  delay(1); // for message appear on monitor

  // Format 
  // InternalFS.format();

  /*
      Serial.println("open START");
    if( file.open("/test.txt", FILE_O_WRITE) )
    {
      Serial.println("open OK");
      file.write("test", 4);
      file.close();
      Serial.println("write END");
    }else
    {
      Serial.println("Failed!");
    }
    */
    
    uint32_t readlen;
    char buffer[64] = { 0 };
    file.open("/test.txt", FILE_O_READ);
    readlen = file.read(buffer, sizeof(buffer));

    buffer[readlen] = 0;
    Serial.println(buffer);
    file.close();


  Serial.println("Done");
}

// the loop function runs over and over again forever
void loop() 
{
  // nothing to do
}
