with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', encoding='utf-8') as f:
    c = f.read()
c = c.replace("tc(dx,R1-30,'Android',ft_title,GREEN)", "tc(dx,R1+40,'Android',ft_title,GREEN)")
with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', 'w', encoding='utf-8') as f:
    f.write(c)
print('Done')