#include <SoftwareSerial.h>
#include <Wire.h>

#define RE 7
#define DE 6
#define RO 8
#define D1 9

const uint32_t TIMEOUT = 500UL;

//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
SoftwareSerial mod(RO, D1); // Rx pin, Tx pin

int npk_ratio[] = { 17,   //N 
                    17,   //P
                    17    //K
                    };

void setup() {
  Serial.begin( 9600 );
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  delay(500);

  Serial.println("works");
  delay(3000);

  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);



  delay(500);
}

// Function to determine the recommended action based on the actual NPK ratio
String recommended_action(int nitrogen, int phosphorous, int potassium) {
  
  String recommendation = "";

  for (int i = 0; i < 3; i++) {
    if (i == 0) {
        if (nitrogen < npk_ratio[0]) {
            recommendation += "Add nitrogen fertilizer. ";
        } else {
            recommendation += "Nitrogen is balanced. ";
        }
    } else if (i == 1) {
        if (phosphorous < npk_ratio[1]) {
            recommendation += "Add phosphorus fertilizer. ";
        } else {
            recommendation += "Phosphorus is balanced. ";
        }
    } else if (i == 2) {
        if (potassium < npk_ratio[2]) {
            recommendation += "Add potassium fertilizer. ";
        } else {
            recommendation += "Potassium is balanced. ";
        }
    }
  }
  return recommendation;

}


void loop() {
  byte val1, val2, val3;

  Serial.print("Nitrogen: ");
  val1 = nitrogen();
  Serial.print(val1);
  Serial.print(" mg/kg\n\n");
  delay(1000);

  Serial.print("Phosphorous: ");
  val2 = phosphorous();
  Serial.print(val2);
  Serial.print(" mg/kg\n\n");
  delay(1000);

  Serial.print("Potassium: ");
  val3 = potassium();
  delay(1000);
  Serial.print(val3);
  Serial.print(" mg/kg\n\n");

  delay(5000);

      // Determine if adjustments are needed based on the actual NPK ratio
  String recommendation = recommended_action(val1, val2, val3);

  // Print the recommended action
  Serial.println("Recommended Action: " + recommendation);

}

byte nitrogen() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  mod.write(nitro, sizeof(nitro));
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
      printHexByte(values[byteCount-1]);
    }
  }
  Serial.println();
  return values[4];
}

byte phosphorous() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  mod.write(phos, sizeof(phos));
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
      printHexByte(values[byteCount-1]);
    }
  }
  Serial.println();
  return values[4];
}

byte potassium() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  mod.write(pota, sizeof(pota));
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
      printHexByte(values[byteCount-1]);
    }
  }
  Serial.println();
  return values[4];
}

void printHexByte(byte b)
{
  Serial.print((b >> 4) & 0xF, HEX);
  Serial.print(b & 0xF, HEX);
  Serial.print(' ');
}
