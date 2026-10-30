"""Звуки шапки: firmware/sounds/0001…0018.mp3 → firmware/hat/sounds.h (вбудовуються в прошивку).

Звуки стискаються в IMA ADPCM (4 біти на відлік, 16 кГц): ~8 КБ на секунду, усі 18 фраз ≈ 1,6 МБ —
влазить у флеш ESP32-C3 зі схемою розділів «Huge APP». Без стиснення (PCM) було б ~6 МБ.

Новий / замінений звук — покладіть mp3 (або wav) у firmware/sounds/ під потрібним номером і запустіть:
    python firmware/tools/gen_sounds.py        (потрібен ffmpeg)
Кожен звук нормалізується за гучністю (пік −1 дБ). Номери й групи фраз — firmware/hat/config.h."""
import struct, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / 'sounds'
OUT = ROOT / 'hat' / 'sounds.h'
RATE = 16000
MAX_SECONDS = 25

STEPS = [7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88,
         97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
         724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660,
         4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818,
         18500, 20350, 22385, 24623, 27086, 29794, 32767]
INDEX = [-1, -1, -1, -1, 2, 4, 6, 8]


def decode(path):
    raw = subprocess.run(['ffmpeg', '-loglevel', 'error', '-i', str(path), '-t', str(MAX_SECONDS),
                          '-ac', '1', '-ar', str(RATE), '-f', 's16le', '-'], check=True, capture_output=True).stdout
    samples = struct.unpack(f'<{len(raw) // 2}h', raw)
    peak = max(1, max(abs(s) for s in samples))
    gain = 29200 / peak                                  # пік ≈ −1 дБ
    return [int(max(-32768, min(32767, s * gain))) for s in samples]


def adpcm(samples):
    """IMA ADPCM, старт: прогноз 0, індекс 0; молодший півбайт — перший відлік (так само декодує audio.h)."""
    pred, idx, out, half = 0, 0, bytearray(), None
    for s in samples:
        step = STEPS[idx]
        diff = s - pred
        code = 0
        if diff < 0:
            code, diff = 8, -diff
        delta = step >> 3
        if diff >= step: code |= 4; diff -= step; delta += step
        step >>= 1
        if diff >= step: code |= 2; diff -= step; delta += step
        step >>= 1
        if diff >= step: code |= 1; delta += step
        pred = max(-32768, min(32767, pred - delta if code & 8 else pred + delta))
        idx = max(0, min(88, idx + INDEX[code & 7]))
        if half is None:
            half = code
        else:
            out.append(half | (code << 4)); half = None
    if half is not None:
        out.append(half)
    return bytes(out)


files = sorted(p for p in SRC.iterdir() if p.suffix.lower() in ('.mp3', '.wav') and p.stem.isdigit())
if not files:
    sys.exit(f'Немає звуків у {SRC}')
count = max(int(p.stem) for p in files)
lines = ['// Згенеровано tools/gen_sounds.py з firmware/sounds/ — не редагувати вручну.', '#pragma once',
         '#include <Arduino.h>', '', f'#define SOUND_RATE {RATE}', '',
         '// IMA ADPCM: 2 відліки в байті, молодший півбайт — перший',
         'struct Sound { const uint8_t *data; uint32_t samples; };', '']
total = 0
present = set()
for p in files:
    n = int(p.stem)
    present.add(n)
    pcm = decode(p)
    data = adpcm(pcm)
    total += len(data)
    print(f'{p.name}: {len(pcm) / RATE:5.2f} с, {len(data) // 1024} КБ')
    lines.append(f'// {n}: {p.name}, {len(pcm) / RATE:.2f} с')
    lines.append(f'static const uint8_t SOUND_{n}[{len(data)}] = {{')
    for i in range(0, len(data), 32):
        lines.append(','.join(str(b) for b in data[i:i + 32]) + ',')
    lines.append('};')
    lines.append(f'#define SOUND_{n}_SAMPLES {len(pcm)}')
lines += ['', 'static const Sound SOUNDS[] = {', '  {nullptr, 0},']
lines += [f'  {{SOUND_{n}, SOUND_{n}_SAMPLES}},' if n in present else '  {nullptr, 0},' for n in range(1, count + 1)]
lines += ['};', 'static const uint8_t SOUND_COUNT = sizeof(SOUNDS) / sizeof(SOUNDS[0]);', '']
OUT.write_text('\n'.join(lines), encoding='utf-8')
print(f'{OUT} — разом {total // 1024} КБ')
