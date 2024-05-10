#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#define RE 7
#define DE 6

const uint32_t TIMEOUT = 500UL;

//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
SoftwareSerial mod(8, 9); // Rx pin, Tx pin

int npk_ratio[] = { 17,   //N 
                    17,   //P
                    17    //K
                    };

void setup() {
  Serial.begin( 9600 );
  lcd.begin(16, 2);  // Initialize the LCD: 16 columns, 2 rows
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
    String recommendation ="add ";

    // Calculate the ratios of the sensor readings
    float ratio_N = nitrogen / npk_ratio[0];
    float ratio_P = phosphorous / npk_ratio[1];
    float ratio_K = potassium / npk_ratio[2];
    

    if (ratio_N < 0.9 || ratio_N > 1.1 || ratio_P < 0.9 || ratio_P > 1.1 || ratio_K < 0.9 || ratio_K > 1.1) {
        // Print the recommendation based on which ratio is off
        if (ratio_N < 0.9 || ratio_N > 1.1) {
          recommendation += " Urea,";
        }
        if (ratio_P < 0.9 || ratio_P > 1.1) {
            recommendation += " DAP,";
        }
        if (ratio_K < 0.9 || ratio_K > 1.1) {
            recommendation += " Potassium Sulphate.";
        }
    } else {
        recommendation += "Nothing";
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

  lcd.clear();  // Clear the LCD screen
  
  // Set the cursor to column 0, row 0 (first line)
  lcd.setCursor(0, 0);
  lcd.print("N:");
  lcd.print(val1);
  lcd.print("P:");
  lcd.print(val2);
  lcd.print("K:");
  lcd.print(val3);
  delay(4000); 

  lcd.clear(); 

  // Set the cursor to column 0, row 1 (second line)
  lcd.setCursor(0, 0);
  lcd.print("Recommended Action:");
  lcd.setCursor(0, 1);
  lcd.print(recommendation);
    // Check if the message is too long to fit on one screen
  if (recommendation.length() > 16) {
    // If the message is too long, scroll it
    for (int i = 0; i < recommendation.length() - 15; i++) {
      lcd.scrollDisplayLeft();
      delay(500);
    }
  }
  delay(4000);  // Wait for 4 seconds before clearing the screen again

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