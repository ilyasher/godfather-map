#pragma once

const char KEY_ENTER = '\n';
const char KEY_UP = '>';
const char KEY_DOWN = '<';
const char KEY_BACKSPACE = 127;

const char keymap[] = {'q','1', 0,
  0, 0, 'z','s','a','w','2', 0,
  0,'c','x','d','e','4','3', 0, // 40
  0,' ','v','f','t','r','5', 0,
  0,'n','b','h','g','y','6', 0,
  0, 0, 'm','j','u','7','8', 0,
  0,',','k','i','o','0','9', 0,
  0,0,0,'l',0,'p',0, 0 }; // 80

// doing it this way allows the keymap to be smaller,
// saving memory.
char access_keymap(int idx) {
  if (idx == 117) {
      return KEY_UP;
  } else if (idx == 114) {
      return KEY_DOWN;
  } else if (idx == 102) {
      return 127;
  } else if (idx == 90) {
      return KEY_ENTER;
  }
  if (idx <= 20 || idx >= 80) {
      return '\0';
  }
  return keymap[idx+21];
}

/*
 * const char keymap[] = {
  0, 0,  0,  0,  0,  0,  0,  0,
  0, 0,  0,  0,  0,  0, 0, 0,
  0, 0 , 0 , 0,  0, 'q','1', 0,
  0, 0, 'z','s','a','w','2', 0,
  0,'c','x','d','e','4','3', 0, // 40
  0,' ','v','f','t','r','5', 0,
  0,'n','b','h','g','y','6', 0,
  0, 0, 'm','j','u','7','8', 0,
  0,',','k','i','o','0','9', 0,
  0,0,0,'l',0,'p',0, 0, // 80
  0, 0,0,0,0,0, 0, 0,
  0, 0, KEY_ENTER, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 127, 0,
  0,0, 0,0,0, 0, 0, 0,
  0,0,KEY_DOWN,0,0,KEY_UP, 0, 0, // 120
  0,0,0,0,0,0, 0, 0,
  0, 0, 0, 0 };
*/
