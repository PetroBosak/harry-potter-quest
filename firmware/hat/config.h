// =====================================================================
//  Налаштування шапки «Сортувальний капелюх» (квест «Гаррі Поттер»)
// =====================================================================
#pragma once

// ---- Піни ESP32-C3 SuperMini (плата challenge-duel/pcb, варіант hat; схема docs/wiring-hat.png) ----
// GPIO 2, 8, 9 не використовуємо — від них залежить старт плати.
// GPIO 21 — TX завантажувача (смикається при старті), тому мотор саме на GPIO 20.
#define PIN_MOTOR    20   // ключ мотора: AO3400A через 100 Ом (J5 МОТОР)
#define PIN_LEDS     10   // стрічка WS2812B (U4 → 330 Ом → J2 LED)
#define PIN_LEDS2     7   // друга стрічка / «очі» (U5 → 330 Ом → J4 LED2)
#define PIN_BUTTON    5   // кнопка (NO → GND, підтяжка 10 кОм до 3V3) — J3
#define PIN_RF        6   // приймач 433 МГц SYN480R, DATA
#define PIN_I2S_BCLK  4   // MAX98357A BCLK
#define PIN_I2S_LRC   3   // MAX98357A LRC
#define PIN_I2S_DIN   1   // MAX98357A DIN
#define PIN_AMP_SD    0   // MAX98357A SD: HIGH = грає, LOW = вимкнений (без шипіння)

// ---- Мотор («рот» капелюха) ----
#define MOTOR_DUTY     100   // потужність мотора, % (менше — повільніше / тихіше)
#define MOTOR_RAMP_MS   80   // плавний старт: за стільки мс від 0 до MOTOR_DUTY
#define MOTOR_FREQ   20000   // частота ШІМ, Гц (20 кГц — не пищить)
#define MOUTH_DELAY_MS 100   // рот починає рухатись через стільки мс після старту фрази (як у старій шапці)

// ---- Світло (0 = стрічки немає) ----
#define LED_COUNT     16    // світлодіодів на J2 (GPIO10)
#define LED2_COUNT     0    // світлодіодів на J4 (GPIO7), напр. 2 «очі»
#define BRIGHTNESS   120    // 0–255
#define MAX_MILLIAMPS 1500  // ліміт струму світлодіодів, мА
#define HOUSE_GLOW_MS 4000  // після фрази факультету капелюх світиться його кольором ще стільки мс

// ---- Звук: вбудований у прошивку (firmware/sounds/*.mp3 → sounds.h) ----
#define VOLUME 100          // 0–100 %. Грубіше — резистором GAIN на MAX98357A

// ---- Фрази: групи треків. Кожне натискання — наступна фраза групи по колу ----
#define GRYFFINDOR_FIRST  1   // 0001…0007 — Грифіндор
#define GRYFFINDOR_LAST   7
#define SLYTHERIN_FIRST   8   // 0008…0014 — Слизерин
#define SLYTHERIN_LAST   14
#define START_FIRST      15   // 0015…0016 — привітання / початок
#define START_LAST       16
#define FINISH_FIRST     17   // 0017…0018 — фінал
#define FINISH_LAST      18

// ---- Брелок 433 МГц ----
// Коди кнопок старого брелока (зі старої прошивки hate_hogrards.ino) — працюють одразу.
// Інший брелок: увімкнути шапку з затиснутою кнопкою і натиснути на брелоку по черзі
// Грифіндор → Слизерин → Старт → Фінал (коди збережуться в пам'яті).
#define FOB_GRYFFINDOR 753828
#define FOB_SLYTHERIN  753826
#define FOB_START      753832
#define FOB_FINISH     753825
#define RF_REPEAT_MS     800  // повтор тієї ж кнопки швидше — ігнорується (брелок шле код кілька разів)
