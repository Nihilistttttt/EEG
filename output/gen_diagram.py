# -*- coding: utf-8 -*-
"""
EEG System Block Diagram v6
- Frame Protocol as standalone column between V3F and ESP8266
- Bidirectional: Pack (V3F->) and Parse (->V3F commands)
- All arrows are H/V only, no diagonals
- Command path: Doctor -> ESP8266 -> Parse -> V3F
"""
from PIL import Image, ImageDraw, ImageFont

W, H = 4200, 2100
img = Image.new('RGB', (W, H), '#FAFAFA')
draw = ImageDraw.Draw(img)

fp = 'C:/Windows/Fonts/msyh.ttc'
FT_TITLE = ImageFont.truetype(fp, 34)
FT_SUBG  = ImageFont.truetype(fp, 26)
FT_NODE  = ImageFont.truetype(fp, 21)
FT_SPEC  = ImageFont.truetype(fp, 17)
FT_ARROW = ImageFont.truetype(fp, 18)
FT_SMALL = ImageFont.truetype(fp, 15)

C_NAVY='#1A237E'; C_BLUE='#1565C0'; C_TEAL='#00796B'; C_GREEN='#2E7D32'
C_ORANGE='#E65100'; C_PURPLE='#6A1B9A'; C_GRAY='#546E7A'; C_DARK='#263238'
C_RED='#C62828'
BG_BLUE='#E3F2FD'; BG_GREEN='#E8F5E9'; BG_TEAL='#E0F2F1'
BG_ORANGE='#FFF3E0'; BG_PURPLE='#F3E5F5'; BG_GRAY='#ECEFF1'

def rrect(x1,y1,x2,y2,fill,outline,lw=2,r=10):
    draw.rounded_rectangle([x1,y1,x2,y2],radius=r,fill=fill,outline=outline,width=lw)

def subgraph(x,y,w,h,title,color=BG_GRAY,outline=C_GRAY):
    rrect(x,y,x+w,y+h,fill=color,outline=outline,lw=2,r=14)
    bbox=draw.textbbox((0,0),title,font=FT_SUBG)
    tw=bbox[2]-bbox[0]+20; th=bbox[3]-bbox[1]+10
    rrect(x+12,y-th//2,x+12+tw,y+th//2,fill='#FAFAFA',outline=outline,lw=1,r=6)
    draw.text((x+12+tw//2,y),title,font=FT_SUBG,fill=outline,anchor='mm')

def node(x,y,w,h,lines,fill='white',outline=C_DARK,lw=2):
    rrect(x,y,x+w,y+h,fill=fill,outline=outline,lw=lw,r=8)
    n=len(lines)
    total_h=n*24 if n>1 else 24
    start_y=y+(h-total_h)//2
    for i,line in enumerate(lines):
        font=FT_NODE if i==0 else FT_SPEC
        col=outline if i==0 else '#455A64'
        draw.text((x+w//2,start_y+i*24+12),line,font=font,fill=col,anchor='mm')

def h_arrow(x1,y1,x2,y2,color=C_GRAY,w=2,head=12):
    """Horizontal-only arrow (same y)."""
    draw.line([(x1,y1),(x2,y2)],fill=color,width=w)
    dx=x2-x1; s=1 if dx>0 else -1
    draw.polygon([(x2,y2),(x2-s*head,y2-head*0.4),(x2-s*head,y2+head*0.4)],fill=color)

def h_biarrow(x1,y1,x2,y2,color=C_GRAY,w=2):
    """Horizontal bidirectional arrow."""
    draw.line([(x1,y1),(x2,y2)],fill=color,width=w)
    dx=x2-x1; s=1 if dx>0 else -1; hs=12
    draw.polygon([(x2,y2),(x2-s*hs,y2-hs*0.4),(x2-s*hs,y2+hs*0.4)],fill=color)
    draw.polygon([(x1,y1),(x1+s*hs,y1-hs*0.4),(x1+s*hs,y1+hs*0.4)],fill=color)

def larrow(points,color=C_GRAY,w=2,head=12):
    for i in range(len(points)-1):
        draw.line([(points[i][0],points[i][1]),(points[i+1][0],points[i+1][1])],fill=color,width=w)
    x1,y1=points[-2]; x2,y2=points[-1]
    dx,dy=x2-x1,y2-y1; ln=(dx**2+dy**2)**0.5
    if ln==0: return
    ux,uy=dx/ln,dy/ln; px,py=-uy,ux
    draw.polygon([(x2,y2),
        (x2-ux*head+px*head*0.4,y2-uy*head+py*head*0.4),
        (x2-ux*head-px*head*0.4,y2-uy*head-py*head*0.4)],fill=color)

def dashed_larrow(points,color=C_GRAY,w=2,head=12,dash_len=10,gap_len=6):
    for i in range(len(points)-1):
        x1,y1=points[i]; x2,y2=points[i+1]
        dx=x2-x1; dy=y2-y1; ln=(dx**2+dy**2)**0.5
        if ln==0: continue
        ux,uy=dx/ln,dy/ln; d=0
        while d<ln:
            seg_end=min(d+dash_len,ln)
            draw.line([(x1+ux*d,y1+uy*d),(x1+ux*seg_end,y1+uy*seg_end)],fill=color,width=w)
            d=seg_end+gap_len
    x1,y1=points[-2]; x2,y2=points[-1]
    dx,dy=x2-x1,y2-y1; ln=(dx**2+dy**2)**0.5
    if ln==0: return
    ux,uy=dx/ln,dy/ln; px,py=-uy,ux
    draw.polygon([(x2,y2),
        (x2-ux*head+px*head*0.4,y2-uy*head+py*head*0.4),
        (x2-ux*head-px*head*0.4,y2-uy*head-py*head*0.4)],fill=color)

def label_at(x,y,label,color=C_GRAY):
    bbox=draw.textbbox((0,0),label,font=FT_ARROW)
    tw=bbox[2]-bbox[0]+10; th=bbox[3]-bbox[1]+6
    draw.rounded_rectangle([x-tw//2,y-th//2,x+tw//2,y+th//2],radius=4,fill='white',outline='#CFD8DC',width=1)
    draw.text((x,y),label,font=FT_ARROW,fill=color,anchor='mm')

# ============================================================
# Title
# ============================================================
draw.text((W//2,30),'EEG System Processing Pipeline  -  CH32H417 Dual-Core + Android',font=FT_TITLE,fill=C_NAVY,anchor='mm')

NW=280; NH=85; NGAP_Y=25

# ============================================================
# Subgraph 1: 采集前端
# ============================================================
sg1_x=50; sg1_y=80; sg1_w=380; sg1_h=360
subgraph(sg1_x,sg1_y,sg1_w,sg1_h,'采集前端',BG_BLUE,C_NAVY)

n_ads_x=sg1_x+50; n_ads_y=sg1_y+50
node(n_ads_x,n_ads_y,NW,NH,['ADS1299','8ch 24bit EEG','SPI3+DMA 250SPS'],'white',C_NAVY)

n_icm_x=sg1_x+50; n_icm_y=n_ads_y+NH+NGAP_Y
node(n_icm_x,n_icm_y,NW,NH,['ICM-42605','6-axis IMU','SPI2'],'white',C_NAVY)

# ============================================================
# Subgraph 2: V3F 时域滤波
# ============================================================
sg2_x=530; sg2_y=80; sg2_w=380; sg2_h=730
subgraph(sg2_x,sg2_y,sg2_w,sg2_h,'V3F \u4E3B\u6838 (100MHz)',BG_BLUE,C_BLUE)

n_drift_x=sg2_x+50; n_drift_y=sg2_y+50
node(n_drift_x,n_drift_y,NW,NH,['Drift Remove','EEG_RemoveRealtimeDrift()','k=0.996'],'white',C_BLUE)

n_notch_x=sg2_x+50; n_notch_y=n_drift_y+NH+NGAP_Y
node(n_notch_x,n_notch_y,NW,NH,['50Hz Notch','IIR_SOS_Step(notch)','2-section IIR'],'white',C_BLUE)

n_bp_x=sg2_x+50; n_bp_y=n_notch_y+NH+NGAP_Y
node(n_bp_x,n_bp_y,NW,NH,['Bandpass 2-40Hz','IIR_SOS_Step(bandpass)','4-section Butterworth'],'white',C_BLUE)

n_ring_x=sg2_x+50; n_ring_y=n_bp_y+NH+NGAP_Y
node(n_ring_x,n_ring_y,NW,NH,['Ring Buffer','RingBuf / RingBufFiltered','256 samples x 8ch'],'white',C_BLUE)

n_posture_x=sg2_x+50; n_posture_y=n_ring_y+NH+NGAP_Y+10
node(n_posture_x,n_posture_y,NW,NH,['Posture Detect','Posture_GetResult()','gravity + confidence'],BG_GREEN,C_GREEN)

for (y_from, y_to) in [(n_drift_y,n_notch_y),(n_notch_y,n_bp_y),(n_bp_y,n_ring_y)]:
    cx=sg2_x+50+NW//2
    draw.line([(cx,y_from+NH),(cx,y_to)],fill=C_BLUE,width=2)
    draw.polygon([(cx,y_to),(cx-5,y_to-10),(cx+5,y_to-10)],fill=C_BLUE)

# ============================================================
# Subgraph 3: V3F 频域分析
# ============================================================
sg3_x=1010; sg3_y=80; sg3_w=380; sg3_h=580
subgraph(sg3_x,sg3_y,sg3_w,sg3_h,'V3F 频域分析',BG_GREEN,C_GREEN)

n_fft_x=sg3_x+50; n_fft_y=sg3_y+50
node(n_fft_x,n_fft_y,NW,NH,['FFT 256pt','Process_FFT_Step()','Hanning step=128'],'white',C_GREEN)

n_band_x=sg3_x+50; n_band_y=n_fft_y+NH+NGAP_Y
node(n_band_x,n_band_y,NW,NH,['Band Power','compute_band_powers()','delta/theta/alpha/beta x 8ch'],'white',C_GREEN)

n_att_x=sg3_x+50; n_att_y=n_band_y+NH+NGAP_Y
node(n_att_x,n_att_y,NW,NH,['6ch Weighted Focus','attention_engine_process()','F3/F4/CP3/CP4/C3/C4'],'white',C_GREEN)

n_send_x=sg3_x+50; n_send_y=n_att_y+NH+NGAP_Y
node(n_send_x,n_send_y,NW,NH,['Send Data','Send_WaveformSingle()','Send_Spectrum() / Send_Focus()'],'white',C_GREEN)

for (y_from, y_to) in [(n_fft_y,n_band_y),(n_band_y,n_att_y),(n_att_y,n_send_y)]:
    cx=sg3_x+50+NW//2
    draw.line([(cx,y_from+NH),(cx,y_to)],fill=C_GREEN,width=2)
    draw.polygon([(cx,y_to),(cx-5,y_to-10),(cx+5,y_to-10)],fill=C_GREEN)

# ============================================================
# Subgraph 4: Frame Protocol (standalone column)
# ============================================================
sg4_x=1490; sg4_y=80; sg4_w=380; sg4_h=730
subgraph(sg4_x,sg4_y,sg4_w,sg4_h,'V3F\u5E27\u534F\u8BAE',BG_TEAL,C_TEAL)

n_pack_x=sg4_x+50; n_pack_y=sg4_y+50
node(n_pack_x,n_pack_y,NW,NH,['Pack Frame','Pack_Frame()','FRAME_CHAR + ESCAPE_CHAR'],'white',C_TEAL)

n_frag_x=sg4_x+50; n_frag_y=n_pack_y+NH+NGAP_Y
node(n_frag_x,n_frag_y,NW,NH,['Spectrum Fragment','32 frags x 4 floats','Pack_Frame_Fragmented()'],'white',C_TEAL)

n_parse_x=sg4_x+50; n_parse_y=n_frag_y+NH+NGAP_Y+10
node(n_parse_x,n_parse_y,NW,NH,['Parse Frame','Parse_Frame()','CMD -> Parse_CommandEx()'],BG_ORANGE,C_ORANGE)

n_cmd_x=sg4_x+50; n_cmd_y=n_parse_y+NH+NGAP_Y
node(n_cmd_x,n_cmd_y,NW,NH,['Command Dispatch','MODE/TRIAL/STOP','DISPLAY_CFG/MODEL,SET'],BG_ORANGE,C_ORANGE)

# Pack -> Fragment (internal)
draw.line([(sg4_x+50+NW//2,n_pack_y+NH),(sg4_x+50+NW//2,n_frag_y)],fill=C_TEAL,width=2)
draw.polygon([(sg4_x+50+NW//2,n_frag_y),(sg4_x+50+NW//2-5,n_frag_y-10),(sg4_x+50+NW//2+5,n_frag_y-10)],fill=C_TEAL)
# Parse -> Command (internal)
draw.line([(sg4_x+50+NW//2,n_parse_y+NH),(sg4_x+50+NW//2,n_cmd_y)],fill=C_ORANGE,width=2)
draw.polygon([(sg4_x+50+NW//2,n_cmd_y),(sg4_x+50+NW//2-5,n_cmd_y-10),(sg4_x+50+NW//2+5,n_cmd_y-10)],fill=C_ORANGE)

# ============================================================
# Subgraph 5: 数据传输 (ESP8266)
# ============================================================
sg5_x=1970; sg5_y=80; sg5_w=380; sg5_h=200
subgraph(sg5_x,sg5_y,sg5_w,sg5_h,'数据传输',BG_TEAL,C_TEAL)

n_esp_x=sg5_x+50; n_esp_y=sg5_y+50
node(n_esp_x,n_esp_y,NW,NH,['ESP8266','WiFi Bridge','TCP <-> UART'],'white',C_TEAL)

# ============================================================
# Subgraph 6: 医患终端
# ============================================================
sg6_x=2450; sg6_y=80; sg6_w=780; sg6_h=580
subgraph(sg6_x,sg6_y,sg6_w,sg6_h,'医患终端',BG_GREEN,C_GREEN)

n_doc_x=sg6_x+30; n_doc_y=sg6_y+50
node(n_doc_x,n_doc_y,NW,NH,['Doctor App','41002/41003/41004','Wave/Spectrum/Focus/MI'],'white',C_GREEN)

n_dd_x=sg6_x+30; n_dd_y=n_doc_y+NH+NGAP_Y
node(n_dd_x,n_dd_y,NW,NH,['TcpServerManager','41002/41003/41004','Frame parse + forward'],'white',C_GREEN)

n_py_x=sg6_x+30; n_py_y=n_dd_y+NH+NGAP_Y
node(n_py_x,n_py_y,NW,NH,['Python Host','41003 <-> Doctor','SSVEP / Train / Test'],'white',C_PURPLE)

n_pat_x=sg6_x+30+NW+40; n_pat_y=n_dd_y
node(n_pat_x,n_pat_y,NW,NH,['Patient App','41004','Arrow/SSVEP/Target'],'white',C_ORANGE)

draw.line([(n_doc_x+NW//2,n_doc_y+NH),(n_dd_x+NW//2,n_dd_y)],fill=C_GREEN,width=2)
draw.polygon([(n_dd_x+NW//2,n_dd_y),(n_dd_x+NW//2-5,n_dd_y-10),(n_dd_x+NW//2+5,n_dd_y-10)],fill=C_GREEN)
draw.line([(n_dd_x+NW//2,n_dd_y+NH),(n_py_x+NW//2,n_py_y)],fill=C_GREEN,width=2)
draw.polygon([(n_py_x+NW//2,n_py_y),(n_py_x+NW//2-5,n_py_y-10),(n_py_x+NW//2+5,n_py_y-10)],fill=C_GREEN)

h_biarrow(n_dd_x+NW,n_dd_y+NH//2,n_pat_x,n_pat_y+NH//2,C_ORANGE,3)
label_at((n_dd_x+NW+n_pat_x)//2,n_dd_y+NH//2-20,'41004',C_ORANGE)

# ============================================================
# Subgraph 7: V5F 方向推理 (below V3F时域)
# ============================================================
sg7_x=530; sg7_y=870; sg7_w=380; sg7_h=580
subgraph(sg7_x,sg7_y,sg7_w,sg7_h,'V5F \u65B9\u5411\u63A8\u7406 (400MHz)',BG_ORANGE,C_ORANGE)

n_ipc_x=sg7_x+50; n_ipc_y=sg7_y+50
node(n_ipc_x,n_ipc_y,NW,NH,['IPC Shared Memory','SendFrameFromV3F()','WFI when g_v5f_active=IDLE'],BG_PURPLE,C_PURPLE)

n_v5p_x=sg7_x+50; n_v5p_y=n_ipc_y+NH+NGAP_Y
node(n_v5p_x,n_v5p_y,NW,NH,['V5F Preprocess','ProcessPreprocess()','Drift+Notch+Bandpass'],'white',C_ORANGE)

n_v5f_x=sg7_x+50; n_v5f_y=n_v5p_y+NH+NGAP_Y
node(n_v5f_x,n_v5f_y,NW,NH,['V5F FFT+Feature','ComputeFFTFeature()','24-dim LogRatio'],'white',C_ORANGE)

n_v5c_x=sg7_x+50; n_v5c_y=n_v5f_y+NH+NGAP_Y
node(n_v5c_x,n_v5c_y,NW,NH,['FFT24/CSP Classifier','FFTClassifierRightProb()','CSPRightProbability()'],'white',C_ORANGE)

for (y_from, y_to) in [(n_ipc_y,n_v5p_y),(n_v5p_y,n_v5f_y),(n_v5f_y,n_v5c_y)]:
    cx=sg7_x+50+NW//2
    draw.line([(cx,y_from+NH),(cx,y_to)],fill=C_ORANGE,width=2)
    draw.polygon([(cx,y_to),(cx-5,y_to-10),(cx+5,y_to-10)],fill=C_ORANGE)

draw.text((sg7_x+50+NW//2,n_v5c_y+NH+15),'WFI when g_v5f_active=IDLE',font=FT_SMALL,fill=C_ORANGE,anchor='mt')

# ============================================================
# Subgraph 8: 结果输出 (below V3F频域)
# ============================================================
sg8_x=1010; sg8_y=870; sg8_w=380; sg8_h=580
subgraph(sg8_x,sg8_y,sg8_w,sg8_h,'V3F \u7ED3\u679C\u8F93\u51FA',BG_ORANGE,C_ORANGE)

n_collect_x=sg8_x+50; n_collect_y=sg8_y+50
node(n_collect_x,n_collect_y,NW,NH,['V3F Collect Output','AutoCollectProcess()','CopyLastV5FFeature()\u2192DIRCSV'],BG_TEAL,C_TEAL)

n_vote_x=sg8_x+50; n_vote_y=n_collect_y+NH+NGAP_Y
node(n_vote_x,n_vote_y,NW,NH,['4-vote Decision','2s window','4 consecutive inferences'],'white',C_ORANGE)

n_local_x=sg8_x+50; n_local_y=n_vote_y+NH+NGAP_Y
node(n_local_x,n_local_y,NW,NH,['V3F Local Infer','Direction_Model_Infer()','V3F_LOCAL_INFER_ENABLE'],BG_GREEN,C_GREEN)

n_result_x=sg8_x+50; n_result_y=n_local_y+NH+NGAP_Y
node(n_result_x,n_result_y,NW,NH,['RESULT Output','INTENT,S_LEFT,S_RIGHT,CONF','0~10000 integer'],'white',C_ORANGE)

# RESULT right side: 2 arrows (4-vote enter + merge exit) -> equal division
result_ry1 = n_result_y + NH // 3
result_ry2 = n_result_y + 2 * NH // 3

# V3F Local Infer -> RESULT Output (vertical, enters top center)
draw.line([(sg8_x+50+NW//2,n_local_y+NH),(sg8_x+50+NW//2,n_result_y)],fill=C_GREEN,width=2)
draw.polygon([(sg8_x+50+NW//2,n_result_y),(sg8_x+50+NW//2-5,n_result_y-10),(sg8_x+50+NW//2+5,n_result_y-10)],fill=C_GREEN)

# 4-vote Decision -> RESULT Output (Z-shape, enters right 1/3)
vote_result_route_x=sg8_x+sg8_w-15
larrow([(n_vote_x+NW,n_vote_y+NH//2),
        (vote_result_route_x,n_vote_y+NH//2),
        (vote_result_route_x,result_ry1),
        (n_result_x+NW,result_ry1)],C_ORANGE,2)
label_at(vote_result_route_x-30,(n_vote_y+NH//2+result_ry1)//2,'4-vote',C_ORANGE)

# ============================================================
# Cross-subgraph arrows — ALL H/V, no diagonals
# Rules: midpoint exit/entry (equal division for shared sides),
#        no box crossing, labels off arrows,
#        right-side entry for left-going, perpendicular to edge
# ============================================================

# 1. ADS1299 -> DriftRemove (horizontal, right midpoint -> left midpoint)
h_arrow(n_ads_x+NW, n_ads_y+NH//2, n_drift_x, n_drift_y+NH//2, C_NAVY, 3)
label_at((n_ads_x+NW+n_drift_x)//2, n_ads_y+NH//2-18, 'SPI3+DMA', C_NAVY)

# 2. ICM -> Posture Detect (L-shape: right, down, right)
#    Exits ICM right midpoint, enters Posture left midpoint
icm_route_x = (sg1_x + sg1_w + sg2_x) // 2
larrow([(n_icm_x+NW, n_icm_y+NH//2),
        (icm_route_x, n_icm_y+NH//2),
        (icm_route_x, n_posture_y+NH//2),
        (n_posture_x, n_posture_y+NH//2)], C_GREEN, 2)
label_at(icm_route_x-30, (n_icm_y+NH//2+n_posture_y+NH//2)//2, 'SPI2', C_GREEN)

# 3. RingBuf -> FFT (L-shape: right, up, right)
#    Exits RingBuf right midpoint, enters FFT left midpoint
ring_route_x = (sg2_x + sg2_w + sg3_x) // 2
larrow([(n_ring_x+NW, n_ring_y+NH//2),
        (ring_route_x, n_ring_y+NH//2),
        (ring_route_x, n_fft_y+NH//2),
        (n_fft_x, n_fft_y+NH//2)], C_GREEN, 3)
label_at(ring_route_x-35, (n_ring_y+NH//2+n_fft_y+NH//2)//2, '256 samples', C_GREEN)

# 4. RingBuf -> IPC (U-shape: right, down, left — enters IPC right 1/3)
#    Two arrows enter IPC right side: this at 1/3, g_v5f_active at 2/3
ipc_ry1 = n_ipc_y + NH // 3
larrow([(n_ring_x+NW, n_ring_y+NH//2),
        (ring_route_x, n_ring_y+NH//2),
        (ring_route_x, ipc_ry1),
        (n_ipc_x+NW, ipc_ry1)], C_PURPLE, 3)
label_at(ring_route_x-35, (n_ring_y+NH//2+ipc_ry1)//2, 'IPC raw frame', C_PURPLE)

# 5. V5F Classifier -> 4-vote Decision (Z-shape: right, up, right)
#    Exits V5F right midpoint, enters 4-vote left midpoint
#    Use different x from arrow 4 to avoid corner collision
v5f_route_x = sg7_x + sg7_w + (sg8_x - sg7_x - sg7_w) * 2 // 4
larrow([(n_v5c_x+NW, n_v5c_y+NH//2),
        (v5f_route_x, n_v5c_y+NH//2),
        (v5f_route_x, n_vote_y+NH//2),
        (n_vote_x, n_vote_y+NH//2)], C_ORANGE, 3)
label_at(v5f_route_x+35, (n_v5c_y+NH//2+n_vote_y+NH//2)//2, 'IPC slot', C_ORANGE)

# 5b. V5F FFT+Feature -> V3F Collect Output (feature readback via IPC)
#     Z-shape: right, up, right (shortest path)
feat_route_x = sg7_x + sg7_w + (sg8_x - sg7_x - sg7_w) * 1 // 4
larrow([(n_v5f_x+NW, n_v5f_y+NH//2),
        (feat_route_x, n_v5f_y+NH//2),
        (feat_route_x, n_collect_y+NH//2),
        (n_collect_x, n_collect_y+NH//2)], C_TEAL, 2)
label_at(feat_route_x-40, (n_v5f_y+NH//2+n_collect_y+NH//2)//2, 'IPC feature', C_TEAL)

# 6. Three data sources merge vertically at merge lane, then single arrow to Pack Frame
merge_x = sg3_x + sg3_w + (sg4_x - sg3_x - sg3_w) * 2 // 3
pack_cy = n_pack_y + NH // 2

# Send Data right side: 2 arrows (exit data + enter command) -> equal division
sd_ry1 = n_send_y + NH // 3
sd_ry2 = n_send_y + 2 * NH // 3

# 6a. Send Data -> merge lane (exits right 1/3)
h_arrow(n_send_x+NW, sd_ry1, merge_x, sd_ry1, C_TEAL, 3)

# 6b. Posture -> merge lane (exits right midpoint)
h_arrow(n_posture_x+NW, n_posture_y+NH//2, merge_x, n_posture_y+NH//2, C_TEAL, 2)

# 6c. RESULT -> merge lane (exits right 2/3)
h_arrow(n_result_x+NW, result_ry2, merge_x, result_ry2, C_TEAL, 3)

# 6d. V3F Collect Output -> merge lane (exits right midpoint)
h_arrow(n_collect_x+NW, n_collect_y+NH//2, merge_x, n_collect_y+NH//2, C_TEAL, 2)

# 6e. Vertical merge line from RESULT level up to Pack Frame level
draw.line([(merge_x, result_ry2), (merge_x, pack_cy)], fill=C_TEAL, width=3)

# 6f. Merged arrow into Pack Frame left midpoint
h_arrow(merge_x, pack_cy, n_pack_x, pack_cy, C_TEAL, 3)

# Labels on merge lane: place above each horizontal segment to avoid blocking
label_at((n_send_x+NW+merge_x)//2, sd_ry1-20, 'EEG Data', C_TEAL)
label_at((n_posture_x+NW+merge_x)//2, n_posture_y+NH//2-20, 'POSTURE', C_TEAL)
label_at((n_result_x+NW+merge_x)//2, result_ry2-20, 'RESULT', C_TEAL)
label_at((n_collect_x+NW+merge_x)//2, n_collect_y+NH//2+20, 'DIRCSV', C_TEAL)

# 7. Pack Frame -> ESP8266 (UART, horizontal)
h_arrow(n_pack_x+NW, pack_cy, n_esp_x, n_esp_y+NH//2, C_TEAL, 3)
label_at((n_pack_x+NW+n_esp_x)//2, pack_cy-18, 'UART', C_TEAL)

# 8. ESP8266 <-> Doctor (41002 TCP, horizontal bidirectional)
h_biarrow(n_esp_x+NW, n_esp_y+NH//2, n_doc_x, n_doc_y+NH//2, C_GREEN, 3)
label_at((n_esp_x+NW+n_doc_x)//2, n_esp_y+NH//2-18, '41002 TCP', C_GREEN)

# 9. DataDispatcher <-> Python (41003 bidirectional, vertical)
draw.line([(n_dd_x+NW//2, n_dd_y+NH), (n_dd_x+NW//2, n_py_y)], fill=C_PURPLE, width=2)
draw.polygon([(n_dd_x+NW//2, n_py_y), (n_dd_x+NW//2-5, n_py_y-10), (n_dd_x+NW//2+5, n_py_y-10)], fill=C_PURPLE)
draw.polygon([(n_dd_x+NW//2, n_dd_y+NH), (n_dd_x+NW//2-5, n_dd_y+NH+10), (n_dd_x+NW//2+5, n_dd_y+NH+10)], fill=C_PURPLE)
label_at(n_dd_x+NW//2+100, (n_dd_y+NH+n_py_y)//2, '41003 CMD<->Data', C_PURPLE)

# 10. ESP8266 -> Parse Frame (command path: down from bottom midpoint, left into right midpoint)
#     Left-going arrow enters Parse Frame from right side
larrow([(n_esp_x+NW//2, n_esp_y+NH),
        (n_esp_x+NW//2, n_parse_y+NH//2),
        (n_parse_x+NW, n_parse_y+NH//2)], C_ORANGE, 3)
label_at(n_esp_x+NW//2+35, (n_esp_y+NH+n_parse_y+NH//2)//2, 'CMD', C_ORANGE)

# 11. Command Dispatch -> Send Data (L-shape: left, then into Send Data right 2/3)
#     Left-going arrow enters Send Data from right side
cmd_route_x = sg3_x + sg3_w + (sg4_x - sg3_x - sg3_w) * 1 // 3
larrow([(n_cmd_x, n_cmd_y+NH//2),
        (cmd_route_x, n_cmd_y+NH//2),
        (cmd_route_x, sd_ry2),
        (n_send_x+NW, sd_ry2)], C_ORANGE, 2)
label_at((n_cmd_x+cmd_route_x)//2, n_cmd_y+NH//2+20, 'CMD\u2192V3F', C_ORANGE)

# 12. Command Dispatch -> V5F IPC (red)
#     Exits Command Dispatch bottom midpoint, routes to IPC right 2/3
ipc_ry2 = n_ipc_y + 2 * NH // 3
ctrl_route_x = sg7_x + sg7_w + (sg8_x - sg7_x - sg7_w) * 3 // 4
larrow([(n_cmd_x+NW//2, n_cmd_y+NH),
        (n_cmd_x+NW//2, sg4_y+sg4_h+30),
        (ctrl_route_x, sg4_y+sg4_h+30),
        (ctrl_route_x, ipc_ry2),
        (n_ipc_x+NW, ipc_ry2)], C_RED, 2)
label_at((n_cmd_x+NW//2+ctrl_route_x)//2, sg4_y+sg4_h+30-18, 'CMD\u2192V5F', C_RED)

# 13. Band Power -> V3F Local Infer (dashed, alternative path)
#     Z-shape: left from Band Power, down via sg3 left margin, right into V3F Local Infer
alt_route_x = sg3_x - 10
local_entry_y = n_local_y + NH // 4
dashed_larrow([(n_band_x,n_band_y+NH//2),
               (alt_route_x,n_band_y+NH//2),
               (alt_route_x,local_entry_y),
               (n_local_x,local_entry_y)],C_GREEN,2)
label_at(alt_route_x,local_entry_y-18,'\u03B8/\u03B1/\u03B2 alt',C_GREEN)

# ============================================================
# Right margin: Key specs
# ============================================================
note_x=sg6_x+sg6_w+40; note_y=870
notes=[
    ('Focus Weights (6ch)', FT_SUBG, C_NAVY),
    ('F3=0.18  F4=0.18', FT_SPEC, C_BLUE),
    ('CP3=0.19  CP4=0.19', FT_SPEC, C_BLUE),
    ('C3=0.13  C4=0.13', FT_SPEC, C_BLUE),
    ('OZ=0  O1=0 (SSVEP only)', FT_SPEC, C_BLUE),
    ('', FT_SMALL, C_DARK),
    ('V5F Feature (24-dim)', FT_SUBG, C_NAVY),
    ('LogRatio(CP3/CP4) x {mu,beta,theta,total}', FT_SPEC, C_ORANGE),
    ('LogRatio(C3/C4) x {mu,beta,theta,total}', FT_SPEC, C_ORANGE),
    ('LogRatio(mu/total) x {CP3,CP4,C3,C4}', FT_SPEC, C_ORANGE),
    ('LogRatio(beta/total) x {CP3,CP4,C3,C4}', FT_SPEC, C_ORANGE),
    ('LogRatio(beta/mu) x {CP3,CP4,C3,C4}', FT_SPEC, C_ORANGE),
    ('LogRatio(CP3/C3) x {mu,beta}', FT_SPEC, C_ORANGE),
    ('LogRatio(CP4/C4) x {mu,beta}', FT_SPEC, C_ORANGE),
    ('', FT_SMALL, C_DARK),
    ('Channel Map (8ch)', FT_SUBG, C_NAVY),
    ('raw_vals[0]=OZ  [1]=O1', FT_SPEC, C_BLUE),
    ('raw_vals[2]=F3  [3]=F4', FT_SPEC, C_BLUE),
    ('raw_vals[4]=CP3 [5]=CP4', FT_SPEC, C_BLUE),
    ('raw_vals[6]=C3  [7]=C4', FT_SPEC, C_BLUE),
    ('', FT_SMALL, C_DARK),
    ('V5F Filter vs V3F', FT_SUBG, C_NAVY),
    ('Drift+Notch: same algo, same params', FT_SPEC, C_ORANGE),
    ('V5F FFT24 bandpass: 8Hz HP (2-sec)', FT_SPEC, C_ORANGE),
    ('V5F CSP bandpass: 2-40Hz (same as V3F)', FT_SPEC, C_ORANGE),
    ('V3F bandpass: 2-40Hz (4-sec Butterworth)', FT_SPEC, C_BLUE),
    ('', FT_SMALL, C_DARK),
    ('V3F Local Infer (alt)', FT_SUBG, C_NAVY),
    ('Direction_Model_Infer() on V3F', FT_SPEC, C_GREEN),
    ('Input: theta/alpha/beta band power', FT_SPEC, C_GREEN),
    ('Guard: V3F_LOCAL_INFER_ENABLE', FT_SPEC, C_GREEN),
    ('', FT_SMALL, C_DARK),
    ('Port Map', FT_SUBG, C_NAVY),
    ('41002: MCU <-> Doctor (TCP)', FT_SPEC, C_TEAL),
    ('41003: Doctor <-> Python (TCP)', FT_SPEC, C_TEAL),
    ('41004: Doctor <-> Patient (TCP)', FT_SPEC, C_ORANGE),
    ('41005: Patient -> Doctor (UDP)', FT_SPEC, C_GRAY),
    ('', FT_SMALL, C_DARK),
    ('Frame Protocol', FT_SUBG, C_NAVY),
    ('CMD_RAW/FILT/BASELINE_WAVE', FT_SPEC, C_TEAL),
    ('CMD_*_SPECTRUM_CH0/1', FT_SPEC, C_TEAL),
    ('CMD_FOCUS: attn,relax,ema,trend', FT_SPEC, C_TEAL),
    ('RESULT: INTENT,S_LEFT,S_RIGHT,CONF', FT_SPEC, C_TEAL),
    ('POSTURE: posture,gravity,conf', FT_SPEC, C_TEAL),
]
for i,(text,font,color) in enumerate(notes):
    draw.text((note_x,note_y+i*28),text,font=font,fill=color,anchor='lt')

out = r'D:\Libraries\Projects\EEG\output\system_block_diagram.png'
img.save(out)
print('Saved to', out)
