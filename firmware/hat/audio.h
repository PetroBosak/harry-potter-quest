// Звук через I2S-підсилювач MAX98357A. Звуки вбудовані в прошивку (sounds.h, IMA ADPCM).
// Грає у фоновій задачі — не заважає мотору і світлу. Новий звук перебиває поточний.
#pragma once
#include <Arduino.h>
#include <esp_arduino_version.h>
#include "config.h"
#include "sounds.h"

#if ESP_ARDUINO_VERSION_MAJOR >= 3
#include <ESP_I2S.h>
static I2SClass i2sOut;
static bool i2sBegin() {
  i2sOut.setPins(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DIN);
  return i2sOut.begin(I2S_MODE_STD, SOUND_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
}
static void i2sWrite(const int16_t *buf, size_t bytes) { i2sOut.write((const uint8_t *)buf, bytes); }
#else
#include <driver/i2s.h>
static bool i2sBegin() {
  i2s_config_t cfg = {};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate = SOUND_RATE;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.dma_buf_count = 8;
  cfg.dma_buf_len = 256;
  cfg.tx_desc_auto_clear = true;
  if (i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL) != ESP_OK) return false;
  i2s_pin_config_t pins = {};
  pins.mck_io_num = I2S_PIN_NO_CHANGE;
  pins.bck_io_num = PIN_I2S_BCLK;
  pins.ws_io_num = PIN_I2S_LRC;
  pins.data_out_num = PIN_I2S_DIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  return i2s_set_pin(I2S_NUM_0, &pins) == ESP_OK;
}
static void i2sWrite(const int16_t *buf, size_t bytes) {
  size_t written;
  i2s_write(I2S_NUM_0, buf, bytes, &written, portMAX_DELAY);
}
#endif

// ---- IMA ADPCM ----
static const int16_t ADPCM_STEPS[89] = {
  7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88,
  97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
  724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660,
  4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818,
  18500, 20350, 22385, 24623, 27086, 29794, 32767};
static const int8_t ADPCM_INDEX[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

struct Adpcm { int32_t pred = 0; int idx = 0; };
static inline int16_t adpcmStep(Adpcm &st, uint8_t code) {
  int step = ADPCM_STEPS[st.idx];
  int delta = step >> 3;
  if (code & 4) delta += step;
  if (code & 2) delta += step >> 1;
  if (code & 1) delta += step >> 2;
  st.pred += (code & 8) ? -delta : delta;
  if (st.pred > 32767) st.pred = 32767;
  if (st.pred < -32768) st.pred = -32768;
  st.idx += ADPCM_INDEX[code & 7];
  if (st.idx < 0) st.idx = 0;
  if (st.idx > 88) st.idx = 88;
  return st.pred;
}

static QueueHandle_t audioQueue;
static volatile uint8_t audioTrack = 0;       // що грає зараз (0 = тиша)

static void audioTask(void *) {
  static int16_t buf[512];                  // 256 кадрів стерео (L = R)
  uint8_t track;
  for (;;) {
    xQueueReceive(audioQueue, &track, portMAX_DELAY);
    digitalWrite(PIN_AMP_SD, HIGH);         // увімкнути підсилювач
    for (;;) {                              // грати track; новий у черзі — перебиває
      const Sound *s = &SOUNDS[track];
      audioTrack = s->samples ? track : 0;
      Adpcm st;
      uint32_t pos = 0;
      bool interrupted = false;
      while (pos < s->samples) {
        if (xQueueReceive(audioQueue, &track, 0) == pdTRUE) { interrupted = true; break; }
        uint32_t n = s->samples - pos;
        if (n > 256) n = 256;
        for (uint32_t i = 0; i < n; i++, pos++) {
          uint8_t b = s->data[pos >> 1];
          int16_t v = adpcmStep(st, (pos & 1) ? (b >> 4) : (b & 0x0F));
          v = (int32_t)v * VOLUME / 100;
          buf[2 * i] = v;
          buf[2 * i + 1] = v;
        }
        i2sWrite(buf, n * 4);
      }
      if (!interrupted) break;
      if (!SOUNDS[track].samples) break;    // 0 = «замовкнути»
    }
    audioTrack = 0;
    memset(buf, 0, sizeof(buf));            // трохи тиші, щоб буфер не повторював хвіст
    for (int i = 0; i < 8; i++) i2sWrite(buf, sizeof(buf));
    if (uxQueueMessagesWaiting(audioQueue) == 0) digitalWrite(PIN_AMP_SD, LOW);   // вимкнути — без шипіння
  }
}

bool audioBegin() {
  pinMode(PIN_AMP_SD, OUTPUT);
  digitalWrite(PIN_AMP_SD, LOW);
  if (!i2sBegin()) return false;
  audioQueue = xQueueCreate(4, 1);
  xTaskCreatePinnedToCore(audioTask, "audio", 4096, NULL, 2, NULL, 0);
  return true;
}

// track = номер звуку (1…), 0 = замовкнути
void play(uint8_t track) {
  if (!audioQueue || track >= SOUND_COUNT) return;
  xQueueSend(audioQueue, &track, 0);
}
