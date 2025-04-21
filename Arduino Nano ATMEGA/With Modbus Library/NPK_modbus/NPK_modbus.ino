// Attempt to access a JXCT NPK sensor to read Nitrogen, Potassium & Phosphorus
// values using an Arduino UNO clone.
//
// This attempt uses the AltSoftSerial & ModbusMaster libraries. Get the libraries
// via the Arduino IDE or manually:
// Get AltSoftSerial at https://github.com/PaulStoffregen/AltSoftSerial
// Get ModbusMaster  at https://github.com/4-20ma/ModbusMaster
//
// RS485 module connected to Arduino UNO as follows:
// RS485 DI signal to pin 9
// RS485 RO signal to pin 8
// RS485 RE signal to pin 7
// RS485 DE signal to pin 6
// RS485 VCC to 5V
// RS485 GND to GND
//
// Assumes that the sensor address is 1, Nitrogen is address 0x1E, Phosphorus is address 0x1F
// and Potassium is address 0x20. Change the code if yours are different.
//
// NOTE: I do not have this sensor, so I simulated it using ModRSsim2 available from https://sourceforge.net/projects/modrssim2/
// 

//https://forum.arduino.cc/t/npk-sensor-keeps-returning-255/1113058/12


#include <ModbusMaster.h>
#include <AltSoftSerial.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define NITROGEN_ADDR   0x1E
#define PHOSPHORUS_ADDR 0x1F
#define POTASSIUM_ADDR  0x20

#define MAX485_DE      6
#define MAX485_RE_NEG  7

int npk_ratio[] = { 17,   //N 
                    17,   //P
                    17    //K
                    };
//https://rema.gov.rw/fileadmin/templates/Documents/rema_doc/publications/Fertilizer-Report.pdf

AltSoftSerial swSerial;
ModbusMaster node;

// Put the MAX485 into transmit mode
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

// Put the MAX485 into receive mode
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  Serial.begin( 9600 );
  lcd.begin(16, 2);  // Initialize the LCD: 16 columns, 2 rows

  // configure the MAX485 RE & DE control signals and enable receive mode
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 9600 baud
  swSerial.begin(9600);

  // Modbus slave ID of NPK sensor is 1
  node.begin(1, swSerial);

  // Callbacks to allow us to set the RS485 Tx/Rx direction
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //ph.begin();

  //Serial.println("initializing ph sensor");

}
// Function to determine the recommended action based on the actual NPK ratio
String recommended_action(int nitrogen, int phosphorous, int potassium) {
    String recommendation;

    if (nitrogen < npk_ratio[0]) {
        recommendation += "Add nitrogen. ";
    } 
    
    if (phosphorous < npk_ratio[1]) {
        recommendation += "Add phosphorus. ";
    }
    
    if (potassium < npk_ratio[2]) {
        recommendation += "Add potassium. ";
    }

    // If no action needed, set recommendation accordingly
    if (recommendation.length() == 0) {
        recommendation = "No action needed.";
    }
    
    return recommendation;
}


void loop() {

  uint8_t result;

  int nitrogen, phosphorous, potassium;

  lcd.clear();  // Clear the LCD screen

 
  lcd.setCursor(0, 0);
  
  // NITROGEN
  result = node.readHoldingRegisters(NITROGEN_ADDR, 1);
  lcd.print("N:");
  if (result == node.ku8MBSuccess)
  {
    Serial.print("   Nitrogen: ");
    Serial.print(node.getResponseBuffer(0x0));
    nitrogen = node.getResponseBuffer(0x0);
    Serial.println(" mg/kg");
      // Set the cursor to column 0, row 0 (first line)
  
    lcd.print(nitrogen);
  } else {
    printModbusError( result );
    lcd.print(result );
  }

  // PHOSPHORUS
  result = node.readHoldingRegisters(PHOSPHORUS_ADDR, 1);
  lcd.print("P:");
  if (result == node.ku8MBSuccess)
  {
    Serial.print("Phosphorous: ");
    Serial.print(node.getResponseBuffer(0x0));
    phosphorous = node.getResponseBuffer(0x0);
    Serial.println(" mg/kg");
    lcd.print(phosphorous );
  } else {
    printModbusError( result );
    lcd.print(result );
  }

  // POTASSIUM
  result = node.readHoldingRegisters(POTASSIUM_ADDR, 1);
  lcd.print("K:");
  if (result == node.ku8MBSuccess)
  {
    Serial.print("  Potassium: ");
    Serial.print(node.getResponseBuffer(0x0));
    potassium = node.getResponseBuffer(0x0);
    Serial.println(" mg/kg");
    lcd.print(potassium);
  } else {
    printModbusError( result );
    lcd.print(result );
    
  }
  Serial.println();
    // Determine if adjustments are needed based on the actual NPK ratio
  String recommendation = recommended_action(nitrogen, phosphorous, potassium);

  // Print the recommended action
  Serial.println("Recommended Action: " + recommendation);

  delay(2000); 

  lcd.clear(); 

    // Set the cursor to column 0, row 1 (second line)
  lcd.setCursor(0, 0);
  lcd.print("Recommended Action:");
  lcd.setCursor(0, 1);
  lcd.print(recommendation);
  
  //display.display();
  delay(2000);


}

// print out the error received from the Modbus library
void printModbusError( uint8_t errNum )
{
  switch ( errNum ) {
    case node.ku8MBSuccess:
      Serial.println(F("Success"));
      break;
    case node.ku8MBIllegalFunction:
      Serial.println(F("Illegal Function Exception"));
      break;
    case node.ku8MBIllegalDataAddress:
      Serial.println(F("Illegal Data Address Exception"));
      break;
    case node.ku8MBIllegalDataValue:
      Serial.println(F("Illegal Data Value Exception"));
      break;
    case node.ku8MBSlaveDeviceFailure:
      Serial.println(F("Slave Device Failure"));
      break;
    case node.ku8MBInvalidSlaveID:
      Serial.println(F("Invalid Slave ID"));
      break;
    case node.ku8MBInvalidFunction:
      Serial.println(F("Invalid Function"));
      break;
    case node.ku8MBResponseTimedOut:
      Serial.println(F("Response Timed Out"));
      break;
    case node.ku8MBInvalidCRC:
      Serial.println(F("Invalid CRC"));
      break;
    default:
      Serial.println(F("Unknown Error"));
      break;
  }
}


