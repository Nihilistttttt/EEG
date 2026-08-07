#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
将旧格式BDF(通道头部按通道排列)转换为新格式(按字段类型排列)
用法: python fix_bdf_header.py [输入BDF] [输出BDF]
"""

import sys
import struct

NCHAN = 8

def convert(in_path, out_path):
    with open(in_path, 'rb') as f:
        raw = f.read()

    print(f"输入文件: {in_path} ({len(raw)} bytes)")

    header_size = int(raw[184:192].decode().strip())
    print(f"头部大小: {header_size}")

    gen_hdr = bytearray(raw[:256])

    old_ch_fields = []
    for ch in range(NCHAN):
        base = 256 + ch * 256
        label      = raw[base + 0   : base + 16]
        transducer = raw[base + 16  : base + 96]
        units      = raw[base + 96  : base + 104]
        phys_min   = raw[base + 104 : base + 112]
        phys_max   = raw[base + 112 : base + 120]
        dig_min    = raw[base + 120 : base + 128]
        dig_max    = raw[base + 128 : base + 136]
        prefilter  = raw[base + 136 : base + 216]
        samples    = raw[base + 216 : base + 224]
        reserved   = raw[base + 224 : base + 256]
        old_ch_fields.append({
            'label': label, 'transducer': transducer, 'units': units,
            'phys_min': phys_min, 'phys_max': phys_max,
            'dig_min': dig_min, 'dig_max': dig_max,
            'prefilter': prefilter, 'samples': samples, 'reserved': reserved,
        })
        print(f"  ch{ch}: label={label.decode().strip()!r} "
              f"units={units.decode().strip()!r} "
              f"phys_min={phys_min.decode().strip()!r} "
              f"phys_max={phys_max.decode().strip()!r}")

    new_ch_hdr = bytearray(b' ' * (NCHAN * 256))

    for ch in range(NCHAN):
        fld = old_ch_fields[ch]
        new_ch_hdr[ch * 16       : ch * 16 + 16]       = fld['label']
        new_ch_hdr[128 + ch * 80 : 128 + ch * 80 + 80] = fld['transducer']
        new_ch_hdr[768 + ch * 8  : 768 + ch * 8 + 8]   = fld['units']
        new_ch_hdr[832 + ch * 8  : 832 + ch * 8 + 8]   = fld['phys_min']
        new_ch_hdr[896 + ch * 8  : 896 + ch * 8 + 8]   = fld['phys_max']
        new_ch_hdr[960 + ch * 8  : 960 + ch * 8 + 8]   = fld['dig_min']
        new_ch_hdr[1024 + ch * 8 : 1024 + ch * 8 + 8]  = fld['dig_max']
        new_ch_hdr[1088 + ch * 80: 1088 + ch * 80 + 80]= fld['prefilter']
        new_ch_hdr[1728 + ch * 8 : 1728 + ch * 8 + 8]  = fld['samples']
        new_ch_hdr[1792 + ch * 32: 1792 + ch * 32 + 32]= fld['reserved']

    data = raw[header_size:]
    print(f"数据大小: {len(data)} bytes")

    with open(out_path, 'wb') as f:
        f.write(gen_hdr)
        f.write(new_ch_hdr)
        f.write(data)

    print(f"输出文件: {out_path} ({256 + len(new_ch_hdr) + len(data)} bytes)")
    print("转换完成!")


if __name__ == '__main__':
    in_path  = sys.argv[1] if len(sys.argv) > 1 else "EEG_0001.BDF"
    out_path = sys.argv[2] if len(sys.argv) > 2 else "EEG_0001_fixed.BDF"
    convert(in_path, out_path)