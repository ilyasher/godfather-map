/*
  PROGMEM string demo
  How to store a table of strings in program memory (flash),
  and retrieve them.

  Information summarized from:
  http://www.nongnu.org/avr-libc/user-manual/pgmspace.html

  Setting up a table (array) of strings in program memory is slightly complicated, but
  here is a good template to follow.

  Setting up the strings is a two-step process. First, define the strings.
*/
#ifndef STRINGS_H
#define STRINGS_H

#include <avr/pgmspace.h>
const char string_0[] PROGMEM = "Team Pacino";
const char string_1[] PROGMEM = "Team Brando";
const char string_2[] PROGMEM = "Get directions";
const char string_3[] PROGMEM = "Input codeword";
const char string_4[] PROGMEM = "Correct!";
const char string_5[] PROGMEM = "Incorrect. You may choose to continue to the next location.";
const char string_6[] PROGMEM = "New Territory Unlocked";
const char string_7[] PROGMEM = "Keep trying";
const char string_8[] PROGMEM = "Skip Puzzle";
const char string_9[] PROGMEM = "the Field South of the Gene Pool";
const char string_10[] PROGMEM = "the Wedding Cake";
const char string_11[] PROGMEM = "Fear";
const char string_12[] PROGMEM = "the Einstein Casino in the Ath";
const char string_13[] PROGMEM = "Annenberg 2nd floor Lounge";
const char string_14[] PROGMEM = "Bridge 210";

// Then set up a table to refer to your strings.
const char *const string_table[] PROGMEM = {string_0, string_1, string_2, string_3, string_4, string_5,
  string_6, string_7, string_8, string_9, string_10, string_11, string_12, string_13, string_14};

char buff[64];  // make sure this is large enough for the largest string it must hold

char *retrieve_string(int idx) {
    /* Using the string table in program memory requires the use of special functions to retrieve the data.
     The strcpy_P function copies a string from program space to a string in RAM ("buffer").
     Make sure your receiving string in RAM is large enough to hold whatever
     you are retrieving from program space. */

    memset(buff, '\0', sizeof(buff));
    strcpy_P(buff, (char *)pgm_read_word(&(string_table[idx])));  // Necessary casts and dereferencing, just copy.
    return buff;
}

#endif // STRINGS_H
