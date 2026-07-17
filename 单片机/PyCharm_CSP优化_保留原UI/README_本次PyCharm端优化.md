# PyCharm端 CSP 训练优化版（保留原UI与单片机协议）

## 未修改的内容

- 医生端原有窗口布局、样式和功能；
- 串口连接、模式切换、原始日志、姿态显示和推理显示；
- DIRCSV与CSP串口协议；
- CSP主CSV列格式：`label,win_id,sample_idx,ch0,ch1,ch2,ch3`；
- V5F所需权重变量名：`csp4ch_matrix`、`csp4ch_mean`、`csp4ch_scale`、`csp4ch_svm_weight`、`csp4ch_svm_bias`；
- 权重自动复制目标：`Common/DualCore_CSP_Model_4ch_Weights.h`。

## 本次必要修改

### 1. 上位机采集组织

- 普通24维COLLECT仍使用原来的4组：LEFT、RIGHT、LEFT、RIGHT；
- CSP模式改为左右各6个独立试次，共12组；
- 顺序平衡随机，并限制连续同类试次不超过2次；
- 主CSV格式保持不变；
- 额外保存同名的`*_meta.json`，记录每个窗口所属试次，避免重叠窗口在训练验证时被拆开。

如需改变CSP试次数量，只修改：

```python
CSP_TRIALS_PER_CLASS = 6
```

### 2. CSP训练程序

训练脚本会自动比较：

- 正则化CSP + Linear SVM；
- 正则化CSP + LDA；
- 正则化CSP + Logistic Regression。

同时比较：

- 两端2个CSP分量；
- 全部4个CSP分量；
- 多个CSP协方差正则参数；
- 多个分类器正则参数。

验证方式：

- 按独立试次分组验证，同一试次的重叠窗口不会进入不同数据折；
- `data`中存在多个采集文件时，额外进行跨文件/跨会话验证；
- 自动惩罚长期偏向LEFT或RIGHT的候选模型。

最终无论选择SVM、LDA还是逻辑回归，均转换为原V5F可直接执行的4维线性`weight + bias`，因此不需要修改单片机推理代码。

## 使用步骤

1. 运行根目录下的`exercise_Data_CSP完整链路.py`采集CSP数据；
2. 数据自动保存到`python_csp/data`；
3. 第一次使用运行`python_csp/安装依赖.bat`；
4. 运行`python_csp/一键训练CSP并替换权重.bat`；
5. 查看`python_csp/output/metrics.txt`；
6. 在MounRiver中Clean、Build并重新烧录V5F。

## 主要输出

- `output/DualCore_CSP_Model_4ch_Weights.h`：MCU权重；
- `output/metrics.txt`：最终模型与验证结果；
- `output/model_search.csv`：全部候选模型比较；
- `output/csp_features.csv`：CSP特征；
- `output/trained_csp_linear.joblib`：Python模型；
- `output/trained_csp_svm.joblib`：为兼容旧使用习惯保留的同内容文件。

## 判断模型是否可靠

重点查看：

- 独立试次验证平衡准确率；
- 跨文件/跨会话验证平衡准确率；
- 验证RIGHT比例是否接近0.5；
- 混淆矩阵是否同时能识别LEFT和RIGHT。

训练集准确率高但独立试次或跨会话准确率低，说明模型仍然过拟合，应该增加重新佩戴后的独立试次，而不是增加网络层数。
