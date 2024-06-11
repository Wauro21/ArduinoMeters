/*
* Arduino Voltimeter
*
* Author: Mauricio Aravena (maurio.aravena@sansano.usm.cl)
* 10/06/202
* 
* Read an analog voltage and displays it on the LCD.
*/

#include <LCD-I2C.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"


#define VOLTAGE_MAX 5
#define ADC_RESOLUTION 10
#define DECIMAL_RESOLUTION 2
#define LCD_VOLTAGE_COLUMN 16-(2+DECIMAL_RESOLUTION) // Place before point and point character = 2 -> Total = 2 + decimal places
// Configure the LCD using the default address 0x27
LCD_I2C lcd(0x27, 16, 2);

// Set pin A0 as the input
const int analog_in = A0;

// Default values
int read_val = 0;
float converted_val = 0.0;
long talk_val = 0;

// Talkie configurations
Talkie voice;
void sayNumber(long n);

void talkVoltage(float value);

void sayRegular(int ten);

void sayIrregular(int ten);

void setup() {

  // Setup the LCD according to the library
  lcd.begin();
  lcd.display();
  lcd.backlight();
  // DEBUG
  Serial.begin(115200);
  delay(4000);
  Serial.println("Working demo");

}

void loop() {

  // Read the value of pin analog_in
  read_val = analogRead(analog_in);

  // Convert the raw value to voltage
  converted_val = (float)(read_val*VOLTAGE_MAX)/(pow(2,ADC_RESOLUTION)-1);

  // Display on the LCD
  lcd.print("Voltage:");
  lcd.setCursor(LCD_VOLTAGE_COLUMN, 1);
  lcd.print(converted_val,DECIMAL_RESOLUTION);

  // TTS:
  //sayNumber(converted_val);
  talkVoltage(converted_val);

  delay(2000);
  lcd.clear();

}


void talkVoltage(float n)
{

  // Get integral part
  double i = 0;
  double f = modf(n, &i);
  int si = round(i);
  int sf = round(f*100);

  //Check correct representation of integral part
  if(sf == 100)
  {
    si += 1;
    sf = 0;
  }

  // Read integral part
  sayIrregular(si);

  // Dot for separation
  voice.say(sp3_POINT);

  // Read floating part
  if(sf < 30)
  {
    sayIrregular(sf);
  }

  else 
  {
    int ten = (sf/10)%10;
    int units = sf%10;
    sayRegular(ten);
    if(units != 0)
    {
      // voice.say(sp2_AND); // Commented for english version
      sayIrregular(units);
    }

  }
}

// Layout this way with spanish translation in mind
void sayRegular(int ten)
{
  switch(ten)
  {
    case 3:
      voice.say(sp2_THIR_);
      voice.say(sp2_T);
      break;
    case 4:
      voice.say(sp2_FOUR);
      voice.say(sp2_T);
      break;
    case 5:
      voice.say(sp2_FIF_);
      voice.say(sp2_T);
      break;
    case 6:
      voice.say(sp2_SIX);
      voice.say(sp2_T);
      break;
    case 7:
      voice.say(sp2_SEVEN);
      voice.say(sp2_T);
      break;
    case 8:
      voice.say(sp2_EIGHT);
      voice.say(sp2_T);
      break;
    case 9:
      voice.say(sp2_NINE);
      voice.say(sp2_T);
      break;
  }
}

void sayIrregular(int ten)
{
  switch(ten)
  {
    case 1:
      voice.say(sp2_ONE);
      break;

    case 2:
      voice.say(sp2_TWO);
      break;

    case 3:
      voice.say(sp2_THREE);
      break;

    case 4:
      voice.say(sp2_FOUR);
      break;

    case 5:
      voice.say(sp2_FIVE);
      break;

    case 6:
      voice.say(sp2_SIX);
      break;

    case 7:
      voice.say(sp2_SEVEN);
      break;

    case 8:
      voice.say(sp2_EIGHT);
      break;

    case 9:
      voice.say(sp2_NINE);
      break;

    case 10:
      voice.say(sp2_TEN);
      break;
    
    case 11:
      voice.say(sp2_ELEVEN);
      break;

    case 12:
      voice.say(sp2_TWELVE);
      break;

    case 13:
      voice.say(sp2_THIR_);
      voice.say(sp2__TEEN);
      break;

    case 14:
      voice.say(sp2_FOUR);
      voice.say(sp2__TEEN);
      break;

    case 15:
      voice.say(sp2_FIF_);
      voice.say(sp2__TEEN);
      break;

    case 16:
      voice.say(sp2_SIX);
      voice.say(sp2__TEEN);
      break;
    
    case 17:
      voice.say(sp2_SEVEN);
      voice.say(sp2__TEEN);
      break;
    
    case 18:
      voice.say(sp2_EIGHT);
      voice.say(sp2__TEEN);
      break;
    
    case 19:
      voice.say(sp2_NINE);
      voice.say(sp2__TEEN);
      break;

    case 20:
      voice.say(sp2_TWENTY);
      break;

    case 21:
      voice.say(sp2_TWENTY);
      voice.say(sp2_ONE);
      break;

    case 22:
      voice.say(sp2_TWENTY);
      voice.say(sp2_TWO);
      break;

    case 23:
      voice.say(sp2_TWENTY);
      voice.say(sp2_THREE);
      break;

    case 24:
      voice.say(sp2_TWENTY);
      voice.say(sp2_FOUR);
      break;

    case 25:
      voice.say(sp2_TWENTY);
      voice.say(sp2_FIVE);
      break;

    case 26:
      voice.say(sp2_TWENTY);
      voice.say(sp2_SIX);
      break;

    case 27:
      voice.say(sp2_TWENTY);
      voice.say(sp2_SEVEN);
      break;

    case 28:
      voice.say(sp2_TWENTY);
      voice.say(sp2_EIGHT);
      break;

    case 29:
      voice.say(sp2_TWENTY);
      voice.say(sp2_NINE);
      break;

  }
}

