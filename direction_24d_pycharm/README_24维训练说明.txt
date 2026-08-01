24列协议/14维空间窄带+收缩LDA采集、训练和烧录说明
=================================================

本版只调整FFT24模型内部特征与融合：
- 串口DIRCSV仍是label+24列，Python UI和保存路径不变；
- feat0..feat13为14个空间窄带特征，feat14..feat23固定为0；
- 训练器只训练前14维，导出时补成原固件24元素数组；
- FFT分支改为连续两窗直接平均，仍每125点更新且不按任务重置；
- CSP分支仍为原EMA 0.60+4窗历史；V3F仍每2秒发送一次RESULT。

一、采集与自动保存
1. Python UI选择COLLECT并点击“应用”。
2. 点击“训练”，完成LEFT、RIGHT、LEFT、RIGHT四个试次。
3. 数据会自动保存到本目录下的data文件夹：
   train_DIRCSV_时间.csv
   train_DIRCSV_时间_meta.json
4. CSV仍是label+24个特征；JSON记录试次编号，用于避免相邻窗口泄漏到验证集。
5. 一次采集只有左右各2个独立试次。正式展示建议重复采集至少5次，
   使data中达到左右各10个独立试次；更稳妥是7次、左右各14个试次。

重要：本版feat0..feat13的含义已经改变。必须先烧录本版V5F，再重新采集。
旧版24维CSV的feat14..feat23不是0，训练器会主动拒绝，禁止新旧数据混训。

二、训练
1. 第一次使用，双击“安装依赖_24维.bat”。
2. 双击“一键训练24维并替换权重.bat”。
3. 训练器读取data中的全部有效CSV/TXT/LOG文件，训练前14维空间窄带收缩LDA
   （solver=lsqr, shrinkage=auto），随后补成兼容固件的24元素权重。
4. 训练器按独立试次验证；data里有多个采集文件时还会做跨文件验证。
5. 正确权重会自动备份并覆盖：
   ../../Common/DualCore_Direction_Model_4ch_Weights.h

三、输出
output/metrics.txt              训练与验证报告
output/metrics.json             机器可读报告
output/model_search.csv         LDA验证结果
output/trained_fft24_lda.joblib Python模型
output/DualCore_Direction_Model_4ch_Weights.h 生成的MCU权重
output/backup/                  原正式权重备份

四、烧录与测试
1. MounRiver中Clean V5F、Build V5F并重新烧录V5F。
2. UI选择COLLECT并点击“应用”，再点击“测试”。
3. 推理结果中的MODEL=FFT24是保留的协议名称，表示当前使用FFT分支；
   本版FFT分支的有效模型维数是14，串口和权重数组仍保持24维兼容格式。

重要：为避免把网络数据或其他人的权重写入正式工程，包内固件权重保留原值。
请先烧录本版V5F并重新采集本人数据，再运行“一键训练24维并替换权重.bat”；
生成头文件中的Model注释应为14D spatial narrow-band shrinkage LDA，
然后再Clean/Build并烧录V5F。

注意：train_DIRCSV不能放入python_csp/data，也不能用CSP训练器训练；
train_CSP同样不能放入本目录data。两种数据格式不可互相恢复或替代。
