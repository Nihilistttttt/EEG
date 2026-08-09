# EEG Project Notes

## MCU Firmware
- Linker scripts: `D:\Libraries\Projects\EEG\EXAM\SRC\Ld\V3F\Link_v3f.ld` and `V5F\Link_v5f.ld`
- Core_V5F_StartAddr: `D:\Libraries\Projects\EEG\EXAM\SRC\Debug\debug.h`
- FLASH partition: V3F=92K(0x0~0x17000), V5F=140K(0x17000~), total=232K
- Build tool: MounRiver Studio 2 (risc-v embedded gcc12)
- Project name: GPIO_Toggle_V3F
- Toolchain: `D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC12\bin\riscv-wch-elf-gcc.exe`
- MounRiver IDE: `D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\MounRiver Studio 2.exe`
- EEG工程根: `D:\Libraries\Projects\EEG\EXAM\GPIO\EEG` (V3F/V5F子目录 + Common)
- SRC公共: `D:\Libraries\Projects\EEG\EXAM\SRC` (Debug/Core/Peripheral/inc)

### MCU语法检查(无需IDE,命令行验证)
MounRiver Studio 2是Electron应用无CLI构建,用riscv-wch-elf-gcc -fsyntax-only做语法检查。
从.cproject提取宏(-D)和include路径(-I),构造命令:

V5F语法检查 (宏: Core_V5F GLXSS_ENABLED):
```powershell
$gcc = "D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC12\bin\riscv-wch-elf-gcc.exe"
$base = "D:\Libraries\Projects\EEG\EXAM\GPIO\EEG"; $src = "D:\Libraries\Projects\EEG\EXAM\SRC"
& $gcc -fsyntax-only -std=gnu99 -march=rv32imac -mabi=ilp32 -DCore_V5F -DGLXSS_ENABLED `
  "-I$base\V5F\User" "-I$base\V5F\Hardware" "-I$base\V5F\USB_Host" "-I$base\Common" `
  "-I$src\Debug" "-I$src\Core" "-I$src\Peripheral\inc" "$base\V5F\User\main.c"
```

V3F语法检查 (宏: Core_V3F GLXSS_ENABLED, include更多子目录):
```powershell
$v3f = "$base\V3F"
& $gcc -fsyntax-only -std=gnu99 -march=rv32imac -mabi=ilp32 -DCore_V3F -DGLXSS_ENABLED `
  "-I$src\Debug" "-I$src\Core" "-I$src\Peripheral\inc" "-I$base\Common" `
  "-I$v3f\User" "-I$v3f\Hardware" "-I$v3f\Protocol" "-I$v3f\Attention" `
  "-I$v3f\EEG_Direction" "-I$v3f\EEG_FFT" "-I$v3f\EEG_Protocol" "-I$v3f\EEG_Filter" `
  "-I$v3f\USB_Host" "-I$v3f\HAL" "-I$v3f\FFT" "$v3f\EEG_FFT\eeg_fft.c"
```
无输出=通过。完整编译链接需在MounRiver Studio 2 GUI里Build。

## embeddedskills (AI skill集)
- 安装位置: `C:\Users\hp\.codeartsdoer\skills\` (每个子skill一个目录,含SKILL.md)
- 已安装12个子skill: can/eide/gcc/jlink/keil/net/openocd/probe-rs/serial/ssh/terminal/workflow
- 注册文件: `C:\Users\hp\.codeartsdoer\skills\UserSkillStatus.txt`
- 来源: https://github.com/zhinkgit/embeddedskills (550★)

### serial skill (已验证可用)
- 扫描串口: `python C:\Users\hp\.codeartsdoer\skills\serial\scripts\serial_scan.py --json`
- 监控串口: `python ...\serial_monitor.py --port COM7 --baudrate 115200 --json`
- 本项目COM7 = WCH-LinkE (VID 1A86 PID 8010)

### openocd skill (已配置,需硬件使能SDI)
- WCH OpenOCD: `D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\bin\openocd.exe`
- OpenOCD scripts: `...\WCH\OpenOCD\OpenOCD\share\openocd\scripts`
- CH32H417 SDI配置: `D:\Libraries\Projects\EEG\.embeddedskills\openocd\ch32h417_sdi.cfg`
  - interface wlinke (WCH-LinkE调试器)
  - transport sdi (WCH两线调试协议)
  - target riscv (RISC-V内核)
  - flash地址 0x08000000
- 烧录命令:
```powershell
$oocd = "D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\bin\openocd.exe"
$scripts = "D:\InstallData\EmbTools\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\share\openocd\scripts"
$cfg = "D:\Libraries\Projects\EEG\.embeddedskills\openocd\ch32h417_sdi.cfg"
$bin = "D:\Libraries\Projects\EEG\EXAM\GPIO\EEG\V5F\obj\Merge.Bin"
& $oocd -s $scripts -f $cfg -c "flash write_image erase $bin 0x08000000 bin" -c "reset run" -c "shutdown"
```
- **注意**: 需先在硬件端使能SDI两线调试接口(boot模式+ISP工具),否则报"WCH-Link failed to connect"
- **连接失败原因**: CH32H417使用USBHS(USB Host)模式时SDI接口不可用,需切回非USBHS模式才能烧录
- WCH ISP工具(GUI): `...\WCH\Others\WCHISPTool\default\WCHISPTool_CH32Vxxx\WCHISPTool_CH32Vxxx.exe`
- WCH-LinkUtility(GUI): `...\WCH\Others\SWDTool\default\WCH-LinkUtility.exe`
- 固件文件:
  - Merge.Bin (130960B) = V3F+V5F双核合并固件,烧录到0x08000000
  - GPIO_Toggle_V5F.elf/.hex/.bin = V5F单独固件
  - GPIO_Toggle_V3F.elf/.hex/.bin = V3F单独固件

## Android
- Build: JDK11 at `D:\InstallData\ProgrammingTools\AndroidStudio\AndroidStudio\jre`
- Gradle: `.\gradlew.bat compileDebugJavaWithJavac` in `doctor_merged/`

## Python
- Training script: `direction_24d_pycharm/train_direction_24d.py`
- LDA: sklearn `LDA(solver='lsqr', shrinkage='auto', priors=[0.5,0.5])` on 14 features