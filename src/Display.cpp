//
// Created by marcel on 26.09.23.
//

#include <ArduinoNvs.h>
#include "Display.h"
#include "SetupMode.h"
#include "Buttons.h"

CRGB Display::pixels[NUMPIXELS];
bool Display::effect_running = false;
bool Display::pause = false;
uint8_t *Display::backup = nullptr;
uint8_t Display::brightness = 125;

void Display::init() {
    Serial.println("***************************************");
    Serial.println("FastLED-Version");
    Serial.println("***************************************");
    FastLED.addLeds<WS2812B, PIN, GRB>(pixels, NUMPIXELS);
    Display::backup = (uint8_t *) malloc(sizeof(pixels) + 8);
    brightness = NVS.getInt("brightness");
    if (brightness == 0) {
        brightness = 125;
    }

    FastLED.setBrightness(brightness);

    Display::clear();
    Display::update();
}

void Display::clear() {
    Display::clear_display();
    Display::update(); // Update strip
}

void Display::set_color(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    Display::set_color(x * 3 + ((x % 2 == 0) ? y : 2 - y), r, g, b);
}

void Display::set_color(int pos, uint8_t r, uint8_t g, uint8_t b) {
    if (pos < 0 || pos >= Display::NUMPIXELS) {
        return;
    }

    pixels[pos].setRGB(r, g, b);
}

void Display::clear_pixel(int x, int y) {
    Display::set_color(x, y, 0, 0, 0);
}

void Display::clear_pixel(int pos) {
    Display::set_color(pos, 0, 0, 0);
}

void Display::clear_display() {

    FastLED.clear();


}

void Display::update() {
    FastLED.show();


}

void Display::fill_segment(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t pc = 0;
    uint8_t mx = 3;
    uint8_t sx = 0;
    switch (segment) {
        case 0:
            mx = 1;
            sx = 0;
            break;
        case 1:
            mx = 3;
            sx = 1;
            break;
        case 2:
            mx = 2;
            sx = 4;
            break;
        case 3:
            mx = 3;
            sx = 6;
            break;
        default:
            mx = 3;
    }
    Display::clear_segment(segment);

    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < mx; x++) {

            if (pc < num_pixel) {
                Display::set_color(sx + x, y, r, g, b);
                pc++;
            };
        }
    }

}

void Display::clear_segment(uint8_t segment) {
    uint8_t pc = 0;
    uint8_t mx = 3;
    uint8_t sx = 0;
    switch (segment) {
        case 0:
            mx = 1;
            sx = 0;
            break;
        case 1:
            mx = 3;
            sx = 1;
            break;
        case 2:
            mx = 2;
            sx = 4;
            break;
        case 3:
            mx = 3;
            sx = 6;
            break;
        default:
            mx = 3;
    }

    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < mx; x++) {
            Display::clear_pixel(sx + x, y);
        }
    }

}

void Display::fill_segment_dice(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b) {
    Display::clear_segment(segment);
    switch (segment) {
        case 0: {
            switch (num_pixel) {
                case 1: {
                    Display::set_color(0, 1, r, g, b);
                }
                    break;
                case 2: {
                    Display::set_color(0, 0, r, g, b);
                    Display::set_color(0, 2, r, g, b);
                }
                    break;
                case 3: {
                    Display::set_color(0, 0, r, g, b);
                    Display::set_color(0, 1, r, g, b);
                    Display::set_color(0, 2, r, g, b);
                }
                    break;
                default: {
                }
            }
        }
            break;
        case 1:
        case 3: {
            int sx = (segment == 1) ? 1 : 6;
            switch (num_pixel) {
                case 0:
                    break;
                case 1: {
                    Display::set_color(sx + 1, 1, r, g, b);
                }
                    break;
                case 2: {
                    Display::set_color(sx, 2, r, g, b);
                    Display::set_color(sx + 2, 0, r, g, b);
                }
                    break;
                case 3: {
                    Display::set_color(sx, 0, r, g, b);
                    Display::set_color(sx + 1, 1, r, g, b);
                    Display::set_color(sx + 2, 2, r, g, b);
                }
                    break;
                case 5: {
                    Display::set_color(sx + 1, 1, r, g, b);
                }
                case 4: {
                    Display::set_color(sx, 0, r, g, b);
                    Display::set_color(sx + 2, 0, r, g, b);
                    Display::set_color(sx, 2, r, g, b);
                    Display::set_color(sx + 2, 2, r, g, b);
                }
                    break;
                case 8:
                case 9 : {
                    Display::set_color(sx + 1, 0, r, g, b);
                    Display::set_color(sx + 1, 2, r, g, b);
                }

                case 7: {
                    if (num_pixel == 7 || num_pixel == 9) {
                        Display::set_color(sx + 1, 1, r, g, b);
                    }
                }
                case 6: {
                    Display::set_color(sx, 0, r, g, b);
                    Display::set_color(sx, 1, r, g, b);
                    Display::set_color(sx, 2, r, g, b);
                    Display::set_color(sx + 2, 0, r, g, b);
                    Display::set_color(sx + 2, 1, r, g, b);
                    Display::set_color(sx + 2, 2, r, g, b);
                }
                    break;

                default: {

                }
            }
        }
            break;
        case 2: {
            switch (num_pixel) {
                case 1: {
                    Display::set_color(4, 1, r, g, b);
                }
                    break;
                case 2: {
                    Display::set_color(4, 1, r, g, b);
                    Display::set_color(5, 1, r, g, b);
                }
                    break;
                case 3: {
                    Display::set_color(4, 0, r, g, b);
                    Display::set_color(4, 1, r, g, b);
                    Display::set_color(4, 2, r, g, b);
                }
                    break;
                case 4: {
                    Display::set_color(4, 0, r, g, b);
                    Display::set_color(4, 2, r, g, b);
                    Display::set_color(5, 0, r, g, b);
                    Display::set_color(5, 2, r, g, b);
                }
                    break;
                case 5: {
                    Display::set_color(4, 0, r, g, b);
                    Display::set_color(4, 1, r, g, b);
                    Display::set_color(4, 2, r, g, b);
                    Display::set_color(5, 0, r, g, b);
                    Display::set_color(5, 2, r, g, b);
                }
                    break;
                case 6: {
                    Display::fill_segment(segment, 6, r, g, b);
                }
                    break;
                default: {

                }
            }
        }
            break;
        default: {

        }
    }


}

void Display::fill_segment_random(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t pc = 0;
    uint8_t mx = 3;
    uint8_t sx = 0;
    switch (segment) {
        case 0:
            mx = 1;
            sx = 0;
            break;
        case 1:
            mx = 3;
            sx = 1;
            break;
        case 2:
            mx = 2;
            sx = 4;
            break;
        case 3:
            mx = 3;
            sx = 6;
            break;
        default:
            mx = 3;
    }

    Display::clear_segment(segment);

    while (pc < num_pixel) {
        uint8_t y = (uint8_t) random(0, 3) % 3;
        uint8_t x = (uint8_t) random(0, mx) % mx;

        if (Display::is_clear(sx + x, y)) {
            Display::set_color(sx + x, y, r, g, b);
            pc++;
        }

    }
}

bool Display::is_clear(uint8_t x, uint8_t y) {
    return Display::is_clear(x * 3 + ((x % 2 == 0) ? y : 2 - y));
}

bool Display::is_clear(int pos) {
    if (pos < 0 || pos >= Display::NUMPIXELS) {
        return false;
    }

    return pixels[pos].r + pixels[pos].g + pixels[pos].b == 0;

}

void Display::start_pause() {
    if (Display::pause) {
        return;
    }
    Display::pause = true;
    vTaskDelay(2 / portTICK_PERIOD_MS);
    memcpy(Display::backup, pixels, sizeof(pixels));
}

void Display::resume() {
    if (!Display::pause) {
        return;
    }
    memcpy(pixels, Display::backup, sizeof(pixels));
    Display::update();
    Display::pause = false;
}

void Display::fill(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < Display::NUMPIXELS; i++) {
        Display::set_color(i, r, g, b);
    }
}

void Display::start_effect() {
    if (Display::effect_running) {
        return;
    }
    if (SetupMode::active) {
        return;
    }
    if (Display::pause) {
        return;
    }
    Display::effect_running = true;

    vTaskDelay(15 / portTICK_PERIOD_MS);
}

void Display::fade_out(int pos, uint8_t scale) {
    if (pos < 0 || pos >= Display::NUMPIXELS) {
        return;
    }

    pixels[pos].fadeToBlackBy(scale);
}

void Display::fade_in(int pos, uint8_t scale) {
    if (pos < 0 || pos >= Display::NUMPIXELS) {
        return;
    }

    pixels[pos].fadeLightBy(scale);
}

void Display::process_button_click(uint8_t button, uint8_t type) {
    if (SetupMode
    ::active) {
        return;
    }

    if (button == 2) {
        if (type == 1 || type == 2) {

            if (!Buttons::is_pressed(1)) {

                Display::brightness += 16;
                if (Display::brightness > 230) {
                    Display::brightness = 32;
                }

                NVS.setInt("brightness", Display::brightness, true);
                FastLED.setBrightness(Display::brightness);
                FastLED.show();

                Serial.println("Brightness: " + String(Display::brightness));

            }

        }

    }
}
