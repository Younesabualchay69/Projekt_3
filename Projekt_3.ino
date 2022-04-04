/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

byte correct_card[4] = {0x8A, 0x41, 0xA7, 0xBE}; // en array med bytes för kotet // 0x finns för att säga att det är hexa decimalt

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // is there a new tag detected?
    if (rfid.PICC_ReadCardSerial()) { // try to read, on success, enter the if statement
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print NUID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      boolean flag = true;
      for (int i = 0; i < rfid.uid.size && i < 4; i++) {
        if (rfid.uid.uidByte[i] != correct_card[i])
          flag = false;
      }
      if (flag)
        Serial.println("COrrect card");

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD, cannot find new tags if we don't call this after communication with a tag
    }
  }
}
