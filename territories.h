#pragma once

#include <FastLED.h>

const int NUM_LEDS = 290;
const int LED_DATA_PIN = 7;

class Territories {
public:
    enum team_t {
        TEAM1 = 0,
        TEAM2,
        TEAM_NEITHER,
        TEAM_BOTH,
    };

    Territories() {}

    void init() {
        FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
        for (int i = 0; i < 1; i+=10) {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
    }

    // move the lights around in a circle around the different territories
    // called every 100ms or so
    void tick_lights(void) {
        Serial.println("ticked");
        memset(leds, CRGB::Black, sizeof(*leds) * NUM_LEDS);
        for (int terr_i = 0; terr_i < NUM_TERRITORIES; terr_i++) {
            int start_i = leds_start_end[terr_i][0];
            int end_i   = leds_start_end[terr_i][1];
            int len = end_i - start_i + 1;
            int index = start_i + (light_clock % len);
//            leds[index] = CRGB::Orange;
            switch (ownership_status[terr_i]) {
                case TEAM1:
                    leds[index] = CRGB::Orange;
                    break;
                case TEAM2:
                    leds[index] = CRGB::Blue;
                    break;
                case TEAM_BOTH:
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

    const char *get_directions(int team) {
        return territory_to_name((territory_name_t) progress[team]);
    }

    team_t get_horserace_winner() {
        return ownership_status[HORSERACE_TERRITORY];
    }

    bool guess_codeword(const char *guess, int team_as_int) {
        team_t team = (team_t) team_as_int;
        if      (!strcmp(guess, "YEEHAW")) {
            add_owner_to_territory(team, HORSERACE_TERRITORY);
            progress[TEAM1]++;
            progress[TEAM2]++;
            return true;
        }
        else if (!strcmp(guess, "GODCHILD")) {
            add_owner_to_territory(team, FEAR_TERRITORY);
            progress[team]++;
            return true;
        }
        else if (!strcmp(guess, "BLACKERJACK")) {
            add_owner_to_territory(team, ATH_TERRITORY);
            progress[team]++;
            return true;
        }
        else if (!strcmp(guess, "WHATASTUD")) {
            // small bug - the winning team can use the same code to reclaim the territory
            // because get_horserace_winner is changed
            if (team != get_horserace_winner()) {
                ownership_status[HORSERACE_TERRITORY] = TEAM1; // FIXME;
            } else {
                // TODO they're not allowed to do this!
            }
            return true;
        }
        return false;
    }

    void force_progress(int team) {
        // TODO
        progress[team]++;
    }

private:

    enum territory_name_t {
        ANNENBERG_TERRITORY = 0,
        ATH_TERRITORY,
        BRIDGE_TERRITORY,
        GATES_TERRITORY,
        FEAR_TERRITORY,
        HORSERACE_TERRITORY,

        NUM_TERRITORIES,
    };

    const int leds_start_end[NUM_TERRITORIES][2] = {
        {7, 95},
        {97, 132},
        {133, 182},
        {183, 217},
        {218, 249},
        {250, 289}
    };

    team_t ownership_status[NUM_TERRITORIES] = {TEAM_NEITHER};
    byte progress[2] = {0};
    
    unsigned int light_clock = 0;
    
    CRGB leds[NUM_LEDS];

    void add_owner_to_territory(team_t owner, territory_name_t territory) {
        if (owner == TEAM1) {
            if (ownership_status[territory] == TEAM2) {
                ownership_status[territory] = TEAM_BOTH;
            } else {
                ownership_status[territory] = TEAM1;
            }
        } else if (owner == TEAM2) {
            if (ownership_status[territory] == TEAM1) {
                ownership_status[territory] = TEAM_BOTH;
            } else {
                ownership_status[territory] = TEAM2;
            }
        }
    }
    
    const char *territory_to_name(territory_name_t t) {
//        return territory_names[(int) t];
        switch (t) {
            case HORSERACE_TERRITORY:    return "Field South of Gene Pool";
            case GATES_TERRITORY:        return "Gates-Thomas 314";
            case FEAR_TERRITORY:         return "Fear";
            case ATH_TERRITORY:          return "the Einstein Casino in the Ath";
            case ANNENBERG_TERRITORY:    return "Annenberg Lounge";
            case BRIDGE_TERRITORY:       return "Bridge 210";
            default:                     return "";
        }
    }

//    const char * territory_names[6] = {
//        "Annenberg Lounge", 
//        "the Einstein Casino in the Ath",
//        "Bridge 210",
//        "Gates-Thomas 314",
//        "Fear",
//        "Field South of Gene Pool",
//    };
};
