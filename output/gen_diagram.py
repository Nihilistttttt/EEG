# -*- coding: utf-8 -*-
"""
EEG System Block Diagram v2 — Processing Pipeline with Function Names
Shows the complete data flow from ADS1299 acquisition through V3F signal
processing, IPC to V5F inference, WiFi transport, and Android display.
"""
from PIL import Image, ImageDraw, ImageFont

W, H = 3200, 2200
img = Image.new('RGB', (W, H), '#FAFAFA')
draw = ImageDraw.Draw(img)

fp = 'C:/Windows/Fonts/msyh.ttc'
FS_TITLE = 38; FS_SECTION = 30; FS_BOX = 24; FS_FUNC = 19; FS_PORT = 20; FS_NOTE = 17; FS_SMALL = 15
ft_title = ImageFont.truetype(fp, FS_TITLE)
ft_section = ImageFont.truetype(fp, FS_SECTION)
ft_box = ImageFont.truetype(fp, FS_BOX)
ft_func = ImageFont.truetype(fp, FS_FUNC)
ft_port = ImageFont.truetype(fp, FS_PORT)
ft_note = ImageFont.truetype(fp, FS_NOTE)
ft_small = ImageFont.truetype(fp, FS_SMALL)

NAVY = '#1A237E'; BLUE = '#1565C0'; TEAL = '#00796B'; GREEN = '#2E7D32'
ORANGE = '#E65100'; PURPLE = '#6A1B9A'; GRAY = '#546E7A'; DARK = '#212121'
RED = '#C62828'; LIGHT_BLUE = '#E3F2FD'; LIGHT_GREEN = '#E8F5E9'
LIGHT_TEAL = '#E0F2F1'; LIGHT_ORANGE = '#FBE9E7'; LIGHT_PURPLE = '#F3E5F5'
LIGHT_GRAY = '#F5F5F5'; MID_GRAY = '#90A4AE'

def rrect(x1, y1, x2, y2, fill, outline, lw=2, r=10):
    draw.rounded_rectangle([x1, y1, x2, y2], radius=r, fill=fill, outline=outline, width=lw)

def tc(x, y, t, font=ft_box, fill=DARK):
    draw.text((x, y), t, font=font, fill=fill, anchor='mm')

def tl(x, y, t, font=ft_func, fill='#37474F'):
    draw.text((x, y), t, font=font, fill=fill, anchor='lt')

def draw_arrow(x1, y1, x2, y2, color=GRAY, w=3, head_size=14):
    draw.line([(x1, y1), (x2, y2)], fill=color, width=w)
    dx, dy = x2 - x1, y2 - y1
    ln = (dx ** 2 + dy ** 2) ** 0.5
    if ln == 0: return
    ux, uy = dx / ln, dy / ln
    hs = head_size
    px, py = -uy, ux
    draw.polygon([(x2, y2),
                  (x2 - ux * hs + px * hs * 0.4, y2 - uy * hs + py * hs * 0.4),
                  (x2 - ux * hs - px * hs * 0.4, y2 - uy * hs - py * hs * 0.4)], fill=color)

def draw_biarr(x1, y1, x2, y2, color=GRAY, w=3):
    draw.line([(x1, y1), (x2, y2)], fill=color, width=w)
    dx, dy = x2 - x1, y2 - y1
    ln = (dx ** 2 + dy ** 2) ** 0.5
    if ln == 0: return
    ux, uy = dx / ln, dy / ln
    hs = 14
    px, py = -uy, ux
    draw.polygon([(x2, y2),
                  (x2 - ux * hs + px * hs * 0.4, y2 - uy * hs + py * hs * 0.4),
                  (x2 - ux * hs - px * hs * 0.4, y2 - uy * hs - py * hs * 0.4)], fill=color)
    draw.polygon([(x1, y1),
                  (x1 + ux * hs + px * hs * 0.4, y1 + uy * hs + py * hs * 0.4),
                  (x1 + ux * hs - px * hs * 0.4, y1 + uy * hs - py * hs * 0.4)], fill=color)

def port_label(x1, y1, x2, y2, label, color=GRAY, above=True):
    mx, my = (x1 + x2) // 2, (y1 + y2) // 2
    dx, dy = x2 - x1, y2 - y1
    ln = (dx ** 2 + dy ** 2) ** 0.5
    sign = -1 if above else 1
    nx, ny = (-dy / ln * 20 * sign, dx / ln * 20 * sign) if ln > 0 else (0, -20)
    bbox = draw.textbbox((0, 0), label, font=ft_port)
    tw, th = bbox[2] - bbox[0] + 12, bbox[3] - bbox[1] + 8
    draw.rounded_rectangle([mx + nx - tw // 2, my + ny - th // 2,
                            mx + nx + tw // 2, my + ny + th // 2], radius=4, fill='white')
    draw.text((mx + nx, my + ny), label, font=ft_port, fill=color, anchor='mm')

def pipeline_box(x, y, w, h, title, funcs, fill, outline, title_font=ft_box):
    rrect(x, y, x + w, y + h, fill, outline, 2, 8)
    tc(x + w // 2, y + 18, title, title_font, outline)
    line_h = 22
    start_y = y + 38
    for i, f in enumerate(funcs):
        tl(x + 10, start_y + i * line_h, f, ft_func, '#37474F')

def step_box(x, y, w, h, label, fill=LIGHT_BLUE, outline=BLUE):
    rrect(x, y, x + w, y + h, fill, outline, 2, 8)
    tc(x + w // 2, y + h // 2, label, ft_func, outline)

# ============================================================
# Layout constants
# ============================================================
MX = 50
TY = 80
COL_W = 720
COL_GAP = 40
ROW_H = 200

C1 = MX
C2 = C1 + COL_W + COL_GAP
C3 = C2 + COL_W + COL_GAP
C4 = C3 + COL_W + COL_GAP

# Title
tc(W // 2, 35, 'EEG System Processing Pipeline — CH32H417 Dual-Core + Android', ft_title, NAVY)

# ============================================================
# Column 1: Acquisition Front-End
# ============================================================
cx1 = C1 + COL_W // 2

rrect(C1, TY, C1 + COL_W, TY + 420, LIGHT_BLUE, NAVY, 3)
tc(cx1, TY + 25, 'ADS1299  8ch 24bit EEG', ft_section, NAVY)

ads_steps = [
    ('SPI3+DRDY', 'ring_buffer_get_frame()'),
    ('Parse', 'ADS1299_ParseRawFrame()'),
    ('Volt', 'ADS1299_CodeToVolt()'),
    ('Map', 'OZ/O1/F3/F4/CP3/CP4/C3/C4'),
]
sy = TY + 55
for i, (lbl, fn) in enumerate(ads_steps):
    by = sy + i * 85
    step_box(C1 + 20, by, COL_W - 40, 70, f'{lbl}\n{fn}', LIGHT_BLUE, NAVY)
    if i < len(ads_steps) - 1:
        draw_arrow(cx1, by + 70, cx1, by + 85, NAVY, 2)

# ICM box
icm_y = TY + 440
rrect(C1, icm_y, C1 + COL_W, icm_y + 120, LIGHT_BLUE, NAVY, 3)
tc(cx1, icm_y + 20, 'ICM-42605  6-axis IMU', ft_section, NAVY)
tl(C1 + 20, icm_y + 50, 'SPI2: ICM42605_BCI_Init()', ft_func, NAVY)
tl(C1 + 20, icm_y + 72, 'Posture: Posture_GetResult()', ft_func, NAVY)

# ============================================================
# Column 2: Signal Processing (CH32H417)
# ============================================================
cx2 = C2 + COL_W // 2

# V3F outer frame
v3f_top = TY
v3f_bot = TY + 920
rrect(C2, v3f_top, C2 + COL_W, v3f_bot, '#FAFAFA', BLUE, 4, 12)
tc(cx2, v3f_top + 22, 'CH32H417 — V3F Core (100MHz)', ft_section, BLUE)

# V3F pipeline steps
v3f_steps = [
    ('Drift Remove', 'EEG_RemoveRealtimeDrift()', LIGHT_BLUE, BLUE),
    ('50Hz Notch', 'IIR_SOS_Step(¬ch)', LIGHT_BLUE, BLUE),
    ('Bandpass 2-40Hz', 'IIR_SOS_Step(&bandpass)', LIGHT_BLUE, BLUE),
    ('Ring Buffer', 'RingBuf / RingBufFiltered', LIGHT_BLUE, BLUE),
    ('Waveform Send', 'Send_WaveformSingle()', LIGHT_BLUE, BLUE),
    ('FFT 256pt step128', 'Process_FFT_Step()', LIGHT_BLUE, BLUE),
    ('Band Power', 'compute_band_powers()', LIGHT_BLUE, BLUE),
    ('6ch Weighted Focus', 'attention_engine_process()', LIGHT_GREEN, GREEN),
    ('Focus Send', 'Send_Focus()', LIGHT_GREEN, GREEN),
    ('Spectrum Send', 'Send_Spectrum()', LIGHT_BLUE, BLUE),
]

sy = v3f_top + 48
step_h = 78
gap_h = 8
for i, (lbl, fn, bg, ol) in enumerate(v3f_steps):
    by = sy + i * (step_h + gap_h)
    rrect(C2 + 15, by, C2 + COL_W - 15, by + step_h, bg, ol, 2, 6)
    tc(C2 + 15 + (COL_W - 30) // 2, by + 22, lbl, ft_box, ol)
    tc(C2 + 15 + (COL_W - 30) // 2, by + 50, fn, ft_func, '#455A64')
    if i < len(v3f_steps) - 1:
        draw_arrow(cx2, by + step_h, cx2, by + step_h + gap_h, ol, 2, 10)

# IPC section
ipc_y = v3f_bot + 20
rrect(C2, ipc_y, C2 + COL_W, ipc_y + 100, LIGHT_PURPLE, PURPLE, 3, 10)
tc(cx2, ipc_y + 22, 'IPC Shared Memory', ft_section, PURPLE)
tl(C2 + 20, ipc_y + 50, 'V3F: DualCore_IPC_SendFrameFromV3F()', ft_func, PURPLE)
tl(C2 + 20, ipc_y + 72, 'V5F: DualCore_V5F_MainLoopProcess()', ft_func, PURPLE)

# V5F outer frame
v5f_top = ipc_y + 120
v5f_bot = v5f_top + 680
rrect(C2, v5f_top, C2 + COL_W, v5f_bot, '#FAFAFA', GREEN, 4, 12)
tc(cx2, v5f_top + 22, 'CH32H417 — V5F Core (400MHz)', ft_section, GREEN)

v5f_steps = [
    ('Preprocess', 'DualCore_V5F_ProcessPreprocess()', LIGHT_GREEN, GREEN),
    ('V5F Drift+Notch+Bandpass', 'DualCore_RemoveRealtimeDrift()\nDualCore_IIR_SOS_Step()', LIGHT_GREEN, GREEN),
    ('Ring Buffer', 'g_v5f_ring[] / g_v5f_csp_ring[]', LIGHT_GREEN, GREEN),
    ('FFT+Feature (Goertzel)', 'DualCore_V5F_ComputeFFTFeature()', LIGHT_GREEN, GREEN),
    ('24-dim Feature', 'DualCore_V5F_LogRatio() x24', LIGHT_GREEN, GREEN),
    ('FFT24 Classifier', 'DualCore_V5F_FFTClassifierRightProb()', LIGHT_ORANGE, ORANGE),
    ('CSP Classifier', 'DualCore_V5F_CSPRightProbability()', LIGHT_ORANGE, ORANGE),
    ('Auto Model Select', 'DualCore_V5F_RunClassifier()', LIGHT_ORANGE, ORANGE),
]

sy = v5f_top + 48
for i, (lbl, fn, bg, ol) in enumerate(v5f_steps):
    by = sy + i * (step_h + gap_h)
    rrect(C2 + 15, by, C2 + COL_W - 15, by + step_h, bg, ol, 2, 6)
    tc(C2 + 15 + (COL_W - 30) // 2, by + 22, lbl, ft_box, ol)
    tc(C2 + 15 + (COL_W - 30) // 2, by + 50, fn, ft_func, '#455A64')
    if i < len(v5f_steps) - 1:
        draw_arrow(cx2, by + step_h, cx2, by + step_h + gap_h, ol, 2, 10)

# V3F result path
result_y = v5f_bot + 20
rrect(C2, result_y, C2 + COL_W, result_y + 120, LIGHT_ORANGE, ORANGE, 3, 10)
tc(cx2, result_y + 22, 'V3F: Result Output', ft_section, ORANGE)
tl(C2 + 20, result_y + 50, '4-vote decision: RESULT,INTENT,S_LEFT,S_RIGHT,CONF', ft_func, ORANGE)
tl(C2 + 20, result_y + 72, 'Retry_Store() → WiFi + Debug serial', ft_func, ORANGE)

# ============================================================
# Column 3: Data Transport
# ============================================================
cx3 = C3 + COL_W // 2

# ESP8266
esp_y = TY
rrect(C3, esp_y, C3 + COL_W, esp_y + 200, LIGHT_TEAL, TEAL, 3, 10)
tc(cx3, esp_y + 25, 'ESP8266  WiFi Transparent', ft_section, TEAL)
tl(C3 + 20, esp_y + 60, 'UART: V3F → ESP8266 (binary frames)', ft_func, TEAL)
tl(C3 + 20, esp_y + 82, 'TCP Server: ESP8266 ↔ Doctor (41002)', ft_func, TEAL)
tl(C3 + 20, esp_y + 104, 'Protocol: Pack_Frame() / Parse_Frame()', ft_func, TEAL)
tl(C3 + 20, esp_y + 126, 'Escape: FRAME_CHAR + ESCAPE_CHAR', ft_func, TEAL)
tl(C3 + 20, esp_y + 148, 'Fragment: Pack_Frame_Fragmented()', ft_func, TEAL)

# Python
py_y = esp_y + 230
rrect(C3, py_y, C3 + COL_W, py_y + 160, LIGHT_PURPLE, PURPLE, 3, 10)
tc(cx3, py_y + 25, 'Python Host (41003)', ft_section, PURPLE)
tl(C3 + 20, py_y + 60, 'SSVEP Analysis: FFT on O1 channel', ft_func, PURPLE)
tl(C3 + 20, py_y + 82, 'Command Test: MODE,TRIAL,STOP...', ft_func, PURPLE)
tl(C3 + 20, py_y + 104, 'Training Collect: DIRCSV parsing', ft_func, PURPLE)
tl(C3 + 20, py_y + 126, 'DISPLAY_CFG: channel/spectrum config', ft_func, PURPLE)

# Frame Protocol Detail
proto_y = py_y + 190
rrect(C3, proto_y, C3 + COL_W, proto_y + 260, LIGHT_GRAY, GRAY, 2, 8)
tc(cx3, proto_y + 20, 'Frame Protocol Detail', ft_section, GRAY)
proto_items = [
    'CMD_RAW_WAVE / CMD_FILT_WAVE / CMD_BASELINE_WAVE',
    'CMD_FREQ_SPECTRUM_CH0/1 (freq-filtered)',
    'CMD_FILT_SPECTRUM_CH0/1 (time-filtered)',
    'CMD_RAW_SPECTRUM_CH0/1 (raw FFT)',
    'CMD_FOCUS: attn, relax, ema, trend, instant',
    'RESULT: seq, INTENT, S_LEFT, S_RIGHT, CONF',
    'POSTURE: posture, gravity, confidence',
    'Spectrum: 32 fragments x 4 floats each',
]
for i, item in enumerate(proto_items):
    tl(C3 + 15, proto_y + 48 + i * 24, item, ft_small, '#455A64')

# ============================================================
# Column 4: Android Doctor + Patient
# ============================================================
cx4 = C4 + COL_W // 2

# Android outer
android_top = TY
android_bot = TY + 920
rrect(C4, android_top, C4 + COL_W, android_bot, LIGHT_GRAY, GREEN, 4, 12)
tc(cx4, android_top + 22, 'Android App', ft_section, GREEN)

# Doctor
doc_top = android_top + 48
doc_bot = doc_top + 400
rrect(C4 + 15, doc_top, C4 + COL_W - 15, doc_bot, LIGHT_GREEN, GREEN, 3, 8)
tc(cx4, doc_top + 20, 'Doctor App (41002/41003/41004)', ft_box, GREEN)

doc_funcs = [
    'TcpServerManager: 41002↔MCU, 41003→Python',
    '41004↔Patient: PAGE/SSVEP/TARGET/RESULT',
    '41005 UDP: patient auto-discovery',
    'DataDispatcher: frame routing',
    'FrameParser: Parse_Frame() equivalent',
    'WaveformFragment: reassemble spectrum',
    'EegMonitorFragment: 8ch wave + spectrum',
    'FocusFragment: attention/relaxation curves',
    'MiTrainFragment: MODE/TRIAL/RESULT flow',
    'SsvepFragment: O1 FFT + freq detection',
    'PostureFragment: IMU posture monitoring',
]
for i, f in enumerate(doc_funcs):
    tl(C4 + 25, doc_top + 44 + i * 28, f, ft_func, '#2E7D32')

# Patient
pat_top = doc_bot + 20
pat_bot = pat_top + 380
rrect(C4 + 15, pat_top, C4 + COL_W - 15, pat_bot, LIGHT_ORANGE, ORANGE, 3, 8)
tc(cx4, pat_top + 20, 'Patient App (41004/41005)', ft_box, ORANGE)

pat_funcs = [
    'DoctorConnector: 41004 TCP control',
    '41005 UDP: discover doctor IP',
    'MiArrowFragment: direction arrow guide',
    'SsvepStimulusFragment: full-screen flicker',
    'TARGET→RESULT: dual arrow display',
    'Completely passive: no brain data received',
]
for i, f in enumerate(pat_funcs):
    tl(C4 + 25, pat_top + 44 + i * 28, f, ft_func, '#BF360C')

# ============================================================
# Horizontal Arrows (main data flow)
# ============================================================

# ADS1299 → V3F (SPI3+DMA)
arr_y1 = TY + 160
draw_arrow(C1 + COL_W, arr_y1, C2, arr_y1, NAVY, 3)
port_label(C1 + COL_W, arr_y1, C2, arr_y1, 'SPI3+DMA', NAVY)

# ICM → V3F (SPI2)
arr_y2 = TY + 500
draw_arrow(C1 + COL_W, arr_y2, C2, arr_y2 + 100, NAVY, 2)
port_label(C1 + COL_W, arr_y2, C2, arr_y2 + 100, 'SPI2', NAVY)

# V3F → ESP8266 (UART)
arr_y3 = TY + 300
draw_arrow(C2 + COL_W, arr_y3, C3, arr_y3, TEAL, 3)
port_label(C2 + COL_W, arr_y3, C3, arr_y3, 'UART', TEAL)

# ESP8266 ↔ Doctor (41002)
arr_y4 = TY + 120
draw_biarr(C3 + COL_W, arr_y4, C4, arr_y4, GREEN, 3)
port_label(C3 + COL_W, arr_y4, C4, arr_y4, '41002 TCP', GREEN)

# Doctor → Python (41003)
arr_y5 = TY + 500
draw_arrow(C4, arr_y5, C3 + COL_W, py_y + 80, PURPLE, 2)
port_label(C4, arr_y5, C3 + COL_W, py_y + 80, '41003', PURPLE)

# ============================================================
# Vertical Arrows (within columns)
# ============================================================

# V3F → IPC
draw_arrow(cx2, v3f_bot, cx2, ipc_y, PURPLE, 3)
port_label(cx2, v3f_bot, cx2, ipc_y, 'IPC notify', PURPLE)

# IPC → V5F
draw_arrow(cx2, ipc_y + 100, cx2, v5f_top, PURPLE, 3)
port_label(cx2, ipc_y + 100, cx2, v5f_top, 'IPC ACK', PURPLE)

# V5F → V3F result (feedback arrow on left side)
feedback_x = C2 + 8
draw_arrow(feedback_x, v5f_top, feedback_x, result_y + 60, ORANGE, 2, 10)
tl(feedback_x + 5, v5f_top + 70, 'V5F pred', ft_small, ORANGE)
tl(feedback_x + 5, v5f_top + 86, 'via IPC slot', ft_small, ORANGE)

# Doctor ↔ Patient (41004)
pat_arr_x = C4 + COL_W - 60
draw_biarr(pat_arr_x, doc_bot, pat_arr_x, pat_top, ORANGE, 3)
port_label(pat_arr_x, doc_bot, pat_arr_x, pat_top, '41004', ORANGE)

# Patient → Doctor UDP (41005)
udp_x = C4 + COL_W - 15
draw_arrow(udp_x, pat_top + 100, udp_x, doc_bot - 20, MID_GRAY, 2, 10)
tl(udp_x - 5, (pat_top + 100 + doc_bot - 20) // 2 - 8, '41005', ft_small, MID_GRAY)
tl(udp_x - 5, (pat_top + 100 + doc_bot - 20) // 2 + 8, 'UDP', ft_small, MID_GRAY)

# ============================================================
# Data flow annotation (right margin)
# ============================================================
note_x = C4 + COL_W + 30
note_y = TY
notes = [
    ('Data Flow Summary:', ft_section, NAVY),
    ('', ft_small, DARK),
    ('1. ADS1299 8ch@250SPS → SPI3+DMA → V3F', ft_func, BLUE),
    ('2. V3F: Drift→Notch→Bandpass→RingBuf', ft_func, BLUE),
    ('3. V3F: FFT 256pt → Band Power → Focus', ft_func, BLUE),
    ('4. V3F: Waveform/Spectrum/Focus → WiFi', ft_func, BLUE),
    ('5. V3F: Raw frame → IPC → V5F', ft_func, PURPLE),
    ('6. V5F: Drift→Notch→Bandpass→RingBuf', ft_func, GREEN),
    ('7. V5F: Goertzel FFT → 24-dim feature', ft_func, GREEN),
    ('8. V5F: FFT24/CSP classifier → P(right)', ft_func, GREEN),
    ('9. V5F result → IPC slot → V3F', ft_func, ORANGE),
    ('10. V3F: 4-vote RESULT → WiFi → Android', ft_func, ORANGE),
    ('', ft_small, DARK),
    ('Focus Weights:', ft_section, NAVY),
    ('F3=0.18 F4=0.18', ft_func, BLUE),
    ('CP3=0.19 CP4=0.19', ft_func, BLUE),
    ('C3=0.13 C4=0.13', ft_func, BLUE),
    ('OZ=0 O1=0 (SSVEP only)', ft_func, BLUE),
    ('', ft_small, DARK),
    ('V5F Feature (24-dim):', ft_section, NAVY),
    ('LogRatio(CP3/CP4) x {mu,beta,theta,total}', ft_func, GREEN),
    ('LogRatio(C3/C4) x {mu,beta,theta,total}', ft_func, GREEN),
    ('LogRatio(mu/total) x {CP3,CP4,C3,C4}', ft_func, GREEN),
    ('LogRatio(beta/total) x {CP3,CP4,C3,C4}', ft_func, GREEN),
    ('LogRatio(beta/mu) x {CP3,CP4,C3,C4}', ft_func, GREEN),
    ('LogRatio(CP3/C3) x {mu,beta}', ft_func, GREEN),
    ('LogRatio(CP4/C4) x {mu,beta}', ft_func, GREEN),
    ('', ft_small, DARK),
    ('Decision Logic:', ft_section, NAVY),
    ('4 consecutive inferences (2s window)', ft_func, ORANGE),
    ('Average features → SVM → P(right)', ft_func, ORANGE),
    ('RESULT: INTENT, S_LEFT, S_RIGHT, CONF', ft_func, ORANGE),
    ('Retry_Store() with ACK from Android', ft_func, ORANGE),
]
for i, (text, font, color) in enumerate(notes):
    tl(note_x, note_y + i * 26, text, font, color)

# ============================================================
# Dashed column separators
# ============================================================
for x in [C1 + COL_W + COL_GAP // 2, C2 + COL_W + COL_GAP // 2, C3 + COL_W + COL_GAP // 2]:
    for y in range(70, H - 30, 18):
        draw.line([(x, y), (x, y + 9)], fill='#CFD8DC', width=1)

out = r'D:\Libraries\Projects\EEG\output\system_block_diagram.png'
img.save(out)
print('Saved to', out)
