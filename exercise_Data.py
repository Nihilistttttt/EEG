#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
EEG 训练/测试上位机 - 动态模式切换（最终完整版）
- 训练：固定4组（左→右→左→右）
- 测试：自动进入推理模式，解析INTENT和S_LEFT/S_RIGHT
- 停止：发送STOP，下位机暂停所有活动
- 中央反馈区动态显示状态
"""

import sys
import re
import random
import csv
from datetime import datetime
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QTextEdit, QLabel, QComboBox, QGroupBox,
    QProgressBar, QMessageBox, QLineEdit, QPlainTextEdit, QTabWidget
)
from PyQt5.QtCore import QThread, pyqtSignal, Qt, QTimer
from PyQt5.QtGui import QFont, QIcon, QTextCursor

# ========================= 串口接收线程 =========================
class SerialThread(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, port, baudrate=2000000):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.running = False

    def run(self):
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=0.1)
            self.running = True
            while self.running:
                if self.serial.in_waiting:
                    line = self.serial.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        self.data_received.emit(line)
                else:
                    self.msleep(10)
        except Exception as e:
            self.data_received.emit(f"SERIAL_ERROR:{str(e)}")
        finally:
            if self.serial and self.serial.is_open:
                self.serial.close()

    def stop(self):
        self.running = False
        self.quit()
        self.wait()

    def send_command(self, cmd):
        if self.serial and self.serial.is_open:
            self.serial.write((cmd + '\r\n').encode())


# ========================= 主窗口 =========================
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("EEG 控制台 - 动态模式切换")
        self.setGeometry(150, 150, 1500, 850)
        self.setMinimumSize(950, 750)

        # ---------- 窗口图标 ----------
        style = self.style()
        self.setWindowIcon(style.standardIcon(style.SP_ComputerIcon))

        # ---------- 数据存储 ----------
        self.train_data = []          # DIRCSV 数据
        self.csp_data = []            # CSP 数据
        self.trial_sequence = []
        self.current_trial_index = 0
        self.is_training = False
        self.is_testing = False
        self.waiting_for_ready = False
        self.pending_train = False
        self.pending_test = False
        self.test_results = []
        self.received_feature_count = 0

        self.csp_window_buffer = []
        self.csp_label = 0
        self.csp_win_id = 0
        self.csp_receiving = False

        self.posture_name = "UNKNOWN"
        self.posture_gx = 0
        self.posture_gy = 0
        self.posture_gz = 0
        self.posture_conf = 0
        self.posture_stable_ms = 0
        self.turn_count = 0
        self.fall_detected = False

        # ---------- UI ----------
        self.init_ui()
        self.apply_stylesheet()
        self.serial_thread = None

        self.refresh_timer = QTimer()
        self.refresh_timer.timeout.connect(self.refresh_ports)
        self.refresh_timer.start(2000)

    # ----------------------- 全局样式 -----------------------
    def apply_stylesheet(self):
        style = """
        QMainWindow {
            background-color: #f0f2f5;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #d0d7de;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px 0 6px;
            color: #24292f;
        }
        QLabel {
            color: #24292f;
            font-size: 9pt;
        }
        QPushButton {
            background-color: #40739e;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: bold;
            font-size: 9pt;
        }
        QPushButton:hover {
            background-color: #487eb0;
        }
        QPushButton:pressed {
            background-color: #353b48;
        }
        QPushButton:disabled {
            background-color: #b2bec3;
            color: #dfe6e9;
        }
        QPushButton#trainBtn { background-color: #00a8ff; }
        QPushButton#trainBtn:hover { background-color: #0097e6; }
        QPushButton#testBtn { background-color: #e1b12c; }
        QPushButton#testBtn:hover { background-color: #fbc531; }
        QPushButton#stopBtn { background-color: #e84118; }
        QPushButton#stopBtn:hover { background-color: #c23616; }
        QPushButton#connectBtn { background-color: #4cd137; }
        QPushButton#connectBtn:hover { background-color: #44bd32; }
        QPushButton#statusBtn { background-color: #9c88ff; }
        QPushButton#statusBtn:hover { background-color: #8c7ae6; }
        QPushButton#applyModeBtn { background-color: #00a8ff; }
        QPushButton#applyModeBtn:hover { background-color: #0097e6; }
        QPushButton#ipcdiagBtn { background-color: #4cd137; }
        QPushButton#ipcdiagBtn:hover { background-color: #44bd32; }
        QPushButton#ipcdiagBtn:checked { background-color: #7f8c8d; }
        QPushButton#postureBtn { background-color: #00cec9; }
        QPushButton#postureBtn:hover { background-color: #01b5b0; }
        QPushButton#postureBtn:checked { background-color: #7f8c8d; }

        QLineEdit, QComboBox, QPlainTextEdit, QTextEdit {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            padding: 4px 8px;
            background-color: #f8f9fa;
            font-size: 9pt;
        }
        QLineEdit:focus, QComboBox:focus, QPlainTextEdit:focus, QTextEdit:focus {
            border: 1px solid #40739e;
            background-color: white;
        }
        QProgressBar {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            text-align: center;
            background-color: #ecf0f1;
            height: 18px;
            font-size: 9pt;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #00a8ff, stop:1 #4cd137);
            border-radius: 6px;
        }
        QPlainTextEdit, QTextEdit {
            background-color: #fafbfc;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 9pt;
        }
        QTabWidget::pane {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #e9ecef;
            border: 1px solid #d0d7de;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 4px 10px;
            margin-right: 2px;
            font-size: 9pt;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 2px solid #00a8ff;
        }
        QTabBar::tab:hover {
            background-color: #dee2e6;
        }
        QLabel#arrowLabel {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #ffffff, stop:1 #f0f2f5);
            border: 2px solid #d0d7de;
            border-radius: 12px;
        }
        """
        self.setStyleSheet(style)

    # ----------------------- UI 构建 -----------------------
    def init_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        main_layout.setSpacing(12)
        main_layout.setContentsMargins(20, 20, 20, 20)

        # ============================================================
        # 1. 顶部控制区：串口 | 模式 | 控制面板
        # ============================================================
        top_control_layout = QHBoxLayout()
        top_control_layout.setSpacing(12)

        # ---- 1.1 串口控制 ----
        port_group = QGroupBox("串口控制")
        port_layout = QHBoxLayout(port_group)
        port_layout.setSpacing(8)
        port_layout.setContentsMargins(10, 12, 10, 10)
        port_layout.addWidget(QLabel("端口:"))
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(100)
        port_layout.addWidget(self.port_combo)
        self.connect_btn = QPushButton("连接")
        self.connect_btn.setObjectName("connectBtn")
        style = self.style()
        self.connect_btn.setIcon(style.standardIcon(style.SP_DriveNetIcon))
        self.connect_btn.clicked.connect(self.toggle_serial)
        port_layout.addWidget(self.connect_btn)
        self.status_label = QLabel("未连接")
        self.status_label.setStyleSheet("color: #e84118; font-weight: bold;")
        port_layout.addWidget(self.status_label)
        port_layout.addStretch()
        top_control_layout.addWidget(port_group, stretch=1)

        # ---- 1.2 EEG模式 ----
        mode_group = QGroupBox("EEG 模式")
        mode_layout = QHBoxLayout(mode_group)
        mode_layout.setSpacing(8)
        mode_layout.setContentsMargins(10, 12, 10, 10)
        mode_layout.addWidget(QLabel("模式:"))
        self.mode_combo = QComboBox()
        self.mode_combo.addItem("COLLECT", 1)
        self.mode_combo.addItem("INFER", 2)
        self.mode_combo.addItem("CSP", 3)
        mode_layout.addWidget(self.mode_combo)
        self.apply_mode_btn = QPushButton("应用")
        self.apply_mode_btn.setObjectName("applyModeBtn")
        self.apply_mode_btn.setIcon(style.standardIcon(style.SP_DialogApplyButton))
        self.apply_mode_btn.clicked.connect(self.apply_mode)
        self.apply_mode_btn.setEnabled(False)
        mode_layout.addWidget(self.apply_mode_btn)
        self.mode_status_label = QLabel("COLLECT")
        self.mode_status_label.setStyleSheet("color: #00a8ff; font-weight: bold;")
        mode_layout.addWidget(self.mode_status_label)
        mode_layout.addStretch()
        top_control_layout.addWidget(mode_group, stretch=1)

        # ---- 1.3 控制面板 ----
        control_group = QGroupBox("控制面板")
        control_layout = QVBoxLayout(control_group)
        control_layout.setSpacing(6)
        control_layout.setContentsMargins(10, 12, 10, 10)

        # 状态行
        status_row = QHBoxLayout()
        status_row.setSpacing(10)
        self.mode_label = QLabel("空闲")
        self.mode_label.setFont(QFont("Segoe UI", 9, QFont.Bold))
        self.mode_label.setStyleSheet("color: #7f8c8d;")
        status_row.addWidget(self.mode_label)
        self.trial_info = QLabel("")
        self.trial_info.setFont(QFont("Segoe UI", 9))
        status_row.addStretch()
        control_layout.addLayout(status_row)

        # 按钮行
        btn_row = QHBoxLayout()
        btn_row.setSpacing(6)
        self.train_btn = QPushButton("训练 (4组)")
        self.train_btn.setObjectName("trainBtn")
        self.train_btn.setIcon(style.standardIcon(style.SP_MediaPlay))
        self.train_btn.clicked.connect(self.start_training)
        self.train_btn.setEnabled(False)
        btn_row.addWidget(self.train_btn)

        self.test_btn = QPushButton("测试")
        self.test_btn.setObjectName("testBtn")
        self.test_btn.setIcon(style.standardIcon(style.SP_DialogYesButton))
        self.test_btn.clicked.connect(self.start_testing)
        self.test_btn.setEnabled(False)
        btn_row.addWidget(self.test_btn)

        self.stop_btn = QPushButton("停止")
        self.stop_btn.setObjectName("stopBtn")
        self.stop_btn.setIcon(style.standardIcon(style.SP_MediaStop))
        self.stop_btn.clicked.connect(lambda: self.stop_all(True))
        self.stop_btn.setEnabled(False)
        btn_row.addWidget(self.stop_btn)

        self.status_btn = QPushButton("状态")
        self.status_btn.setObjectName("statusBtn")
        self.status_btn.setIcon(style.standardIcon(style.SP_MessageBoxInformation))
        self.status_btn.clicked.connect(self.send_status)
        self.status_btn.setEnabled(False)
        btn_row.addWidget(self.status_btn)

        self.ipcdiag_btn = QPushButton("IPCDIAG: ON")
        self.ipcdiag_btn.setObjectName("ipcdiagBtn")
        self.ipcdiag_btn.setCheckable(True)
        self.ipcdiag_btn.setChecked(True)
        self.ipcdiag_btn.clicked.connect(self.toggle_ipcdiag)
        self.ipcdiag_btn.setEnabled(False)
        btn_row.addWidget(self.ipcdiag_btn)

        self.posture_btn = QPushButton("姿态数据: ON")
        self.posture_btn.setObjectName("postureBtn")
        self.posture_btn.setCheckable(True)
        self.posture_btn.setChecked(True)
        self.posture_btn.clicked.connect(self.toggle_posture)
        self.posture_btn.setEnabled(False)
        btn_row.addWidget(self.posture_btn)

        self.cmd_edit = QLineEdit()
        self.cmd_edit.setPlaceholderText("命令...")
        self.cmd_edit.returnPressed.connect(self.send_manual_cmd)
        btn_row.addWidget(self.cmd_edit)

        control_layout.addLayout(btn_row)
        top_control_layout.addWidget(control_group, stretch=2)
        main_layout.addLayout(top_control_layout)

        # ============================================================
        # 2. 中央反馈区：大状态显示
        # ============================================================
        arrow_group = QGroupBox("状态反馈")
        arrow_layout = QVBoxLayout(arrow_group)
        arrow_layout.setContentsMargins(15, 15, 15, 15)
        self.arrow_label = QLabel()
        self.arrow_label.setObjectName("arrowLabel")
        self.arrow_label.setAlignment(Qt.AlignCenter)
        self.arrow_label.setMinimumHeight(160)
        self.arrow_label.setFont(QFont("Segoe UI", 48, QFont.Bold))
        self.arrow_label.setText("空闲")
        self.arrow_label.setStyleSheet("color: #2f3640;")
        arrow_layout.addWidget(self.arrow_label)
        main_layout.addWidget(arrow_group, stretch=1)

        # ============================================================
        # 3. 进度与状态区
        # ============================================================
        progress_group = QGroupBox("进度")
        progress_layout = QVBoxLayout(progress_group)
        progress_layout.setSpacing(6)
        progress_layout.setContentsMargins(15, 12, 15, 12)
        self.progress = QProgressBar()
        self.progress.setRange(0, 100)
        self.progress.setValue(0)
        progress_layout.addWidget(self.progress)
        self.stat_label = QLabel("特征: 0 行 | 测试结果: 0")
        self.stat_label.setStyleSheet("color: #2f3640; font-weight: bold; font-size: 9pt;")
        progress_layout.addWidget(self.stat_label)
        self.posture_label = QLabel("姿态: 等待数据")
        self.posture_label.setStyleSheet("color: #00cec9; font-weight: bold; font-size: 9pt;")
        progress_layout.addWidget(self.posture_label)
        main_layout.addWidget(progress_group)

        # ============================================================
        # 4. 底部日志区：标签页
        # ============================================================
        log_tabs = QTabWidget()
        log_tabs.setDocumentMode(True)
        log_tabs.setTabPosition(QTabWidget.South)

        raw_tab = QWidget()
        raw_layout = QVBoxLayout(raw_tab)
        raw_layout.setContentsMargins(10, 10, 10, 10)
        self.raw_text = QPlainTextEdit()
        self.raw_text.setReadOnly(True)
        self.raw_text.setFont(QFont("Consolas", 9))
        self.raw_text.setMaximumBlockCount(500)
        raw_layout.addWidget(self.raw_text)
        clear_raw_btn = QPushButton("清空原始数据")
        clear_raw_btn.setStyleSheet("background-color: #7f8c8d; padding: 4px 12px; font-size: 9pt;")
        clear_raw_btn.setIcon(style.standardIcon(style.SP_TrashIcon))
        clear_raw_btn.clicked.connect(lambda: self.raw_text.clear())
        raw_layout.addWidget(clear_raw_btn, alignment=Qt.AlignRight)
        log_tabs.addTab(raw_tab, "原始数据")

        log_tab = QWidget()
        log_layout = QVBoxLayout(log_tab)
        log_layout.setContentsMargins(10, 10, 10, 10)
        self.result_text = QTextEdit()
        self.result_text.setReadOnly(True)
        self.result_text.setFont(QFont("Consolas", 9))
        log_layout.addWidget(self.result_text)
        log_tabs.addTab(log_tab, "解析日志")

        main_layout.addWidget(log_tabs, stretch=2)

        # 初始状态
        self.update_central_status("空闲", "#7f8c8d")

    # ======================= 中央状态更新 =======================
    def update_central_status(self, text, color="#2f3640", is_arrow=False):
        if is_arrow:
            border_color = "#0984e3" if "左" in text else "#e84118"
            self.arrow_label.setStyleSheet(
                f"color: {color}; background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                f"stop:0 #dfe6e9, stop:1 #b2bec3); font-size: 56pt; font-weight: bold; "
                f"border-radius: 12px; border: 3px solid {border_color};"
            )
        else:
            self.arrow_label.setStyleSheet(
                f"color: {color}; background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                f"stop:0 #f0f2f5, stop:1 #dcdde1); font-size: 40pt; font-weight: bold; "
                f"border-radius: 12px; border: 2px solid #d0d7de;"
            )
        self.arrow_label.setText(text)

    # ======================= 串口管理 =======================
    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        current = self.port_combo.currentText()
        self.port_combo.clear()
        for p in ports:
            self.port_combo.addItem(p.device)
        if current and self.port_combo.findText(current) >= 0:
            self.port_combo.setCurrentText(current)

    def toggle_serial(self):
        if self.serial_thread and self.serial_thread.isRunning():
            self.serial_thread.stop()
            self.serial_thread = None
            self.connect_btn.setText("连接")
            self.connect_btn.setIcon(self.style().standardIcon(self.style().SP_DriveNetIcon))
            self.connect_btn.setStyleSheet("background-color: #4cd137;")
            self.status_label.setText("未连接")
            self.status_label.setStyleSheet("color: #e84118; font-weight: bold;")
            self.train_btn.setEnabled(False)
            self.test_btn.setEnabled(False)
            self.stop_btn.setEnabled(False)
            self.status_btn.setEnabled(False)
            self.apply_mode_btn.setEnabled(False)
            self.ipcdiag_btn.setEnabled(False)
            self.posture_btn.setEnabled(False)
            self.append_log("[系统] 串口已断开", "gray")
            self.update_central_status("空闲 (串口断开)", "#7f8c8d")
            return

        port = self.port_combo.currentText()
        if not port:
            QMessageBox.warning(self, "提示", "请选择串口")
            return
        self.serial_thread = SerialThread(port, 2000000)
        self.serial_thread.data_received.connect(self.parse_serial_line)
        self.serial_thread.start()
        self.connect_btn.setText("断开")
        self.connect_btn.setIcon(self.style().standardIcon(self.style().SP_MediaStop))
        self.connect_btn.setStyleSheet("background-color: #e84118;")
        self.status_label.setText("已连接")
        self.status_label.setStyleSheet("color: #4cd137; font-weight: bold;")
        self.train_btn.setEnabled(True)
        self.test_btn.setEnabled(True)
        self.status_btn.setEnabled(True)
        self.apply_mode_btn.setEnabled(True)
        self.ipcdiag_btn.setEnabled(True)
        self.posture_btn.setEnabled(True)
        self.append_log("[系统] 串口已连接", "green")
        self.serial_thread.send_command("STATUS")
        self.update_central_status("空闲", "#7f8c8d")

    def send_manual_cmd(self):
        cmd = self.cmd_edit.text().strip()
        if cmd and self.serial_thread:
            self.serial_thread.send_command(cmd)
            self.append_log(f"[手动发送] {cmd}", "magenta")
            self.cmd_edit.clear()

    def send_status(self):
        if self.serial_thread:
            self.serial_thread.send_command("STATUS")

    def toggle_ipcdiag(self):
        if not self.serial_thread:
            return
        if self.ipcdiag_btn.isChecked():
            self.serial_thread.send_command("IPCDIAG,ON")
            self.ipcdiag_btn.setText("IPCDIAG: ON")
            self.append_log("[系统] IPCDIAG 已开启", "green")
        else:
            self.serial_thread.send_command("IPCDIAG,OFF")
            self.ipcdiag_btn.setText("IPCDIAG: OFF")
            self.append_log("[系统] IPCDIAG 已关闭", "orange")

    def toggle_posture(self):
        if not self.serial_thread:
            return
        if self.posture_btn.isChecked():
            self.serial_thread.send_command("POSTURE,ON")
            self.posture_btn.setText("姿态数据: ON")
            self.append_log("[系统] 姿态数据流已开启", "green")
        else:
            self.serial_thread.send_command("POSTURE,OFF")
            self.posture_btn.setText("姿态数据: OFF")
            self.append_log("[系统] 姿态数据流已关闭（事件仍正常输出）", "orange")

    def apply_mode(self):
        if not self.serial_thread:
            QMessageBox.warning(self, "提示", "请先连接串口")
            return
        mode = self.mode_combo.currentData()
        self.serial_thread.send_command(f"MODE,SET,{mode}")
        self.append_log(f"[系统] 发送模式切换: {mode}", "cyan")

    # ======================= 日志 =======================
    def append_log(self, text, color=None):
        if color:
            self.result_text.append(f'<span style="color:{color};">{text}</span>')
        else:
            self.result_text.append(text)
        self.result_text.moveCursor(QTextCursor.End)

    def append_raw(self, line):
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.raw_text.appendPlainText(f"[{timestamp}] {line}")
        cursor = self.raw_text.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.raw_text.setTextCursor(cursor)

    # ======================= 串口解析 =======================
    def parse_serial_line(self, line):
        self.append_raw(line)

        # ---- 任务开始 ----
        match = re.match(r'TASK,(\w+),start', line)
        if match:
            side = match.group(1)
            if side == 'LEFT':
                self.show_arrow_left()
                self.append_log(">>> 左手任务开始 <<<", "blue")
            else:
                self.show_arrow_right()
                self.append_log(">>> 右手任务开始 <<<", "red")
            if self.is_training:
                total = len(self.trial_sequence)
                cur = self.current_trial_index + 1
                self.trial_info.setText(f"试次 {cur}/{total} ({side})")
            return

        # ---- 任务完成 ----
        if line == 'TASK,DONE':
            self.append_log("--- 试次完成 ---", "green")
            self.on_trial_done()
            return

        # ---- 停止 ----
        if line == 'TASK,STOPPED':
            self.append_log("[系统] 已停止", "orange")
            self.stop_all(False)
            return

        # ---- DIRCSV ----
        match = re.match(r'DIRCSV,(\d+),(.+)$', line)
        if match:
            label = int(match.group(1))
            feat_str = match.group(2)
            try:
                features = [int(x) for x in feat_str.split(',') if x.strip()]
                if len(features) == 24:
                    self.train_data.append((label, features))
                    self.received_feature_count += 1
                    self.stat_label.setText(
                        f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                    )
                    if self.is_training:
                        total = len(self.trial_sequence)
                        done = len(self.train_data) // 20
                        progress = min(100, int(done / total * 100))
                        self.progress.setValue(progress)
            except:
                pass
            return

        # ---- CSP ----
        if line.startswith('CSP_BEGIN,'):
            parts = line.split(',')
            try:
                self.csp_label = int(parts[1].split('=')[1])
                self.csp_win_id = int(parts[2].split('=')[1])
                self.csp_window_buffer = []
                self.csp_receiving = True
                self.append_log(f"[CSP] 窗口 {self.csp_win_id} 开始 (label={self.csp_label})", "cyan")
            except:
                pass
            return

        if line.startswith('CSP,'):
            if self.csp_receiving:
                parts = line.split(',')
                try:
                    idx = int(parts[2])
                    chs = [int(parts[3]), int(parts[4]), int(parts[5]), int(parts[6])]
                    self.csp_window_buffer.append((idx, chs))
                except:
                    pass
            return

        if line.startswith('CSP_END,'):
            if self.csp_receiving:
                self.csp_receiving = False
                self.csp_data.append((self.csp_label, self.csp_win_id, self.csp_window_buffer))
                self.received_feature_count += 1
                self.stat_label.setText(
                    f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                )
                if self.is_training:
                    total = len(self.trial_sequence)
                    done = len(self.csp_data) // 4
                    progress = min(100, int(done / total * 100))
                    self.progress.setValue(progress)
                self.append_log(f"[CSP] 窗口 {self.csp_win_id} 完成", "cyan")
            return

        # ---- 推理结果（测试模式） ----
        # 匹配 INTENT=xxx 和 S_LEFT=xxx S_RIGHT=xxx
        if 'INTENT=' in line and 'S_LEFT=' in line and 'S_RIGHT=' in line:
            intent_match = re.search(r'INTENT=(\w+)', line)
            s_left_match = re.search(r'S_LEFT=(\d+)', line)
            s_right_match = re.search(r'S_RIGHT=(\d+)', line)
            if intent_match and s_left_match and s_right_match:
                intent = intent_match.group(1)
                sl = int(s_left_match.group(1))
                sr = int(s_right_match.group(1))
                self.test_results.append((intent, sl, sr))
                self.stat_label.setText(
                    f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                )
                if intent == 'LEFT':
                    self.show_arrow_left()
                    color = "blue"
                elif intent == 'RIGHT':
                    self.show_arrow_right()
                    color = "red"
                else:
                    color = "black"
                conf = abs(sl - sr)
                self.append_log(f"[TEST] {intent} (左:{sl}, 右:{sr}, 置信度:{conf})", color)
                return

        # ---- 就绪信号 ----
        if line == 'READY_TRAIN':
            self.append_log("[系统] 训练模式就绪", "green")
            self.mode_label.setText("训练中")
            self.mode_label.setStyleSheet("color: #00a8ff; font-weight: bold;")
            self.update_central_status("训练中", "#00a8ff")
            self.waiting_for_ready = False
            if self.is_training:
                self.send_next_trial()
            return

        if line == 'READY_TEST':
            self.append_log("[系统] 测试模式就绪", "green")
            self.mode_label.setText("测试中")
            self.mode_label.setStyleSheet("color: #e1b12c; font-weight: bold;")
            self.update_central_status("测试中", "#e1b12c")
            self.waiting_for_ready = False
            # 测试模式没有额外操作，等待推理结果
            return

        # ---- 模式设置成功 ----
        if line.startswith('MODE_SET_OK,'):
            mode = int(line.split(',')[1])
            mode_names = {1: "COLLECT", 2: "INFER", 3: "CSP"}
            name = mode_names.get(mode, "UNKNOWN")
            self.mode_status_label.setText(name)
            color = "blue" if mode == 1 else "red" if mode == 2 else "green"
            self.mode_status_label.setStyleSheet(f"color: {color}; font-weight: bold;")
            self.append_log(f"[系统] 模式已切换至: {name}", "cyan")
            idx = self.mode_combo.findData(mode)
            if idx >= 0:
                self.mode_combo.setCurrentIndex(idx)

            # 如果当前有等待的启动命令，则发送对应的工作模式指令
            if self.pending_train:
                self.pending_train = False
                self.serial_thread.send_command("MODE,TRAIN")
                self.append_log("[系统] 发送 MODE,TRAIN", "magenta")
            if self.pending_test:
                self.pending_test = False
                self.serial_thread.send_command("MODE,TEST")
                self.append_log("[系统] 发送 MODE,TEST", "magenta")
            return

        # ---- 错误 ----
        if line.startswith('ERROR'):
            self.append_log(f"[错误] {line}", "red")
            QMessageBox.warning(self, "下位机错误", line)
            return

        # ---- 状态 ----
        if line.startswith('STATUS,'):
            self.append_log(f"[状态] {line}", "cyan")
            parts = line.split(',')
            for part in parts:
                if part.startswith('app_mode='):
                    mode = int(part.split('=')[1])
                    mode_names = {1: "COLLECT", 2: "INFER", 3: "CSP"}
                    name = mode_names.get(mode, "UNKNOWN")
                    self.mode_status_label.setText(name)
                    color = "blue" if mode == 1 else "red" if mode == 2 else "green"
                    self.mode_status_label.setStyleSheet(f"color: {color}; font-weight: bold;")
                    idx = self.mode_combo.findData(mode)
                    if idx >= 0:
                        self.mode_combo.setCurrentIndex(idx)
            return

        # ---- IPCDIAG 确认 ----
        if line == 'IPCDIAG,ON':
            self.ipcdiag_btn.setChecked(True)
            self.ipcdiag_btn.setText("IPCDIAG: ON")
            self.append_log("[系统] IPCDIAG 已开启", "green")
            return
        if line == 'IPCDIAG,OFF':
            self.ipcdiag_btn.setChecked(False)
            self.ipcdiag_btn.setText("IPCDIAG: OFF")
            self.append_log("[系统] IPCDIAG 已关闭", "orange")
            return

        # ---- POSTURE 确认 ----
        if line == 'POSTURE,ON':
            self.posture_btn.setChecked(True)
            self.posture_btn.setText("姿态数据: ON")
            self.append_log("[系统] 姿态数据流已开启", "green")
            return
        if line == 'POSTURE,OFF':
            self.posture_btn.setChecked(False)
            self.posture_btn.setText("姿态数据: OFF")
            self.append_log("[系统] 姿态数据流已关闭（事件仍正常输出）", "orange")
            return

        # ---- POSTURE 数据（含重力向量） ----
        match = re.match(r'POSTURE,(\w+),gx=(-?\d+),gy=(-?\d+),gz=(-?\d+),conf=(-?\d+),stable=(\d+)', line)
        if match:
            self.posture_name = match.group(1)
            self.posture_gx = int(match.group(2))
            self.posture_gy = int(match.group(3))
            self.posture_gz = int(match.group(4))
            self.posture_conf = int(match.group(5))
            self.posture_stable_ms = int(match.group(6))
            self.update_posture_display()
            return

        # ---- PSTATUS 数据（轻量，无重力向量） ----
        match = re.match(r'PSTATUS,(\w+),conf=(-?\d+),stable=(\d+),turns=(\d+)', line)
        if match:
            self.posture_name = match.group(1)
            self.posture_conf = int(match.group(2))
            self.posture_stable_ms = int(match.group(3))
            self.turn_count = int(match.group(4))
            self.update_posture_display()
            return

        # ---- 翻身事件 ----
        match = re.match(r'TURN_EVENT,count=(\d+),from=(\w+),to=(\w+),tick=(\d+)', line)
        if match:
            self.turn_count = int(match.group(1))
            turn_from = match.group(2)
            turn_to = match.group(3)
            self.posture_name = turn_to
            self.append_log(f"[翻身] 第{self.turn_count}次: {turn_from} -> {turn_to}", "#00cec9")
            self.update_posture_display()
            return

        # ---- 坠床事件 ----
        match = re.match(r'FALL_EVENT,gyro=(-?\d+),acc=(-?\d+),posture=(\w+),tick=(\d+)', line)
        if match:
            gyro = int(match.group(1))
            acc = int(match.group(2))
            fall_posture = match.group(3)
            self.fall_detected = True
            self.posture_name = fall_posture
            self.append_log(f"[!!坠床!!] 角速度={gyro/10.0:.0f}dps 加速度={acc/100.0:.2f}g", "red")
            self.update_posture_display()
            QTimer.singleShot(10000, self.clear_fall_alert)
            return

        # ---- 未翻身报警 ----
        match = re.match(r'NO_TURN_ALERT,duration_min=(\d+)', line)
        if match:
            dur = int(match.group(1))
            self.append_log(f"[未翻身报警] 已{dur}分钟未翻身!", "red")
            return

    # ======================= 箭头显示 =======================
    def show_arrow_left(self):
        self.update_central_status("← 左手", "#0984e3", is_arrow=True)

    def show_arrow_right(self):
        self.update_central_status("→ 右手", "#e84118", is_arrow=True)

    def show_arrow_ready(self):
        if self.is_training:
            self.update_central_status("训练中", "#00a8ff")
        elif self.is_testing:
            self.update_central_status("测试中", "#e1b12c")
        else:
            self.update_central_status("空闲", "#7f8c8d")

    # ======================= 姿态显示 =======================
    def update_posture_display(self):
        posture_cn = {
            "SUPINE": "仰躺", "PRONE": "趴着",
            "LEFT": "左侧卧", "RIGHT": "右侧卧",
            "SITTING": "坐起", "UNKNOWN": "未知"
        }
        name = posture_cn.get(self.posture_name, self.posture_name)
        conf_pct = self.posture_conf / 100.0
        stable_s = self.posture_stable_ms / 1000.0

        if self.fall_detected:
            self.posture_label.setText(
                f"!! 坠床报警 !!  姿态: {name}  翻身: {self.turn_count}次"
            )
            self.posture_label.setStyleSheet("color: #e84118; font-weight: bold; font-size: 11pt; background-color: #ffeaa7;")
            return

        self.posture_label.setText(
            f"姿态: {name} ({conf_pct:.0f}%)  稳定: {stable_s:.1f}s  "
            f"重力: X={self.posture_gx} Y={self.posture_gy} Z={self.posture_gz}  "
            f"翻身: {self.turn_count}次"
        )
        self.posture_label.setStyleSheet("color: #00cec9; font-weight: bold; font-size: 9pt;")

    def clear_fall_alert(self):
        self.fall_detected = False
        self.update_posture_display()

    # ======================= 训练流程 =======================
    def start_training(self):
        if not self.serial_thread:
            return

        mode = self.mode_combo.currentData()
        if mode not in (1, 3):
            self.append_log("[警告] 训练模式仅支持 COLLECT 或 CSP，自动切换至 COLLECT", "orange")
            mode = 1
            self.mode_combo.setCurrentIndex(0)

        # 固定顺序：左 → 右 → 左 → 右
        self.trial_sequence = ['LEFT', 'RIGHT', 'LEFT', 'RIGHT']
        self.current_trial_index = 0
        self.train_data.clear()
        self.csp_data.clear()
        self.received_feature_count = 0
        self.is_training = True
        self.is_testing = False
        self.waiting_for_ready = True
        self.pending_train = True
        self.pending_test = False
        self.progress.setValue(0)
        self.stat_label.setText("特征: 0 行 | 测试结果: 0")
        self.trial_info.setText("")

        self.append_log("="*60, "blue")
        self.append_log("=== 开始训练（4组，左右左右） ===", "blue")
        self.append_log(f"顺序: {' -> '.join(self.trial_sequence)}", "blue")
        self.append_log("="*60, "blue")

        self.train_btn.setEnabled(False)
        self.test_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)

        self.update_central_status("训练启动中...", "#00a8ff")
        self.serial_thread.send_command(f"MODE,SET,{mode}")

    def send_next_trial(self):
        if self.current_trial_index < len(self.trial_sequence):
            side = self.trial_sequence[self.current_trial_index]
            self.serial_thread.send_command(f"TRIAL,{side}")
            self.append_log(
                f">>> 发送 {side} 试次 ({self.current_trial_index+1}/{len(self.trial_sequence)})",
                "magenta"
            )
        else:
            self.append_log("所有试次已发送，等待数据...", "green")

    def on_trial_done(self):
        if self.is_training:
            self.current_trial_index += 1
            if self.current_trial_index < len(self.trial_sequence):
                self.send_next_trial()
            else:
                self.append_log("="*60, "green")
                self.append_log(f"=== 训练完成！共接收 {self.received_feature_count} 个特征 ===", "green")
                self.append_log("="*60, "green")
                self.stop_all(False)
                self.save_training_data()
                self.update_central_status("训练完成", "#4cd137")

    def save_training_data(self):
        if self.train_data:
            filename = f"train_DIRCSV_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
            try:
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['label'] + [f'feat{i}' for i in range(24)])
                    for label, feats in self.train_data:
                        writer.writerow([label] + feats)
                self.append_log(f"[保存] DIRCSV 数据已保存至 {filename}", "green")
            except Exception as e:
                self.append_log(f"[错误] 保存失败: {e}", "red")
        elif self.csp_data:
            filename = f"train_CSP_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
            try:
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['label', 'win_id', 'sample_idx', 'ch0', 'ch1', 'ch2', 'ch3'])
                    for label, win_id, samples in self.csp_data:
                        for idx, chs in samples:
                            writer.writerow([label, win_id, idx] + chs)
                self.append_log(f"[保存] CSP 数据已保存至 {filename}", "green")
            except Exception as e:
                self.append_log(f"[错误] 保存失败: {e}", "red")
        else:
            self.append_log("[警告] 无数据可保存", "orange")

    # ======================= 测试流程 =======================
    def start_testing(self):
        if not self.serial_thread:
            return

        # 切换到推理模式
        mode = 2
        self.mode_combo.setCurrentIndex(1)
        self.pending_test = True
        self.pending_train = False
        self.is_testing = True
        self.is_training = False
        self.test_results.clear()
        self.stat_label.setText(f"特征: {self.received_feature_count} 行 | 测试结果: 0")
        self.show_arrow_ready()

        self.append_log("="*60, "red")
        self.append_log("=== 开始测试（推理模式） ===", "red")
        self.append_log("等待模式切换...", "red")
        self.append_log("="*60, "red")

        self.train_btn.setEnabled(False)
        self.test_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)

        self.update_central_status("测试启动中...", "#e1b12c")
        self.serial_thread.send_command(f"MODE,SET,{mode}")

    # ======================= 停止 =======================
    def stop_all(self, send_cmd=True):
        if send_cmd and self.serial_thread:
            self.serial_thread.send_command("STOP")
        self.is_training = False
        self.is_testing = False
        self.waiting_for_ready = False
        self.pending_train = False
        self.pending_test = False
        self.train_btn.setEnabled(True)
        self.test_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.progress.setValue(0)
        self.mode_label.setText("空闲")
        self.mode_label.setStyleSheet("color: #7f8c8d; font-weight: bold;")
        self.trial_info.setText("")
        self.append_log("[系统] 已停止所有活动", "orange")
        self.update_central_status("空闲", "#7f8c8d")

    def closeEvent(self, event):
        if self.serial_thread:
            self.serial_thread.stop()
        event.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())