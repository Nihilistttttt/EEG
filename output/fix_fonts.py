# -*- coding: utf-8 -*-
import re

with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', encoding='utf-8') as f:
    c = f.read()

# Add font size constants after fp line
c = c.replace(
    "fp = 'C:/Windows/Fonts/msyh.ttc'",
    "fp = 'C:/Windows/Fonts/msyh.ttc'\nFS_LAYER=36; FS_TITLE=32; FS_BOX=28; FS_SUB=22; FS_PORT=20"
)

# Replace hardcoded sizes with variables
c = c.replace('ImageFont.truetype(fp, 36)', 'ImageFont.truetype(fp, FS_LAYER)')
c = c.replace('ImageFont.truetype(fp, 32)', 'ImageFont.truetype(fp, FS_TITLE)')
c = c.replace('ImageFont.truetype(fp, 28)', 'ImageFont.truetype(fp, FS_BOX)')
c = c.replace('ImageFont.truetype(fp, 22)', 'ImageFont.truetype(fp, FS_SUB)')
c = c.replace('ImageFont.truetype(fp, 20)', 'ImageFont.truetype(fp, FS_PORT)')

with open(r'D:\Libraries\Projects\EEG\output\gen_diagram.py', 'w', encoding='utf-8') as f:
    f.write(c)
print('Done')