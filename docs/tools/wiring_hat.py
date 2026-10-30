"""Схема підключення шапки «Сортувальний капелюх» (квест «Гаррі Поттер»).
Плата challenge-duel/pcb, варіант hat: ESP32-C3 SuperMini, приймач SYN480R, MAX98357A, ключ мотора AO3400A.
python docs/tools/wiring_hat.py → docs/wiring-hat.svg (+ wiring-hat.png, якщо є Microsoft Edge)
Зроблено з firmware/tools/wiring_diagram.py репозиторію challenge-duel (схема щита c3syn)."""
import subprocess
from pathlib import Path

W, H = 1240, 880
RED, BLK, OR, DATA, AUD, SIG, MOT = '#e5383b', '#222', '#f28c28', '#1fa3c9', '#8e44ad', '#2a9d4b', '#b5651d'
out = []


def box(x, y, w, h, title, sub='', fill='#f6f4fb'):
    out.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="8" fill="{fill}" stroke="#555" stroke-width="1.5"/>')
    out.append(f'<text x="{x+w/2}" y="{y+20}" text-anchor="middle" font-weight="700" font-size="14">{title}</text>')
    if sub:
        out.append(f'<text x="{x+w/2}" y="{y+37}" text-anchor="middle" font-size="11" fill="#666">{sub}</text>')


def wire(pts, color, label=None, lx=None, ly=None, width=2.5, dash=False):
    d = ' '.join(f'{x},{y}' for x, y in pts)
    da = ' stroke-dasharray="6 4"' if dash else ''
    out.append(f'<polyline points="{d}" fill="none" stroke="{color}" stroke-width="{width}" stroke-linejoin="round"{da}/>')
    if label:
        out.append(f'<text x="{lx}" y="{ly}" font-size="11" fill="{color}" font-weight="700">{label}</text>')


def dot(x, y, c):
    out.append(f'<circle cx="{x}" cy="{y}" r="4" fill="{c}"/>')


def pin(x, y, name, side):
    anchor = 'start' if side == 'l' else 'end'
    tx = x + 8 if side == 'l' else x - 8
    out.append(f'<circle cx="{x}" cy="{y}" r="3.5" fill="#fff" stroke="#333"/>')
    out.append(f'<text x="{tx}" y="{y+4}" text-anchor="{anchor}" font-size="12" font-family="DejaVu Sans Mono, monospace">{name}</text>')


def resistor(x1, x2, y, label, color):
    out.append(f'<rect x="{(x1+x2)/2-16}" y="{y-6}" width="32" height="12" fill="#fff" stroke="{color}" stroke-width="2"/>')
    out.append(f'<text x="{(x1+x2)/2}" y="{y-10}" text-anchor="middle" font-size="11" fill="#333">{label}</text>')


def text(x, y, t, size=12, anchor='start', fill='#333', weight='normal'):
    out.append(f'<text x="{x}" y="{y}" font-size="{size}" text-anchor="{anchor}" fill="{fill}" font-weight="{weight}">{t}</text>')


def cap(x, y1, y2, label=None):
    """Неполярний конденсатор вертикально між y1 і y2 (пластини посередині)."""
    ym = (y1 + y2) / 2
    out.append(f'<line x1="{x-9}" y1="{ym-3}" x2="{x+9}" y2="{ym-3}" stroke="#333" stroke-width="2"/>'
               f'<line x1="{x-9}" y1="{ym+3}" x2="{x+9}" y2="{ym+3}" stroke="#333" stroke-width="2"/>')
    if label:
        text(x + 12, ym + 4, label, 9)


# ---- рейки живлення ----
TOP5, TOPG, BOT5, BOTG = 120, 150, 640, 670
wire([(60, TOP5), (1220, TOP5)], RED, '+5 В', 1180, TOP5 - 6, 3.5)
wire([(80, TOPG), (1220, TOPG)], BLK, 'GND', 1185, TOPG + 16, 3.5)
wire([(60, TOP5), (60, BOT5), (1220, BOT5)], RED, '+5 В', 1180, BOT5 - 6, 3.5)
wire([(80, TOPG), (80, BOTG), (1220, BOTG)], BLK, 'GND', 1185, BOTG + 16, 3.5)

# ---- блок живлення + вимикач ----
box(20, 20, 150, 60, 'БЖ 5 В', 'J1 на платі')
out.append('<rect x="210" y="34" width="60" height="30" rx="5" fill="#fff" stroke="#555"/>')
text(240, 54, 'вимикач', 11, 'middle')
wire([(170, 40), (210, 40)], RED)
wire([(270, 40), (300, 40), (300, TOP5)], RED); dot(300, TOP5, RED)
wire([(150, 80), (150, TOPG)], BLK); dot(150, TOPG, BLK)

# ---- ESP32-C3 SuperMini ----
box(480, 190, 160, 320, 'ESP32-C3', 'SuperMini', '#eef6ff')
L = {'GND': 260, 'GPIO6': 300, '3V3': 340, 'GPIO5': 380, 'GPIO20': 470}   # у порядку виводів SYN480R
R = {'5V': 230, 'GND ': 262, 'GPIO10': 300, 'GPIO7': 330, 'GPIO4': 380, 'GPIO3': 420, 'GPIO1': 460, 'GPIO0': 490}
SILK = {'3V3': '3.3', 'GND': 'G', 'GND ': 'G', '5V': '5V'}   # як надруковано на платі SuperMini
for side, pins, x in (('l', L, 480), ('r', R, 640)):
    for n, y in pins.items():
        pin(x, y, SILK.get(n, n.replace('GPIO', '')), side)
        out[-1] = out[-1].replace('font-size="12"', 'font-size="15" font-weight="700"')
        if n.startswith('GPIO'):
            text(x + 34 if side == 'l' else x - 34, y + 4, n, 9, 'start' if side == 'l' else 'end', fill='#999')
text(560, 528, 'цифри — як надруковано на платі', 10, 'middle', fill='#666')
# діод SS34: +5 В → 5V SuperMini (USB ноутбука і блок можна разом)
wire([(640, 230), (680, 230), (680, TOP5)], RED); dot(680, TOP5, RED)
out.append('<polygon points="671,170 689,170 680,184" fill="#fff" stroke="#333" stroke-width="2"/>')
out.append('<line x1="670" y1="185" x2="690" y2="185" stroke="#333" stroke-width="2.5"/>')
text(694, 182, 'SS34', 10, fill='#333', weight='700')
wire([(640, 262), (700, 262), (700, TOPG)], BLK); dot(700, TOPG, BLK)

# ---- SYN480R ----
box(160, 190, 140, 170, 'SYN480R', '433 МГц')
for n, y in (('ANT', 220), ('GND', 260), ('DAT', 300), ('VCC', 340)): pin(300, y, n, 'r')
wire([(300, 260), (480, 260)], BLK)
wire([(300, 300), (480, 300)], SIG, 'дані 433', 365, 294)
wire([(300, 340), (480, 340)], OR, '3,3 В', 380, 334)
wire([(300, 220), (340, 220), (340, 175), (370, 165)], '#777', width=3)
text(376, 170, 'антена 17,3 см (пад ANT на платі)', 11, fill='#777', weight='700')
text(230, 376, 'квадратний пад = GND', 10, 'middle', fill='#666')

# ---- кнопка ----
out.append('<circle cx="230" cy="420" r="30" fill="#ffd9de" stroke="#555" stroke-width="1.5"/>')
out.append('<circle cx="230" cy="420" r="20" fill="#ff8fa0"/>')
text(230, 470, 'кнопка (J3)', 12, 'middle', weight='700')
text(230, 486, 'NO + COM', 11, 'middle', fill='#666')
wire([(260, 410), (380, 410), (380, 380), (480, 380)], SIG, 'NO', 300, 404)
dot(430, 380, SIG)                                   # підтяжка 10 кОм до 3,3 В (R2 на платі)
wire([(430, 380), (430, 360)], SIG)
out.append('<rect x="424" y="346" width="12" height="14" fill="#fff" stroke="#c97a00" stroke-width="2"/>')
dot(430, 340, OR); wire([(430, 346), (430, 340)], OR)
text(442, 362, '10 кОм', 10)
wire([(200, 430), (140, 430), (140, BOTG)], BLK, 'COM', 150, 424); dot(140, BOTG, BLK)

# ---- рівень 3,3 → 5 В: 74AHCT1G125 (як на платі) ----
box(760, 196, 120, 72, '74AHCT1G125', 'U4 · J2 LED')
pin(760, 255, '2 A', 'l'); pin(880, 255, 'Y 4', 'r')
wire([(800, 196), (800, TOP5)], RED); dot(800, TOP5, RED); text(796, 192, '5', 10, 'end')
wire([(840, 196), (840, TOPG)], BLK); dot(840, TOPG, BLK); text(845, 192, '3, 1', 10)
out.append('<line x1="817" y1="168" x2="817" y2="182" stroke="#333" stroke-width="2"/><line x1="823" y1="168" x2="823" y2="182" stroke="#333" stroke-width="2"/>')
wire([(800, 175), (817, 175)], RED, width=2); wire([(823, 175), (840, 175)], BLK, width=2)
dot(800, 175, RED); dot(840, 175, BLK)
text(820, 164, '100 нФ', 9, 'middle')
box(760, 278, 120, 64, '74AHCT1G125', 'U5 · J4 LED2')
pin(760, 330, '2 A', 'l'); pin(880, 330, 'Y 4', 'r')
wire([(640, 300), (720, 300), (720, 255), (760, 255)], DATA, 'дані LED', 650, 294)
wire([(640, 330), (760, 330)], DATA, 'LED2', 672, 324)
# 1000 мкФ (C1) — він же накопичувальний для мотора
out.append('<line x1="900" y1="182" x2="920" y2="182" stroke="#333" stroke-width="2"/><line x1="900" y1="190" x2="920" y2="190" stroke="#333" stroke-width="2"/>')
wire([(910, TOP5), (910, 182)], RED); wire([(910, 190), (910, 200), (928, 200), (928, TOPG)], BLK)
dot(910, TOP5, RED); dot(928, TOPG, BLK)
text(935, 205, '1000 мкФ (C1)', 10)


# ---- стрічки WS2812B ----
def strip(x, y, n, label, dash=False):
    da = ' stroke-dasharray="6 4"' if dash else ''
    out.append(f'<rect x="{x}" y="{y}" width="{n*26+10}" height="34" rx="4" fill="#111" stroke="#555"{da}/>')
    for i in range(n):
        out.append(f'<rect x="{x+8+i*26}" y="{y+8}" width="18" height="18" rx="3" fill="#9b5de5"/>')
    text(x + 4, y + 50, label, 11, weight='700')


strip(980, 238, 7, 'стрічка WS2812B (J2, LED_COUNT)')
wire([(880, 255), (980, 255)], DATA); resistor(895, 955, 255, '330 Ом', DATA)
text(982, 232, 'DIN', 10, fill=DATA, weight='700')
wire([(1172, 248), (1210, 248), (1210, TOP5)], RED); dot(1210, TOP5, RED); text(1214, 244, '+5', 10, fill=RED)
wire([(1172, 262), (1196, 262), (1196, TOPG)], BLK); dot(1196, TOPG, BLK)
strip(980, 318, 3, 'необов\'язково (J4, LED2_COUNT)', dash=True)
wire([(880, 330), (980, 330)], DATA, dash=True); resistor(895, 955, 330, '330 Ом', DATA)

# ---- мотор: ключ AO3400A по мінусу (J5 МОТОР) ----
MX0 = 1150
wire([(MX0, 442), (MX0, 420)], RED)
wire([(1100, 420), (1210, 420), (1210, BOT5)], RED, '+5 В (M+)', 1112, 413); dot(1210, BOT5, RED)
dot(MX0, 420, RED)
out.append(f'<circle cx="{MX0}" cy="470" r="28" fill="#fff3d6" stroke="#555" stroke-width="2"/>')
text(MX0, 477, 'M', 20, 'middle', weight='700')
text(MX0 + 34, 450, 'мотор', 11, weight='700')
text(MX0 + 34, 463, '«рота»', 11, weight='700')
wire([(MX0, 498), (MX0, 545)], MOT, 'M−', MX0 + 6, 530, 3)
# 100 нФ прямо на клемах мотора
wire([(1190, 420), (1190, 487)], RED, width=2); dot(1190, 420, RED)
cap(1190, 487, 497)
wire([(1190, 497), (1190, 512), (MX0, 512)], MOT, width=2); dot(MX0, 512, MOT)
text(1196, 510, '100 нФ', 9)
# діод Шотткі паралельно мотору: катод до +5 В
wire([(1100, 420), (1100, 463)], '#555', width=2)
wire([(1100, 478), (1100, 522), (MX0, 522)], '#555', width=2); dot(MX0, 522, MOT)
out.append('<polygon points="1091,478 1109,478 1100,464" fill="#fff" stroke="#333" stroke-width="2"/>')
out.append('<line x1="1090" y1="463" x2="1110" y2="463" stroke="#333" stroke-width="2.5"/>')
text(1094, 497, 'SS34', 10, 'end', weight='700')
# MOSFET
box(1110, 545, 80, 60, 'AO3400A', 'Q1, SOT-23')
text(MX0 + 8, 541, 'D', 10, weight='700'); text(1104, 580, 'G', 10, 'end', weight='700'); text(MX0 + 6, 620, 'S', 10, weight='700')
wire([(MX0, 605), (MX0, BOTG)], BLK); dot(MX0, BOTG, BLK)
# GPIO20 → 100 Ом → затвор; 10 кОм затвор → GND
wire([(480, 470), (450, 470), (450, 590), (1110, 590)], SIG, 'GPIO20 → мотор (ШІМ 20 кГц)', 460, 584)
resistor(1040, 1100, 590, '100 Ом', SIG)
dot(1020, 590, SIG)
wire([(1020, 590), (1020, 600)], SIG, width=2)
out.append('<rect x="1014" y="600" width="12" height="26" fill="#fff" stroke="#555" stroke-width="2"/>')
wire([(1020, 626), (1020, BOTG)], '#555', width=2); dot(1020, BOTG, BLK)
text(1008, 618, '10 кОм', 10, 'end')

# ---- MAX98357A ----
box(780, 346, 120, 184, 'MAX98357A')
text(840, 524, 'I2S, 3 Вт', 10, 'middle', fill='#666')
pin(780, 380, 'BCLK', 'l'); pin(780, 420, 'LRC', 'l'); pin(780, 460, 'DIN', 'l'); pin(780, 490, 'SD', 'l')
pin(900, 510, 'GAIN', 'r'); pin(900, 450, '+', 'r'); pin(900, 480, '−', 'r')
wire([(640, 380), (780, 380)], AUD, 'BCLK', 700, 374)
wire([(640, 420), (780, 420)], AUD, 'LRC', 700, 414)
wire([(640, 460), (780, 460)], AUD, 'DIN', 700, 454)
wire([(640, 490), (780, 490)], AUD, 'SD (вкл/викл)', 655, 508)
wire([(810, 530), (810, BOT5)], RED); dot(810, BOT5, RED); text(815, 548, 'VIN', 10)
wire([(850, 530), (850, BOTG)], BLK); dot(850, BOTG, BLK); text(855, 548, 'GND', 10)
# GAIN: 100 кОм на GND = 15 дБ
wire([(900, 510), (930, 510), (930, 525)], '#555')
out.append('<rect x="924" y="525" width="12" height="30" fill="#fff" stroke="#555" stroke-width="2"/>')
wire([(930, 555), (930, 575)], '#555'); out.append('<line x1="920" y1="575" x2="940" y2="575" stroke="#222" stroke-width="3"/>')
text(942, 542, '100 кОм', 10); text(942, 554, '→ 15 дБ', 10, fill='#777')
out.append('<circle cx="1010" cy="465" r="30" fill="#eee" stroke="#555" stroke-width="1.5"/><circle cx="1010" cy="465" r="14" fill="#bbb"/>')
text(1010, 512, 'динамік 4–8 Ом', 11, 'middle', weight='700')
wire([(900, 450), (980, 450)], '#555'); wire([(900, 480), (980, 480)], '#555')

# ---- заголовок і примітки ----
text(W / 2, 25, 'Сортувальний капелюх — схема підключення · ESP32-C3 SuperMini · SYN480R', 18, 'middle', weight='700')
notes = [
    'Крапка = з\'єднання. Перетин без крапки = дроти не з\'єднані. Так само розведена плата challenge-duel/pcb/shield67-hat.',
    'Усі GND спільні. Мотор і стрічки живляться від +5 В блока (клемник J1), не від ESP32.',
    'Мотор: GPIO20 → 100 Ом → затвор AO3400A, 10 кОм затвор → GND (мотор стоїть, поки ESP стартує). Мотор — між M+ (+5 В) і M− (стік), J5.',
    'SS34 паралельно мотору (катод до +5 В) гасить кидок від мотора; 100 нФ на клемах (ще один варто припаяти прямо на мотор).',
    'SYN480R: VCC → 3,3 В, DAT → GPIO6, ANT → провід 17,3 см. Кнопка J3: GPIO5 → кнопка → GND (підтяжка 10 кОм на платі).',
    'Стрічка WS2812B: GPIO10 → U4 → 330 Ом → J2. Друга (необов\'язково): GPIO7 → U5 → 330 Ом → J4. Кількість — LED_COUNT / LED2_COUNT у config.h.',
    'MAX98357A: динамік на «+» і «−» (НЕ на GND), GAIN через 100 кОм на GND = 15 дБ. Діод SS34 (D1): можна разом блок і USB ноутбука.',
    'Увімкнути шапку з затиснутою кнопкою → навчання брелока (Грифіндор, Слизерин, Старт, Фінал). Кнопка в роботі — замовкнути.',
]
for i, n in enumerate(notes):
    text(20, 708 + i * 18, '• ' + n, 12, fill='#444')

svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}" font-family="DejaVu Sans, Arial">
<rect width="{W}" height="{H}" fill="#fff"/>
{chr(10).join(out)}
</svg>'''
DOCS = Path(__file__).resolve().parent.parent
p = DOCS / 'wiring-hat.svg'
p.write_text(svg, encoding='utf-8')
print(p)
EDGE = Path(r'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe')
if EDGE.exists():
    png = DOCS / 'wiring-hat.png'
    tmp = Path(__import__('tempfile').gettempdir()) / 'edge-headless'
    subprocess.run([str(EDGE), '--headless=new', '--disable-gpu', '--hide-scrollbars', f'--window-size={W},{H}',
                    f'--user-data-dir={tmp}', f'--screenshot={png}', p.as_uri()], capture_output=True, timeout=60)
    print(png)
