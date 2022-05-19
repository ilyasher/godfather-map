#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "keymap.h"
#include "display.h"
#include "territories.h"
#include "strings.h"

// Pins for the keyboard
const int DATA  = 1;
const int CLOCK = 2;

const int LIGHT_UPDATE_INTERVAL = 100; // in milliseconds

// Display wiring: red 5v, black ground, white data, green clock
Display lcd;

// LED wiring: red 5v, white ground, green data
Territories territories;

//void scan_I2C_devices();

void setup()
{
  Serial.begin(9600);
  territories.init();
  lcd.init();
  lcd.welcome();
  delay(2000);

  // Interrupts necessary for key presses to work
  noInterrupts();

    // Interrupts for the keyboard (the clock pin)
    PCICR |= 0b00000010;   // enable Port C pin change interrupt PCI "PCINT1"
    bitSet(PCMSK1, CLOCK);

    // Internal TIMER0 interrupt
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);

  interrupts();
}

void loop() {

  // Choose between "Get directions" and "Input Keyword"
  int directions_or_codeword = lcd.get_user_choice(retrieve_string(2), retrieve_string(3));

  // Choose between "Team Pacino" and "Team Brando"
  // Note: increment by 1 because team is either 1 or 2
  int team = lcd.get_user_choice(retrieve_string(0), retrieve_string(1)) + 1;

  if (directions_or_codeword == 0)
  {  
    // Want directions
    const char *directions = territories.get_directions(team);

    // Using global buff from strings.h to save on memory
    memset(buff, '\0', sizeof(buff));
    strcat(buff, "Go to ");
    strcat(buff, directions);
    lcd.display_message(buff);
  } else 
  {
    // Want to input codeword

    // Prompt with "Input codeword" and wait for answer
    char *codeword = lcd.get_user_input(retrieve_string(3));
    
    if (territories.guess_codeword(codeword, team)) {
      // Got it right!
      // Display "Correct!"
      lcd.display_message(retrieve_string(4));

      // Display "New territory unlocked!"
      lcd.display_message(retrieve_string(6));
      
    } else {
      // Got it wrong
      // Display "Incorrect, try again or skip"
      lcd.display_message(retrieve_string(5));
      
      // Select between "Keep trying" and "Skip"
      int force_progress = lcd.get_user_choice(retrieve_string(7), retrieve_string(8));
      if (force_progress) {
        // Display "New territory unlocked!"
        lcd.display_message(retrieve_string(6));
        territories.force_progress(team);  
      }
    }
  }
}


/* Interrupt handler for keyboard presses/releases. */
ISR(PCINT1_vect)      // interrupt service routine
{
  int val = 0;
  char key = '\0';
  for(int i=0; i<11; i++)
  {
    while(bitRead(PINC, CLOCK) == true);
    val |= bitRead(PINC, DATA)<<i;
    while(bitRead(PINC, CLOCK) == false);
  }
  val = (val>>1) & 255;

  static bool keyReleased = false;
  switch (val)
  {
    case 224: // sent with up and down arrows for some reason
      break;
    case 240: // key release
      keyReleased = true;
      break;
    default:
      if (!keyReleased) // don't log key releases
      {
        key = access_keymap(val);
      }
      keyReleased = false;
      break;
  }

#if 1
  Serial.print("Key value: ");
  Serial.println(val);
  Serial.print("Key char: ");
  Serial.println(key);
#endif

  lcd.key = key;

  PCIFR = 0x02;                           // clears the PCI flag 1
}

SIGNAL(TIMER0_COMPA_vect) 
{
  unsigned long currentMillis = millis();
  if (currentMillis % LIGHT_UPDATE_INTERVAL == 0) {
      territories.tick_lights();
  }
}
//
//void scan_I2C_devices() {
//  Serial.println ();
//    Serial.println ("I2C scanner. Scanning ...");
//    byte count = 0;
//    Wire.begin();
//    for (byte i = 8; i < 120; i++) {
//      Wire.beginTransmission (i);
//      if (Wire.endTransmission () == 0) {
//        Serial.print ("Found address: ");
//        Serial.print (i, DEC);
//        Serial.print (" (0x");
//        Serial.print (i, HEX);
//        Serial.println (")");
//        count++;
//        delay (1);  // maybe unneeded?
//      } // end of good response
//    } // end of for loop
//    Serial.println ("Done.");
//    Serial.print ("Found ");
//    Serial.print (count, DEC);
//    Serial.println (" device(s).");
//}
