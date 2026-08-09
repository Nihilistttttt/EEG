# EEG Project Notes

## MCU Firmware
- Linker scripts: `D:\Libraries\Projects\EEG\EXAM\SRC\Ld\V3F\Link_v3f.ld` and `V5F\Link_v5f.ld`
- Core_V5F_StartAddr: `D:\Libraries\Projects\EEG\EXAM\SRC\Debug\debug.h`
- FLASH partition: V3F=92K(0x0~0x17000), V5F=140K(0x17000~), total=232K
- Build tool: MounRiver Studio 2 (risc-v embedded gcc12)
- Project name: GPIO_Toggle_V3F

## Android
- Build: JDK11 at `D:\InstallData\ProgrammingTools\AndroidStudio\AndroidStudio\jre`
- Gradle: `.\gradlew.bat compileDebugJavaWithJavac` in `doctor_merged/`

## Python
- Training script: `direction_24d_pycharm/train_direction_24d.py`
- LDA: sklearn `LDA(solver='lsqr', shrinkage='auto', priors=[0.5,0.5])` on 14 features