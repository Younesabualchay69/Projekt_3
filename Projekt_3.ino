

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Ultrasonic.h>
#define SS_PIN 10
#define RST_PIN 5

Ultrasonic ultrasonic(5, 6); // pinnar för sensorn // (Trig, Echo)

MFRC522 rfid(SS_PIN, RST_PIN);

/*byte Younes_card[4] = {0x8A, 0x41, 0xA7, 0xBE}; // en array med bytes för kotet // 0x finns för att säga att det är hexa decimalt
  byte Visitor_card[4] = {0x9A, 0x0C, 0xCB, 0x6A};
*/

byte valid_cards[20][4] = {
  {0x8A, 0x41, 0xA7, 0xBE},
  {0x9A, 0x0C, 0xCB, 0x6A},
  {0x31, 0xAE, 0x18, 0x3B},
  {0x0D, 0xEE, 0xE6, 0xDE},
  /* {0x74, 0x5E, 0x3C, 0x43}*/
};

int number_of_cards = 4;

int distance;
const int led = 7;
const int Gled = 3;
const int buzzer = 4;
const int btn = 2;
const int btn_2 = A0;


Servo servo;
int angle = 10;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  servo.attach(8);
  servo.write(angle);
  pinMode(led, OUTPUT);
  pinMode(Gled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT);
  pinMode(btn_2, INPUT);


  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  // Få avståndet
  distance = ultrasonic.read();
  Serial.print("Distance in CM: ");
  Serial.println(distance);
  delay(700);

  noTone(buzzer);
  if (distance < 80) {
    digitalWrite(led, HIGH);
    if (rfid.PICC_IsNewCardPresent()) { // Här kollar den om den har upptäckt ett nytt kort
      if (rfid.PICC_ReadCardSerial()) { // Försöker läsa, Om den lyckas, initiera if-satsen
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        tone(buzzer, 1000);
        Serial.print("RFID/NFC Tag Type: ");
        Serial.println(rfid.PICC_GetTypeName(piccType));

        // print NUID in Serial Monitor in the hex format
        // Den läser uniqe ID för kortet så att alla kort får olika användare
        Serial.print("UID:");
        for (int i = 0; i < rfid.uid.size; i++) {  // intiterar en loop med längden på arrayen på NFC kortet
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "); // printar varje element i kortets array
          Serial.print(rfid.uid.uidByte[i], HEX);

        }
        Serial.println();
        if (digitalRead(btn_2) == HIGH) {   //Raderar kortet som avläses
          Serial.println("btn_2");
          /* Find the right card */
          int right_card = -1;
          bool flag = true;
          // Itererar över kort
          for (int i = 0; i < number_of_cards; i++) {
            if (flag == true && i != 0)
              break;
            else
              flag = true;
            Serial.print("Checking card ");
            Serial.println(i);
            for (int j = 0; j < rfid.uid.size && j < 4; j++) {
              if (rfid.uid.uidByte[j] != valid_cards[i][j]) {
                flag = false;
              }
            }
            if (flag == true) {
              right_card = i;
              break;
            }
          }
          Serial.print("Hittade kortet på index: ");
          Serial.println(right_card);
          /* ta bort right card */
          if (right_card != -1) {
            Serial.println("Kort innan");
            for (int i = 0; i < number_of_cards; i++) {
              for (int j = 0; j < 4; j++) {
                Serial.print(valid_cards[i][j], HEX);
                Serial.print(" ");
              }
              Serial.println("");
            }
            for (int card = right_card; card < number_of_cards - 1; card++) {
              Serial.print("Moving card ");
              Serial.print(card + 1);
              Serial.print(" to card ");
              Serial.println(card);
              for (int i = 0; i < 4; i++) {
                valid_cards[card][i] = valid_cards[card + 1][i];
                Serial.print(card);
                Serial.print(" ");
                Serial.print(i);
                Serial.print(" - ");
                Serial.print(card + 1);
                Serial.print(" ");
                Serial.println(i);
              }
            }
            number_of_cards--;
            Serial.println("Cards after");
            for (int i = 0; i < number_of_cards; i++) {
              for (int j = 0; j < 4; j++) {
                Serial.print(valid_cards[i][j], HEX);
                Serial.print(" ");
              }
              Serial.println("");
            }
          }
        }

        if (digitalRead(btn) == HIGH) {
          if (number_of_cards < 20) {
            for (int i = 0; i < 4; i++) {
              valid_cards[number_of_cards][i] = rfid.uid.uidByte[i];
            }
            number_of_cards++;
          }
          else {
            Serial.println("Du har redan registrerat detta kort");
          }
        }

        else {
          boolean flag = true;
          for (int i = 0; i < 20; i++) {
            if (flag == true && i != 0)
              break;
            else
              flag = true;
            for (int j = 0; j < rfid.uid.size && j < 4; j++) {
              if (rfid.uid.uidByte[j] != valid_cards[i][j]) {
                flag = false;
              }
            }
          }                                                             /*for (int i = 0; i < rfid.uid.size && i < 4; i++) {
                                                                    if (rfid.uid.uidByte[i] != Younes_card[i])
                                                                    flag = false;
                                                                    }*/


          if (flag) {
            digitalWrite(buzzer, HIGH);
            delay(50);
            digitalWrite(buzzer, LOW);
            digitalWrite(led, LOW);
            digitalWrite(Gled, HIGH);
            Serial.println("Correct card");
            for (angle = 10; angle < 180; angle++)
            {
              servo.write(angle);
              delay(5);
            }
            delay(50);

            for (angle = 180; angle > 10; angle--)
            {
              servo.write(angle);
              delay(5);
            }
            digitalWrite(Gled, LOW);
            digitalWrite(led, HIGH);
          }

          else {
            Serial.println("Dra åt helvette, åtkomst nekad");
            digitalWrite(buzzer, HIGH);

            digitalWrite(buzzer, LOW);


          }
        }  

        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD, cannot find new tags if we don't call this after communication with a tag
      }
    }
  }
  if (distance > 80) {
    digitalWrite(led, LOW);
  }
}
