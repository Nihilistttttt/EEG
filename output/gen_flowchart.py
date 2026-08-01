# -*- coding: utf-8 -*-
from PIL import Image, ImageDraw, ImageFont

W,H=2400,1400
img=Image.new('RGBA',(W,H),(0,0,0,0))
draw=ImageDraw.Draw(img)
fp='C:/Windows/Fonts/msyh.ttc'
FT_T=ImageFont.truetype(fp,34);FT_S=ImageFont.truetype(fp,23)
FT_N=ImageFont.truetype(fp,20);FT_P=ImageFont.truetype(fp,17)
FT_A=ImageFont.truetype(fp,17)

CN='#1A237E';CB='#1565C0';CT='#00796B';CG='#2E7D32';CO='#E65100';CP='#6A1B9A';CR='#C62828';CA='#FF8F00';CD='#263238';CG2='#546E7A'
BB='#E3F2FD';BG='#E8F5E9';BT='#E0F2F1';BO='#FFF3E0';BP='#F3E5F5';BA='#FFF8E1'

def rr(x1,y1,x2,y2,f,o,lw=2,r=10):
    draw.rounded_rectangle([x1,y1,x2,y2],radius=r,fill=f,outline=o,width=lw)
def sg(x,y,w,h,t,f=BP,o=CP):
    rr(x,y,x+w,y+h,f=f,o=o,lw=2,r=14)
    b=draw.textbbox((0,0),t,font=FT_S);tw=b[2]-b[0]+18;th=b[3]-b[1]+8
    rr(x+10,y-th//2,x+10+tw,y+th//2,f='white',o=o,lw=1,r=6)
    draw.text((x+10+tw//2,y),t,font=FT_S,fill=o,anchor='mm')
def nd(x,y,w,h,lines,f='white',o=CD,lw=2):
    rr(x,y,x+w,y+h,f=f,o=o,lw=lw,r=8)
    n=len(lines);th=n*24 if n>1 else 24;sy=y+(h-th)//2
    for i,l in enumerate(lines):
        ft=FT_N if i==0 else FT_P;c=o if i==0 else '#455A64'
        draw.text((x+w//2,sy+i*24+12),l,font=ft,fill=c,anchor='mm')
def la(pts,c=CG2,w=2,hd=10):
    for i in range(len(pts)-1):
        draw.line([(pts[i][0],pts[i][1]),(pts[i+1][0],pts[i+1][1])],fill=c,width=w)
    x1,y1=pts[-2];x2,y2=pts[-1];dx,dy=x2-x1,y2-y1;ln=(dx**2+dy**2)**0.5
    if ln==0:return
    ux,uy=dx/ln,dy/ln;px,py=-uy,ux
    draw.polygon([(x2,y2),(x2-ux*hd+px*hd*.4,y2-uy*hd+py*hd*.4),(x2-ux*hd-px*hd*.4,y2-uy*hd-py*hd*.4)],fill=c)
def ha(x1,y,x2,c=CG2,w=2):
    draw.line([(x1,y),(x2,y)],fill=c,width=w);dx=x2-x1;s=1 if dx>0 else -1;h=10
    draw.polygon([(x2,y),(x2-s*h,y-h*.4),(x2-s*h,y+h*.4)],fill=c)
def hba(x1,y,x2,c=CG2,w=2):
    draw.line([(x1,y),(x2,y)],fill=c,width=w);dx=x2-x1;s=1 if dx>0 else -1;h=10
    draw.polygon([(x2,y),(x2-s*h,y-h*.4),(x2-s*h,y+h*.4)],fill=c)
    draw.polygon([(x1,y),(x1+s*h,y-h*.4),(x1+s*h,y+h*.4)],fill=c)
def lb(x,y,t,c=CG2):
    b=draw.textbbox((0,0),t,font=FT_A);tw=b[2]-b[0]+8;th=b[3]-b[1]+4
    draw.rounded_rectangle([x-tw//2,y-th//2,x+tw//2,y+th//2],radius=3,fill='white',outline='#CFD8DC',width=1)
    draw.text((x,y),t,font=FT_A,fill=c,anchor='mm')

NW=280;NH=75;G=20

draw.text((W//2,22),'面向卧床患者的脑电交互与智能监护系统 — 功能流程图',font=FT_T,fill=CN,anchor='mm')

# === Row1: 数据流主线 ===
# 硬件采集
sx1=30;sy1=60;sw1=330;sh1=260
sg(sx1,sy1,sw1,sh1,'硬件采集',BB,CN)
a1x=sx1+35;a1y=sy1+40
nd(a1x,a1y,NW,NH,['八通道脑电采集','ADS1299, 250Hz'],'white',CN)
a2x=sx1+35;a2y=a1y+NH+G
nd(a2x,a2y,NW,NH,['六轴IMU姿态感知','ICM-42605'],'white',CN)

# V3F
sx2=420;sy2=60;sw2=330;sh2=420
sg(sx2,sy2,sw2,sh2,'V3F 主核',BB,CB)
b1x=sx2+35;b1y=sy2+40
nd(b1x,b1y,NW,NH,['实时信号预处理','滤波+频谱计算'],'white',CB)
b2x=sx2+35;b2y=b1y+NH+G
nd(b2x,b2y,NW,NH,['专注度评估','多通道加权融合'],BP,CP)
b3x=sx2+35;b3y=b2y+NH+G
nd(b3x,b3y,NW,NH,['卧床姿态监护','翻身检测与久卧提醒'],BG,CG)
b4x=sx2+35;b4y=b3y+NH+G
nd(b4x,b4y,NW,NH,['帧协议打包与解析','数据帧收发+命令路由'],'white',CB)
for yf,yt in [(b1y,b2y),(b2y,b3y),(b3y,b4y)]:
    cx=sx2+35+NW//2
    draw.line([(cx,yf+NH),(cx,yt)],fill=CB,width=2)
    draw.polygon([(cx,yt),(cx-4,yt-8),(cx+4,yt-8)],fill=CB)

# V5F
sx3=810;sy3=60;sw3=330;sh3=260
sg(sx3,sy3,sw3,sh3,'V5F MI推理核',BO,CO)
c1x=sx3+35;c1y=sy3+40
nd(c1x,c1y,NW,NH,['MI特征提取与判别','感觉运动区四通道'],'white',CO)
c2x=sx3+35;c2y=c1y+NH+G
nd(c2x,c2y,NW,NH,['双路融合与投票决策','FFT+CSP连续一致输出'],BT,CT)
draw.line([(sx3+35+NW//2,c1y+NH),(sx3+35+NW//2,c2y)],fill=CO,width=2)
draw.polygon([(sx3+35+NW//2,c2y),(sx3+35+NW//2-4,c2y-8),(sx3+35+NW//2+4,c2y-8)],fill=CO)

# ESP8266
sx5=1200;sy5=60;sw5=330;sh5=120
sg(sx5,sy5,sw5,sh5,'数据传输',BT,CT)
e1x=sx5+35;e1y=sy5+40
nd(e1x,e1y,NW,NH,['WiFi数据传输','TCP与UART桥接'],'white',CT)

# 医患终端
sx6=1200;sy6=220;sw6=600;sh6=260
sg(sx6,sy6,sw6,sh6,'医患终端',BG,CG)
f1x=sx6+25;f1y=sy6+40
nd(f1x,f1y,NW,NH,['医生端','监测/专注度/MI/SSVEP'],'white',CG)
f2x=sx6+25;f2y=f1y+NH+G
nd(f2x,f2y,NW,NH,['离线训练主机','LDA训练→权重固化'],'white',CP)
f3x=sx6+25+NW+25;f3y=f1y
nd(f3x,f3y,NW,NH,['患者端AR眼镜','闪烁刺激与箭头引导'],BA,CA)
draw.line([(f1x+NW//2,f1y+NH),(f1x+NW//2,f2y)],fill=CG,width=2)
draw.polygon([(f1x+NW//2,f2y),(f1x+NW//2-4,f2y-8),(f1x+NW//2+4,f2y-8)],fill=CG)
hba(f1x+NW,f1y+NH//2,f3x,CA,3)
lb((f1x+NW+f3x)//2,f1y+NH//2-16,'控制指令',CA)

# === Row2: SSVEP闭环 ===
sx7=30;sy7=580;sw7=1140;sh7=160
sg(sx7,sy7,sw7,sh7,'SSVEP视觉刺激闭环',BA,CA)
s1x=sx7+20;s1y=sy7+40
nd(s1x,s1y,200,65,['医生端选择刺激频率','多频点可选'],BG,CG)
s2x=s1x+220;s2y=s1y
nd(s2x,s2y,200,65,['AR眼镜呈现闪烁','患者注视刺激'],BA,CA)
s3x=s2x+220;s3y=s1y
nd(s3x,s3y,200,65,['枕叶脑电采集','双通道FBCCA检测'],BO,CO)
s4x=s3x+220;s4y=s1y
nd(s4x,s4y,200,65,['SSVEP方向判断','滑动窗口投票'],BO,CO)
s5x=s4x+220;s5y=s1y
nd(s5x,s5y,200,65,['医生端显示结果','刺激参数与频谱'],BG,CG)
for xf,xt in [(s1x+200,s2x),(s2x+200,s3x),(s3x+200,s4x),(s4x+200,s5x)]:
    ha(xf,s1y+32,xt,CO,2)

# === Row2: MI闭环 ===
sx8=30;sy8=800;sw8=1220;sh8=160
sg(sx8,sy8,sw8,sh8,'运动想象闭环',BO,CO)
m1x=sx8+20;m1y=sy8+40
nd(m1x,m1y,180,65,['MI数据采集','多试次采集'],BT,CT)
m2x=m1x+200;m2y=m1y
nd(m2x,m2y,180,65,['离线LDA训练','特征统计与判别'],BP,CP)
m3x=m2x+200;m3y=m1y
nd(m3x,m3y,180,65,['权重固化部署','写入V5F端侧'],BG,CG)
m4x=m3x+200;m4y=m1y
nd(m4x,m4y,180,65,['在线MI推理','特征提取与判别'],BO,CO)
m5x=m4x+200;m5y=m1y
nd(m5x,m5y,180,65,['多轮投票决策','连续一致则输出'],BO,CO)
m6x=m5x+200;m6y=m1y
nd(m6x,m6y,180,65,['方向与置信度','左/右+置信度'],'white',CO)
for xf,xt in [(m1x+180,m2x),(m2x+180,m3x),(m3x+180,m4x),(m4x+180,m5x),(m5x+180,m6x)]:
    ha(xf,m1y+32,xt,CO,2)

# === Row2: 电极布局 ===
sx9=30;sy9=1020;sw9=1140;sh9=160
sg(sx9,sy9,sw9,sh9,'10-20电极布局与通道分工',BB,CN)
nd(sx9+20,sy9+40,350,65,['O1, OZ → SSVEP检测','枕叶视觉诱发电位'],BO,CO)
nd(sx9+390,sy9+40,350,65,['CP3/CP4/C3/C4 → 运动想象','感觉运动区ERD/ERS'],BT,CT)
nd(sx9+760,sy9+40,350,65,['6通道 → 专注度评估','额叶+感觉运动区加权'],BP,CP)

# === Cross arrows (strict H/V) ===
# ADS1299→V3F滤波
ha(a1x+NW,a1y+NH//2,b1x,CN,3)
lb((a1x+NW+b1x)//2,a1y+NH//2-16,'脑电数据',CN)
# ICM→V3F姿态 (L-shape: right, down, right)
irx=(sx1+sw1+sx2)//2
la([(a2x+NW,a2y+NH//2),(irx,a2y+NH//2),(irx,b3y+NH//2),(b3x,b3y+NH//2)],CG,2)
lb(irx-25,(a2y+NH//2+b3y+NH//2)//2,'IMU数据',CG)
# V3F→V5F IPC (L-shape: right, down, right)
firx=(sx2+sw2+sx3)//2
la([(b1x+NW,b1y+NH//2),(firx,b1y+NH//2),(firx,c1y+NH//2),(c1x,c1y+NH//2)],CP,3)
lb((b1x+NW+c1x)//2,b1y+NH//2-20,'共享内存',CP)
# V5F→V3F 4轮投票 (L-shape: right, up, left)
la([(c2x+NW,c2y+NH//2),(sx3+sw3+15,c2y+NH//2),(sx3+sw3+15,b4y+NH//2),(b4x+NW,b4y+NH//2)],CO,3)
lb(sx3+sw3+15-28,(c2y+NH//2+b4y+NH//2)//2,'推理结果',CO)
# V3F→ESP8266
mlx=sx5-20
la([(b4x+NW,b4y+NH//3),(mlx,b4y+NH//3),(mlx,e1y+NH//2),(e1x,e1y+NH//2)],CT,3)
lb(mlx-25,(b4y+NH//3+e1y+NH//2)//2,'打包数据',CT)
# ESP8266→医生端 (L-shape: down, right, down)
e42x=e1x+2*NW//3;dtmx=f1x+NW//2;edry=e1y+NH+(f1y-e1y-NH)//2
la([(e42x,e1y+NH),(e42x,edry),(dtmx,edry),(dtmx,f1y)],CG,3)
lb((e42x+dtmx)//2,edry-16,'波形数据',CG)
# ESP8266→V3F命令 (绕V5F右侧下行: down, left-to-V5F-right, down, left)
ecx=e1x+NW//3;cry=sy5+sh5+10;cmdvx=sx3+sw3+25
la([(ecx,e1y+NH),(ecx,cry),(cmdvx,cry),(cmdvx,b4y+2*NH//3),(b4x+NW,b4y+2*NH//3)],CO,3)
lb(cmdvx+28,(cry+b4y+2*NH//3)//2,'反向命令',CO)

out=r'D:\Libraries\Projects\EEG\output\functional_flowchart.png'
img.save(out)
print('Saved to',out)
