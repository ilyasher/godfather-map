#pragma once

#include "keymap.h"

class Display {
public:

    // volatile because it is modified by the 
    // keypress interrupt service routine
    volatile char key = '\0';

    Display() {}

    /* Call once in setup() */
    void init() {
        lcd.init();
        lcd.backlight();
    }

    /* Display welcome message. */
    void welcome() {
        clear_screen();
        lcd.setCursor(4,0);
        lcd.print("Welcome");
    }

    /* Prompt user to choose between choice1 and choice2,
     * and wait until the Enter key is pressed. 
     * Note: choices are cut off after 14 chars. */
    int get_user_choice(const char *choice1, const char *choice2) {
        clear_screen();
        lcd.print("> ");
        lcd.print(choice1);
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.print(choice2);
        
        int choice = 0;
        while (key != KEY_ENTER) {
            if (key == KEY_DOWN) {
                lcd.setCursor(0,0);
                lcd.print(' ');
                lcd.setCursor(0,1);
                lcd.print('>');
                choice = 1;
                key = '\0';
            } else if (key == KEY_UP) {
                lcd.setCursor(0,0);
                lcd.print('>');
                lcd.setCursor(0,1);
                lcd.print(' ');
                choice = 0;
                key = '\0';
            }
        }
        key = '\0';
        return choice;
    }

    /* Display 'question' on the first line, and prompt a user
     * to enter text on the second line. Respond to keyboard inputs
     * until the Enter key is pressed, after which the inputted word
     * is returned. */
    char *get_user_input(const char *question) {
        clear_screen();
        lcd.print(question);
        lcd.setCursor(0,1);
        lcd.print(prompt);
        lcd.cursor();
        col_index = prompt_length;
        memset(curr_word, '\0', sizeof(curr_word));
        char c;
        while (key != KEY_ENTER) {
            switch (key) {
                case '\0': break;
                case KEY_ENTER: break;
                case KEY_BACKSPACE:
                    key = '\0';
                    if (col_index <= prompt_length) {
                        break;
                    }
                    col_index--;
                    lcd.setCursor(col_index, 1);
                    lcd.print(' ');
                    lcd.setCursor(col_index, 1);
                    curr_word[col_index-prompt_length+1] = '\0';
                    break;
                default:
                    c = key;
                    key = '\0';
                    if (col_index >= num_cols) {
                        break;
                    }
                    // Convert to uppercase
                    if ('a' <= c && c <= 'z') {
                        c -= 'a' - 'A';
                    }
                    // col_index starts at 2
                    curr_word[col_index-prompt_length] = c;
                    lcd.setCursor(col_index, 1);
                    lcd.print(c);
                    col_index++;
                    break;
            }
        }
        key = '\0';
        return curr_word;
    }

    /* Display a message on the first line of the screen. 
     * Wait until the user hits the Enter key. */
    void display_message(const char *msg) {
      
        if (strlen(msg) > 16) {
            return display_long_message(msg);
        }

        clear_screen();
        lcd.print(msg);
        lcd.setCursor(0,1);
        lcd.print("> OK");
        while (key != '\n');
        key = '\0';
    }

    void clear_screen() {
        lcd.clear();
        lcd.noCursor();
        lcd.setCursor(0, 0);
    }
    
private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

    const int num_rows = 2;
    const int num_cols = 15;
    byte row_index = 0;
    byte col_index = 0;

    char curr_word[17] = {'\0'};
    const char *prompt = "> ";
    const int prompt_length = 2;

    // Displaying a long message requires the text to scroll.
    void display_long_message(const char *msg) {
        clear_screen();
        lcd.print(msg);
        int len = strlen(msg);
        int idx = 0;
        lcd.setCursor(0,1);
        lcd.print("> OK");
        while (key != '\n') {
            lcd.setCursor(0,0);
            lcd.print(&msg[idx]);

            // Pause at the beginning of the message for 1000ms
            if (idx == 0) {
                delay_with_enter_break(1000);
            }
            
            idx ++;

            // Scroll delay. Adjust to make text scroll faster/slower
            delay_with_enter_break(300);

            // Pause at the end of the message for 1000ms
            if (idx + 16 > len) {
                idx = 0;
                delay_with_enter_break(1000);
            }
        }
        key = '\0';
    }

    void delay_with_enter_break(int ms) {
        for (int i = 0; i < ms/10; i++) {
            if (key == '\n') {
                return;
            }
            delay(10);
        }
    }
};
