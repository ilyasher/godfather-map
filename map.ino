#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "keymap.h"
#include "display.h"
#include "territories.h"

const int DATA  = 1;
const int CLOCK = 2;

// saves on memory by declaring strings here
const char *team1_name = "Team Pacino";
const char *team2_name = "Team Pacino";
const char *directions_choice = "Get directions";
const char *input_codeword_choice = "Input codeword";
const char *message_correct = "Correct!";
const char *message_incorrect = "Incorrect. You may choose to continue to the next location.";

// red 5v, black ground, white data, green clock
Display lcd;

// red 5v, white ground, green data
Territories territories;

void scan_I2C_devices();

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

  int directions_or_codeword = lcd.get_user_choice(directions_choice, input_codeword_choice);
  int team = lcd.get_user_choice(team1_name, team2_name);

  char msg[20] = {'\0'};

  if (directions_or_codeword == 0)
  {  
    // Want directions
    const char *directions = territories.get_directions(team);
    strcat(msg, "Go to ");
    strcat(msg, directions);
    lcd.display_message(msg);
  } else 
  {
    // Want to input codeword
    char *codeword = lcd.get_user_input(input_codeword_choice);
    if (territories.guess_codeword(codeword, team)) {
      // Got it right!
      lcd.display_message(message_correct);
    } else {
      // Got it wrong
      lcd.display_message(message_incorrect);
      
      // Maybe we want to give them the option to skip anyway
      int force_progress = lcd.get_user_choice("Keep trying", "Skip puzzle");
      if (force_progress) {
        lcd.display_message("New territory unlocked");
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
  Serial.println(val);
  Serial.println(key);
  lcd.key = key;

  PCIFR = 0x02;                           // clears the PCI flag 1
}

SIGNAL(TIMER0_COMPA_vect) 
{
  unsigned long currentMillis = millis();
  if (currentMillis % 100 == 0) {
      territories.tick_lights();
  }

}

void scan_I2C_devices() {
  Serial.println ();
    Serial.println ("I2C scanner. Scanning ...");
    byte count = 0;
    Wire.begin();
    for (byte i = 8; i < 120; i++) {
      Wire.beginTransmission (i);
      if (Wire.endTransmission () == 0) {
        Serial.print ("Found address: ");
        Serial.print (i, DEC);
        Serial.print (" (0x");
        Serial.print (i, HEX);
        Serial.println (")");
        count++;
        delay (1);  // maybe unneeded?
      } // end of good response
    } // end of for loop
    Serial.println ("Done.");
    Serial.print ("Found ");
    Serial.print (count, DEC);
    Serial.println (" device(s).");
}
