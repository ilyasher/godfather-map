#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "keymap.h"
#include "display.h"
#include "territories.h"
#include "strings.h"

// Pins for the keyboard
const int DATA  = 1;
const int CLOCK = 2;

const int LIGHT_UPDATE_INTERVAL = 200; // in milliseconds

// Display wiring: red 5v, black ground, white data, green clock
Display lcd;

// LED wiring: red 5v, white ground, green data
Territories territories;

void setup()
{
  Serial.begin(115200);
  territories.init();
  lcd.init();
  
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
  int directions_or_codeword = lcd.get_user_choice(2, 3);

  // Choose between "Team Pacino" and "Team Brando"
  // Note: increment by 1 because team is either 1 or 2
  int team = lcd.get_user_choice(0, 1) + 1;

  Serial.print(directions_or_codeword);
  Serial.println(team);

  if (directions_or_codeword == 0)
  {  
    // Want directions
//    STR_IDX_T directions = territories.get_directions(team);
//    Serial.println("Got here");

    // Using global buff from strings.h to save on memory
//    memset(buff, '\0', sizeof(buff));
//    strcat(buff, "Go to ");
//    strcat(buff, retrieve_string(0));
//    strcat(buff, retrieve_string(directions));
    lcd.display_message_buffer(retrieve_string(territories.get_directions(team)));
  } else 
  {
    // Want to input codeword

    // Prompt with "Input codeword" and wait for answer
    char *codeword = lcd.get_user_input(3);
    
    if (territories.guess_codeword(codeword, team)) {
      // Got it right!
      // Display "Correct!"
      lcd.display_message(4);

      // Display "New territory unlocked!"
      lcd.display_message(6);
      
    } else {
      // Got it wrong
      // Display "Incorrect, try again or skip"
      lcd.display_message(5);
      
      // Select between "Keep trying" and "Skip"
      int force_progress = lcd.get_user_choice(7, 8);
      if (force_progress) {
        // Display "New territory unlocked!"
        lcd.display_message(6);
        territories.force_progress(team);  
      }
    }
  }
}
//

/* Interrupt handler for keyboard presses/releases. */

ISR(PCINT1_vect)      // interrupt service routine
{
noInterrupts();
//  static unsigned long last_keypress_time = 0;

//  unsigned long ms = millis();
//  Serial.println(ms);
//  Serial.println(last_keypress_time);
//  if (ms - last_keypress_time < 10) {
////interrupts();
//    return;
//  }

  int val = 0;
  char key = '\0';
  for(int i=0; i<11; i++)
  {
    while(bitRead(PINC, CLOCK) == true);
    val |= bitRead(PINC, DATA)<<i;
    while(bitRead(PINC, CLOCK) == false);
    Serial.print(i);
  }
  val = (val>>1) & 255;
  Serial.println();

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
//        key = access_keymap(val);
        key = keymap[val];
      }
      keyReleased = false;
      break;
  }

  lcd.key = key;

//#if 0
//  Serial.print("KV: ");
//  Serial.println(val);
//  Serial.print("KC: ");
//  Serial.println(key);
//#endif

//  last_keypress_time = ms;

  PCIFR = 0x02;                           // clears the PCI flag 1
interrupts();
}

SIGNAL(TIMER0_COMPA_vect) 
{
noInterrupts();
  unsigned long currentMillis = millis();
  if (currentMillis % LIGHT_UPDATE_INTERVAL == 0) {
      territories.tick_lights();
  }
interrupts();
}
