#pragma once

#include <FastLED.h>
#include "strings.h"

// "const int" uses no additional memory
const int NUM_LEDS = 265;
//const int NUM_LEDS = 200;
const int LED_DATA_PIN = 7;

const int TEAM_NEITHER = 0b00;
const int TEAM1        = 0b01;
const int TEAM2        = 0b10;
const int TEAM_BOTH    = 0b11;

class Territories {
public:

    Territories() {}

    void init() {
        FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);

        // Turn on a Red light so we know this works
        leds[0] = CRGB::Red;

        FastLED.show();
    }

    // move the lights around in a circle around the different territories
    // called every 100ms or so
    void tick_lights(void) {
//        Serial.println("ticked"); // debug

        // Turn all the lights off
        memset(leds, CRGB::Black, sizeof(*leds) * NUM_LEDS);

        // Celebration
        if (progress[0] == 7) {
//        if (true) {
            for (int i = 0; i < NUM_LEDS; i++) {
                if ((i + light_clock) % 30 == 0) {
                    leds[i] = CRGB::Green;
                }
            }
            FastLED.show();
            light_clock++;
            return;
        }

        // For each territory, turn a single light on. 
        for (int terr_i = 0; terr_i < NUM_TERRITORIES; terr_i++) {

            // Calculate the index of the light to turn on
            int start_i = leds_start_end[terr_i][0];
            int end_i   = leds_start_end[terr_i][1];
            if (end_i == 255) {
                end_i = 264;
            }
            int len = end_i - start_i + 1;
            int index = start_i + (light_clock % len);

            
            switch (ownership_status[terr_i]) {
                case TEAM1:
                    leds[index] = CRGB::Orange;
                    break;
                case TEAM2:
                    leds[index] = CRGB::Blue;
                    break;
                case TEAM_BOTH:
                    // Alternate Blue/Orange with every tick
                    if (index % 2 == 0) {
                        leds[index] = CRGB::Blue;
                    } else {
                        leds[index] = CRGB::Orange;
                    }
                    break;
                default:
                    break;
            }
        }
        FastLED.show();
        light_clock++;
    }

    inline STR_IDX_T get_directions(int team) {
//        Serial.println(get_next_territory(team));
//        Serial.println(territory_to_name(get_next_territory(team));
//        return 0;
        return territory_to_name(get_next_territory(team));
    }

    inline int get_horserace_winner() {
        return ownership_status[HORSERACE_TERRITORY];
    }

    bool guess_codeword(const char *guess, int team) {
#if 0
//        Serial.print("Team #");
//        Serial.print(team);
//        Serial.print(" guessed codeword '");
//        Serial.print(guess);
//        Serial.println("'");
#endif
        if      (!strcmp(guess, "BADNEWS")) {
            if (progress[team] == 0) {
                add_owner_to_territory(team, HORSERACE_TERRITORY);

                horserace_winner = team;
                horserace_loser  = (TEAM1 + TEAM2) - team; // Math is a beautiful thing

                // Both teams progress
                progress[TEAM1]++;
                progress[TEAM2]++;
                return true;
            }
        }
        else if (!strcmp(guess, "GODCHILD")) {
            if ((team == horserace_winner && progress[team] == 1)
              ||(team == horserace_loser  && progress[team] == 2)) {
                add_owner_to_territory(team, FEAR_TERRITORY);
                progress[team]++;
                return true;
            }
        }
        else if (!strcmp(guess, "BLACKERJACK")) {
            if ((team == horserace_winner && progress[team] == 2)
              ||(team == horserace_loser  && progress[team] == 1)) {
                add_owner_to_territory(team, ATH_TERRITORY);
                progress[team]++;
                return true;
            }
        }
        else if (!strcmp(guess, "UNLUCKY")) {
            if ((!horsehead_complete) && (team == horserace_loser)) {
                ownership_status[HORSERACE_TERRITORY] = team;
                horsehead_complete = true;
                return true;
            }
        }
        else if (!strcmp(guess, "BRIBE")) {
            if (progress[team] == 3) {
                // Joint effort, both teams get added to the territory
                add_owner_to_territory(TEAM_BOTH, ARMS_TERRITORY);

                // Both teams progress
                progress[TEAM1]++;
                progress[TEAM2]++;
                return true;
            }
        }
        else if (!strcmp(guess, "CAUSEWAY")) {
            if ((team == TEAM2 && progress[team] == 4)
              ||(team == TEAM1 && progress[team] == 5)) {
                add_owner_to_territory(team, BRIDGE_TERRITORY);
                progress[team]++;
                return true;
            }
        }
        else if (!strcmp(guess, "STRACCI")) {
            if ((team == TEAM2 && progress[team] == 5)
              ||(team == TEAM1 && progress[team] == 4)) {
                add_owner_to_territory(team, ANNENBERG_TERRITORY);
                progress[team]++;
                return true;
            }
        }
        else if (!strcmp(guess, "CUPCAKE")) {
            if (progress[team] == 6) {
                add_owner_to_territory(team, GATES_TERRITORY);
                progress[TEAM1]++;
                progress[TEAM2]++;
                return true;
            }
        }
        return false;
    }

    void force_progress(int team) {
        progress[team]++;
    }

private:

    enum territory_name_t {
        ANNENBERG_TERRITORY = 0,
        ATH_TERRITORY,
        BRIDGE_TERRITORY,
        ARMS_TERRITORY,
        GATES_TERRITORY,
        FEAR_TERRITORY,
        HORSERACE_TERRITORY,

        NUM_TERRITORIES,
    };

    const byte leds_start_end[NUM_TERRITORIES][2] = {
        {7, 95},
        {97, 132},
        {133, 150},
        {151, 182},
        {183, 217},
        {218, 249},
        {250, 255}
    };

    int ownership_status[NUM_TERRITORIES] = {TEAM_NEITHER};

    // 0 --> Go to Horse race
    // 1 --> Go to Fear or Ath
    // 2 --> Go to Ath or Fear
    // 3 --> Go to Senator's Office
    // 4 --> Go to Annenberg or Bridge
    // 5 --> Go to Bridge or Annenberg
    // 6 --> Open suitcases and go to Wedding Cake
    // 7 --> Done!
    // Note: progress[0] is unused
    byte progress[3] = {0};

    byte horserace_winner = TEAM_NEITHER;
    byte horserace_loser  = TEAM_NEITHER;

    // Flip to true once the losing horserace team sends the picture
    // and enters the revenge password
    bool horsehead_complete = false;
    
    unsigned int light_clock = 0;
    
    CRGB leds[NUM_LEDS];

    void add_owner_to_territory(int new_owner, territory_name_t territory) {
        ownership_status[territory] |= new_owner;
    }
    
    inline STR_IDX_T territory_to_name(territory_name_t t) {
        switch (t) {
            case HORSERACE_TERRITORY:    return 9;
            case GATES_TERRITORY:        return 10;
            case FEAR_TERRITORY:         return 11;
            case ATH_TERRITORY:          return 12;
            case ANNENBERG_TERRITORY:    return 13;
            case BRIDGE_TERRITORY:       return 14;
            case ARMS_TERRITORY:         return 15;
            default:                     return 10;
        }
    }

    territory_name_t get_next_territory(int team) {
        switch (progress[team]) {
            case 0: return HORSERACE_TERRITORY;
            case 1: return team == horserace_loser ? ATH_TERRITORY : FEAR_TERRITORY;
            case 2: return team == horserace_loser ? FEAR_TERRITORY : ATH_TERRITORY;
            case 3: return ARMS_TERRITORY;
            case 4: return team == TEAM1 ? ANNENBERG_TERRITORY : BRIDGE_TERRITORY;
            case 5: return team == TEAM2 ? ANNENBERG_TERRITORY : BRIDGE_TERRITORY;
            default: return GATES_TERRITORY;
        }
    }
};
