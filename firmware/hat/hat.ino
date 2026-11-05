// =====================================================================
//  Квест «Гаррі Поттер» — прошивка Сортувального капелюха (ESP32-C3 SuperMini)
//  Плата: challenge-duel/pcb, варіант hat. Налаштування — config.h, рухи рота — mouth.h.
//
//  Як працює (як стара шапка hate_hogrards.ino, але без DFPlayer і реле):
//    брелок Грифіндор / Слизерин / Старт / Фінал → наступна фраза групи по колу,
//    мотор ворушить «рот» у такт фразі (плавний старт ШІМ), світло — колір факультету.
//    Кнопка на шапці — замовкнути (звук, мотор і світло зупиняються).
//    Увімкнути з затиснутою кнопкою — навчання іншого брелока (див. README.md).
//
//  Бібліотеки: FastLED 3.9.20 (не 3.10.x), rc-switch (sui77)
//  Плата: ESP32C3 Dev Module, Partition Scheme «Huge APP», USB CDC On Boot «Enabled»
// =====================================================================
#include <Arduino.h>
#include <FastLED.h>
#include <RCSwitch.h>
#include <Preferences.h>
#include <esp_arduino_version.h>
#include "config.h"
#include "audio.h"
#include "mouth.h"

enum Cmd : uint8_t { CMD_GRYFFINDOR, CMD_SLYTHERIN, CMD_START, CMD_FINISH, CMD_COUNT };
static const char *CMD_NAMES[CMD_COUNT] = {"Грифіндор", "Слизерин", "Старт", "Фінал"};
static const uint8_t GROUP_FIRST[CMD_COUNT] = {GRYFFINDOR_FIRST, SLYTHERIN_FIRST, START_FIRST, FINISH_FIRST};
static const uint8_t GROUP_LAST[CMD_COUNT] = {GRYFFINDOR_LAST, SLYTHERIN_LAST, START_LAST, FINISH_LAST};
// Кольори: основний і «іскри»
static const CRGB COLOR_MAIN[CMD_COUNT] = {CRGB(200, 0, 0), CRGB(0, 160, 40), CRGB(90, 0, 200), CRGB(255, 150, 0)};
static const CRGB COLOR_SPARK[CMD_COUNT] = {CRGB(255, 170, 0), CRGB(180, 180, 200), CRGB(0, 120, 255), CRGB(255, 255, 160)};

RCSwitch rf;
Preferences prefs;
uint32_t fobCode[CMD_COUNT] = {FOB_GRYFFINDOR, FOB_SLYTHERIN, FOB_START, FOB_FINISH};
uint8_t nextTrack[CMD_COUNT] = {GRYFFINDOR_FIRST, SLYTHERIN_FIRST, START_FIRST, FINISH_FIRST};

CRGB leds[LED_COUNT > 0 ? LED_COUNT : 1];
CRGB leds2[LED2_COUNT > 0 ? LED2_COUNT : 1];

// ---- стан ----
int8_t activeCmd = -1;          // яка група говорить / світиться (-1 = чекаємо)
uint8_t mouthTrack = 0;         // 0 = рот не рухається
uint32_t talkStart = 0;         // коли стартувала фраза
uint32_t glowUntil = 0;         // до коли світитися кольором факультету після фрази
bool learning = false;
uint8_t learnStep = 0;

// ---------------------------------------------------------------------
//  Мотор: ШІМ з плавним стартом
// ---------------------------------------------------------------------
#define MOTOR_MAX ((uint32_t)MOTOR_DUTY * 255 / 100)
float motorDuty = 0;
bool motorOn = false;
uint32_t motorTick = 0;

void motorWrite(uint8_t d) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_MOTOR, d);
#else
  ledcWrite(0, d);
#endif
}

void motorBegin() {
  pinMode(PIN_MOTOR, OUTPUT);
  digitalWrite(PIN_MOTOR, LOW);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_MOTOR, MOTOR_FREQ, 8);
#else
  ledcSetup(0, MOTOR_FREQ, 8);
  ledcAttachPin(PIN_MOTOR, 0);
#endif
  motorWrite(0);
}

void motorUpdate(uint32_t now) {
  uint32_t dt = now - motorTick;
  if (dt == 0) return;
  motorTick = now;
  float target = motorOn ? MOTOR_MAX : 0;
  if (motorDuty < target) {
    motorDuty += (float)MOTOR_MAX * dt / (MOTOR_RAMP_MS > 0 ? MOTOR_RAMP_MS : 1);
    if (motorDuty > target) motorDuty = target;
  } else {
    motorDuty = target;          // вимикаємо одразу — рот закривається пружиною / вагою
  }
  motorWrite((uint8_t)motorDuty);
}

// Рот за таблицею mouth.h: чи має мотор бути увімкнений через t мс від старту фрази
bool mouthOpen(uint8_t track, uint32_t t) {
  if (track == 0 || track >= MOUTH_COUNT || t < MOUTH_DELAY_MS) return false;
  t -= MOUTH_DELAY_MS;
  const Mouth &m = MOUTHS[track];
  for (uint8_t i = 0; i < m.count; i++) {
    if (t < m.steps[i]) return (i & 1) == 0;   // парні кроки — увімкнено
    t -= m.steps[i];
  }
  return false;
}

// ---------------------------------------------------------------------
//  Команди
// ---------------------------------------------------------------------
void say(Cmd c) {
  uint8_t t = nextTrack[c];
  nextTrack[c] = (t >= GROUP_LAST[c]) ? GROUP_FIRST[c] : t + 1;
  activeCmd = c;
  mouthTrack = t;
  talkStart = millis();
  glowUntil = 0;
  play(t);
  Serial.printf("%s: фраза %u\n", CMD_NAMES[c], t);
}

void stopAll() {
  play(0);
  mouthTrack = 0;
  motorOn = false;
  activeCmd = -1;
  glowUntil = 0;
  Serial.println("Стоп");
}

// ---------------------------------------------------------------------
//  Брелок
// ---------------------------------------------------------------------
void loadFob() {
  prefs.begin("hat", true);
  for (int i = 0; i < CMD_COUNT; i++) fobCode[i] = prefs.getUInt((String("code") + i).c_str(), fobCode[i]);
  prefs.end();
}

void saveFob() {
  prefs.begin("hat", false);
  for (int i = 0; i < CMD_COUNT; i++) prefs.putUInt((String("code") + i).c_str(), fobCode[i]);
  prefs.end();
}

void twitch() {                      // коротко смикнути мотор — підтвердження при навчанні
  motorWrite(MOTOR_MAX);
  delay(120);
  motorWrite(0);
}

uint32_t lastCode = 0, lastCodeAt = 0;

void handleRf() {
  if (!rf.available()) return;
  uint32_t code = rf.getReceivedValue();
  unsigned bits = rf.getReceivedBitlength();
  rf.resetAvailable();
  if (code == 0) return;
  uint32_t now = millis();
  if (code == lastCode && now - lastCodeAt < RF_REPEAT_MS) { lastCodeAt = now; return; }
  lastCode = code;
  lastCodeAt = now;
  Serial.printf("Брелок: %lu (%u біт)\n", (unsigned long)code, bits);

  if (learning) {
    for (int i = 0; i < learnStep; i++) if (fobCode[i] == code) return;
    fobCode[learnStep] = code;
    Serial.printf("  %s = %lu\n", CMD_NAMES[learnStep], (unsigned long)code);
    twitch();
    if (++learnStep == CMD_COUNT) {
      saveFob();
      learning = false;
      Serial.println("Брелок збережено");
    }
    return;
  }
  for (int i = 0; i < CMD_COUNT; i++)
    if (code == fobCode[i]) { say((Cmd)i); return; }
}

// ---------------------------------------------------------------------
//  Кнопка
// ---------------------------------------------------------------------
bool btnState = false;
uint32_t btnChanged = 0;

void handleButton(uint32_t now) {
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (pressed == btnState || now - btnChanged < 30) return;
  btnState = pressed;
  btnChanged = now;
  if (pressed) stopAll();
}

// ---------------------------------------------------------------------
//  Світло
// ---------------------------------------------------------------------
void fillAll(const CRGB &c) {
  fill_solid(leds, LED_COUNT > 0 ? LED_COUNT : 1, c);
  fill_solid(leds2, LED2_COUNT > 0 ? LED2_COUNT : 1, c);
}

void sparkle(CRGB *arr, int n, const CRGB &base, const CRGB &spark, uint8_t level) {
  for (int i = 0; i < n; i++) {
    arr[i] = base;
    arr[i].nscale8_video(level);
    if (random8() < 20) arr[i] = spark;
  }
}

void drawLeds(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 20) return;      // 50 кадрів/с
  last = now;
  if (LED_COUNT == 0 && LED2_COUNT == 0) return;

  if (learning) {                   // навчання: блимає білим, кількість спалахів = номер кроку
    uint32_t ph = now % 2000;
    bool on = ph < (learnStep + 1) * 300u && (ph % 300) < 150;
    fillAll(on ? CRGB(120, 120, 120) : CRGB::Black);
  } else if (activeCmd >= 0) {      // говорить або світиться після фрази
    bool talking = audioTrack != 0 || now - talkStart < 300;
    uint8_t level;
    if (talking) {
      level = motorOn ? 255 : 110;  // яскравіше, коли «рот» відкритий
    } else {
      if (glowUntil == 0) glowUntil = now + HOUSE_GLOW_MS;
      if ((int32_t)(now - glowUntil) >= 0) { activeCmd = -1; return; }
      level = 60 + 195 * (glowUntil - now) / HOUSE_GLOW_MS;   // повільно гасне
    }
    sparkle(leds, LED_COUNT, COLOR_MAIN[activeCmd], COLOR_SPARK[activeCmd], level);
    sparkle(leds2, LED2_COUNT, COLOR_MAIN[activeCmd], COLOR_SPARK[activeCmd], level);
  } else {                          // чекаємо: ледь «дихає» фіолетовим
    uint8_t b = 10 + scale8(sin8(now / 16), 50);
    fillAll(CRGB(90, 30, 160).nscale8_video(b));
  }
  FastLED.show();
}

// ---------------------------------------------------------------------
void setup() {
  pinMode(PIN_RF_TX, OUTPUT);       // FS1000A у J6 мовчить; поки працює завантажувач, може коротко передати
  digitalWrite(PIN_RF_TX, LOW);
  Serial.begin(115200);
  motorBegin();                     // першим — щоб мотор точно стояв
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  if (LED_COUNT > 0) FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, LED_COUNT);
  if (LED2_COUNT > 0) FastLED.addLeds<WS2812B, PIN_LEDS2, GRB>(leds2, LED2_COUNT);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.clear(true);

  loadFob();
  delay(50);
  if (digitalRead(PIN_BUTTON) == LOW) {   // увімкнули з затиснутою кнопкою — навчання брелока
    learning = true;
    learnStep = 0;
    btnState = true;
    Serial.println("Навчання брелока: натисніть Грифіндор, Слизерин, Старт, Фінал");
  }

  bool soundOk = audioBegin();
  rf.enableReceive(digitalPinToInterrupt(PIN_RF));
  Serial.printf("Сортувальний капелюх. Звук: %s, фраз: %u\n", soundOk ? "ок" : "помилка I2S", SOUND_COUNT - 1);
}

void loop() {
  uint32_t now = millis();
  handleRf();
  handleButton(now);
  motorOn = mouthTrack && mouthOpen(mouthTrack, now - talkStart);
  if (mouthTrack && now - talkStart > MOUTH_DELAY_MS + 30000) mouthTrack = 0;   // запобіжник
  motorUpdate(now);
  drawLeds(now);
  delay(1);
}
