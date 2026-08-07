#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BDF文件常规分析验证脚本
使用MNE-Python标准流程读取并分析BDF文件, 验证文件是否可正常使用
用法: python verify_bdf.py [BDF文件路径]
"""

import sys
import numpy as np
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

import mne

def analyze_bdf(path):
    print("=" * 70)
    print(f"BDF文件常规分析验证: {path}")
    print("=" * 70)

    # 1. 用MNE标准方式读取
    print("\n[1] 使用 mne.io.read_raw_bdf 读取...")
    try:
        raw = mne.io.read_raw_bdf(path, preload=True, verbose=True)
        print("  读取成功!")
    except Exception as e:
        print(f"  读取失败: {e}")
        return

    # 2. 打印基本信息
    print("\n[2] 基本信息:")
    print(f"  通道数: {raw.info['nchan']}")
    print(f"  采样率: {raw.info['sfreq']} Hz")
    print(f"  时长: {raw.times[-1]:.2f} s")
    print(f"  通道标签: {raw.ch_names}")
    print(f"  测量日期: {raw.info['meas_date']}")

    # 3. 数据统计
    print("\n[3] 数据统计 (每通道):")
    data = raw.get_data()
    print(f"  数据形状: {data.shape} (channels x samples)")
    print(f"  {'通道':<8} {'min(μV)':>10} {'max(μV)':>10} {'mean(μV)':>10} {'std(μV)':>10} {'峰峰值(μV)':>12}")
    for i, ch in enumerate(raw.ch_names):
        ch_data = data[i] * 1e6
        print(f"  {ch:<8} {ch_data.min():>10.1f} {ch_data.max():>10.1f} "
              f"{ch_data.mean():>10.1f} {ch_data.std():>10.1f} {ch_data.ptp():>12.1f}")

    # 4. 检查异常
    print("\n[4] 异常检查:")
    issues = []
    for i, ch in enumerate(raw.ch_names):
        ch_data = data[i] * 1e6
        if abs(ch_data.mean()) > 1000:
            issues.append(f"  [警告] {ch}: DC偏移过大 ({ch_data.mean():.1f}μV)")
        if ch_data.std() < 1:
            issues.append(f"  [警告] {ch}: 信号方差过小 (std={ch_data.std():.2f}μV), 可能是死通道")
        if ch_data.std() > 500:
            issues.append(f"  [警告] {ch}: 信号方差过大 (std={ch_data.std():.1f}μV), 可能含噪声/伪迹")
        if np.any(np.abs(ch_data) > 10000):
            issues.append(f"  [警告] {ch}: 存在极端值 (>{10000}μV), 可能饱和")
    if issues:
        for issue in issues:
            print(issue)
    else:
        print("  未发现明显异常")

    # 5. 可视化
    print("\n[5] 可视化...")

    fig = plt.figure(figsize=(14, 10), facecolor='#1a1a2e')

    # 5a. 原始波形 (去直流)
    ax1 = fig.add_subplot(3, 1, 1, facecolor='#1a1a2e')
    colors = ['#4FC3F7', '#FF8A65', '#81C784', '#FFD54F',
              '#7C4DFF', '#4DD0E1', '#F06292', '#A1887F']
    t = raw.times
    for i, ch in enumerate(raw.ch_names):
        ch_data = data[i] * 1e6
        ch_data_dc = ch_data - ch_data.mean()
        offset = (raw.info['nchan'] - 1 - i) * 300
        color = colors[i % len(colors)]
        ax1.plot(t, ch_data_dc + offset, linewidth=0.5, color=color, label=ch)
        ax1.axhline(y=offset, color='#2A3441', linewidth=0.3, alpha=0.5)
    ax1.set_title("波形 (去直流, 每通道偏移300μV)", color='white', fontsize=10)
    ax1.set_ylabel("μV", color='white')
    ax1.tick_params(colors='gray', labelsize=7)
    ax1.legend(loc='upper right', fontsize=7, ncol=8, facecolor='#1a1a2e', edgecolor='gray',
               labelcolor='white')
    for spine in ax1.spines.values():
        spine.set_color('gray')

    # 5b. 功率谱密度
    ax2 = fig.add_subplot(3, 1, 2, facecolor='#1a1a2e')
    psds, freqs = mne.time_frequency.psd_array_welch(data, raw.info['sfreq'], fmax=60)
    for i, ch in enumerate(raw.ch_names):
        color = colors[i % len(colors)]
        ax2.semilogy(freqs, psds[i], linewidth=0.8, color=color, label=ch)
    ax2.set_title("功率谱密度 (Welch)", color='white', fontsize=10)
    ax2.set_ylabel("μV²/Hz", color='white')
    ax2.set_xlim(0, 60)
    ax2.tick_params(colors='gray', labelsize=7)
    ax2.legend(loc='upper right', fontsize=7, ncol=8, facecolor='#1a1a2e', edgecolor='gray',
               labelcolor='white')
    for spine in ax2.spines.values():
        spine.set_color('gray')

    # 5c. 每通道方差柱状图
    ax3 = fig.add_subplot(3, 1, 3, facecolor='#1a1a2e')
    stds = [data[i].std() * 1e6 for i in range(raw.info['nchan'])]
    bars = ax3.bar(raw.ch_names, stds, color=colors[:raw.info['nchan']], edgecolor='white', linewidth=0.5)
    ax3.set_title("每通道标准差", color='white', fontsize=10)
    ax3.set_ylabel("μV", color='white')
    ax3.tick_params(colors='gray', labelsize=7)
    for spine in ax3.spines.values():
        spine.set_color('gray')
    for bar, std in zip(bars, stds):
        ax3.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
                 f'{std:.1f}', ha='center', color='white', fontsize=7)

    plt.tight_layout()
    plt.show()

    # 6. MNE标准plot (交互式)
    print("\n[6] 启动MNE标准波形浏览器 (关闭窗口后退出)...")
    raw.plot(n_channels=raw.info['nchan'], scalings='auto', title='MNE标准BDF波形浏览')
    plt.show()

    print("\n分析完成! 验证通过")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path = "EEG_0001.BDF"
    analyze_bdf(path)