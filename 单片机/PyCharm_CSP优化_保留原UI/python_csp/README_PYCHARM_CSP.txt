CSP训练工具已升级，详细说明请打开：README_CSP完整链路.md

使用：
1. 将上位机保存的 train_CSP_时间.csv 放入 python_csp/data；
2. 第一次双击 安装依赖.bat；
3. 双击 一键训练CSP并替换权重.bat；
4. 在MounRiver中 Clean/Build V5F，并重新烧录V5F；
5. RESULT中 MODEL=CSP 表示CSP推理已生效。

================ 本次优化说明 ================
本版新增正则化CSP、按试次/跨文件验证，以及SVM、LDA、逻辑回归自动比较。
医生端UI布局、CSP CSV格式和MCU权重变量名保持不变。
详细说明见上一级目录 README_本次PyCharm端优化.md。
