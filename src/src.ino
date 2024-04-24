#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 5
#define DOOR_OPEN_TIME 5000

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;
Servo servo;
// Init array that will store new NUID

byte nuidPICC[4] = {115, 205, 84, 5};

void doorAuthenticated();
void doorNotAuthenticated();

void setup()
{
    Serial.begin(9600);
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
    servo.attach(3); // Init Servo

    pinMode(BUZZER_PIN, OUTPUT);
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
    servo.write(0);

    Serial.println(F("This code authenticates based on MIFARE nuid."));
}

void loop()
{
    if (!rfid.PICC_IsNewCardPresent())
        return;

    if (!rfid.PICC_ReadCardSerial())
        return;

    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
    }

    if (nuidAuthenticator(rfid.uid.uidByte, nuidPICC, 4))
        doorAuthenticated();
    else
        doorNotAuthenticated();
    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}



bool nuidAuthenticator(byte stored[], byte scanned[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (stored[i] != scanned[i])
            return false;
    }
    return true;
}