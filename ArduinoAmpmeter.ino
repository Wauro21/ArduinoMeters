/*
 * Arduino Ampmeter
 *
 * Author: Mauricio Aravena (maurio.aravena@sansano.usm.cl)
 * 10/06/202
 *
 * Read an analog voltage on a resistance an get the current value and displays it on the LCD.
 */

#include <LCD-I2C.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

// ADC DEFINITIONS
#define VOLTAGE_MAX 5
#define ADC_RESOLUTION 10
#define DECIMAL_RESOLUTION 0
#define ADC_INPUT_A A0
#define ADC_INPUT_B A1
// CURRENT FACTORS
#define RESISTANCE_VALUE 10 // 10 ohm resistance
#define DISPLAY_FACTOR 1000 // Show mA
// LCD DEFINITIONS
#define LCD_VOLTAGE_COLUMN 9
// DEBOUNCE DEFINITIONS
#define INPUT_BUTTON_PIN 2 // This allows the use of interruptions
#define PRESS_BUTTON_MASK 0x80000000
#define HOLD_BUTTON_MASK 0xFFFFFFFF

// Configure the LCD using the default address 0x27. 16 columns and 2 rows
LCD_I2C lcd(0x27, 16, 2);

// Talkie configurations
Talkie voice;

// Button debounce state variables
int button_state = 0;
uint32_t button_debounce = 0U;

// Default values for voltage reading
int read_val_a = 0;
int read_val_b = 0;
int adc_difference = 0;
bool val_sign = false; // False if positive
float converted_val = 0.0;
long talk_val = 0;

// ----------------- FUNCTIONS ----------------- //

/// @brief Converts the readed voltage into speech. i.e. 2.03 V -> Two point zero three volts
/// @param value The voltage value obtained from the conversion of the ADC raw value
/// @param sign A boolean representing if the value if negative (true) or positive (false)
void talkVoltage(float value, bool sign);

/// @brief Read the provided number via the speaker
/// @param n Number to be readed
void sayNumber(long n);

/// @brief Read and performs software debounce the input button. For this function to work no delay operation must be performed on the loop
/// @param state The current state of the debouncing operation
/// @return Returns the state of the button. 1 for simple push and 2 for hold. Zero in any other case
int readButtonDebounce(uint32_t *state);

// ----------------- SETUP ----------------- //

void setup()
{
  // Setup the LCD according to the library
  lcd.begin();
  lcd.display();
  lcd.backlight();

  // Setup pin for button input
  pinMode(INPUT_BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
  // Read the value of the analog pins
  read_val_a = analogRead(ADC_INPUT_A);
  read_val_b = analogRead(ADC_INPUT_B);

  // Perform the calculation for the voltage across terminals
  adc_difference = read_val_a - read_val_b;

  // Check the difference sign
  if (adc_difference < 0)
  {
    val_sign = true;
  }

  else
  {
    val_sign = false;
  }

  // Get absolute value
  adc_difference = abs(adc_difference);

  // Convert the raw value to voltage
  converted_val = (float)(adc_difference * VOLTAGE_MAX) / (pow(2, ADC_RESOLUTION) - 1);

  // Adapt for resistance
  converted_val = converted_val * DISPLAY_FACTOR / RESISTANCE_VALUE;

  // Display on the LCD
  lcd.print("Current:");
  // ->Display sign
  lcd.setCursor(LCD_VOLTAGE_COLUMN - 1, 1);
  if (val_sign)
  {
    lcd.print('-');
  }
  else
  {
    lcd.print(' ');
  }
  // -> Display value
  lcd.setCursor(LCD_VOLTAGE_COLUMN, 1);
  lcd.print(converted_val, DECIMAL_RESOLUTION);
  lcd.print("mA");

  // Check button state
  button_state = readButtonDebounce(&button_debounce);

  // Only if button is pressed do something
  if (button_state == 1)
  {
    if (val_sign)
    {
      voice.say(sp3_MINUS);
    }

    // talkVoltage(converted_val, val_sign);
    sayNumber((long)converted_val);

    // Say Unit
    voice.say(sp2_MILLI);
    voice.say(sp2_AMPS);
  }

  lcd.clear(); // Comented in this case. If not the screen is faslty reset and displays errors.
}

// ----------------- FUNCTIONS DEFINITIONS ----------------- //

void talkVoltage(float n, bool sign)
{

  // Get integral part
  double i = 0;
  double f = modf(n, &i);
  int si = round(i);
  int sf = round(f * 100);

  // Check correct representation of integral part
  if (sf == 100)
  {
    si += 1;
    sf = 0;
  }

  // Read the sign
  if (sign)
  {
    voice.say(sp3_MINUS);
  }

  // Read integral part
  sayIrregular(si);

  // Dot for separation
  voice.say(sp3_POINT);

  // Read floating part
  int ten = (sf / 10) % 10;
  int units = sf % 10;
  if (sf < 30)
  {
    if (ten == 0)
    {
      voice.say(sp2_ZERO);
    }
    sayIrregular(sf);
  }

  else
  {
    sayRegular(ten);
    if (units != 0)
    {
      // voice.say(sp2_AND); // Commented for english version
      sayIrregular(units);
    }
  }

  // Finally say the units of the measurement
  voice.say(sp2_AMPS);
}

void sayNumber(long n)
{
  if (n < 0)
  {
    voice.say(sp2_MINUS);
    sayNumber(-n);
  }
  else if (n == 0)
  {
    voice.say(sp2_ZERO);
  }
  else
  {
    if (n >= 1000)
    {
      int thousands = n / 1000;
      sayNumber(thousands);
      voice.say(sp2_THOUSAND);
      n %= 1000;
      if ((n > 0) && (n < 100))
        voice.say(sp2_AND);
    }
    if (n >= 100)
    {
      int hundreds = n / 100;
      sayNumber(hundreds);
      voice.say(sp2_HUNDRED);
      n %= 100;
      if (n > 0)
        voice.say(sp2_AND);
    }
    if (n > 19)
    {
      int tens = n / 10;
      switch (tens)
      {
      case 2:
        voice.say(sp2_TWENTY);
        break;
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
      n %= 10;
    }
    switch (n)
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
    }
  }
}

int readButtonDebounce(uint32_t *state)
{
  // Read the current state of the button
  uint8_t temporal_read = (uint8_t)digitalRead(INPUT_BUTTON_PIN);
  uint8_t read_value = (uint8_t)(~temporal_read & bit(INPUT_BUTTON_PIN) >> INPUT_BUTTON_PIN); // Negative logic --> when button pressed is low, resulting = 1111_1111 | for high = 0000_0001
  // Debounce the value
  *state = (*state << 1) | read_value;

  // Check current state
  switch (*state)
  {
  case PRESS_BUTTON_MASK:
    return 1;
  case HOLD_BUTTON_MASK:
    return 2;
  default:
    return 0;
  }
}