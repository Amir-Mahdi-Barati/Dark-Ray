#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define LED_COUNT   150
#define BUTTON_PIN  5

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#define BRIGHTNESS  220
#define HOLD_TIME   2000   // ۲ ثانیه نگه داشتن برای برگشت به افکت‌ها

// حالت‌ها
int mode = 0;  // 0 = افکت‌ها, 1 = رنگ ثابت‌ها
int effectIndex = 0;
int colorIndex = 0;

// زمان‌ها برای کنترل دکمه
unsigned long buttonPressTime = 0;
bool buttonHeld = false;
bool buttonWasPressed = false;

// رنگ‌ها
uint32_t colors[10];

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  // 🎨 رنگ‌های ثابت زیبا
  colors[0] = strip.Color(255, 0, 0);
  colors[1] = strip.Color(255, 128, 0);
  colors[2] = strip.Color(255, 255, 0);
  colors[3] = strip.Color(0, 255, 0);
  colors[4] = strip.Color(0, 255, 255);
  colors[5] = strip.Color(0, 0, 255);
  colors[6] = strip.Color(128, 0, 255);
  colors[7] = strip.Color(255, 0, 255);
  colors[8] = strip.Color(255, 255, 255);
  colors[9] = strip.Color(255, 50, 150);
}

void loop() {
  handleButton();

  if (mode == 0) {       // افکت‌ها
    runEffect(effectIndex);
  } else if (mode == 1) { // رنگ‌های ثابت
    showColor(colors[colorIndex]);
    delay(20);
  }
}

/* ------------------ کنترل دکمه ------------------ */
void handleButton() {
  bool pressed = !digitalRead(BUTTON_PIN);

  if (pressed && !buttonWasPressed) {  
    // شروع فشار دادن
    buttonPressTime = millis();
    buttonHeld = false;
  }

  if (!pressed && buttonWasPressed) {  
    // دکمه رها شد
    unsigned long pressDuration = millis() - buttonPressTime;

    if (pressDuration < HOLD_TIME && mode == 1) {
      // فشار کوتاه → تغییر رنگ در حالت رنگ ثابت
      colorIndex = (colorIndex + 1) % 10;
      showColor(colors[colorIndex]);
    }
  }

  // اگر دکمه بیش از ۲ ثانیه نگه داشته شد → سوئیچ بین حالت‌ها
  if (pressed && !buttonHeld && (millis() - buttonPressTime > HOLD_TIME)) {
    buttonHeld = true;
    mode = (mode == 0) ? 1 : 0;  // جابه‌جایی حالت
    if (mode == 1) {
      colorIndex = 0;
      showColor(colors[colorIndex]);
    }
    delay(300);  // ضد لرزش
  }

  buttonWasPressed = pressed;
}

/* ------------------ افکت‌ها ------------------ */

void runEffect(int i) {
  switch (i) {
    case 0: rainbowFlow(); break;
    case 1: rainbowCycle(); break;
    case 2: waveMotion(); break;
    case 3: fireFlicker(); break;
    case 4: meteorDual(); break;
    case 5: lightTrail(); break;
    case 6: cylonEye(); break;
    case 7: sparkleEffect(); break;
    case 8: pulseGlow(); break;
    case 9: colorBreath(); break;
  }

  // هر 3 دقیقه افکت بعدی
  static unsigned long lastChange = 0;
  if (millis() - lastChange > 10000) {
    effectIndex = (effectIndex + 1) % 10;
    lastChange = millis();
  }
}

/* ------------------ افکت‌های زیبا ------------------ */

void rainbowFlow() {
  static uint16_t j = 0;
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, Wheel((i + j) & 255));
  strip.show();
  j++;
  delay(10);
}

void rainbowCycle() {
  static uint16_t j = 0;
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
  strip.show();
  j++;
  delay(15);
}

void waveMotion() {
  static float t = 0;
  t += 0.2;
  for (int i = 0; i < strip.numPixels(); i++) {
    int r = (sin(i * 0.1 + t) + 1) * 127;
    int g = (sin(i * 0.1 + t + 2) + 1) * 127;
    int b = (sin(i * 0.1 + t + 4) + 1) * 127;
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  delay(20);
}

void fireFlicker() {
  for (int i = 0; i < strip.numPixels(); i++) {
    int flicker = random(120, 255);
    strip.setPixelColor(i, strip.Color(flicker, (int)(flicker * 0.36), 0));
  }
  strip.show();
  delay(50);
}

void meteorDual() {
  static int pos = 0;
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, fadeToBlack(strip.getPixelColor(i), 64));

  strip.setPixelColor(pos, strip.Color(255, 255, 255));
  strip.setPixelColor(strip.numPixels() - 1 - pos, strip.Color(0, 100, 255));
  strip.show();

  pos++;
  if (pos >= strip.numPixels()) pos = 0;
  delay(20);
}

void lightTrail() {
  static int pos = 0;
  strip.clear();
  for (int i = 0; i < 10; i++) {
    int p = (pos - i + strip.numPixels()) % strip.numPixels();
    strip.setPixelColor(p, strip.Color(255 - i * 25, 50, i * 25));
  }
  strip.show();
  pos++;
  if (pos >= strip.numPixels()) pos = 0;
  delay(15);
}

void cylonEye() {
  static int pos = 0;
  static int dir = 1;
  strip.clear();
  strip.setPixelColor(pos, strip.Color(255, 0, 0));
  strip.setPixelColor(pos + 1, strip.Color(150, 0, 0));
  strip.show();
  pos += dir;
  if (pos <= 0 || pos >= strip.numPixels() - 2) dir *= -1;
  delay(15);
}

void sparkleEffect() {
  int p = random(strip.numPixels());
  strip.setPixelColor(p, strip.Color(random(255), random(255), random(255)));
  strip.show();
  delay(10);
}

void pulseGlow() {
  static int b = 0;
  static int dir = 1;
  b += dir * 5;
  if (b >= 255 || b <= 0) dir *= -1;
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, strip.Color(0, b, 255 - b));
  strip.show();
  delay(20);
}

void colorBreath() {
  static int t = 0;
  t = (t + 3) % 360;
  int r = (sin(t * 3.14 / 180) + 1) * 127;
  int g = (sin((t + 120) * 3.14 / 180) + 1) * 127;
  int b = (sin((t + 240) * 3.14 / 180) + 1) * 127;
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
  delay(25);
}

/* ------------------ ابزارها ------------------ */

uint32_t fadeToBlack(uint32_t color, byte fadeValue) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  r = (r <= 10) ? 0 : (r - (r * fadeValue / 256));
  g = (g <= 10) ? 0 : (g - (g * fadeValue / 256));
  b = (b <= 10) ? 0 : (b - (b * fadeValue / 256));

  return strip.Color(r, g, b);
}

byte Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void showColor(uint32_t c) {
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, c);
  strip.show();
}
