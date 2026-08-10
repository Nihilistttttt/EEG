# -*- coding: utf-8 -*-
# Generate 16x16 bitmap glyphs for Chinese color words.
import os
from PIL import Image, ImageDraw, ImageFont

CHARS = ["红", "黄", "绿", "蓝", "白", "黑"]

fp = r"C:\Windows\Fonts\msyh.ttc"
R, GRID = 64, 16
font = ImageFont.truetype(fp, R)

print("static const uint8_t s_font_cn[6][32] = {")
for ch in CHARS:
    img = Image.new("L", (R, R), 0)
    d = ImageDraw.Draw(img)
    d.text((R // 2, R // 2), ch, font=font, fill=255, anchor="mm")
    img = img.resize((GRID, GRID), Image.LANCZOS)
    px = img.load()
    vals = []
    for y in range(GRID):
        hi = lo = 0
        for x in range(GRID):
            if px[x, y] >= 100:
                if x < 8:
                    hi |= 1 << (7 - x)
                else:
                    lo |= 1 << (15 - x)
        vals.append("0x%02X,0x%02X" % (hi, lo))
    print("    { %s }, /* %s */" % (", ".join(vals), ch))
print("};")
