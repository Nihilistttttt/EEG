训练完成后这里会生成 MCU 可替换的 5 个文件：
- Direction_Classifier.c
- Direction_Classifier.h
- Direction_Model.c
- Direction_Model.h
- Direction_Model_4ch_Weights.h

把它们覆盖到当前 MCU 工程里的同名文件，然后 Clean V3F / Build V3F / Clean V5F / Build V5F / 下载 V5F。
