#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define PIN_NEOPIXEL 6
#define NUMPIXELS 40
Adafruit_NeoPixel strip(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

#define BTN_UP     2
#define BTN_DOWN   3
#define BTN_SELECT 4
#define BTN_BACK   5
#define POT_PIN    0

#define ENCODER_CLK 7
#define ENCODER_DT  8
#define ENCODER_SW  9
int lastClkState;
bool encoderButtonPressed = false;

const char* mainMenu[] = {"Color", "Effect", "Settings", "About"};
const int mainMenuLength = sizeof(mainMenu) / sizeof(mainMenu[0]);

int menuIndex = 0;
bool inSubMenu = false;
int subMenuIndex = 0;
int currentMenu = 0;
bool settingChanged = false;

unsigned long lastPress = 0;
const unsigned long debounce = 200;

uint32_t colors[] = {
  strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255),
  strip.Color(255, 255, 0), strip.Color(0, 255, 255), strip.Color(255, 0, 255),
  strip.Color(255, 255, 255), strip.Color(128, 0, 128), strip.Color(255, 128, 0), strip.Color(0, 128, 255)
};
int selectedColor = 0;
int selectedEffect = 0;

const int SPEED_MIN = 10;
const int SPEED_MAX = 255;
const int BRIGHTNESS_MIN = 0;
const int BRIGHTNESS_MAX = 255;

int speed = 50;
int brightness = 128;

// selected index in settings menu (0: speed config, 1: brightness)
int settingsIndex = 0;
// setting menu's options count
const int settingsMenuLength = 2;

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  lastClkState = digitalRead(ENCODER_CLK);

  strip.begin();
  strip.setBrightness(brightness);
  strip.show();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2.5);
  display.setCursor(25, 14);
  display.println("DarkRay");
  display.setTextSize(1);
  display.setCursor(15, 40);
  display.println("Developer Edition");
  display.display();
  delay(2000);
}

void loop() {
  handleInput();
  drawMenu();
  if (!inSubMenu) runSelectedEffect();
}

void handleInput() {
  if (millis() - lastPress < debounce) return;

  if (!digitalRead(BTN_UP)) {
    if (inSubMenu) {
      if (currentMenu == 2) {
        if (settingsIndex == 0) {
          speed = constrain(speed + 10, SPEED_MIN, SPEED_MAX);
        } else if (settingsIndex == 1) {
          brightness = constrain(brightness + 10, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
          strip.setBrightness(brightness);
          strip.show();
        }
      } else {
        subMenuIndex = max(subMenuIndex - 1, 0);
      }
    } else {
      menuIndex = (menuIndex + mainMenuLength - 1) % mainMenuLength;
    }
    lastPress = millis();
  }

  if (!digitalRead(BTN_DOWN)) {
    if (inSubMenu) {
      if (currentMenu == 2) {
        if (settingsIndex == 0) {
          speed = constrain(speed - 10, SPEED_MIN, SPEED_MAX);
        } else if (settingsIndex == 1) {
          brightness = constrain(brightness - 10, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
          strip.setBrightness(brightness);
          strip.show();
        }
      } else {
        subMenuIndex++;
      }
    } else {
      menuIndex = (menuIndex + 1) % mainMenuLength;
    }
    lastPress = millis();
  }

  if (!digitalRead(BTN_SELECT)) {
    if (!inSubMenu) {
      inSubMenu = true;
      currentMenu = menuIndex;
      subMenuIndex = 0;
      if (currentMenu == 2) settingsIndex = 0;
    } else {
      if (currentMenu == 2) {
        settingsIndex = (settingsIndex + 1) % settingsMenuLength;
      } else {
        applySelection();
        settingChanged = true;
      }
    }
    lastPress = millis();
  }

  if (!digitalRead(BTN_BACK)) {
    inSubMenu = false;
    lastPress = millis();
  }
// ---------- rotary encoder ----------
int newClkState = digitalRead(ENCODER_CLK);
if (newClkState != lastClkState) {
  if (digitalRead(ENCODER_DT) != newClkState) {
    // turning right
    if (inSubMenu) {
      if (currentMenu == 2) {
        if (settingsIndex == 0)
          speed = constrain(speed + 10, SPEED_MIN, SPEED_MAX);
        else if (settingsIndex == 1) {
          brightness = constrain(brightness + 10, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
          strip.setBrightness(brightness);
          strip.show();
        }
      } else {
        subMenuIndex++;
      }
    } else {
      menuIndex = (menuIndex + 1) % mainMenuLength;
    }
  } else {
    // turning left
    if (inSubMenu) {
      if (currentMenu == 2) {
        if (settingsIndex == 0)
          speed = constrain(speed - 10, SPEED_MIN, SPEED_MAX);
        else if (settingsIndex == 1) {
          brightness = constrain(brightness - 10, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
          strip.setBrightness(brightness);
          strip.show();
        }
      } else {
        subMenuIndex = max(subMenuIndex - 1, 0);
      }
    } else {
      menuIndex = (menuIndex + mainMenuLength - 1) % mainMenuLength;
    }
  }
  lastClkState = newClkState;
  // prevent noises
  lastPress = millis();
}

// ---------- rotary btn ----------
if (!digitalRead(ENCODER_SW)) {
  if (!encoderButtonPressed && millis() - lastPress > debounce) {
    if (!inSubMenu) {
      inSubMenu = true;
      currentMenu = menuIndex;
      subMenuIndex = 0;
      if (currentMenu == 2) settingsIndex = 0;
    } else {
      if (currentMenu == 2) {
        settingsIndex = (settingsIndex + 1) % settingsMenuLength;
      } else {
        applySelection();
        settingChanged = true;
      }
    }
    encoderButtonPressed = true;
    lastPress = millis();
  }
} else {
  encoderButtonPressed = false;
}

}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);

  int visibleLines = 4;
  int startIdx = 0;

  if (inSubMenu && subMenuIndex >= visibleLines) {
    startIdx = subMenuIndex - visibleLines + 1;
  }

  if (!inSubMenu) {
    display.setCursor(1, 1);
    display.println("   == Main Menu ==");
    for (int i = 0; i < mainMenuLength; i++) {
      display.setCursor(10, 12 + (i - startIdx) * 10);
      if (i == menuIndex) display.print("> ");
      else display.print("  ");
      display.println(mainMenu[i]);
    }
  } else {
    switch (currentMenu) {
      case 0:
        display.setCursor(1, 1);
        display.println("  == Select Color ==");
        for (int i = startIdx; i < startIdx + visibleLines && i < 10; i++) {
          display.setCursor(10, 12 + (i - startIdx) * 10);
          display.print((i == subMenuIndex) ? "> " : "  ");
          display.print("Color "); display.println(i);
        }
        break;
      case 1:
        display.setCursor(1, 1);
        display.println(" == Select Effect ==");
        for (int i = startIdx; i < startIdx + visibleLines && i < 19; i++) {
          display.setCursor(10, 12 + (i - startIdx) * 10);
          display.print((i == subMenuIndex) ? "> " : "  ");
          display.print("Effect "); display.println(i);
        }
        break;
      case 2:
        display.setCursor(1, 1);
        display.println("   == Settings ==");
        display.setCursor(10, 12);
        display.print((settingsIndex == 0) ? "> " : "  ");
        display.print("Speed: ");
        display.println(speed);
        display.setCursor(10, 22);
        display.print((settingsIndex == 1) ? "> " : "  ");
        display.print("Brightness: ");
        display.println(brightness);
        break;
      case 3:
        display.setCursor(10, 5);
        display.println("DarkRay : V1.0 Pro");
        display.setCursor(10, 12);
        display.println("------------------");
        display.setCursor(2, 24);
        display.println("production:2025/06/05");
        display.setCursor(2, 37);
        display.println("Developer:AmirMahdi");
        display.setCursor(2, 49);
        display.println("Made in: Iran ");        
        break;
    }
  }
  display.display();
}

void applySelection() {
  if (currentMenu == 0) {
    selectedColor = subMenuIndex % (sizeof(colors) / 4);
    setColor(colors[selectedColor]);
  } else if (currentMenu == 1) {
    selectedEffect = subMenuIndex;
  }
}

void setColor(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void runSelectedEffect() {
  switch (selectedEffect) {
    case 0: rainbowCycle(); break;
    case 1: blinkEffect(); break;
    case 2: bounceEffect(); break;
    case 3: waveEffect(); break;
    case 4: fadeEffect(); break;
    case 5: meteorEffect(); break;
    case 6: fireEffect(); break;
    case 7: iceEffect(); break;

    // new effects (disabled by default)
    case 8: rainbowFade(15); break;
    case 9: rainbowFade(5); break;
    case 10: rainbowFade(40); break;
    case 11: rainbowSparkle(50); break;
    case 12: sparkleRandomRainbow(30); break;
    case 13: rainbowKnightRider(30); break;
    case 14: rainbowKnightRider(10); break;
    case 15: rainbowKnightRider(60); break;
    case 16: rainbowBounce(20); break;
    case 17: rainbowBounce(5); break;
    case 18: rainbowBounce(40); break;



    default: setColor(colors[selectedColor]); break;
  }
}

void simpleEffect(int effectNumber) {
  // blinking mode for the new effects
  static bool on = false;
  on = !on;
  if (on) {
    // white (ON)
    setColor(strip.Color(255, 255, 255));
  } else {
    // OFF
    setColor(0);
  }
  delay(speed);
}

// main effects

void rainbowCycle() {
  static uint16_t j = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.ColorHSV((i * 65536 / strip.numPixels() + j) % 65536));
  }
  strip.show();
  j += 256;
  delay(speed);
}

void blinkEffect() {
  static bool on = false;
  on = !on;
  setColor(on ? colors[selectedColor] : 0);
  delay(speed * 2);
}

void bounceEffect() {
  static int pos = 0;
  static int dir = 1;
  strip.clear();
  strip.setPixelColor(pos, colors[selectedColor]);
  strip.show();
  pos += dir;
  if (pos == 0 || pos == NUMPIXELS - 1) dir *= -1;
  delay(speed);
}

void waveEffect() {
  static int t = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    float wave = sin((i + t) * 0.3);
    int brightnessFactor = map(wave * 100, -100, 100, 0, 255);
    uint8_t r = (colors[selectedColor] >> 16) & 0xFF;
    uint8_t g = (colors[selectedColor] >> 8) & 0xFF;
    uint8_t b = colors[selectedColor] & 0xFF;
    strip.setPixelColor(i, strip.Color(r * brightnessFactor / 255, g * brightnessFactor / 255, b * brightnessFactor / 255));
  }
  strip.show();
  t++;
  delay(speed);
}

void fadeEffect() {
  static int f = 0;
  static int dir = 1;
  int val = abs(sin(f * 0.1) * 255);
  uint8_t r = (colors[selectedColor] >> 16) & 0xFF;
  uint8_t g = (colors[selectedColor] >> 8) & 0xFF;
  uint8_t b = colors[selectedColor] & 0xFF;
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r * val / 255, g * val / 255, b * val / 255));
  }
  strip.show();
  f += dir;
  if (f > 63) dir = -1;
  if (f < 0) dir = 1;
  delay(speed);
}

void meteorEffect() {
  static int pos = 0;
  strip.clear();
  for (int i = 0; i < 4; i++) {
    int idx = (pos - i + NUMPIXELS) % NUMPIXELS;
    int fade = 255 - i * 60;
    uint8_t r = (colors[selectedColor] >> 16) & 0xFF;
    uint8_t g = (colors[selectedColor] >> 8) & 0xFF;
    uint8_t b = colors[selectedColor] & 0xFF;
    strip.setPixelColor(idx, strip.Color(r * fade / 255, g * fade / 255, b * fade / 255));
  }
  strip.show();
  pos = (pos + 1) % NUMPIXELS;
  delay(speed);
}

void fireEffect() {
  for (int i = 0; i < NUMPIXELS; i++) {
    int flicker = random(192, 255);
    strip.setPixelColor(i, strip.Color(flicker, flicker / 4, 0));
  }
  strip.show();
  delay(speed);
}

void iceEffect() {
  for (int i = 0; i < NUMPIXELS; i++) {
    int cool = random(100, 255);
    strip.setPixelColor(i, strip.Color(cool / 4, cool / 2, cool));
  }
  strip.show();
  delay(speed);
}
void rainbowFade(int wait) {
  static int j = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    uint8_t brightness = (sin(i + j * 0.1) + 1) * 127;
    uint32_t col = Wheel((i * 256 / NUMPIXELS + j) & 255);
    strip.setPixelColor(i, dimColor(col, brightness));
  }
  strip.show();
  j = (j + 1) % 256;
  delay(wait);
}

void rainbowSparkle(int wait) {
  static unsigned long lastSparkle = 0;
  static int pos = 0;
  if (millis() - lastSparkle > wait) {
    // clear the previous
    strip.setPixelColor(pos, 0);
    pos = random(NUMPIXELS);
    strip.setPixelColor(pos, Wheel(random(255)));
    strip.show();
    lastSparkle = millis();
  }
}

void sparkleRandomRainbow(int wait) {
  strip.clear();
  for (int i = 0; i < NUMPIXELS / 3; i++) {
    int pos = random(NUMPIXELS);
    strip.setPixelColor(pos, Wheel(random(255)));
  }
  strip.show();
  delay(wait);
}

// the rainbow larson rider effect (the light moves backwards and forwards)
void rainbowKnightRider(int wait) {
  static int pos = 0;
  static int dir = 1;

  strip.clear();
  strip.setPixelColor(pos, Wheel((pos * 256 / NUMPIXELS) & 255));
  strip.show();
  pos += dir;
  if (pos == NUMPIXELS - 1 || pos == 0) dir = -dir;
  delay(wait);
}

// rainbow bounce effect
void rainbowBounce(int wait) {
  static int pos = 0;
  static int dir = 1;

  strip.clear();
  for (int i = 0; i <= pos; i++) {
    strip.setPixelColor(i, Wheel((i * 256 / NUMPIXELS) & 255));
  }
  strip.show();
  pos += dir;
  if (pos == NUMPIXELS - 1 || pos == 0) dir = -dir;
  delay(wait);
}



// --------- auxiliary functions ---------

uint32_t dimColor(uint32_t color, uint8_t brightness) {
  return strip.Color((uint8_t)((color >> 16 & 0xFF) * brightness / 255),
                      (uint8_t)((color >> 8 & 0xFF) * brightness / 255),
                      (uint8_t)((color & 0xFF) * brightness / 255));
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
