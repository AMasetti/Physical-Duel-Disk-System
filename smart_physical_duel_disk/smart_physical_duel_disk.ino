#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <SoftwareSerial.h>

// NFC pins declaration
#define PN532_IRQ   (4)
#define PN532_RESET (23)

// Buton pin declarations
const int buttonPin1 = 9;
const int buttonPin2 = 8;
const int buttonPin3 = 7;
const int buttonPin4 = 6;
const int buttonPin5 = 5;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;
int buttonState5 = 0;

// Long press on buttons 1 and 5
long buttonTimer1 = 0;
long buttonTimer5 = 0;
long longPressTime1 = 250;
long longPressTime5 = 250;
boolean buttonActive1 = false;
boolean buttonActive5 = false;
boolean longPressActive1 = false;
boolean longPressActive5 = false;

bool write_mode = false;

String inputString = "";
bool stringComplete = false;

SoftwareSerial BT(0, 1);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif


void setup(void) {

	// Set pins as INPUT_PULLUP no resistor needed
	pinMode(buttonPin1, INPUT_PULLUP);
	pinMode(buttonPin2, INPUT_PULLUP);
	pinMode(buttonPin3, INPUT_PULLUP);
	pinMode(buttonPin4, INPUT_PULLUP);
	pinMode(buttonPin5, INPUT_PULLUP);

	// Activate write mode
	buttonState1 = digitalRead(buttonPin1);
	if (buttonState1 == LOW){
		write_mode = true;
		Serial.begin(115200);
		Serial.println("Write Card ON");
	}

	BT.begin(9600);
	inputString.reserve(200);

	// Start NFC module
	nfc.begin();
	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		BT.print("NFC Reader not found");
		while (1) {
			BT.print("NFC Reader not found");
		}
	}
	nfc.SAMConfig();
}

String PrevCard = "89371139";

void loop(void) {

	uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	if (!write_mode) {
		// Read Buttons input
		buttonState1 = digitalRead(buttonPin1);
		buttonState2 = digitalRead(buttonPin2);
		buttonState3 = digitalRead(buttonPin3);
		buttonState4 = digitalRead(buttonPin4);
		buttonState5 = digitalRead(buttonPin5);

		// Buttons logic 1 to 5 in order 
		// Buttons 1 and 5 have long press detection, that why the complex code
		if (buttonState1 == LOW) {
			if (buttonActive1 == false) {
				buttonActive1 = true;
				buttonTimer1 = millis();
			}
			if ((millis() - buttonTimer1 > longPressTime1) && (longPressActive1 == false)) {
				longPressActive1 = true;
				BT.println("back");
			}
		} 
		else {
			if (buttonActive1 == true) {
				if (longPressActive1 == true) {
					longPressActive1 = false;
				}
				else {BT.println("0");}
				buttonActive1 = false;
			}
		}
		if (buttonState2 == LOW) BT.println("1");
		if (buttonState3 == LOW) BT.println("2");
		if (buttonState4 == LOW) BT.println("3");

		if (buttonState5 == LOW) {
			if (buttonActive5 == false) {
				buttonActive5 = true;
				buttonTimer5 = millis();
			}
			if ((millis() - buttonTimer5 > longPressTime5) && (longPressActive5 == false)) {
				longPressActive5 = true;
				BT.println("next");
			}
		} 
		else {
			if (buttonActive5 == true) {
				if (longPressActive5 == true) {
					longPressActive5 = false;
				}
				else {BT.println("4");}
				buttonActive5 = false;
			}
		}

		// ------------------ End of Buttons Logic / Start of NFC Logic ------------------

		delay(50);

		success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

		if (success) {
			if (uidLength == 7) {
				uint8_t data1[32];
				uint8_t data2[32];
				// Card info is stored in two pages inside the NFC Chip
				// z.B. BEWD card ID is 89371139
				// Page 4 would read 8937
				// Page 5 would read 1139
				// It is considered STANDARD for the Physical Duel Disk to use this pages for Cards
				success = nfc.mifareultralight_ReadPage (4, data1);
				success = nfc.mifareultralight_ReadPage (5, data2);

			    if (success) {
			    	// When a Card ID was read we spit out that Card ID via BT-Serial
					String ReadBlock;
					String ReadBlock2;
					// To prevent the creation of an empty string I set the default value of 11111111 before actually setting the card value
					String ReadCard = "11111111";
					// Set contents of Page 4
					for (char c : data1) ReadBlock += c;
					ReadCard[0] = ReadBlock[0];
					ReadCard[1] = ReadBlock[1];
					ReadCard[2] = ReadBlock[2];
					ReadCard[3] = ReadBlock[3];
					// Set contents of Page 5
					for (char c : data2) ReadBlock2 += c;
					ReadCard[4] = ReadBlock2[0];
					ReadCard[5] = ReadBlock2[1];
					ReadCard[6] = ReadBlock2[2];
					ReadCard[7] = ReadBlock2[3];

					// Prevent double read within the second
					if (ReadCard == PrevCard) {
						delay(1000);
						PrevCard = "89371139";
					}
					else {
				        PrevCard = ReadCard;
				        BT.println(ReadCard);
					}
			    }
			    else {
			    	// If the read was unsuccessful we give out a bunch of zeroes to the Backend  via BT-Serial to indicate a read error
			    	BT.println("00000000");
			    }
			}
		}
	}
	else{
		
		if (stringComplete) {
			Serial.println(inputString);
			success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

			if (success) {

				Serial.println("Found a Smart Card");
				Serial.println("Chip: ISO 14443A");
				Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
				Serial.print("UID Value: "); nfc.PrintHex(uid, uidLength); Serial.println("");

				if (uidLength == 7) {
					Serial.println("Reading page 4");
					uint8_t data4[32];
					uint8_t data5[32];

					// Write the STANDARD Pages 4 and 5
					memcpy(data4, (const uint8_t[]) reinterpret_cast<const uint8_t*>(&inputString[0]), sizeof data4);
					success = nfc.mifareultralight_WritePage (4, data4);
					memcpy(data5, (const uint8_t[]) reinterpret_cast<const uint8_t*>(&inputString[4]), sizeof data5);
					success = nfc.mifareultralight_WritePage (5, data5);

					// Perform a read and show the user the writen Card ID via the Serial Monitor
					uint8_t data_read_4[32];
					uint8_t data_read_5[32];
					success = nfc.mifareultralight_ReadPage (4, data_read_4);
					success = nfc.mifareultralight_ReadPage (5, data_read_5);
					if (success){
						String ReadBlock_4;
						String ReadBlock_5;
						String ReadCard = "11111111";
						for (char c : data_read_4) ReadBlock_4 += c;
						ReadCard[0] = ReadBlock_4[0];
						ReadCard[1] = ReadBlock_4[1];
						ReadCard[2] = ReadBlock_4[2];
						ReadCard[3] = ReadBlock_4[3];
						Serial.print(ReadCard[0]);
						Serial.print(ReadCard[1]);
						Serial.print(ReadCard[2]);
						Serial.print(ReadCard[3]);
						for (char c : data_read_5) ReadBlock_5 += c;
						ReadCard[4] = ReadBlock_5[0];
						ReadCard[5] = ReadBlock_5[1];
						ReadCard[6] = ReadBlock_5[2];
						ReadCard[7] = ReadBlock_5[3];
						Serial.print(ReadCard[4]);
						Serial.print(ReadCard[5]);
						Serial.print(ReadCard[6]);
						Serial.println(ReadCard[7]);
						// Wait a bit before reading the card again
						delay(1000);
					}
	    			else {
						Serial.println("Problem writing the Smart Card, please try again");
						Serial.println("No need to input the Card ID again");

	    			}
				}
			}
    	}
    	inputString = "";
		stringComplete = false;
  	}	
}

void serialEvent() {
  while (Serial.available()) {
  	// Append input bytes to the input string until completed by a \n
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
