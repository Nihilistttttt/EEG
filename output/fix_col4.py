# -*- coding: utf-8 -*-
with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', encoding='utf-8') as f:
    c = f.read()

old = """# ===== Col4 =====
dx=C4+CW//2
rrect(C4,R1-10,C4+CW,R2+CH+10,'#F5F5F5',GREEN,4)
tc(dx,R1-30,'Android',ft_title,GREEN)
doc_items=['8ch波形/频谱','专注度曲线','MI训练/推理','SSVEP控制','姿态监护']
rrect(C4,R1,C4+CW,R1+CH,'#E8F5E9',GREEN,3)
tc(dx,R1+60,'医生端 Android',ft_box,GREEN)
cy=content_y(R1,R1+CH,90,len(doc_items),50)
for i,s in enumerate(doc_items):
    tc(dx,cy+i*50,s,ft_sub,GREEN)

pat_items=['MI箭头引导','目标+结果双箭头','SSVEP全屏闪烁','AR眼镜适配','完全被动化']
rrect(C4,R2,C4+CW,R2+CH,'#FBE9E7',ORANGE,3)
tc(dx,R2+60,'患者端 Android',ft_box,ORANGE)
cy=content_y(R2,R2+CH,90,len(pat_items),50)
for i,s in enumerate(pat_items):
    tc(dx,cy+i*50,s,ft_sub,ORANGE)"""

new = """# ===== Col4 =====
dx=C4+CW//2
rrect(C4,R1-10,C4+CW,R2+CH+10,'#F5F5F5',GREEN,4)
tc(dx,R1-30,'Android',ft_title,GREEN)
doc_items=['8ch波形/频谱','专注度曲线','MI训练/推理','SSVEP控制','姿态监护']
rrect(C4+15,v3t,C4+CW-15,v3b,'#E8F5E9',GREEN,3)
tc(dx,v3t+45,'医生端',ft_box,GREEN)
cy=content_y(v3t,v3b,80,len(doc_items),50)
for i,s in enumerate(doc_items):
    tc(dx,cy+i*50,s,ft_sub,GREEN)

pat_items=['MI箭头引导','目标+结果双箭头','SSVEP全屏闪烁','AR眼镜适配','完全被动化']
rrect(C4+15,v5t,C4+CW-15,v5b,'#FBE9E7',ORANGE,3)
tc(dx,v5t+45,'患者端',ft_box,ORANGE)
cy=content_y(v5t,v5b,80,len(pat_items),50)
for i,s in enumerate(pat_items):
    tc(dx,cy+i*50,s,ft_sub,ORANGE)"""

c = c.replace(old, new)
with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', 'w', encoding='utf-8') as f:
    f.write(c)
print('Done')