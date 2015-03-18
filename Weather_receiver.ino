

/*
 AUTHOR: Stephane Nadeau
 VERSION: 3
 DATE: 8 March 2015
 V3
   - Fix the position of kpa
 V2
   - Second line of the display is now a fix 20 chars, fix many small interface bug.
   - Version added
 V1
   - Fix error with outside temperature, a negative sign was visible when the temp is positive.
 
*/


//*-----( Import needed libraries )-----*/
#include <Wire.h>
#include <VirtualWire.h>
#include <VirtualWire_Config.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>


/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


/*-----( Declare Variables )-----*/
unsigned long previousMillis = 0; // last time update
long interval = 19000; // interval at which to do show a communication error (milliseconds)
int HumiditySensor;
int TempSensor1;
int TempSensor2;
int PressionSensor;
int analoglm35;
int addr = 0;
int Version = 3;
String DataString;

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);  // Used to type in characters
  Clear_eeprom(); // clearing the eeprom memory with all 0
  EEPROM.write(0,Version);
  Wire.begin();

  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines and turn on backlight
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
//  lcd.setCursor(0,0);
//  lcd.print("Initialisation ...");
// setup LM35
  

}/*--(end setup )---*/

void loop()
{
  unsigned long currentMillis = millis();
  char StringReceived[22];
  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
   
    for (i = 0; i < buflen; i++)
    {
      Serial.print(char(buf[i]));
      StringReceived[i] = char(buf[i]);
     // lcd.print(char(buf[i]));
    }
    sscanf(StringReceived, "%i,%i,%i,%i",&HumiditySensor, &TempSensor1,&TempSensor2,&PressionSensor); // Converts a string to an array
    memset( StringReceived, 0, sizeof( StringReceived));
    
    int analoglm35 = analogRead(A0);
    float TempInside = (analoglm35*0.0048)/0.01;
    Serial.println(TempInside);
    lcd.setCursor(0,0);
    lcd.print(" int    ext   Hum(%)");
   
    
    lcd.setCursor(0,1);
    char string[20];
    lcd.print(TempInside);
    
    lcd.setCursor(8,1);
    lcd.print(TempSensor1);
    lcd.print(".");
    if (TempSensor2 < 0){
      TempSensor2 = TempSensor2 * -1;
    }
    else{
      if (TempSensor2 < 10){
        lcd.print(0);
      }
    }
    lcd.print(TempSensor2);
    lcd.setCursor(15,1);
    lcd.print(HumiditySensor);
    
    double PressionSensor2 = PressionSensor * 0.1;
    lcd.setCursor(0,3);
    lcd.print("P:");
    lcd.print(PressionSensor2);
    lcd.setCursor(9,3);
    lcd.print("kpa");
    lcd.setCursor(17,3);
    lcd.print("V");
    lcd.print(EEPROM.read(0));
    Serial.println("");
    
    previousMillis = currentMillis;
  }
  else
  
  
  if(currentMillis - previousMillis > interval) {
    // previousMillis = currentMillis; 
      Serial.println("Comm error");
      lcd.setCursor(0,4);
      lcd.print("Communication Error ");
      lcd.setCursor(0,1);
      lcd.print("                    ");
      lcd.setCursor(0,2);
      lcd.print("                    ");
      lcd.setCursor(0,3);
      lcd.print("                    ");
  }
    
}

void Clear_eeprom(){
  int addr = 0;
  
  while (addr < 512){
    EEPROM.write(addr,0);
    addr++;
  }

}

void insertPression(int pression){
  EEPROM.write(0, pression);
  
}



