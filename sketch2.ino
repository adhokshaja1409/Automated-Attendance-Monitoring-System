#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
  }
}

void loop() {
    byte dataBlock[10];
    Serial.println(F("Enter USN"));
    delay(10000);
    Serial.readBytes(dataBlock, 10);
    
   if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
    byte sector         = 1;
    byte blockAddr      = 4;
    //usn=Serial.readString();
    //dataBlock= usn;
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    else{
      Serial.println("Write Sucessful");
    }
    Serial.println();
    digitalWrite(RST_PIN, HIGH);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
