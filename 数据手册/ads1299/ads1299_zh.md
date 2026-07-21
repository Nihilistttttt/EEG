# ADS1299-x is a low-noise 4-channel, 6-channel, 8-channel, 24-bit analog-to-digital converter suitable for EEG and biopotential measurement

# 1 特性

Up to 8 low-noise programmable gain amplifiers (PGA) and 8
High-resolution synchronous sampling analog-to-digital converter (ADC
输入 reference noise: $1 \mu \mathsf {V} _ {\mathsf {P P}}$(bandwidth: $70 H z)$
输入 bias 电流: $3 0 0 {\mathsf {pA}}$
Data rate: 250 sampling rates per second (SPS) to 16 thousand times per second
采样 (kSPS)  
Common-mode rejection ratio (CMRR) : -110dB
可编程增益：1，2，4，6，8，12 或者 24  
Single-pole or bipolar 电源 供电: Analog: 4.75V to 5.25V, digital: 1.8V to 3.6V
内置偏置驱动放大器，  
持续断线检测， 测试信号  
内置振荡器  
内部或者外部基准  
灵活的省电、待机模式  
Compatible with the ADS129x 引脚
A serial interface compatible with the Serial Peripheral Interface (SPI)
Operating temperature range: $\ scriptscriptstyle-4 0 ^ {\circ} \mathsf {C}$to $+ 85 ^ {\circ} \mathsf {C}$

# 2 应用

医疗器械， 包括：

Electroencephalogram (EEG) study, fetal electrocardiogram (ECG), sleep study monitor, bisspectral index (BIS), induced audio potential (EAP)

# 3 说明

The ADS1299-4, ADS1299-6 and ADS1299 devices are a series of four-channel, six-channel and eight-channel low-noise, 24-bit synchronous sampling Sigma analog-to-digital converters (ADCs). This series is equipped with a built-in programmable gain amplifier (PGA), internal reference and on-board oscillator. The ADS1299-x is equipped with all the common functions required for extracranial electroencephalogram (EEG) and electrocardiogram (ECG) applications. With high integration and outstanding performance, the ADS1299-$\mathsf {x}$can build scalable medical instrument systems in a significantly reduced size, with notably lower 电源 consumption and overall cost.

The ADS1299- $\cdot \mathsf {x}$is equipped with a flexible 输入 multiplexer in each channel, which can be independently connected to the internally generated signal to complete test, temperature and lead disconnection detection. In addition, any configuration of the 输入 channel can be selected to generate the patient bias 输出 signal. Optional SRB pins are provided, designed to route common signals to multi-channel inputs with a reference montage configuration. The ADS1299- $\cdot \mathsf {x}$operates at a data transfer rate ranging from 250SPS to 16kSPS. Lead disconnection detection can be achieved within the 器件 by using an excitation 电流 trap/电流 source.

In systems with a large number of channels, a Daisy chain configuration can be adopted to series multiple ADS1299-4, ADS1299-6 or ADS1299 devices. The ADS1299-x is packaged in TQFP-64 and operates within a temperature range of -40 ^ {\circ} \mathsf {C}$to + 85 ^ {\circ} \mathsf {C}$.

器件信息(1)  

< HTML > < body > < table > < tr > < td > 器件 models < / td > < td > encapsulate < / td > < td > package size (nominal value) < / td > < / tr > < tr > < td > ADS1299 - x < / td > < td > TQFP (64) < / td > < td > 10.00 mm X 10.00 mm < / td > < / tr > < / table > < / body > < / HTML >

(1) 要了解所有可用封装， 请参见数据表末尾的可订购产品附录。

# 方框图

![](images/57446770f3403887028c65d042322d5ba99c877bf4b1ec64d8cb077a5e7825d8.jpg)

# 目录

1 特性.   
2 应用. 1   
3 说明. 1   
4 修订历史记录 ........................................................... 2   
5设备对比
6引脚配置与功能
7规格
7.1绝对最高评级
7.2 ESD等级
7.3推荐工作条件
7.4热信息8
7.5电气特性....9
7.6时间要求:串行接口 . ...................12
7.7交换特性：串口
7.8典型特征13
8参数测量信息
8.1噪声测量  .........................................16
9详细描述18
9.1概述  ................................................................18
9.2功能框图。19
9.3特性描述. ...... .20.
9.4设备功能模式

9.5编程. ....38 9.6注册映射. ......................44
10应用与实现。61 10.1应用信息：61 10.2典型应用
11电源建议…70 11.1上电顺序70 11.2连接设备到单极（5v和3.3 V）电源70 11.3连接设备到双极$(\pm) 2。5 \lor$ a…n…d. 3…V7)1耗材...........
12版式72 12.1版式指南72 12.2版式示例
13 器件和文档支持 74 13.1 文档支持 . 74 13.2 相关链接 13.3 接收文档更新通知 .................. 13.4 社区资源 13.5 商标 ......... 13.6 静电放电警告. 13.7 Glossary ............   
14 机械、 封装和可订购信息 75

# 4 修订历史记录

注：之前版本的页码可能与当前版本有所不同。

从修订版B（2016年10月）到修订版C的变化

最大结参数名称改为结在绝对最大额定参数表中  ..........................................修改工作条件推荐表：修改自由空气条件为环境条件语句，修改输入电压参数规格，增加$\mathsf {V} _ {\mathsf {C M}}$和$\mathsf {f} _ {\mathsf {C L K}}$符号...... ..........8改变电特性表条件语句：在温度条件中增加$\mathsf {T} _ {\mathsf {A}}$，将DVDD条件移至AVDD - AVSS条件........... ..................之后9将输入偏置电流参数测试条件从输入改为InxP和INxN。9改变漂移参数单位从ppm p p m美元  / { } ^ { \ 保监会}加元和改变内部时钟精度参数测试条件从$ - 4 0 ^{\保监会}C \ T _{一}\ le + 8 5 ^{\保监会}T美元加元_{一}= - 4 0 ^{\保监会}+ 8美元加元5 ^{\保监会}电特性表中……加元10修改参数$\mathsf {I} _ {\mathsf {A VDD}}$和IDVDD参数[从参数名称中删除（normal mode），在测试条件中添加normal mode]，并在电气特性表.........中，从功耗参数名称中删除Quiescent11将“时序要求：串行接口”表中的“自由空气”改为“环境”。12修改了“模拟输入”部分.............修改了理想输出码与输入信号表：将第一列中的所有$V _ {R E F}$改为$F S$ in，并删除了脚注1 38修改了CONFIG1寄存器位寄存器中第4位和第3位的复位设置。46修改了CONFIG2寄存器中7 ~ 5位的复位值设置：split cells apart ...........47修改CONFIG3寄存器中第6 ~ 5位的复位值设置：split cells apart ...... 48修改配置寄存器3字段说明.............中第3位的描述中AVDD AVSS为$A VDD + AVSS $48修改了引线控制寄存器字段描述表：将01位的设置从12位的3:2改为$ 24 \:nA $，将比特的描述改为1:0。49修改未使用的输入和输出部分：增加DRDY描述，删除未浮动的未使用的数字输入。61从“布局”第72部分删除“布局指南”第二部分

从修订版A（2012年8月）到修订版B页的变化

已添加 ESD 额定值表， 特性 描述部分， 器件功能模式， 应用和实施部分， 电源相关建议部分， 布局部分， 器件和文 档支持部分以及机械、封装和可订购信息部分.   
Ads129.9-4 and ADS129.9-6 have been added to the document
已添加   
已删除 低功耗 特性要点.   
已更改 颅外脑电图 (EEG) （位于应用和 说明部分. 已删除 最后一个应用要点 ................ 已更改 说明部分： 已添加有关 SRB 引脚的句子， 已更改第 二段的最后一句   
All ADS1299 series devices in the entire document have been modified to ADS1299-X. Block diagrams have been changed: Dashed boxes have been added
修改了引线检测的规格，电气特性表的频率参数。10电气特性表... ...中增加了ADS1299-4和ADS1299-6的电源电流（偏置关闭）和功耗（模拟电源$= 5 V,$偏置放大器关闭）部分的规格。11更改了噪声测量部分。16修改功能框图，显示ADS1299-4未涵盖的5-8通道和ADS1299-6未涵盖的7-8通道...... 19更改图18中的INxP和INxN引脚…20更改图23：将PgaP， PgaN更改为PgaP， PgaN 23更改输入共模范围部分：更改输入共模范围说明...... 23更改输入差分动态范围部分中的差分输入电压范围24更改图34：MUX8[2:0]在IN8N上$= 0 1 0$，在BIAS_MEAS上$= 1$…29修改引线检测部分第二段第一句。30修改了“交流断线（一次性或周期性）”章节. .........改变偏置引线部分。……。32修改了图38的标题和“Bias Drive （DC Bias Circuit）”部分的下电描述。33将图39中的“START Opcode”修改为“START”。34更改了重置$\overline {{(R E S E 7)}}$一节的清晰性35更改了标题，第一段，开始操作码和停止操作码为开始和停止（图42），停止操作码为停止命令（图43）在连续转换模式. ....一节36数据输入（DIN）章节增加最后一句话39 RDATAC：读取数据连续........章节中发送多字节命令章节增加交叉引用41将图46中的RDATAC Opcode更改为RDATAC。41将图46中的“RDATA Opcode”修改为“RDATA .........”42将RREG: Read From Register部分........图48中的SCLK速率限制，OPCODE 1和OPCODE 2的描述修改为BYTE和BYTE 2修改了MISC1: Miscellaneous register章节中寄存器描述和位5的描述59将图68中的输出名称从RA、LA和RL改为电极1、电极2和BIAS电极。
63改变了Power-Up sequence部分。70年……

#从原始（2012年7月）到修订版A的变化

已更改 器件系列和订购信息表的产品栏

5 .设备对比

表身体< html > < > < > < tr > < td >产品< / td > < td >包选择< / td > < td >操作温度范围< / td > < td >频道< / td > < td > ADC分辨率< / td > < td >最大采样率< / td > < / tr > < tr > < td > ADS1299-4 < / td > < td > tqfp - 64 < / td > < td > -40 C + 85°℃< / td > < td > 4 < / td > < td > 24 < / td > < td > 16增殖< / td > < / tr > < tr > < td > ADS1299-6 < / td > < td > tqfp - 64 < / td > < td > -40°C + 85°℃< / td > < td > 6 < / td > < td > 24 < / td > < td > 16增殖< / td > < / tr > < tr > < td > ADS1299 < / td > < td > tqfp - 64 < / td > < td > -40 C + 85°℃< / td > < td > 8 < / td > < td > 24 < / td > < td > 16过度增殖< / td > < / tr > < /表> < /身体> < / html >

# 6引脚配置和功能

![](images/c047a9638b8474afa301268b28587bf5cc476d3fa53e3d3fa74e4e419a13c831.jpg)

#引脚函数

表身体< html > < > < > < tr > < td colspan =“2”>销< / td > < td行宽=“2”>类型< / td > < td行宽=“2”>描述< / td > < / tr > < tr > < td >名称< / td > < td >。< / td > < / tr > < tr > < td > (AVDD: < / td > < td > 19日,21日,22日,56岁的59 < / td > < td >供应< / td > < td >模拟供应。将1-F电容连接到AVSS。< / td > < / tr > < tr > < td > < / td > < td > 59 < / td > < td >供应< / td > < td >泵模拟供应。将1 uf电容连接到AVSS，引脚58。负责< / td > < / tr > < tr > < td行宽=“2”> AVDD1 < / td > < td > 54 < / td > < td >供应< / td > < td >模拟供应。将1-Fcapacitor连接到AVSS1。</td></tr><tr><td> 20,23,32；57</td><td>电源</td><td>模拟地</td></tr><tr><td>电源</td><td>58</td><td>电源</td><td>53</td><td>电源</td><td>接地模拟模拟</td></ td>< tr><td>BIASIN</td><td>62</td><td>模拟输入</td></tr><tr><td> <td>BIASOUT</td><td>63</td><td>输出模拟</td><td>偏置驱动输出< / td > < / tr > < tr > < td > BIASREF < / td > < td > 60 < / td > < td >模拟输入< / td > < td >偏见驱动非反相输入< / td > < / tr > < tr > < td > c < / td > < td > 39道明> < /数字输入< td > < / td > < td >芯片选择、积极低< / td > < / tr > < tr > < td > CLK < / td > < td > 37道明> < /数字输入< td > < / td > < td >主时钟nput < / td > < / tr > < tr > < td > CLKSEL < / td > < td > 52 < / td > < td >数字输入< / td > < td >主时钟选择(1)< / td > < / tr > < tr > < td > DAISY_IN < / td > < td > 41 < / td > < td >数字输入< / td > < td >菊花链输入< / td > < / tr > < tr > < td > DGND < / td > < td > 33岁,49岁供应51 < / td > < td > < / td > < td >数字地面< / td > < / tr > < tr > < td > DIN < / td > < td > 34 < / td > < td >数字输入< / td > < td >串行数据输入< / td > < / tr > < tr > < td > DOUT < / td > < td > 43 < / td > < td >数字输出< / td > < td >串行数据输出< / td > < / tr > < tr > < td > DRDY < / td > < td > 47 < / td > < td >数字输出< / td > < td >数据准备,积极低< / td > < / tr > < tr > < td > DVDD < / td > < td > 48岁,50 < / td > < td >供应< / td > < td >数字电源。1 μ f电容接至DGND。通用输入/输出引脚</td></tr><tr><td>GPIO1</td><td>42</td><td>数字输入/输出</td><td>连接到DGND，未使用≥10-kΩ电阻。< / td > < / tr > < tr > < td > GPIO2 < / td > < td > 44道明> < /数字输入/输出< td > < / td > < td > G如果未使用。< / td > < / tr > < tr > < td > GPIO3 < / td > < td > 45道明> < /数字输入/输出< td > < / td > < td >如果未使用。Connect</td></tr><tr><td>GPIO4</td><td>46</td><td>数字输入/输出</td><td>通用输入/输出引脚4。如果不使用，用≥10-kΩ电阻接DGND。< / td > < / tr > < tr > < td > IN1N IN1P < / td > < td > 15 < / td > < td >模拟输入< / td > < td >微分模拟-输入1 (2)< / td > < / tr > < tr > < td > IN2N < / td > < td > 16 < / td > < td >模拟输入< / td > < td >微分模拟正输入1 (2)< / td > < / tr > < tr > < td > IN2P < / td > < td > 13 < / td > < td >模拟输入< / td > < td >微分模拟-输入2 (2)< / td > < / tr > < tr > < td > IN3N < / td > < td > 14 < / td > < td >模拟输入< / td > < td >微分模拟正输入2 (2)< / td > < / tr > < tr > < td > < / td > < td > 11 < / td > < td >模拟输入< / td > < td >微分模拟负输入3(2)</td></tr><tr><td>IN3P</td><td>12</td><td> <td>模拟输入3(2)</td></ td>< tr><td>IN4N</td><td>9</td><td>模拟输入4(2)</td></ td>< tr><td>IN4P</td><td> <td>模拟输入4(2)</td></tr><tr><td> <td>模拟输入5 (2)</td></tr><tr><td> IN5P</td><td></td><td>模拟输入输入</td><td>差分模拟正输入5(2)(ADS1299-6和仅ADS1299)</td></tr><tr><td>IN6N</td><td></td><td>模拟输入</td><td>差分模拟负输入6(2)(ADS1299-6和仅ADS1299)</td></tr><tr><td>IN6P</td><td></td><td>模拟输入</td></ td>< tr><td>IN7N</td><td></td><td>模拟输入</td><td>差分模拟负输入7(2)（ADS1299-6和仅ADS1299）仅限)</td></tr><tr><td>IN7P</td><td></td><td>模拟输入</td><td>模拟输入</td></ td>< tr><td>IN8N</td><td>7∞5641</td><td>模拟输入</td></ td>< tr><td>IN8P</td><td> <td>模拟输入</td></ td>< tr><td>NC</td><td>27,29</td><td></td><td>未连接，开路离开</td></tr><tr><td>保留</td><td>64</td><td>模拟输出</td><td>保留供将来使用开路离开</td></tr><tr><td>复位</td><td>36</td><td>数字输入</td><td>系统复位，有效低</td></tr><tr><td>RESV1</td><td>31</td><td>数字输入</td><td>保留供将来使用，直连DGND</td></tr><tr><td>SCLK</td><td>40</td><td>数字输入</td></tr><tr><td>SRB1</td><td>17</td><td>模拟输入/输出</td><td> <td>SRB2</td><td> <td>模拟输入/输出</td></tr></table></body></html>

（12）通过≥10-kΩ电阻将双态模式设定引脚高置为DVDD或低置为DGND。将未使用的模拟输入直接连接到AVDD。

#引脚函数（续）

表身体< html > < > < > < tr > < td colspan =“2”>销< / td > < td行宽=“2”>类型< / td > < td行宽=“2”>描述< / td > < / tr > < tr > < td >名称< / td > < td >。</td></tr><tr><td>START</td><td>38</td><td>数字输入</td></tr><tr><td>PWDN</td><td>35</td><td>数字输入</td><td>下电，有效低电平</td></tr><tr><td>VCAP1</td><td>28</td><td>模拟输出</td><td>旁路电容引脚。将100uf电容连接到AVSS。模拟量</td></tr><tr><td>VCAP2</td><td>30</td><td>模拟量输出</td><td>模拟旁路电容引脚。将1 uf电容连接到AVSS。</td></tr><tr><td>VCAP3</td><td>55</td><td>模拟输出</td><td>A连接1-uF和0.1 μ f </td></tr><tr><td>VCAP4</td><td>26</td><td>模拟输出</td><td>模拟旁路电容引脚。将1 uf电容连接到AVSS。</td></tr><tr><td>VREFN</td><td>25</td><td>模拟输入</td><td>模拟参考负电压。</td></tr><tr><td>VREFP</td><td>24</td><td>模拟输入/输出</td><td>正模拟参考电压。连接最小10uf电容至VREFN </td></tr></table></body></html>

# 7规格

# 7.1绝对最高评级(1)

<html><body><table><tr><td colspan="2"></td><td>MIN</td><td>MAX</td><td>UNIT</td></tr><tr><td rowspan="7">Voltage</td><td>AVDD to AVSS</td><td>-0.3</td><td>5.5</td><td rowspan="7"></td></tr><tr><td>DVDD DGND to</td><td>-0.3</td><td>3.9</td></tr><tr><td>AVSS to DGND</td><td>-3</td><td>0.2</td></tr><tr><td>VREFP to AVSS</td><td>-0.3</td><td>AVDD 0.3 ×</td></tr><tr><td>VREFN to AVSS</td><td>-0.3</td><td>AVDD + 0.3</td></tr><tr><td>Analog input</td><td>AVSS -0.3</td><td>AVDD + 0.3</td></tr><tr><td>Digital input</td><td>DGND - 0.3</td><td>DVDD + 0.3</td></tr><tr><td>Current</td><td>Input, continuous, any pin except power supply pins (2)</td><td>-10</td><td>10</td><td rowspan="2">mA</td></tr><tr><td rowspan="2">Temperature</td><td>Junction, TJ</td><td></td><td>150</td></tr><tr><td>Storage, Tstg</td><td>-60</td><td>150</td><td>C</td></tr></table></body></html>

(1)超出绝对最大额定值的应力可能会对设备造成永久性损坏。这些仅是压力额定值，并不意味着设备在这些或超出推荐工作条件的任何其他条件下的功能操作。长时间暴露在绝对最大额定条件下可能会影响设备的可靠性。
(2)输入引脚用二极管箝位到电源轨上。如果模拟输入电压超过AVDD $+ 0，则将输入电流限制在$ 10 ~ \mathsf {m A}$或更小。3 {\:} \ V $或小于$\mathsf {A V S S} - 0。3 \v $，或者数字输入电压超过DVDD $+ 0。3 {\:} \mathsf {V}$ or小于DGND $\bar{-} 0。3 \v $。

# 7.2 ESD评级

<html><body><table><tr><td colspan="2"></td><td>VALUE</td><td> <td>UNIT</td></tr><tr><td rowspan="2">V(ESD)</td><td ><td>人体模型（HBM）， ANSI/ESDA/JEDEC JS-001(1) per</td><td>±1000</td><td rowspan="2"></td></tr><tr><td> . CDM，根据JEDEC规范JESD22- C101 </td><td> </tr></table></body></html>

（12） JEDEC文件JEP155指出，500-V HBM允许使用标准ESD控制过程进行安全制造。JEDEC文件JEP157指出，250 v CDM可以通过标准ESD控制过程进行安全制造。

# 7.3推荐的操作条件

超出工作环境温度范围（除非另有说明）

表身体< html > < > < > < tr > < td > < / td > < td > < / td > < td > MIN < / td > < td >笔名< / td > < td > MAX < / td > < td >单位< / td > < / tr > < tr > < td colspan =“6”>电源< / td > < / tr > < tr > < td >模拟电源< / td > < td > AVDD, AVSS < / td > < td > 4.75 < / td > < td > 5 < / td > < td > < / td > 5.25 V < td > < / td > < / tr > < tr > < td >数字电源< / td > < td > DVDD, DGND < / td > < td > 1.8 < / td > < td > 1.8 < / td > < td > < / td > 3.6 V < td > < / td > < / tr > < tr > < td >模拟到数字供应< / td > < td > AVDD --2.1 DVDD < / td > < td > < / td > < td > < / td > < td > 3.6 < / td > < td > < / td > < / tr > < tr > < td colspan =“6”>模拟输入< / td > < / tr > < tr > < td >全差分输入电压< / td > < td > VINxP−VINxN < / td > < td > + VREF < / td > < td >获得< / td > < td > < / td > < td > < / td > < / tr > < tr > < td >的共模输入范围VCM < / td > < td > (VIN×P + VINxN) / 2 < / td > < td >分段的< / td > < td >看到的共模输入范围PGA设置和输入范围部分< / td > < td > < / td > < td > < / td > < / tr > < tr > < td colspan =“6”>参考电压输入< / td > < / tr > < tr > < td > VREF参考输入电压< / td > < td > = (VVREFP VVREFN) VREF < / td > < td > < / td > < td > 4.5 < / td > < td > < / td > < td > V < / td > < / tr > < tr > < td colspan =“2”> VREFN负输入< / td > < td > < / td > < td > AVSS < / td > < td > < / td > < td > V < / td > < / tr > < tr > < td colspan =“2”> VREFP积极输入< / td > < td > < / td > < td > AVSS + 4.5 < / td > < td > < / td > < td > V < / td > < / tr > < tr > < td colspan =“6”>时钟输入< / td > < / tr > < tr > < td >外部时钟输入频率fcLK < / td > < td > CLKSEL销= 0 < / td > < td > 1.5 < / td > < td > 2.048 < / td > < td > 2.25 < / td > < td > MHz < / td > < / tr > < tr > < td colspan =“6”>数字输入< / td > < / tr > < tr > < td输入电压colspan = " 2 " > < / td > < td > DGND - 0.1 < / td > < td > < / td > < td > DVDD + 0.1 < / td > < td > < / td > < / tr > < tr > < td colspan =“6”>温度范围< / td > < / tr > < tr > < td colspan =“2”>助教工作温度范围< / td > < td > < / td > < td > -40 < / td > < td > 85 < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 7.4热信息

<html><body><table><tr><td rowspan="3">THERMAL METRIC(1)</td><td>ADS1299-4，ADS1299-6, < / td > < td行宽=“3”> ADS1299单位< / td > < / tr > < tr > < td > PAG (TQFP) < / td > < / tr > < tr > < td > 64针< / td > < / tr > < tr > < td > Junction-to-ambient热RθJA < / td > < td >电阻< / td > < td > 46.2 C / W < / td > < / tr > < tr > < td > Junction-to-case(上)热RθJC(上)< / td > < td >阻力5.8 < / td > < td >°C / W < / td > < / tr > < tr > < td > Junction-to-board热ROJB < / td > < td > 19.6 < / td > < td > C / W < / td > < / tr > < tr > < td >阻力YJT Junction-to-top表征参数< / td > < td > 0.2 < / td > < td > C / W < / td > < / tr > < tr > < td > YJB Junction-to-board19.2特征参数< / td > < td > < / td > < td >°C / W < / td > < / tr > < tr > < td > RθJC (bot) Junction-to-case(底部)热< / td > < td > n / a < / td > < td >°C / W < / td > < / tr > < /表> < /身体> < / html >

(1)有关传统和新型热指标的更多信息，请参阅半导体和IC封装热指标应用报告。

# 7.5电气特性

最小和最大规格适用于$\mathsf {T} _ {\mathsf {A}} = - 4 0 ^ {\circ} \mathsf {C}$到$ 85 ^ {\circ} \mathsf {C}$。典型的规格是$\mathsf {T} _ {\mathsf {A}} = + 2 5 ^ {\circ} \mathsf {C}$。所有规格均为$\mathsf {A VDD} - \mathsf {A V S S} = 5 \vee$, DVDD $) = 3。3 ~ \lor$, $\mathsf {V} _ {\mathsf {R E F}} = 4。5 \: \mathsf {V}$，外部$\mathsf {f} _ {\mathsf {C L K}} = 2。0 4 8 ~ \mathsf {M H z}$，数据速率$= 2 5 0$ SPS，增益$= 1 2$（除非另有说明）

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> PARAMETER & lt; /戴利的小于戴利> >一直CONDITIONS & lt; /小敏戴利的小于戴利> > & lt; /戴利的小于戴利> > TYP & lt; /戴利的小于戴利> >马克斯& lt; /戴利的小于戴利> > minutes & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>拟物INPUTS & lt; /戴利的小于戴利colspan =“3”> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利rowspan =“2”>输入capacitance & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 20 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > pF & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>我们= + 25°℃,InxP, and INxN = 2.5 V & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >±300 & lt; /戴利的小于戴利> >帕拉& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利rowspan =“2”>输入新世界current & lt; /戴利的小于戴利> > AV°C + 85°C大一点,必须去洗手间InxP, and INxN = 2.5 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >±300 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利rowspan =“2”> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>饱lead-off & lt; /戴利的小于戴利> > 1000 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr >小于戴利rowspan =“2”的小于tr > >华盛顿工作投入impedance PGA PERFORMANCE & lt; /戴利的小于戴利> > 电流 source lead-off detection = nA) (LEADOFF 6 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 500 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > MΩ& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Gain settings & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 1、2、4、6、8、12、24 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> BW Bandwidth & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >的猫桌子写成5 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADC PERFORMANCE & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Resolution & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> diana Data rate & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 24 250 & lt; /戴利的小于戴利> > 16000 & lt; /戴利的小于戴利> >比特位SPS & lt; /戴利> & lt; / tr >小于戴利colspan =“6”的小于tr > > fcLK = 2.048两兆的外面CHANNEL PERFORMANCE & lt; /戴利> & lt; / tr >小于戴利rowspan =“4”的小于tr > > 输入-referred noise INL Integralnonlinearity & lt; /戴利的小于在2.5 points,戴利> > = + 25°℃& lt; /戴利的小于戴利> > 10秒of data, gain = 24 (1) & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利tr > >(单位赫兹大70赫兹)& lt; /戴利的小于戴利> >史之后of data, gain = 24, & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 1.35 & lt; /戴利的小于戴利> > VPP & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> 250 points,我们=烷基40°C + 85°大℃& lt; /戴利的小于戴利> >一二千年史of data,gain = 24, & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >有160 & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利tr > >“老怎么& lt; /戴利的小于戴利> > sample rates, and gain settings & lt; /戴利的小于戴利> >的猫Noise Measurements & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>胶版印刷error code & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > Full-scale access gain, 12百或我们& lt; /戴利的小于戴利> > 8 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >每& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>胶版印刷socket error drift & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 60 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >μV & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Gain error code & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; / 80戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >话务员/ C & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > Excluding 电压 reference error code & lt; /戴利的小于戴利> > 0.1 & lt; /戴利的小于戴利> >±0.5。& lt; /戴利的小于戴利> > % of佛& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Gain drift & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > Excluding 电压 reference drift & lt; /戴利的小于戴利> > 3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >每/°℃& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Gain痕迹射击位置& lt; /戴利的小于戴利> > channels & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 0.2秒& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > % of佛& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利交羚colspan =“6”> CHANNEL PERFORMANCE & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> CMRR Common-mode & lt; /戴利的小于戴利> > rejection转换率fcm & lt; /戴利的小于戴利> > 50赫兹,and 60赫兹(2)& lt; /戴利的小于戴利> > -110 -120 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> PSRR 电源-供电 & lt; /戴利的小于戴利> > rejection转换率& lt; /帧戴利的小于戴利> > = 50赫兹,and 60赫兹& lt; / 96戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Crosstalk & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >,and 60赫兹fiN= 50Hz & lt; /戴利的小于戴利> > -110 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> SNR & lt; /戴利的小于戴利> > Signal-to-noise转换率& lt; /戴利的小于戴利> >借端=−两dBFs,费= 10-Hz输入,gain = 12 & lt; /戴利的小于第121戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> THD《harmonic distortion & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >借端=−出半dBFs,fN = 10赫兹& lt; /戴利的小于戴利> > -99 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr >小于戴利colspan =“6”的小于tr > > PATIENT新世界上面& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Integrated noise & lt; /戴利的小于戴利> > BW & lt; /戴利的小于戴利> > = 150赫兹& lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > VRMS & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Gain bandwidth product & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 50-kΩ| l 10-pF load, Gain = 1 & lt; /戴利的小于戴利> > 100 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >千位& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Slew rate & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 50-kΩI 10-pF load,gain = & lt; /戴利的小于戴利> > 0.07 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > V / s & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> THD《harmonic distortion & lt; /戴利的小于戴利> >费= 10赫兹,& lt; /戴利的小于戴利> > gain = 1 & lt; /戴利的小于戴利> > -80 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > dB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Common-mode & lt; /戴利的小于戴利> >投入range & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 0 & lt; /戴利的小于戴利> > 003 AVSS + AVDD大于0.3 & lt; /戴利的小于戴利> > V & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Short-circuit 电流 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >每年有110 & lt; /戴利的小于戴利> > & lt; /鬼戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> Quiescent缩写& lt; /戴利的小于戴利> > consumption & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 20 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > A & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

(1)间隔10秒测量的噪声数据。未在生产环境中执行测试。输入参考噪声计算与输入短（无电极电阻）超过10秒的间隔。(2)用共模信号$\mathsf {a V S S} + 0测量CMRR。3 \: \mathsf {V}$到AVDD 0.3 V。所示值是8个通道中的最小值。

#电气特性（续）

最小和最大规格适用于$\mathsf {T} _ {\mathsf {A}} = - 4 0 ^ {\circ} \mathsf {C}$到$ 85 ^ {\circ} \mathsf {C}$。典型的规格是$\mathsf {T} _ {\mathsf {A}} = + 2 5 ^ {\circ} \mathsf {C}$。所有规格均为$\mathsf {A VDD - A V S S} = 5 \vee$, DVDD $\mathsf {\Omega} ^ {\prime} = 3。3 \ mathm {\Omega} \vee$, $\mathsf {V} _ {\mathsf {R E F}} = 4。5 \: \mathsf {V}$，外部$f _ {\scriptscriptstyle {\ mathm {C L K}}} = 2。0 4 8 ~ \mathsf {M H} z$，数据速率$= 2 5 0$ SPS，增益$= 1 2$（除非另有说明）

<html><body><table><tr><td colspan="2">oteu</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td colspan="2">PARAMETER</td><td>TEST CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNIT</td></tr><tr><td colspan="2">LEAD-OFF DETECT</td><td colspan="4"></td></tr><tr><td rowspan="3">Frequency</td><td colspan="2">Continuous</td><td>see</td><td>At dc, fDR/ 4, Register Maps for settings</td><td></td><td rowspan="3">Hz</td></tr><tr><td colspan="2">One time or periodic</td><td></td><td>7.8, 31.2</td></tr><tr><td colspan="2">ILEAD_ OFF[1:0] = 00</td><td></td><td>6</td><td>nA</td></tr><tr><td rowspan="4" colspan="2">Current</td><td>ILEAD OFF[1:0] = 01</td><td></td><td>24</td><td></td><td rowspan="3"></td></tr><tr><td>ILEAD OFF[1:0] = 10</td><td></td><td>6</td><td>A</td></tr><tr><td>ILEAD_OFF[1:0] =</td><td>11</td><td>24</td><td></td></tr><tr><td></td><td></td><td></td><td></td></tr><tr><td colspan="2">Current accuracy</td><td></td><td>±30</td><td>±20%</td><td></td><td></td></tr><tr><td colspan="2">Comparator threshold accuracy</td><td colspan="3"></td><td></td><td>mV</td></tr><tr><td colspan="2">EXTERNAL REFERENCE</td><td colspan="3"></td><td></td><td>kΩ</td></tr><tr><td colspan="2">Input impedance</td><td colspan="5">5.6</td></tr><tr><td colspan="2">INTERNAL REFERENCE</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td colspan="2">VREF Internal reference voltage</td><td></td><td></td><td>4.5</td><td></td><td></td></tr><tr><td colspan="2">accuracy Drift VREF</td><td>TA = −40°C to</td><td>+85°℃</td><td>±0.2% 35</td><td></td><td>ppm/°C</td></tr><tr><td colspan="2">Start-up time</td><td colspan="2"></td><td>150</td><td></td><td>ms</td></tr><tr><td colspan="2">SYSTEM MONITORS Analog supply</td><td colspan="4"></td><td></td></tr><tr><td rowspan="2">Reading error</td><td>Digital supply</td><td></td><td></td><td>2%</td><td></td><td rowspan="2"></td></tr><tr><td colspan="2"></td><td></td><td>2%</td><td></td></tr><tr><td>Device wake up</td><td></td><td>From STANDBY</td><td>power-up to DRDY low</td><td>150</td><td></td><td>ms</td></tr><tr><td>Temperature</td><td>Voltage</td><td>TA = +25°℃</td><td>mode</td><td>31.25 145</td><td></td><td>mV</td></tr><tr><td>sensor reading</td><td>Coefficient</td><td></td><td></td><td>490</td><td></td><td></td></tr><tr><td rowspan="3">Test signal</td><td colspan="2" rowspan="3">Signal frequency</td><td></td><td></td><td></td><td>V/C</td></tr><tr><td>See Register</td><td>Maps section for settings fcLK / 221,</td><td>fcLK /220</td><td>Hz</td></tr><tr><td>Signal voltage</td><td>See Register Maps section for settings</td><td>±1, ±2 ±2%</td><td>mV</td></tr><tr><td colspan="8">CLOCK Accuracy</td></tr><tr><td colspan="3" rowspan="2">Internal oscillator clock frequency</td><td>Nominal frequency</td><td colspan="2">2.048</td><td></td><td rowspan="2">MHz</td></tr><tr><td>TA = +25°℃</td><td></td><td>±0.5%</td></tr><tr><td colspan="3">Internal clock accuracy</td><td>= —40C to +85°℃ TA</td><td></td><td>±2.5%</td><td></td></tr><tr><td colspan="3">Internal oscillator start-up time</td><td></td><td>20</td><td></td><td>μS</td></tr><tr><td colspan="3">Internal oscillator power consumption</td><td></td><td>120</td><td></td><td>μW</td></tr><tr><td colspan="3">DIGITAL INPUT/OUTPUT (DVDD = 1.8 V to 3.6 V)</td><td></td><td></td><td></td><td></td></tr><tr><td colspan="2">VH High-level input voltage</td><td></td><td></td><td>0.8 DVDD</td><td>DVDD + 0.1</td><td>V</td></tr><tr><td colspan="3"></td><td></td><td>-0.1</td><td>0.2 DVDD</td><td></td></tr><tr><td colspan="3">VL Low-level input voltage</td><td>TOH = -500 A</td><td></td><td></td><td>V</td></tr><tr><td colspan="3">High-level output voltage VOH</td><td></td><td>0.9 DVDD</td><td></td><td>v</td></tr><tr><td colspan="3">Low-level output voltage VOL</td><td>IOL = +500 μA</td><td></td><td>0.1 DVDD</td><td>V</td></tr><tr><td colspan="3">Input current</td><td>DVDD 0V<VDigitalinput</td><td>-10</td><td>10</td><td>μA</td></tr></table></body></html>

#电气特性（续）

最小和最大规格适用于$\mathsf {T} _ {\mathsf {A}} = - 4 0 ^ {\circ} \mathsf {C}$到$ 85 ^ {\circ} \mathsf {C}$。典型的规格是$\mathsf {T} _ {\mathsf {A}} = + 2 5 ^ {\circ} \mathsf {C}$。所有规格均为$\mathbf {A VDD - A V S S} = 5 \vee$ V, DVDD $\mathbf {\zeta} = 3。3 ~ \mathsf {V}$ V, $\mathsf {V} _ {\mathsf {R E F}} = 4。5 \: \mathsf {V}$ external $f _ {\scriptscriptstyle {\ mathm {C L K}}} = 2。0 4 8 ~ \mathsf {M H} z$，数据速率$= 2 5 0$ SPS，增益$= 1 2$（除非另有说明）

<html><body><table><tr><td colspan="2">PARAMETER</td><td>TEST CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNIT</td></tr><tr><td colspan="6">SUPPLY CURRENT (Bias Turned Off)</td><td></td></tr><tr><td rowspan="2">AVDD AVDD current</td><td>ADS1299-4</td><td rowspan="2">Normal mode, AVDD AVSS = 5 V</td><td></td><td>4.06</td><td></td><td rowspan="4">mA</td></tr><tr><td>ADS1299-6</td><td></td><td>5.57</td><td></td></tr><tr><td rowspan="6"></td><td>ADS1299</td><td rowspan="2"></td><td></td><td>7.14</td><td></td></tr><tr><td>ADS1299-4</td><td>0.54</td><td></td><td></td></tr><tr><td rowspan="5">ADS1299-6 DVDD current</td><td>Normal mode, DVDD</td><td></td><td>0.66</td><td></td><td rowspan="5">mA</td></tr><tr><td>ADS1299</td><td></td><td></td><td></td></tr><tr><td>ADS1299-4</td><td></td><td>0.27</td><td></td></tr><tr><td>ADS1299-6</td><td></td><td>0.34</td><td></td></tr><tr><td>Normal mode, DVDD ADS1299</td><td></td><td>0.5</td><td></td></tr><tr><td colspan="7">POWER DISSIPATION (Analog Supply = 5 V, Bias Amplifiers Turned Off)</td></tr><tr><td rowspan="9">Power dissipation</td><td rowspan="2">Normal ADS1299-4</td><td colspan="4">mode</td><td></td></tr><tr><td>Power-down</td><td></td><td>22</td><td>24</td><td>mW μW</td></tr><tr><td rowspan="2"></td><td>Standby mode, internal reference</td><td></td><td>10 5.1</td><td></td><td>mW</td></tr><tr><td></td><td>Normal mode</td><td>30</td><td>33</td><td>mW</td></tr><tr><td rowspan="2">ADS1299-6</td><td>Power-down</td><td></td><td>10</td><td></td><td>μW</td></tr><tr><td></td><td>Standby mode, internal reference</td><td>5.1</td><td></td><td>mW</td></tr><tr><td rowspan="3"></td><td>Normal mode</td><td></td><td>39</td><td>42</td><td>mW</td></tr><tr><td>ADS1299</td><td>Power-down</td><td>10</td><td></td><td>μW</td></tr><tr><td></td><td>Standby mode, internal reference</td><td>5.1</td><td></td><td>mW</td></tr></table></body></html>

时序要求：串行接口

超出工作环境温度范围（除非另有说明）

<html><body><table><tr><td rowspan="2"></td><td colspan="2">2.7 V≤ DVDD ≤ 3.6 V</td><td colspan="2">1.8 V ≤ DVDD ≤ 2.0 V</td><td rowspan="2">UNIT</td></tr><tr><td>MIN</td><td>MAX</td><td>MIN</td><td>MAX</td></tr><tr><td>tcLK Master</td><td>clock period</td><td>414</td><td>666</td><td>414</td><td>666</td><td>ns</td></tr><tr><td>tcssc</td><td>Delay time, CS low to first SCLK</td><td>6</td><td></td><td>17</td><td></td><td>ns</td></tr><tr><td>tsCLK</td><td>SCLK period</td><td>50</td><td></td><td>66.6</td><td></td><td>ns</td></tr><tr><td>tsPWH,</td><td>Pulse duration, SCLK pulse duration, high or low</td><td>15</td><td></td><td>25</td><td></td><td>ns</td></tr><tr><td>tDIST</td><td>Setup time, DIN valid to SCLK falling edge</td><td>10</td><td></td><td>10</td><td></td><td>ns</td></tr><tr><td>tDIHD</td><td>Hold time, valid DIN after SCLK falling edge</td><td>10244</td><td></td><td></td><td></td><td>ns</td></tr><tr><td>tcSH</td><td>Pulse duration, CS high</td><td></td><td></td><td></td><td></td><td>tcLK</td></tr><tr><td>tsccs</td><td>Delay time, final SCLK falling edge to CS high</td><td></td><td></td><td>1124</td><td></td><td>tCLK</td></tr><tr><td>tSDECODE</td><td>Command decode time</td><td></td><td></td><td>4</td><td></td><td>tCLK</td></tr><tr><td>tDISCK2ST</td><td>Setup time, DAISY_ IN valid to SCLK rising edge</td><td>10</td><td></td><td>10</td><td></td><td>ns</td></tr><tr><td>tDISCK2HT</td><td>Hold time, DAISY IN valid after SCLK rising edge</td><td>10</td><td></td><td>10</td><td></td><td>ns</td></tr></table></body></html>

7.7开关特性：串行接口

超出工作环境温度范围（除非另有说明）

表身体< html > < > < > < tr > < td行宽=“2”>参数< / td > < td行宽= " 2 " > < / td > < td colspan =“2”> 2.7 V≤DVDD≤3.6 V < / td > < td colspan =“2”> 1.8 V≤DVDD≤2.0 V < / td > < td行宽=“2”>单位< / td > < / tr > < tr > < td > MIN < / td > < td > MAX < / td > < td > MIN < / td > < td > MAX < / td > < / tr > < tr > < td > tDOHD < / td > < td >保存时间,SCLK下降沿,无效DOUT < / td > < td > 10 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td > tDOPD < / td > < td >传播延迟时间,SCLK前沿DOUT有效的< / td > < td > < / td > < td > < / td > < td > < / td > < td > 32 < / td > < td > ns < / td > < / tr > < tr > < td > tcSDOD < / td > < td >传播延迟时间,CS低DOUT驱动< / td > < td > 10 < / td > < td > < / td > < td > 20 < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td > tcSDOz < / td > < td >传播延迟时间,CS高DOUT Hi-Z < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > 20 < / td > < td > ns < / td > < / tr > < /表> < /身体> < / html >

tCLK CLK uhrumuuuruuuuuuur tCSSC CSH SCLK CS SCLK 3 8 tSPWH 1 2tSPWL 3 8 tS tSDECODE DIHD T tDIST tDOPD DIN tCSDOD tCSDOZ Hi-Z Hi-Z DOUT

注：SPI设置为${\mathsf {C P O L}} = 0$和${\mathsf {C P H A}} = 1$。

![](images/651c17a2089a83b93361c44655c6e92289b3ff6d93c6e7f17f54673b8363f903.jpg)  
图1所示。串行接口时序
图2。菊花链接口定时

![](images/b8141f752fe34a57120ead46534f1c2248b9d9837d966f8645225315fee6715f.jpg)  
图7。泄漏电流与温度
图8。PSRR vs频率

#典型特征（续）

${\sf T} _ {\sf A} = 2 5 ^ {\circ} {\sf C}$, $\mathsf {A V D D} = 5 \mathsf {V}$, $\mathsf {A V S S} = 0 \vee$, $\mathsf {D V D D} = 3。3 \ mathm {~ V ~}$，内部VREFP $= 45 \vee$, VREFN $\mathbf {\sigma} = \mathbf {\sigma}$ AVSS，外部时钟$= 2。0 4 8$ MHz，数据速率$= 25 0$ SPS，增益$= 1 2$（除非另有说明）

![](images/b72f351f0ac572c87a9b654540668cc1c3d7dd9326d9d5f44feffe94941544bc.jpg)

![](images/6bdfa329e704dd08fd964bf32c3bc43db4e780e30eac45a8cff109cb46ca2871.jpg)

![](images/9d3f41fdb5e9e5ec33ff8a7ad72c0dda40208448e3400a9e2d3fece01bea3505.jpg)  
图9。THD vs .频率

![](images/c923fcaa3040fa643129e89579d9b7b087f41abeb4b6f1d607485dc57dbc78a3.jpg)  
图10。INL vs PGA增益

![](images/6d0534d7684c58c614b05532737a205dd906d8699c4aa07c4fa74a34abb935b3.jpg)  
图11。INL与温度

![](images/581f826f072eb5dd7319aeff97a7bd71c3d95fa32cd46641a8d7e853a470c858.jpg)  
图12。THD FFT图（60hz信号）
图13。FFT图（60hz信号）
图14。偏置vs PGA增益（绝对值）

#典型特征（续）

${ \sf T } _ { \sf A } = 2 5 ^ { \circ } { \sf C }$, $\mathsf { A V D D } = 5 \mathsf { V }$, $\mathsf { A V S S } = 0 \vee$, $\mathsf { D V D D } = 3 . 3 \mathrm { ~ V ~ }$，内部VREFP $\mathtt { \Omega } = 4 . 5 ~ \mathtt { V }$, VREFN $\mathbf { \sigma } = \mathbf { \sigma }$ AVSS，外部时钟$= 2 . 0 4 8$ MHz，数据速率$= 2 5 0$ SPS，增益$= 1 2$（除非另有说明）

![](images/b3706430e88477abbc1dfadbb808e96784c24a2f4bc7cfe52320711ce5c0cd20.jpg)  
图17。导通电流源精度分布

参数化测量信息

# 8.1噪音测量

# NOTE

除特殊说明外，ADS1299- $x$为ADS1299-4、ADS1299-6和ADS1299的所有规格和功能描述。

通过调整数据速率和PGA设置，优化ADS1299- $\cdot \mathsf {x}$噪声性能。降低数据速率以提高平均，噪声相应降低。增加PGA值以降低输入噪声。这种较低的噪声水平在测量低水平生物电位信号时特别有用。表4总结了在5v模拟电源下ADS1299-x的噪声性能。这些数据代表了$T _ {\mathsf {A}} = + 2 5 ^ {\circ} \mathsf {C}$的典型噪声性能。所显示的数据是对多个设备的读数取平均值的结果，并且是在输入短路的情况下测量的。至少1000个连续读数用于计算每个读数的均方根值和峰对峰噪声。对于较低的数据速率，该比率约为6.6。

表1显示了使用内部参考进行的测量。当使用低噪声外部基准（如REF5045）时，这些数据也代表了ADS1299- $\cdot \mathsf {x}$噪声性能。

表1、表2、表3和表4列出了所示条件下的输入引用噪声，单位为$\mu \mathsf {V} _ {\mathsf {R M S}}$和$\mu \mathsf {V} _ {\mathsf {P P}}$。对应数据以有效位数（ENOB）为单位，其中有效值噪声的ENOB定义如式1所示：

$$
E N O B = \log ＿ {2} （\frac V R E F{}{\sqrt 2{}\times\tan\times V ＿ {\mathrm R M S{）}}}
$$

用同样的方法计算峰对峰噪声的无噪声位。

表1、表2、表3、表4中的动态范围数据由式2计算：

$$
{ \mathsf { D y n a m i c \ R a n g e } } = 2 0 \times \mathsf { l o g } \left( { \frac { \mathsf { V R E F } } { { \sqrt { 2 } } \times { \mathsf { G a i n } } \times { \mathsf { V } } _ { \mathsf { R M S } } } } \right)
$$

表1。输入参考噪声$( \mu \nu _ { \mathsf { R M S } } , \mu \nu _ { \mathsf { P P } } )$ （cid:）正常模式下5v模拟电源和4.5 v参考电源(1)

<html><body><table><tr><td rowspan="2">DR BITS OF CONFIG1</td><td rowspan="2">OUTPUT DATA RATE (SPS)</td><td rowspan="2">-3-dB BANDWIDTH (Hz)</td><td colspan="4">PGA GAIN =</td><td colspan="6">PGA GAIN = 2</td></tr><tr><td>VRMS</td><td>μVpP</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td><td>VRMS</td><td>μVpP</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td></tr><tr><td>000</td><td>16000</td><td>4193</td><td>21.70</td><td>151.89</td><td>103.3</td><td>15.85</td><td>17.16</td><td>10.85</td><td>75.94</td><td>103.3</td><td>15.85</td><td>17.16</td></tr><tr><td>001</td><td>8000</td><td>2096</td><td>6.93</td><td>48.53</td><td>113.2</td><td>17.50</td><td>18.81</td><td>3.65</td><td>25.52</td><td>112.8</td><td>17.43</td><td>18.74</td></tr><tr><td>010</td><td>4000</td><td>1048</td><td>4.33</td><td>30.34</td><td>117.3</td><td>18.18</td><td>19.49</td><td>2.28</td><td>15.95</td><td>116.9</td><td>18.11</td><td>19.41</td></tr><tr><td>011</td><td>2000</td><td>524</td><td>3.06</td><td>21.45</td><td>120.3</td><td>18.68</td><td>19.99</td><td>1.61</td><td>11.29</td><td>119.9</td><td>18.60</td><td>19.91</td></tr><tr><td>100</td><td>1000</td><td>262</td><td>2.17</td><td>15.17</td><td>123.3</td><td>19.18</td><td>20.49</td><td>1.14</td><td>7.98</td><td>122.9</td><td>19.10</td><td>20.41</td></tr><tr><td>101</td><td>500</td><td>131</td><td>1.53</td><td>10.73</td><td>126.3</td><td>19.68</td><td>20.99</td><td>0.81</td><td>5.65</td><td>125.9</td><td>19.60</td><td>20.91</td></tr><tr><td>110</td><td>250</td><td>65</td><td>1.08</td><td>7.59</td><td>129.3</td><td>20.18</td><td>21.48</td><td>0.57</td><td>3.99</td><td>128.9</td><td>20.10</td><td>21.41</td></tr><tr><td>111</td><td>n/a</td><td>n/a</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr></table></body></html>

(1)本表使用至少1000个连续读数来计算均方根值和峰间噪声值。

Table 2. Input-Referred pNpoliysea $( \mu \nu _ { \mathsf { R M S } } , \mu \nu _ { \mathsf { P P } } )$ rien cNeo(r1)mal Mode 5-V Analog Su nd 4.5-V   

<html><body><table><tr><td rowspan="2">DR BITS OF CONFIG1</td><td rowspan="2">OUTPUT DATA RATE (SPS)</td><td rowspan="2">-3-dB BANDWIDTH (Hz)</td><td colspan="5">PGA GAIN = 4</td><td colspan="5">PGA GAIN = 6</td></tr><tr><td>μVRMS</td><td>VpP</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td><td>VRMS</td><td>VpP</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td></tr><tr><td>000</td><td>16000</td><td>4193</td><td>5.60</td><td>39.23</td><td>103.0</td><td>15.81</td><td>17.12</td><td>3.87</td><td>27.10</td><td>102.7</td><td>15.76</td><td>17.06</td></tr><tr><td>001</td><td>8000</td><td>2096</td><td>1.98</td><td>13.87</td><td>112.1</td><td>17.31</td><td>18.62</td><td>1.31</td><td>9.19</td><td>112.1</td><td>17.32</td><td>18.62</td></tr><tr><td>010</td><td>4000</td><td>1048</td><td>1.24</td><td>8.66</td><td>116.1</td><td>17.99</td><td>19.29</td><td>0.93</td><td>6.50</td><td>115.1</td><td>17.82</td><td>19.12</td></tr><tr><td>011</td><td>2000</td><td>524</td><td>0.88</td><td>6.13</td><td>119.2</td><td>18.49</td><td>19.79</td><td>0.66</td><td>4.60</td><td>118.1</td><td>18.32</td><td>19.62</td></tr><tr><td>100</td><td>1000</td><td>262</td><td>0.62</td><td>4.34</td><td>122.2</td><td>18.99</td><td>20.29</td><td>0.46</td><td>3.25</td><td>121.1</td><td>18.81</td><td>20.12</td></tr><tr><td>101</td><td>500</td><td>131</td><td>0.44</td><td>3.07</td><td>125.2</td><td>19.49</td><td>20.79</td><td>0.33</td><td>2.30</td><td>124.1</td><td>19.31</td><td>20.62</td></tr><tr><td>110</td><td>250</td><td>65</td><td>0.31</td><td>2.16</td><td>128.2</td><td>19.99</td><td>21.30</td><td>0.23</td><td>1.62</td><td>127.2</td><td>19.82</td><td>21.13</td></tr><tr><td>111</td><td>n/a</td><td>n/a</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr></table></body></html>

(1)本表使用至少1000个连续读数来计算均方根值和峰间噪声值。

表3。输入参考噪声$( \mu \nu _ { \mathsf { P M S } } , \mu \nu _ { \mathsf { P P } } )$ （cid:）正常模式下5v模拟电源和4.5 v参考电源(1)

<html><body><table><tr><td rowspan="2">DR BITS OF CONFIG1</td><td rowspan="2">OUTPUT DATA RATE (SPS)</td><td rowspan="2">-3-dB BANDWIDTH (Hz)</td><td colspan="5">PGA GAIN = 8</td><td colspan="5">PGA GAIN = 12</td></tr><tr><td>μVRMS</td><td>μVpp</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td><td>μVRMS</td><td>μVpp</td><td>DYNAMIC RANGE (dB)</td><td>NOISE- FREE BITS</td><td>ENOB</td></tr><tr><td>000</td><td>16000</td><td>4193</td><td>3.05</td><td>21.32</td><td>102.3</td><td>15.69</td><td>16.99</td><td>2.27</td><td>15.89</td><td>101.3</td><td>15.53</td><td>16.83</td></tr><tr><td>001</td><td>8000</td><td>2096</td><td>1.11</td><td>7.80</td><td>111.0</td><td>17.14</td><td>18.45</td><td>0.92</td><td>6.41</td><td>109.2</td><td>16.84</td><td>18.14</td></tr><tr><td>010</td><td>4000</td><td>1048</td><td>0.79</td><td>5.52</td><td>114.0</td><td>17.64</td><td>18.95</td><td>0.65</td><td>4.53</td><td>112.2</td><td>17.34</td><td>18.64</td></tr><tr><td>011</td><td>2000</td><td>524</td><td>0.56</td><td>3.90</td><td>117.1</td><td>18.14</td><td>19.44</td><td>0.46</td><td>3.20</td><td>115.2</td><td>17.84</td><td>19.14</td></tr><tr><td>100</td><td>1000</td><td>262</td><td>0.39</td><td>2.76</td><td>120.1</td><td>18.64</td><td>19.94</td><td>0.32</td><td>2.26</td><td>118.3</td><td>18.34</td><td>19.65</td></tr><tr><td>101</td><td>500</td><td>131</td><td>0.28</td><td>1.95</td><td>123.1</td><td>19.14</td><td>20.44</td><td>0.23</td><td>1.61</td><td>121.2</td><td>18.83</td><td>20.14</td></tr><tr><td>110</td><td>250</td><td>65</td><td>0.20</td><td>1.38</td><td>126.1</td><td>19.64</td><td>20.95</td><td>0.16</td><td>1.13</td><td>124.3</td><td>19.34</td><td>20.65</td></tr><tr><td>111</td><td>n/a</td><td>n/a</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr></table></body></html>

(1)本表使用至少1000个连续读数来计算均方根值和峰间噪声值。

表4。输入参考噪声$( \mu \nu _ { \mathsf { P M S } } , \mu \nu _ { \mathsf { P P } } )$ （id:）在正常模式5-V模拟电源和$4 . 5 \cdot V$参考(1)

<html><body><table><tr><td rowspan="2">DR BITS OF CONFIG1 REGISTER</td><td rowspan="2">OUTPUT DATA RATE (SPS)</td><td rowspan="2">-3-dB BANDWIDTH (Hz)</td><td colspan="5">PGA GAIN = 24</td></tr><tr><td>μVRMS</td><td>μVpP</td><td>DYNAMIC RANGE (dB)</td><td>NOISE-FREE BITS</td><td>ENOB</td></tr><tr><td>000</td><td>16000</td><td>4193</td><td>1.66</td><td>11.64</td><td>98.0</td><td>14.98</td><td>16.28</td></tr><tr><td>001</td><td>8000</td><td>2096</td><td>0.80</td><td>5.57</td><td>104.4</td><td>16.04</td><td>17.35</td></tr><tr><td>010</td><td>4000</td><td>1048</td><td>0.56</td><td>3.94</td><td>107.4</td><td>16.54</td><td>17.84</td></tr><tr><td>011</td><td>2000</td><td>524</td><td>0.40</td><td>2.79</td><td>110.4</td><td>17.04</td><td>18.35</td></tr><tr><td>100</td><td>1000</td><td>262</td><td>0.28</td><td>1.97</td><td>113.5</td><td>17.54</td><td>18.85</td></tr><tr><td>101</td><td>500</td><td>131</td><td>0.20</td><td>1.39</td><td>116.5</td><td>18.04</td><td>19.35</td></tr><tr><td>110</td><td>250</td><td>65</td><td>0.14</td><td>0.98</td><td>119.5</td><td>18.54</td><td>19.85</td></tr><tr><td>111</td><td>n/a</td><td>n/a</td><td></td><td></td><td></td><td></td><td></td></tr></table></body></html>

(1)本表使用至少1000个连续读数来计算均方根值和峰间噪声值。

# 9详细描述

# 9.1概述

ADS1299-x是一款低噪声、低功耗、多通道、同时采样、24位、delta-sigma $( \Delta \Sigma )$模数转换器（ADC），带有集成可编程增益放大器（PGA）。这些设备集成了各种脑电图特定功能，使该系列非常适合可扩展的心电图（ECG），脑电图（EEG）应用。通过关闭ECG或eeg专用电路，这些器件还可用于高性能、多通道数据采集系统。

该器件具有高度可编程的多路复用器，允许温度，电源，输入短和偏置测量。此外，多路复用器允许任何输入电极被编程为患者参考驱动器。PGA增益可以从七个设置（1,2,4,6,8,12和24）中选择一个。器件中的adc提供250 SPS到16 kSPS的数据速率。与设备的通信是使用spi兼容的接口完成的。该器件提供4个通用输入/输出（GPIO）引脚，供通用使用。多个设备可以使用START引脚同步。

内部引用产生低噪声$4。5 ~ \mathsfit {V}$内部电压，当使能时内部振荡器产生2.048 mhz时钟。多功能患者偏置驱动块允许选择任何电极组合的平均值，以产生患者驱动信号。引线检测可以通过使用电流源或电流汇来完成。可提供一次性带内引线选项和连续带外内部引线选项。

# 9.2功能框图

![](images/7bfbd584a2cc8ca5d78f2e09c60a274d766d6a65ee86e57d1ffa290057e501ed.jpg)

# 9.3特性描述

介绍ADS1299-x内部功能元件的详细信息。首先讨论模拟模块，然后是数字接口。实现脑电图特定功能的块将在本节末尾介绍。

在本文中，$\pmb {f} _ {CLK}$表示CLK引脚信号频率，$t _ {CLK}$表示CLK引脚信号周期，$f _ {D R}$表示输出数据速率，$t _ {D R}$表示输出数据周期，$f _ {M O D}$表示调制器对输入采样的频率。

# 9.3.1模拟功能

# 9.3.1.1输入复用器

ADS1299- $\cdot \mathsf {x}$输入多路复用器非常灵活，并提供许多可配置的信号切换选项。图18显示了该设备的单个通道上的多路复用器。请注意，该器件有四个（ADS1299-4），六个（ADS1299-6）或八个（ADS1299）这样的块，每个通道一个。SRB1、SRB2和BIASIN对所有块都是通用的。INxP和INxN分别用于四个、六个或八个块。这种灵活性允许重要的设备和子系统诊断，校准和配置。通过使用CONFIG3寄存器中的BIAS_MEAS位和MISC1寄存器中的SRB1位（详细信息请参见寄存器映射部分的CONFIG3：配置寄存器3小节），将适当的值写入CHnSET[3:0]寄存器（参见CHnSET：单个通道设置部分）来切换每个通道的设置选择。请参阅输入多路复用器部分，了解有关多路复用器特定于eeg的功能的更多信息。

![](images/303e219caf247ca67685e42af9e16efb8bb8dc5a31c7d44e457644b1c1d59e59.jpg)  
图18。输入多路复用块为一个通道

(1) MAIN等于$\mathsf {M} \mathsf {U} \乘以[2:0]= 0 0 0$,$\mathsf {M} \乘以[2:0]= 1 1 0$，或$\mathsf {M U} \乘以[2{:}0]= 1 1 1$。

#功能描述（续）

# 9.3.1.1.1设备噪声测量

设置CHnSET[2:0] $= 0 0 1$设置$[(\mathsf {V} _ {\mathsf {V R E F P}} + \mathsf {V} _ {\mathsf {V R E F N}}) \ /$ 2]到两个通道输入的共模电压。
此设置可用于测试用户系统中的固有设备噪声。

# 9.3.1.1.2测试信号（TestP和TestN）

设置CHnSET $[2]；0] = 1 0 1$提供内部生成的测试信号，用于上电时的子系统验证。该功能允许对设备内部信号链进行测试。

测试信号是通过寄存器设置来控制的（详细信息请参见寄存器映射部分中的CONFIG2: Configuration register $^{2}$小节）。TEST_AMP控制信号幅度，test_freq控制所需频率的切换。

# 9.3.1.1.3温度传感器（TempP, TempN）

ADS1299- $\cdot \mathsf {x}$包含一个片上温度传感器。该传感器使用两个内部二极管，其中一个二极管的电流密度是另一个二极管的16倍，如图19所示。二极管电流密度的差异产生与绝对温度成正比的电压差。

由于封装对印刷电路板（PCB）的热阻较低，因此内部器件温度与PCB温度密切相关。请注意，ADS1299- $\cdot \mathsf {x}$的自加热导致读数高于周围PCB的温度。

公式3的比例因子将温度读数转换为摄氏度。在使用这个方程之前，温度读取代码必须首先缩放到微伏。

Temperature美元(^超circ出于美观\ mathsf那C) = \ left (\ frac超mathsf可不,T e m p e r a T u r e \ r e a d i n g \(\“缪V)出于美观- 1 4 5、3 0 0 \ \“缪V出于美观可不,9 4 0 \ \“缪\ mathsf可不,V  / } ^ { \ circ出于美观\ mathsf那C出于美观\好了)+ 2 5 ^超circ出于美观\ mathsf那C美元。

温度传感器监视器

![](images/e42ad083a8fbbab3b4c1f1531998685bb2df657cf5572bdf064e94f2b128af22.jpg)  
图19所示。输入温度传感器测量

# 9.3.1.1.4供电测量（MVDDP， MVDDN）

设置CHnSET[2:0] $\begin{array} { r l } { \mathbf { \Phi } = } & { { } 0 1 \mathbf { \ P } } \end{array}$将通道输入设置为设备的不同电源电压。
对于通道1、2、5、6、7和8，（MVDDP - MVDDN）为$[0]。5 \times (\mathsf {A V D D} + \mathsf {A V S S})]$。

对于通道3和4，（MVDDP - MVDDN）是DVDD 4。

为避免测量电源时PGA饱和，请将增益设置为1。

# 9.3.1.1.5引线激励信号（LoffP, LoffN）

引线激励信号在开关之前被送入多路复用器。检测引线条件的比较器也在开关之前连接到多路复用器块。有关引线块的详细说明，请参阅引线检测部分。

#功能描述（续）

# 9.3.1.1.6辅助单端输入

BIASIN引脚主要用于在偏置电极脱落的情况下将偏置信号路由到任何电极。但是，BIASIN引脚可以用作多个单端输入通道。相对于BIASIN引脚的电压，可以使用8个通道中的任何一个来测量BIASREF引脚的信号。此测量通过将通道多路复用器设置为“010”和将CONFIG3寄存器的BIAS _MEAS位设置为“1”来完成。

# 9.3.1.2模拟输入

器件的模拟输入直接连接到集成的低噪声、低漂移、高输入阻抗、可编程增益放大器。放大器位于单个通道多路复用器之后。

ADS1299- $\cdot \mathsf {x}$模拟输入是完全差分的。差分输入电压$(V _ {| | \mathsf {N X P}} - \mathsf {V} _ {| | \mathsf {N X N}})$可以从- vref增益到$V _ {\sf REF} /$增益。请参阅数据格式部分，了解模拟输入和数字代码之间的相关性。驱动ADS1299-x模拟输入的一般方法有两种：伪微分或全微分，如图20、图21和图22所示。

![](images/2f8056114147c26024288133b4295cbdd258b33187bfa264f688e6d696618f38.jpg)  
图20。驱动ADS1299-x的方法：伪微分或全微分

![](images/bd5102a54bd11447dc6b4802454cc769ca5171dfa20aa94f52d1015c36cf8b0d.jpg)

将INxN引脚保持在公共电压，最好是中电源，以配置伪差分信号的全差分输入。将INxP引脚围绕公共电压-VREF增益旋转至$\mathsf {V} _ {\mathsf {REF}} /$增益，并保持在绝对最大规格范围内。当输入配置为伪差分模式时，共模电压$(\mathsf {V} _ {\mathsf {C M}})$随信号电平的变化而变化。验证最小和最大点处的差分信号符合输入共模范围一节中讨论的共模输入规范。

将INxP和INxN处的信号配置为$ 18 0 ^ {\circ}$离相，以公共电压为中心，以使用全差分输入方法。INxP和INxN输入都从公共电压$+ \ V _ {2} \ V _ {\sf R E F} /$增益振荡到公共电压- 1 / 2 $V _ {\mathsf {R E F}} \ /$增益。最大和最小点处的差分电压等于$- V _ {\mathsf {R E F}} /$增益到$V _ {\sf R E F} /$增益，并以固定共模电压$(\mathsf {V} _ {\mathsf {C M}})$为中心。在差分配置中使用ADS1299-x来最大化数据转换器的动态范围。为了获得最佳性能，建议将公共电压设置在模拟电源的中点$\左((\mathsf {A V D D} + \mathsf {A V S S}) / 2 \右]$。

# 9.3.1.3 PGA设置和输入范围

低噪声PGA是一个差分输入和输出放大器，如图23所示。PGA有7个增益设置（1、2、4、6、8、12和24），可以通过写入CHnSET寄存器来设置（详细信息请参见寄存器映射部分的CHnSET：单个通道设置小节）。ADS1299-x具有CMOS输入，因此电流噪声可以忽略不计。表5显示了各种增益设置的典型带宽值。注意，表5显示了小信号带宽。对于大信号，性能受到PGA转换率的限制。

![](images/f5d2dc8efa849b6de2ae886eb97e813f468721202c2e245b76d60692443f37e9.jpg)  
图23。PGA实现

表5所示。PGA增益与带宽

<html><body><table><tr><td>GAIN</td><td>NOMINAL BANDWIDTH AT ROOM TEMPERATURE (kHz)</td></tr><tr><td></td><td>662</td></tr><tr><td>2</td><td>332</td></tr><tr><td>4</td><td>165</td></tr><tr><td>6</td><td>110</td></tr><tr><td>8</td><td>83</td></tr><tr><td>12</td><td>55</td></tr><tr><td>24</td><td>27</td></tr></table></body></html>

实现增益的PGA电阻串有$ 39。6 ~ \mathsf {k} \Omega$的电阻增益为12。在差分输入信号存在的情况下，该电阻提供了通过PGA输出的电流路径。该电流是在输入端存在差分信号时为设备指定的静态电流之外的。

# 9.3.1.3.1输入共模范围

为了保持在PGA的线性工作范围内，输入信号必须满足本节讨论的某些要求。

图23中放大器的输出不能比$2 0 0 ~ \mathsf {m V}更接近电源（AVSS和AVDD）。美元。如果放大器的输出被驱动到电源轨道的$2 0 0 ~ \mathsf {m V}$范围内，则放大器饱和并因此成为非线性。为了防止这种非线性工作状态，输出电压不得超过前端的共模范围。

前端的可用输入共模范围取决于各种参数，包括最大差分输入信号、电源电压、PGA增益和放大器净空的$2 0 0 ~ \mathsf {m V}$。这个范围如式4所示：

$$
\mathsf { A V D D - 0 . 2 ~ V \mathrm { \Omega - \left( \frac { \mathsf { G a i n } \times \mathsf { V _ { M A X \_ D I F F } } } { 2 } \right) \mathrm { \Omega > \mathsf { C M } > \mathsf { A V S S } + 0 . 2 ~ V \mathrm { \Omega + \left( \frac { \mathsf { G a i n } \times \mathsf { V _ { M A X \_ D I F F } } } { 2 } \right) \Omega } } } }
$$

where:

VMAX_DIFF $\mathbf {\tau} = \mathbf {\tau}$ PGA输入端的最大差分信号$\mathbf {C M} =$共模范围

例如:

如果$\mathsf {A V D} = 5 \:\ mathsf {v} $，那么$\mathsf {v}_{\mathsf {M A x\ D I F}} = 3 5 0~\ mathsf {M v} $2。3 V < C M < 2现在7 V美元。

# 9.3.1.3.2输入差分动态范围

差分输入电压范围$(V \vert \varsigma _ {\sf I N X P} - \sf V _ {\sf I N X N})$取决于系统中使用的模拟电源和基准。这个范围如公式5所示。

$$
\mathsf { F u l l - S c a l e \ R a n g e } = \frac { \pm V _ { \mathsf { R E F } } } { \mathsf { G a i n } } = \frac { 2 V _ { \mathsf { R E F } } } { \mathsf { G a i n } }
$$

# 9.3.1.3.3 ADC ΔΣ调制器

每个ADS1299-x通道都有一个24位$\Delta \Sigma$ ADC。该转换器采用针对低噪声应用优化的二阶调制器。调制器以$( \mathsf { f } _ { \mathsf { M O D } } = \mathsf { f } _ { \mathsf { C L K } } / 2 )$的速率对输入信号进行采样。与任何$\Delta \Sigma$调制器的情况一样，器件噪声的形状直到fMOD $1 2$，如图24所示。下一节介绍的片上数字抽取滤波器可用于滤除较高频率的噪声。这些片上抽取滤波器还提供抗混叠滤波。这种$\Delta \Sigma$转换器功能大大降低了奈奎斯特adc通常所需的模拟抗混叠滤波器的复杂性。

![](images/3158a32174c9f305c116fd9d3a923d5d19cf47159adba062ddd73e8de6e9980c.jpg)  
图24。调制器噪声频谱高达$\pmb{0。5 \times}$ fMOD

# 9.3.1.3.4参考

图25显示了ADS1299- $\cdot \mathsf {x}$内部引用的简化框图。相对于AVSS产生4.5 v参考电压。当使用内部基准电压时，将$V _ {P E F N}$连接到AVSS。

![](images/8800892354b12f9bf474968ec4afb00651388b2b64aa6aa4dfd2578575cdd402.jpg)  
图25。内部参考

(1) For $V _ { \mathsf { R E F } } = 4 . 5 \$ V: ${ \sf R 1 } = 9 . 8 ~ { \sf k \Omega }$ , ${ \sf R } 2 = 1 3 . 4 { \sf k } \Omega$ , and ${ \sf R } 3 = 3 6 . 8 5 { \sf kOmega }$ .

外部限带电容器决定参考噪声贡献的量。对于高端脑电图系统，电容值的选择应使带宽限制在小于$ 10 H 2$，以使参考噪声不会支配系统噪声。

或者，可以关闭内部参考缓冲区的电源，并将外部参考应用于VREFP。图26显示了一个典型的外部参考驱动电路。下电由CONFIG3寄存器中的PD _REFBUF位控制。当两个设备级联时，这种断电也用于共享内部引用。缺省情况下，设备以外部引用方式唤醒。

![](images/3a4de03c26e84653ea18afd83847ec802f9eb4501c4acb53c8b2fa3e84b27b39.jpg)  
图26。外部参考驱动

# 9.3.2数字功能

# 9.3.2.1数字抽取滤波器

数字滤波器接收调制器输出并抽取数据流。通过调整过滤量，可以在分辨率和数据速率之间进行权衡：更高分辨率过滤更多，更高数据速率过滤更少。更高的数据速率通常用于脑电图应用中的交流引线检测。

每个通道上的数字滤波器由一个三阶sinc滤波器组成。sinc滤波器抽取比可以通过CONFIG1寄存器中的DR位来调整（详细信息请参见寄存器映射部分）。此设置是影响所有通道的全局设置，因此，所有通道在设备中以相同的数据速率运行。

# 9.3.2.1.1 Sinc级过滤器（sinx x）

sinc滤波器是一个可变抽取率，三阶，低通滤波器。数据以fMOD的速率从调制器提供给滤波器的这一部分。正弦滤波器衰减调制器高频噪声，然后将数据流抽取成并行数据。抽取率影响整个转换器的数据速率。

式6给出了sinc滤波器的缩放后的z域传递函数。

$$
| {  \sf h } (z) | = \left| \frac  { 1 - z ^ { - \sf n }  }  { 1 - z ^ { - 1 }  }  \rightbb0 ^ { 3 }
$$

sinc滤波器的频域传递函数如式7所示。

$$
\sf H ( f ) \mid \ = \left| \frac { \sin \left( \displaystyle \frac { N \pi f } { f _ { \mathrm { M O D } } } \right) } { N \times \sin \left[ \displaystyle \frac { \pi f } { f _ { \mathrm { M O D } } } \right] } \right| ^ { 1 }
$$

式中：${\sf N} =$抽取比

sinc滤波器具有在输出数据速率及其倍数处出现的陷波（或零点）。在这些频率下，滤波器具有无限衰减。图27显示了sinc滤波器的频率响应，图28显示了sinc滤波器的滚降。随着输入的阶跃变化，滤波器需要$3 \乘以1 _ {\mathsf {D} \mathsf {R}}$来结算。在START信号的上升沿之后，滤波器花费$\mathsf {t} _ {\mathsf {S E t t L E}}$时间来给出第一个数据输出。在SPI接口部分的开始小节中讨论了各种数据速率下滤波器的稳定时间。图29和图30分别显示了在不同数据速率下，直到${\mathfrak {f}} _ {\mathsf {MOD}} \ / \ 2$和fMOD / 16为止的滤波器传递函数。图31显示了扩展到$4 \乘以\mathsf {f} _ {\mathsf {M O D}}$的传递函数。ADS1299- $\cdot \mathsf {x}$通频带在每个fMOD重复自己。系统中的输入R-C抗混叠滤波器的选择应使在$\mathsf {f} _ {\mathsf {M O D}}$的倍数附近的任何干扰都得到充分衰减。

![](images/1f9da2ec15793594b96bff3c3d93ab3b3de3ac74cc121413145f91dfcc7df354.jpg)  
图27所示。Sinc滤波器频率响应

![](images/0fe7db0f3546dc656a6148b1dfda489f29a442556d9020ea8228ca207bf42e13.jpg)  
图28。Sinc过滤器滚落

![](images/7ea03ba56fc95f9704a6993ff18bbcbd801fce4f65bb43fb261519f574667692.jpg)  
图29。片上抽取滤波器的传递函数直到fMOD / 2

![](images/2bd7cb6b52e4245a76c8e7691bc2bb465d7bf0306465bb6daf809c71c9f0f12b.jpg)  
图30。片上抽取滤波器的传递函数直到fMOD 16

![](images/d08a2ceaff1a24834397a5f01f4cf1c41b14b4fbac6dea75c13cdcec44e27458.jpg)  
图31。片上抽取滤波器的传递函数直到$4 + \cos$对于DR[2:0] $\mathbf { \tau } = \mathbf { 0 0 0 }$和DR[2:0] $\mathbf { \lambda } = \mathbf { \lambda }$ 110

# 9.3.2.2时钟

ADS1299-x提供内部和外部两种器件时钟方式。内部时钟非常适合低功耗，电池供电的系统。内部振荡器在室温下精确修剪。准确度在指定的温度范围内变化；见电气特性。时钟选择由CLKSEL引脚和CLK_EN寄存器位控制。

CLKSEL引脚选择内部或外部时钟。CONFIG1寄存器中的CLK_ EN位使能和禁用在CLK引脚中输出的振荡器时钟。这两个引脚的真值表如表6所示。CLK_ EN位在菊花链配置中使用多个设备时非常有用。下电时，建议关闭外部时钟，以节省电。

表6所示。CLKSEL引脚和CLK_EN位

表身体< html > < > < > < tr > < td > CLKSEL销< / td > < td > CONFIG1。CLK_EN BIT</td>< /td>时钟源</td><td>CLK 引脚 STATUS</td></ td>< tr><td>0</td><td>X</td><td>外部时钟</td><td>输入：外部时钟</td></ td>< tr><td></td><td>0</td><td>内部时钟振荡器</td><td>3-state</td></ td>< tr><td></td>< /td><td>内部时钟振荡器</td><td>输出：内部时钟振荡器</td></tr></table></body></html>

9.3.2.3 GPIO

ADS1299- $\cdot \mathsf {x}$在正常工作模式下共有四个通用数字I/O （GPIO）引脚。数字$1 / \mathsf {O}$引脚可以单独配置为通过GPIOC位寄存器的输入或输出。GPIO寄存器中的GPIOD位控制引脚电平。当读取GPIOD位时，返回的数据是引脚的逻辑电平，无论它们被编程为输入还是输出。当GPIO引脚配置为输入时，对相应GPIOD位的写操作无效。当配置为输出时，写入GPIOD位将设置输出值。

如果配置为输入，这些引脚必须驱动（不浮动）。GPIO引脚在上电或复位后被设置为输入。图32显示了GPIO端口结构。如果不使用，引脚应短接到DGND。

![](images/6702b60ffe426404ad8cbdcec2c6ec17aea83ef9866f3448187bf590e6091629.jpg)  
图32。GPIO端口引脚

# 9.3.2.4心电图和脑电图的特定特征

# 9.3.2.4.1输入多路复用器（重路由BIAS驱动信号）

输入多路复用器具有用于偏置驱动信号的脑电图特定功能。当选择适当的通道进行BIAS推导，在芯片外部安装反馈元件，并关闭环路时，BIAS信号在BIASOUT引脚可用。该信号既可以经过滤波后馈送，也可以直接馈送到BIASIN引脚，如图33所示。通过将相应通道集寄存器的MUX位设置为$\mathsf {P}$ -侧的“110”或n侧的“111”，可以将该BIASIN信号多路复用到任何输入电极。图33显示了从通道1、2和3产生并路由到通道8的n侧的BIAS信号。该特性可用于动态改变作为参考信号的电极，以驱动患者身体。

![](images/98a71c64c4ea18b3bc008e4554ac90286d02a9e0d1b3d6a01cfa8a85c13a8957.jpg)  
  
图33。配置BIASOUT信号路由到IN8N示例

(1)典型值仅为示例。

9.3.2.4.2输入多路复用器（测量BIAS驱动信号）

此外，可以将BIASOUT信号路由到一个通道（不用于计算BIAS）进行测量。图34显示了将BIASIN信号路由到通道8的寄存器设置。测量是根据BIASREF引脚上的电压完成的。如果BIASREF被选择为内部的，那么BIASREF是$[(\mathsf {A V D D} + \mathsf {A V S S}) /$ 2]。此特性对于产品开发期间的调试非常有用。

![](images/18862990478cd6123ba1a5f5933a45c28e0cdc175f121f1a5df67f52b594f3a6.jpg)  
1.5 nF(1)   
版权所有：德州仪器有限公司
图34。配置为8通道回读的BIASOUT信号

(1)典型值仅为示例。

# 9.3.2.4.3引线断开检测

已知病人电极阻抗随时间衰减。必须连续监测这些电极连接，以验证是否存在合适的连接。ADS1299-x引线检测功能模块为用户从各种引线检测策略中进行选择提供了极大的灵活性。虽然称为引线检测，但这实际上是一种电极检测。

其基本原理是注入激励电流并测量电压以确定电极是否断开。如图35引线检测功能框图所示，该电路提供了两种不同的方法来确定患者电极的状态。两种方法的不同之处在于激励信号的频率含量。可以使用LOFF_SENSP和LOFF_SENSN寄存器在每个通道的基础上选择性地完成引线。此外，内部激励电路可以被禁用，并且仅可以启用传感电路。

![](images/e470705b6e0007e133191d423e6faf9eae70f6b09ed3ce669e7eb7d4b06e20c5.jpg)  
图35。脱铅检测

# 9.3.2.4.3.1直流引线

在这种方法中，引线激励采用直流信号。直流激励信号可以从外部上拉或下拉电阻或内部电流源或接收器中选择，如图36所示。通道的一侧被拉向供电，另一侧被拉向地。通过设置LOFF_FLIP寄存器中的位，可以交换上拉和下拉电流（如图36b和图36c所示）。在电流源或电流汇的情况下，电流的大小可以通过使用LOFF寄存器中的ILEAD_OFF[1:0]位来设置。与10-MΩ上拉或下拉电阻相比，电流源或接收器提供更大的输入阻抗。

![](images/5dd665eab37850ea6ba4836d48db7088e85155dc5198dcb41f97a9a3f0a3353f.jpg)  
图36。直流引线励磁选项

响应的感知可以通过从设备搜索数字输出代码或通过用片上比较器监测输入电压来完成。如果任一电极断开，上拉电阻和下拉电阻使通道饱和。搜索输出代码确定是p侧关闭还是n侧关闭。为了确定哪一个是错误的，必须使用比较器。输入电压也使用比较器和3位DAC进行监测，其电平由LOFF寄存器中的COMP TH[2:0]位设置。比较器的输出存储在LOFF_STATP和LOFF_STATN寄存器中。这些寄存器作为输出数据流的一部分可用。（参见SPI接口部分的数据输出（DOUT）小节。）如果不使用直流引线，则可以通过在CONFIG4寄存器中设置PD_LOFF_COMP位来关闭引线比较器的电源。

在引线部分给出了一个打开直流引线的示例过程。

9.3.2.4.3.2交流导通（单次或周期）

在这种方法中，使用带内交流信号作为激励。交流信号是通过在输入端以固定频率交替地提供电流源和接收器来产生的。频率可以通过LOFF寄存器中的FLEAD OFF[1:0]位来选择。激励频率选择为两个带内频率选择中的一个。8 ~ \mathsf {H z}$或$ 312 \ \mathsf {H z})$)。该带内激励信号通过通道并在输出端进行测量。

交流信号的感应是通过将信号通过通道进行数字化，然后在输出端进行测量来完成的。交流励磁信号以感兴趣的频带内的频率引入。该信号可以单独滤出并处理。通过测量在激励信号频率处的输出幅值，可以计算电极阻抗。

对于连续导通，必须在输入端外部施加带外交流电流源或吸收。然后可以对该信号进行数字处理以确定电极阻抗。

# 9.3.2.4.4偏置引线

正常工作时引线断开检测

在正常工作期间，由于必须关闭BIAS放大器，因此不能使用ADS1299- $\cdot \mathsf {x}$ BIAS上电引脚功能。

上电时引线断开检测

该功能包含在ADS1299-x中，用于确定偏置电极是否正确连接。在上电时，ADS1299- $\cdot \mathsf {x}$使用电流源和比较器来确定BIAS电极连接状态，如图37所示。比较器的参考电平被设置为确定可接受的BIAS阻抗阈值。

![](images/68d4dc7a6eac05547f842a1415b9f5795692d68bab4ae788aa0c9a9f06d6a97c.jpg)  
图37。上电引线检测

当BIAS放大器上电时，电流源没有作用。只有比较器可以用来检测BIAS放大器输出端的电压。比较器阈值由相同的LOFF[7:5]位设置，用于设置其他负输入的阈值。

9.3.2.4.5偏置驱动（直流偏置电路）

使用偏置电路来对抗脑电图系统中由电源线和其他来源（包括荧光灯）引起的共模干扰。偏置电路感知一组选定电极的共模电压，并通过用反向共模信号驱动机体产生负反馈回路。负反馈回路将共模运动限制在一个狭窄的范围内，这取决于回路增益。稳定整个回路是特定于基于回路中各个极点的单个用户系统的。ADS1299-x集成了复用器来选择通道和运算放大器。所有放大器端子均可在引脚处使用，允许用户选择用于反馈回路的组件。图38中的电路显示了偏置电路的整体功能连接。

![](images/88880f701e548d6774d1151ce2e62dbe9a62cd4923081d289deaeab3aad2ff11.jpg)  
图38。偏置驱动放大器通道选择

(1)典型值。

偏置驱动器的参考电压可以选择内部生成$[(\mathsf {A V D D} + \mathsf {A V S S})$) / 2]或外部提供电阻分压器。偏置环路的内部和外部参考电压的选择是通过将适当的值写入CONFIG2寄存器中的BIASREF位来定义的。

如果不使用偏置功能，放大器可以使用PD bias位关闭电源（详细信息请参见寄存器映射部分的CONFIG3: Configuration Register 3小节）。当多个ADS1299-x器件连接在一起时，使用PD BIAS位将除一个外的所有偏置放大器断电。

BIASIN引脚功能将在输入多路复用器部分进行解释。使用偏置放大器的示例程序显示在偏置驱动部分。

# 9.3.2.4.5.1多设备的偏置配置

图39显示了连接到偏置驱动器的多个设备。

![](images/e10ceb311c238790987658fb57cef209761d87eb35c1f23010bec2c40ba0437d.jpg)  
图39。多设备的BIAS驱动连接

# 9.4设备功能模式

# 9.4.1开始

将START引脚拉高至少$2 \ mathm {\ t {_ {C L K}}}$周期，或发送START命令开始转换。当START为低值且START命令未发送时，设备不发出DRDY信号（转换停止）。

当使用START命令控制转换时，请将START引脚保持在低位。ADS1299-x具有两种模式来控制转换：连续模式和单发模式。模式由SINGLE_SHOT （CONFIG4寄存器的第3位）选择。在多设备配置中，START引脚用于同步设备（参见SPI接口部分的多设备配置小节了解更多详细信息）。

# 9.4.1.1结算时间

稳定时间（tSETTLE）是当启动信号被拉高时，转换器输出完全稳定数据所需的时间。当START拉高时，DRDY也拉高。下一个DRDY下降沿表示数据已经准备好。图40显示了时序图，表7列出了不同数据速率的稳定时间。沉淀时间取决于$\mathsf {f} _ {\mathsf {C L K}}$和抽取比率（由CONFIG1寄存器中的DR[2:0]位控制）。当初始稳定时间过去后，DRDY下降沿以设定的数据速率tDR发生。如果DOUT上没有回读数据，并且输出移位寄存器需要更新，则DRDY在返回低值之前变为高值$4 \ mathm {{\ t _ {C L K}}}$，表示新数据准备就绪。表7列出了沉淀时间与tCLK的关系。请注意，当START保持高位并且输入信号中有阶跃变化时，需要$3 \乘以t _ {\ mathm {D R}}$才能使滤波器稳定到新值。在第四个DRDY脉冲上可以得到固定的数据。

![](images/b05c3f68875ce56cd0adffb372983ec83bd6ff360c3f5f92ef3a5b9dcbfb3745.jpg)  
图40。沉淀时间

#设备功能模式（续）

表7所示。不同数据速率的结算时间

<html><body><table><tr><td>DR[2:0]</td><td>NORMAL MODE</td><td>UNIT</td></tr><tr><td>000</td><td>521</td><td>tCLK</td></tr><tr><td>001</td><td>1033</td><td>tCLK</td></tr><tr><td>010</td><td>2057</td><td>tCLK</td></tr><tr><td>011</td><td>4105</td><td>tCLK</td></tr><tr><td>100</td><td>8201</td><td>tCLK</td></tr><tr><td>101</td><td>16393</td><td>tCLK</td></tr><tr><td>110</td><td>32777</td><td>tCLK</td></tr></table></body></html>

# 9.4.2复位（Reset）

复位ADS1299-x有两种方法：将reset引脚拉低或发送reset命令。当使用RESET引脚时，请确保在将引脚调高之前遵循最小脉冲持续时间定时规范。RESET命令在命令的第八个SCLK下降沿生效。重置后，需要18个tCLK周期来完成配置寄存器到默认状态的初始化并开始转换周期。注意，只要使用WREG命令将CONFIG1寄存器设置为新值，就会自动向数字过滤器发出内部重置。

9.4.3 电源-Down （PWDN）告警解释

当PWDN被拉低时，所有片上电路断电。要退出掉电模式，请将PWDN引脚调高。从掉电模式退出后，内部振荡器和基准需要时间唤醒。下电时，建议关闭外部时钟，以节省电。

# 9.4.4数据检索

# 9.4.4.1 Data Ready （DRDY）告警解释

DRDY是一种从高电平到低电平的输出信号，表示新的转换数据已经准备好了。CS信号对数据准备信号没有影响。DRDY行为取决于设备是处于RDATAC模式还是使用RDATA命令按需读取数据。（请参阅SPI命令定义部分的RDATAC：连续读取数据和RDATA：读取数据小节了解更多详细信息）。

当使用RDATA命令读取数据时，读取操作可以与下一个DRDY发生重叠，而不会损坏数据。

START引脚或START命令将设备置于正常数据捕获模式或脉冲数据捕获模式。

图41显示了数据检索期间DRDY、DOUT和SCLK之间的关系（在ADS1299的情况下）。DOUT锁存于SCLK上升沿处。DRDY在SCLK下降沿处被拉高。注意，无论从设备中检索数据还是通过DIN引脚发送命令，DRDY在第一个SCLK下降沿上都是高的。

![](images/3b15d58a0e73871666d33e7241175d33e4f8b07c3dc0829f57a2899fe3970d1a.jpg)  
图41。DRDY与数据检索$\ textcircles {C S} = 0)$

#设备功能模式（续）

# 9.4.4.2回读数据

数据检索可以通过以下两种方法之一完成：

1. RDATAC: read data continuous命令将设备设置为连续读取数据而不发送命令的模式。请参阅RDATAC：读取数据连续部分了解更多详细信息。2. RDATA：读取数据命令要求向设备发送一个命令，用最新的数据加载输出移位寄存器。请参阅RDATA: Read Data小节了解更多细节。

转换数据是通过在DOUT上移出数据来读取的。DOUT上数据的MSB在第一个SCLK上升沿上被时钟锁定。DRDY在第一个SCLK下降沿返回高值。在整个读操作中，DIN应该保持较低。

数据输出中的位数取决于通道数和每个通道的位数。对于8通道ADS1299，数据输出的数量为[（24个状态位$+ 24个$ bits $\乘以8个$通道）$= 2 1 6个$ bits]。24个状态位的格式为：（1100 + LOFF STATP $^ +$ LOFF _STATN $^ +$ bits[4:7]的GPIO寄存器）。每个通道数据的数据格式为双补先MSB。当使用用户寄存器设置通道下电时，相应的通道输出被设置为$" 0 "$。然而，通道输出序列保持不变。

ADS1299-x还提供多次回读功能。在RDATAC模式下，只需提供更多的sclk就可以多次读出数据，在这种情况下，MSB数据字节在读取最后一个字节后重复。对于多次回读，CONFIG1寄存器中的DAISY_EN位必须设置为“1”。

# 9.4.5连续转换模式

转换开始时，开始引脚是高或开始命令被发送。如图42所示，当转换开始时，DRDY输出变高，当数据准备好时，DRDY输出变低。转换无限期地继续下去，直到开始引脚被取低或停止命令被传输。当START引脚拉低或发出STOP命令时，允许正在进行的转换完成。图43和表8说明了在这种模式下控制转换时，START引脚或START和STOP命令所需的DRDY定时。tSDSU时序指示何时将START引脚置低，或何时在DRDY下降沿之前发送STOP命令以停止进一步转换。tDSHD时序指示何时将START引脚置低或在DRDY下降沿后发送STOP命令以完成当前转换并停止进一步转换。为了保持转换器连续运行，START引脚可以永久固定在高位。

当从单镜头模式切换到连续转换模式时，将START信号调低并调高，或者发送一个STOP命令，然后发送一个START命令。这种转换模式是理想的应用程序，需要一个固定的连续流的转换结果。

![](images/edaec420f693f841ab53aaf912f1bd82c09977d4fec1d8b4f81ccaa394ff6e47.jpg)  
图42。连续转换方式

(1) START和STOP命令在第7条SCLK下降沿生效。

#设备功能模式（续）

![](images/76262d81188cac38d7b76803651e5536c26cf8e78cd6ad4934fc85b078839666.jpg)  
图43。开始DRDY计时

(1) START和STOP命令在命令末尾的第七个SCLK下降沿生效。

表8所示。图43(1)时序特性

<html><body><table><tr><td></td><td>MIN</td><td>UNIT</td></tr><tr><td>START 引脚 low或STOP命令到DRDY设置时间以停止进一步转换tsDSU</td><td>16</td><td>tCLK</td></tr><tr><td>tDSHD START 引脚 low或STOP命令完成当前转换</td><td>16</td><td>tCLK</td></tr></table></body></html>

(1) START和STOP命令在命令末尾的第七个SCLK下降沿生效。

# 9.4.6单发模式

通过将CONFIG4寄存器中的SINGLE _SHOT位设置为“1”来启用单镜头模式。在单拍模式下，当START引脚取高或发送START命令时，ADS1299-x执行单次转换。如图44所示，当转换完成时，DRDY变低，并停止进一步的转换。无论是否读取转换数据，DRDY都保持较低。要开始新的转换，请将START引脚调低，然后调高，或再次发送START命令。当从连续转换模式切换到单镜头模式时，将START信号调低并调高，或者发送一个STOP命令，然后发送一个START命令。

![](images/7c33fe75ae61f414bc7113d205ba967ceba3e7efb1003bbc3b8eeaf12196de79.jpg)  
图44。单镜头模式下无数据检索的DRDY

这种转换模式非常适合需要非标准或非连续数据速率的应用。发出START命令或切换START引脚高复位数字滤波器，有效地将数据速率降低四倍。这种模式使系统更容易受到混叠效应的影响，需要更复杂的模拟或数字滤波。主机处理器上的负载增加，因为处理器必须切换START引脚或发送START命令来启动新的转换周期。

# 9.5编程

# 9.5.1数据格式

该设备提供24位二进制二进制补码格式的数据。使用公式8计算一个代码（LSB）的大小。

$$
1 \mathsf {L S B} = (2 \times \mathsf {V _ {R E F}} / \mathsf {G a i n}) / 2 ^ {2 4} = + \mathsf {F S} / 2 ^ {2 3}
$$

正满量程输入产生7FFFFFh的输出码，负满量程输入产生800000h的输出码。对于超过满量程的信号，输出在这些代码处进行剪辑。表9总结了不同输入信号的理想输出代码。当模拟输入处于正满量程或负满量程时，所有24位切换。

表9所示。理想输出码与输入信号

<html><body><table><tr><td>INPUT SIGNAL, VIN (INxP INxN)</td><td>IDEAL OUTPUT CODE(1)</td></tr><tr><td>≥ FS</td><td>7FFFFFh</td></tr><tr><td>+FS 1 (223</td><td>000001h</td></tr><tr><td>0</td><td>000000h</td></tr><tr><td>-FS 1) (223</td><td>FFFFFFh</td></tr><tr><td>≤ −FS 223-1) (223</td><td>800000h</td></tr></table></body></html>

(1)排除噪声、线性、偏置和增益误差的影响。

# 9.5.2 SPI接口

spi兼容串行接口由四个信号组成：$\overline {{\mathsf {C S}}}$、SCLK、DIN和DOUT。该接口读取转换数据，读写寄存器，并控制ADS1299- $\cdot \mathsf {x}$操作。数据就绪输出DRDY（参见Data Ready $\overline {{(DRDY)}}}$一节）用作状态信号，表示数据准备就绪。当有新数据可用时，DRDY变低。

$({\overline {{C S}}})$

$\overline {{\mathsf {C S}}}$引脚激活SPI通信。在数据事务之前，CS必须低，并且在整个SPI通信期间必须保持低。当CS值高时，DOUT引脚进入高阻抗状态。因此，忽略对串行接口的读写，并复位串行接口。DRDY引脚操作独立于CS。即使CS很高，DRDY仍然表明新的转换已经完成，并且作为对SCLK的响应而被强制提高。

$\overline {{\mathsf {C S}}}$ high只去激活与设备的SPI通信，串行接口复位。数据转换继续进行，可以监视DRDY信号以检查是否准备好了新的转换结果。监控DRDY信号的主设备可以通过拉低CS引脚来选择合适的从设备。在串行通信完成后，总是等待四个或更多的$\mathsf {t} _ {\mathsf {C L K}}$周期，然后再取CS高。

9.5.2.2 Serial Clock （SCLK）

SCLK为串行通信提供时钟。SCLK是一个施密特触发输入，但TI建议保持SCLK尽可能无噪声，以防止故障无意中转移数据。数据在SCLK的下降沿上移到DIN，在SCLK的上升沿上移出DOUT。

绝对最大SCLK限制在图1中指定。当使用SCLK移入命令时，请确保将整套SCLK发送给设备。如果不这样做，可能会导致设备串行接口被置于未知状态，需要将CS调高以恢复。

对于单个设备，SCLK所需的最小速度取决于通道数、分辨率位数和输出数据速率。（如果有多个级联设备，请参见“多设备配置”章节的“级联模式”小节。）

例如，如果ADS1299在500-SPS模式（8通道，24位分辨率）下使用，则最小SCLK速度为$ 11 0 \mathsf {k H z}$。

数据检索可以通过将设备置于RDATAC模式或根据需要发出RDATA命令来完成。公式9中的SCLK速率限制适用于RDATAC。对于RDATA命令，如果必须在两个连续的DRDY信号之间读取数据，则适用该限制。公式9假设在数据捕获之间没有发出其他命令。

$$
\tan \tt t _ { S C L K } < \frac { \Delta t _ { D R } - 4 \Delta t _ { C L K } } { \Delta _ { B I T S } \times N _ { C H A N N E L S } + 2 4 }
$$

9.5.2.3数据输入（DIN）

DIN与SCLK一起用于向设备发送数据。DIN上的数据被移到SCLK下降沿上的器件中。

本设备的通信本质上是全双工的。即使数据被移出，设备也会监视被移进的命令。在发送命令时，存在于输出移位寄存器中的数据被移出。因此，在移出数据时，请确保在DIN引脚上发送的任何内容都是有效的。当读取数据时不需要向设备发送命令时，在DIN上发送NOP命令。在DIN上发送多字节命令时，请确保在发送多字节命令部分中满足tSDECODE时序。

9.5.2.4数据输出（DOUT）

DOUT与SCLK一起用于从设备读取转换和注册数据。数据在SCLK的上升沿被打卡，MSB首先。当CS高时，DOUT进入高阻抗状态。图45显示了ADS1299数据输出协议。

DRDY   
SCLK 5151515/5/5L5/515 216个SCLK
DOUT STAT CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 T 24位24位24位24位24位24位24位24位24位24位24位24位24位24位24位24位24位24位DIN

# 9.5.3 SPI命令定义

ADS1299-x提供灵活的配置控制。表10总结了控制和配置设备操作的命令。这些命令是独立的，除了寄存器的读写操作需要第二个命令字节和数据。CS可以在命令之间设置高值或低值，但在整个命令操作中必须保持低值（特别是对于多字节命令）。系统命令和RDATA命令由设备在第七个SCLK下降沿上解码。寄存器读和写命令在第八个SCLK下降沿上解码。在发出命令后将CS拉高时，请确保遵循SPI时序要求。

表10。命令定义

表身体< html > < > < > < tr > < td >命令< / td > < td >描述< / td > < td >第一个字节< / td > < td >第二字节< / td > < / tr > < tr > < td colspan =“2”>系统命令< / td > < td colspan = " 2 " > < / td > < / tr > < tr > < td >唤醒< / td > < td >唤醒从待机模式< / td > < td > 0000 0010 (2 h) < / td > < td > < / td > < / tr > < tr > < td >备用< / td > < td >进入待机模式< / td > < td > 0000 0100 (04 h) < / td > < td > < / td > < / tr > < tr > < td >重置< / td > < td >重置设备< / td > < td > 0000 0110 (06 h) < / td > < td > < / td > < / tr > < tr > < td >开始< / td > < td >启动和重启(同步)转换< / td > < td > 0000 1000(08年h) < / td > < td > < / td > < / tr > < tr > < td >停止< / td > < td >停止转换< / td > < td > 0000 1010(0啊)< / td > < td > < / td > < / tr > < tr > < td colspan =“2”>数据读取命令< / td > < td colspan = " 2 " > < / td > < / tr > < tr > < td colspan =“2”> RDATAC < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > SDATAC < / td > < td > (1) < / td > < td > 00010000 (10 h) < / td > < td > < / td > < / tr > < tr > < td > RDATA < / td > < td >停止读取数据连续模式读取数据读回。< / td > < td > 0001 0001 (11 h) < / td > < td > < / td > < / tr > < tr > < td >注册读命令< / td > < td >命令;支持多个</td><td>0001 0010 (12h)</td>< /td></ td></td>< tr><td colspan="2"></td>< /td></ td></td></ td></td><td> RREG</td><td> (2xh) </td><td>000n nnnn(2)</td></ td>< tr><td>WREG</td><td>写n nnnn(2)</td></tr></table></body></html>

(1)在RDATAC模式下，忽略RREG命令。(2) n NNNN $\mathbf {\sigma} = \mathbf {\sigma}$要读或写的寄存器数- 1。例如，要读写三个寄存器，设置$n n n n = 0$（0010）。$r r r r =$读或写命令的起始寄存器地址。

# 9.5.3.1发送多字节命令

ADS1299- $\cdot \mathsf {x}$串行接口以字节为单位解码命令，需要$4 \tan \angle B$周期来解码和执行。因此，在发送多字节命令（如RREG或WREG）时，必须使用$4 ~ \mathrm {t _ {C L K}}$句点将一个字节（或命令）的末尾与下一个字节（或命令）的末尾隔开。

假设CLK是2美元。0 4 8 ~ \mathsf {M H z}$，则tSDECODE $(4 ~ \mathsf {t _ {C L K}})$为$1。6 ~ \mu \mathsf {s}$。当SCLK为16 MHz时，一个字节可以在500 ns内传输。这个字节传输时间不符合tSDECODE规范；因此，必须插入延迟，使第二个字节的末尾到达$1。6 ~ \mu \mathsf {s}$以后。如果SCLK为4mhz，则在$2 ~ {\mu \ s}$中传输一个字节。因为这个传输时间超过了tSDECODE规范，处理器可以毫无延迟地发送后续字节。在后面的场景中，可以对串行端口进行编程，使其从每个周期的单字节传输变为多个字节传输。

# 9.5.3.2 WAKEUP：退出备用模式

WAKEUP命令用于退出低功耗待机模式；参见SPI命令定义部分的STANDBY：进入STANDBY模式小节。退出待机模式需要时间（详见电气特性）。此命令没有SCLK速率限制，可以在任何时间发出。任何下列命令必须在$4 \tan \angle B E C$ cycles的延迟之后发送。

# 9.5.3.3 STANDBY：进入STANDBY模式

STANDBY命令用来进入低功耗待机模式。除参考部分外，电路的所有部分都关闭了。待机模式功耗在电气特性中指定。此命令没有SCLK速率限制，可以在任何时间发出。设备进入待机状态后，除了wake - up命令外，不能再下发其他命令。

# 9.5.3.4 RESET：将寄存器重置为默认值

RESET命令重置数字滤波器周期，并将所有寄存器设置返回默认值。有关详细信息，请参阅SPI接口部分的Reset $\overline {{(R E S E 7)}}$小节。此命令没有SCLK速率限制，可以在任何时间发出。$ 18 ~ \ mathm {t _ {C L K}}$执行RESET命令需要周期。在此期间避免发送任何命令。

# 9.5.3.5 START：启动转换

START命令启动数据转换。将START引脚绑低以通过命令控制转换。如果转换正在进行中，则此命令不起作用。STOP命令停止转换。如果START命令后面紧跟着STOP命令，那么它们之间必须有一个4 tclk的周期延迟。当START命令发送到设备时，保持START引脚低，直到发出STOP命令。（有关详细信息，请参阅SPI接口部分的Start小节。）此命令没有SCLK速率限制，可以在任何时间发出。

# 9.5.3.6 STOP：停止转换

STOP命令停止转换。将START引脚绑低以通过命令控制转换。当发送STOP命令时，正在进行的转换完成，并停止进一步的转换。如果转换已经停止，则此命令不起作用。此命令没有SCLK速率限制，可以在任何时间发出。

# 9.5.3.7 RDATAC：读取数据连续

RDATAC命令允许在每个DRDY上输出转换数据，而不需要发出后续的读取数据命令。这种模式将转换数据放在输出寄存器中，并且可以直接移出。所述读取数据连续模式为设备默认模式；设备上电时默认为此模式。

使用Stop Read Data Continuous命令取消RDATAC模式。如果设备处于RDATAC模式，则必须先发出SDATAC命令，然后才能向设备发送任何其他命令。此命令没有SCLK速率限制。然而，随后的数据检索sclk或SDATAC命令在完成之前应该等待至少4个tCLK周期（参见发送多字节命令一节）。RDATAC时序如图46所示。如图46所示，在DRDY脉冲周围有一个4 tCLK周期的隔离区，在这个区域内不能发出这个命令。如果没有从设备检索到数据，DOUT和DRDY在此模式下的行为类似。要在发出RDATAC命令后从设备检索数据，请确保START引脚高或发出START命令。图46显示了使用RDATAC命令的推荐方法。RDATAC非常适合于数据记录器或记录仪等应用程序，在这些应用程序中，寄存器是一次性设置的，不需要重新配置。

![](images/d39933f1849c21818e63e2d2b8d09396c85bfac04ae95c4a085b4abdb7047af7.jpg)  
图46。RDATAC用法

(1) tUPDATE $= 4$ / fCLK。请勿在此期间读取数据。

# 9.5.3.8 SDATAC：停止读数据连续

SDATAC命令用来取消读数据连续模式。这个命令没有SCLK速率限制，但是下一个命令必须等待$4 \tan \angle E$周期才能完成。

# 9.5.3.9 RDATA：读取数据

RDATA命令在不处于读取数据连续模式时用最新的数据加载输出移位寄存器。在DRDY变低后发出此命令以读取转换结果。该命令没有SCLK速率限制，后续命令或数据检索SCLK也不需要等待时间。要在发出RDATA命令后从设备检索数据，请确保START引脚高或发出START命令。当使用RDATA命令读取数据时，读取操作可以与下一个DRDY发生重叠，而不会损坏数据。图47显示了使用RDATA命令的推荐方法。RDATA最适合ECG和eeg类型的系统，其中必须在转换周期之间经常读取或更改寄存器设置。

![](images/22c1283e183327591381cc0fd1b9c17ca044860b2009fc2f84d0fc7085325c41.jpg)  
图47。RDATA用法

# 9.5.3.10 RREG: Read From Register

这个命令读取寄存器数据。Register Read命令是一个双字节命令，后面跟着寄存器数据输出。第一个字节包含命令和寄存器地址。第二个命令字节指定要读取的寄存器数- 1。

第一个命令字节：001r rrrr，其中r rrrr是起始寄存器地址。

第二个命令字节：000n nnnn，其中n nnnn是要读取的寄存器数- 1。

操作的第17个SCLK上升沿将第一个寄存器的MSB计时出来，如图48所示。当设备处于连续读数据模式时，必须先下发SDATAC命令，才能下发RREG命令。RREG命令可以在任何时候发出。但是，由于该命令是一个多字节命令，因此存在SCLK速率限制，这取决于如何发出SCLK以满足tSDECODE定时。有关详细信息，请参阅SPI接口部分的串行时钟（SCLK）小节。注意，对于整个命令，CS必须是低的。

![](images/049961e5bcf3c350dcd0a57bc6a55252684a7382710d46be28433632e4fd403b.jpg)  
图48。RREG命令示例：从寄存器00h开始读取两个寄存器（ID Register） （BYTE $\mathbf {1} = \mathbf {00 1 00 00 00 00}$, BYTE $\mathsf {\pmb {2}} = \mathsf {\pmb {00 00}} \mathsf {\bar {00 0}} \mathsf {\pmb {00 1}}）$

# 9.5.3.11 WREG: Write to Register

这个命令写寄存器数据。Register Write命令是一个两字节的命令，后面跟着寄存器数据输入。第一个字节包含命令和寄存器地址。第二个命令字节指定要写入的寄存器数- 1。

第一个命令字节：010r rrrr，其中r rrrr是起始寄存器地址。

第二个命令字节：000n nnnn，其中n nnnn是要写入的寄存器数- 1。

在命令字节之后，接着是寄存器数据（以MSB-first格式），如图49所示。WREG命令可以在任何时候发出。但是，由于该命令是一个多字节命令，因此存在SCLK速率限制，这取决于如何发出SCLK以满足tSDECODE定时。有关详细信息，请参阅SPI接口部分的串行时钟（SCLK）小节。注意，对于整个命令，CS必须是低的。

![](images/b26d20833f682988efcb837d3bfac66bc422bdaf27bbfc2c71c1d04e0514755d.jpg)  
图49。WREG命令示例：从00h开始写两个寄存器（ID Register） （BYTE $1 = 000 $ 0000, BYTE $\bar {2} = 000 $ 0001）

# 9.6寄存器映射

表11描述了各种ADS1299-x寄存器。

表11所示。寄存器分配

<html><body><table><tr><td rowspan="2">ADDRESS</td><td rowspan="2">REGISTER</td><td rowspan="2">DEFAULT SETTING</td><td colspan="7">REGISTER BITS</td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>Read Only ID Registers</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td colspan="9">ID</td></tr><tr><td>00h Global Settings Across</td><td>Channels</td><td>xxh</td><td></td><td>REV ID[2:0]</td><td></td><td></td><td>DEV ID[1:0]</td><td></td><td>NU_CH[1:0]</td></tr><tr><td>01h</td><td>CONFIG1</td><td>96h</td><td></td><td>DAISY_EN</td><td>CLK_EN</td><td></td><td>0</td><td>DR[2:0]</td><td></td></tr><tr><td>02h</td><td>CONFIG2</td><td>COh</td><td></td><td></td><td>0</td><td>INT_CAL</td><td>0 CAL_</td><td>AMP0</td><td>CAL_FREQ[1:0]</td></tr><tr><td>03h</td><td>CONFIG3</td><td>60h</td><td>PD_REFBUF</td><td></td><td>BIAS_</td><td>MEAS</td><td>BIASREF_INT</td><td>PD_BIAS BIAS_LOFF_ SENS</td><td>BIAS_STAT</td></tr><tr><td>04h</td><td>LOFF</td><td>00h</td><td></td><td>COMP_TH[2:0]</td><td></td><td></td><td>ILEAD_OFF[1:0]</td><td></td><td>FLEAD_OFF[1:0]</td></tr><tr><td colspan="10">Channel-Specific Settings</td></tr><tr><td>05h</td><td>CH1SET</td><td>61h</td><td>PD1</td><td>GAIN1[2:0]</td><td></td><td>SRB2</td><td></td><td>MUX1[2:0]</td><td></td></tr><tr><td>06h</td><td>CH2SET</td><td>61h</td><td>PD2</td><td></td><td>GAIN2[2:0]</td><td></td><td>SRB2</td><td>MUX2[2:0]</td><td></td></tr><tr><td>07h</td><td>CH3SET</td><td>61h</td><td>PD3</td><td></td><td>GAIN3[2:0]</td><td></td><td>SRB2</td><td>MUX3[2:0]</td><td></td></tr><tr><td>08h</td><td>CH4SET</td><td>61h</td><td>PD4</td><td></td><td>GAIN4[2:0]</td><td></td><td>SRB2</td><td>MUX4[2:0]</td><td></td></tr><tr><td>09h</td><td>CH5SET (1)</td><td>61h</td><td>PD5</td><td></td><td>GAIN5[2:0]</td><td></td><td>SRB2</td><td>MUX5[2:0]</td><td></td></tr><tr><td>0Ah</td><td>CH6SET (1)</td><td>61h</td><td>PD6</td><td></td><td>GAIN6[2:0]</td><td></td><td>SRB2</td><td>MUX6[2:0]</td><td></td></tr><tr><td>0Bh</td><td>CH7SET (2)</td><td>61h</td><td>PD7</td><td></td><td>GAIN7[2:0]</td><td>SRB2</td><td></td><td>MUX7[2:0]</td><td></td></tr><tr><td>0Ch</td><td>CH8SET (2)</td><td>61h</td><td>PD8</td><td>GAIN8[2:0]</td><td></td><td>SRB2</td><td></td><td>MUX8[2:0]</td><td></td></tr><tr><td>0Dh</td><td>BIAS_SENSP</td><td>00h</td><td>BIASP8(2)</td><td>BIASP7(2)</td><td>BIASP6(1) BIASP5(1)</td><td></td><td>BIASP4</td><td>BIASP3 BIASP2</td><td>BIASP1</td></tr><tr><td>0Eh</td><td>BIAS_SENSN</td><td>00h</td><td>BIASN8(2)</td><td>BIASN7(2)</td><td>BIASN6(1) BIASN5(1)</td><td></td><td>BIASN4</td><td>BIASN3 BIASN2</td><td>BIASN1</td></tr><tr><td>0Fh</td><td>LOFF_SENSP</td><td>00h</td><td>LOFFP8(2)</td><td>LOFFP7(2)</td><td>LOFFP6(1) LOFFP5(1)</td><td>LOFFP4</td><td>LOFFP3</td><td>LOFFP2</td><td>LOFFP1</td></tr><tr><td>10h</td><td>LOFF_SENSN</td><td>00h</td><td>LOFFM8(2)</td><td>LOFFM7(2)</td><td>LOFFM6(1) LOFFM5(1)</td><td>LOFFM4</td><td>LOFFM3</td><td>LOFFM2</td><td>LOFFM1</td></tr><tr><td>11h</td><td>LOFF_FLIP</td><td>00h</td><td>LOFF_FLIP8(2)</td><td>LOFF_FLIP7(2)</td><td>LOFF_FLIP6(1) LOFF_FLIP5(1)</td><td>LOFF_FLIP4</td><td>LOFF_FLIP3</td><td>LOFF_FLIP2</td><td>LOFF_ FLIP1</td></tr><tr><td colspan="10">Lead-Off Status Registers (Read-Only Registers)</td></tr><tr><td>12h</td><td>LOFF_STATP</td><td>00h</td><td>IN8P OFF</td><td>IN7P OFF</td><td>IN6P_ OFF IN5P_</td><td>OFF IN4P_OFF</td><td>IN3P_OFF</td><td>IN2P_OFF</td><td>IN1P_OFF</td></tr><tr><td>13h</td><td>LOFF_STATN</td><td>00h</td><td>IN8M OFF</td><td>IN7M_OFF</td><td>IN6M _OFF IN5M</td><td>OFF IN4M</td><td>OFF IN3M</td><td>OFF IN2M_ OFF</td><td>IN1M_ OFF</td></tr><tr><td colspan="2">GPIO and OTHER Registers</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>14h</td><td>GPIO</td><td>0Fh</td><td></td><td>GPIOD[4:1]</td><td></td><td></td><td></td><td>GPIOC[4:1]</td><td></td></tr><tr><td>15h</td><td>MISC1</td><td>00h</td><td></td><td></td><td>SRB1</td><td>0</td><td>0 0</td><td>0</td><td>0</td></tr><tr><td>16h</td><td>MISC2</td><td>00h</td><td>0</td><td>0</td><td>0</td><td>SINGLE</td><td>0</td><td>0 PD_LOFF</td><td>0</td></tr><tr><td>17h</td><td>CONFIG4</td><td>00h</td><td></td><td></td><td></td><td></td><td>SHOT</td><td>COMP</td><td>0</td></tr></table></body></html>

（12）寄存器或位只在ADS1299-6和ADS1299中可用。ADS1299-4的寄存器位设置为0h或00h。寄存器或位只能在ADS1299中可用。在ADS1299-4和ADS1299-6中寄存器位设置为0h或00h。

# 9.6.1用户注册描述

在设备制造期间对只读ID控制寄存器进行编程，以指示设备特性。

# 9.6.1.1 ID: ID控制寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 00h）（复位= xxh）

图50。ID控制寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>0</td></tr><tr><td></td><td>REV_ID[2:0]</td><td></td><td>1</td><td>DEV ID[1:0]</td><td>NU_CH[1:0]</td><td></td></tr><tr><td></td><td>R-xh</td><td></td><td>R-1h</td><td>R-3h</td><td></td><td>R-xh</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；- $\mathsf {n} =重置后的$值

表12。ID控制寄存器字段描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7:5 < / td > < td > REV_ID (2:0) < / td > < td > R < / td > < td > xh < / td > < td >保留。这些位表示设备的修订，如有更改，恕不另行通知。</td></tr><tr><td></td><td>保留</td><td>R</td><td>1h</td><td>保留。总是读1。< / td > < / tr > < tr > < td > 3:2 < / td > < td > DEV_ID (1:0) < / td > < td > R < / td > < td > 3 h < / td > < td >设备标识。Bits表示设备。11 ADS1299-x这些< / td > < / tr > < tr > < td > 1:0 < / td > < td > NU_CH (1:0) < / td > < td > R < / td > < td > xh < / td > < td >通道的数量。这些位表示通道的数量。10:8通道ADS1299</td></tr></table></body></html>

# 9.6.1.2 CONFIG1：配置寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 01h）（复位= 96h）

这个寄存器配置DAISY_EN位、时钟和数据速率。

图51。CONFIG1：配置寄存器

<html><body><table><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>1</td><td>DAISY_EN</td><td>CLK_EN</td><td>1</td><td>0</td><td></td><td>DR[2:0]</td><td></td></tr><tr><td>R/W-1h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-1h</td><td>R/W-0h</td><td></td><td>R/W-6h</td><td></td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表13。配置寄存器字段说明

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>复位描述</td><td></td></tr><tr><td>7</td><td>保留</td><td>R/W</td><td> h</td><td>保留写1h Always</td></tr><tr><td>6</td><td>DAISY EN</td><td>R/W</td><td>Oh</td><td> DAISY -chain或multiple read - back mode决定启用哪种模式。0：雏花链模式多读回模式</td></tr><tr><td></td><td>CLK_EN</td><td>R/W</td><td>1 Oh CLK</td><td>该位决定当CLKSEL引脚= 1时，内部振荡器信号是否连接到CLK引脚0：振荡器时钟输出禁用连接(1)</td></tr><tr><td>4:3</td><td>保留</td><td>R/W</td><td>1 2h保留</td><td>：振荡器时钟输出使能总是写2h</td></tr><tr><td>2:0</td><td>DR[2:0]</td><td>R/W</td><td>6h输出数据这些位决定fcLK / 2。000:fMOD 001 010 011: tMOD:fMOD fMOD</td><td>rate设备输出数据速率。fmoD = 64 (16 kSPS) 128 (8 kSPS) 256 (4 kSPS) 512 (2 kSPS) 100 fmoD 1024(1 kSPS) 101 2048 (500 SPS) 110 4096 (250 SPS) fmoD </td></tr></table></body></html>

(1)驱动外部设备时消耗额外的功率。

# 9.6.1.3 CONFIG2：配置寄存器2 (address $\mathbf {\sigma} = \mathbf {\sigma}$ 02h) （reset = C0h）

这个寄存器配置测试信号的生成。有关更多细节，请参阅输入多路复用器部分。

图52。CONFIG2：配置寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td></td><td>1</td><td>0</td><td>INT_CAL</td><td>0</td><td>CAL AMP</td><td>CAL</td><td>FREQ[1:0]</td></tr><tr><td>R/W-1h</td><td>R/W-1h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td></td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表14。配置寄存器2字段说明

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7:5 < / td > < td >保留< / td > < td > R / W < / td > < td > 6 h < / td > < td >保留总是写6 h < / td > < / tr > < tr > < td > < / td > < td > INT_CAL < / td > < td > R / W < / td > < td >哦< / td > < td >测试源这一点决定了源的测试信号。0：外部驱动测试信号</td></tr><tr><td>3</td><td>保留</td><td>R/W</td><td>Oh</td><td>：内部生成测试信号保留写入0h Always</td></tr><tr><td></td><td>CAL_AMP</td><td>R/W</td><td>Oh</td><td>测试信号振幅这些位决定校准信号的振幅。0:1 ×−(VREFP−VREFN) 2400 1:2 ×−(VREFP -VREFN) 2400</td></tr><tr><td>1:0</td><td>CAL_ FREQ[1:0]</td><td>R/W</td><td>Oh</td><td>测试信号频率这些位决定校准信号频率。00在fcl01脉冲在fcLk 10不使用11:Atdc 221 220</td></tr></table></body></html>

# 9.6.1.4 CONFIG3：配置寄存器3 (address $\mathbf {\sigma} = \mathbf {\sigma}$ 03h) （reset = 60h）

配置寄存器3配置内部或外部引用和BIAS操作。

图53。CONFIG3：配置寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>PD_REFBUF</td><td></td><td></td><td>BIAS_MEAS</td><td>BIASREF INT</td><td>PD_BIAS</td><td>BIAS_LOFF SENS</td><td>BIAS STAT</td></tr><tr><td>R/W-0h</td><td>R/W-1h</td><td>R/W-1h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；$\mathsf {R} =$只读；$- \mathsf {n} =重置后的$值

表15。配置寄存器3字段说明

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>复位</td><td>描述</td></tr><tr><td>Z</td><td>PD_REFBUF</td><td>R/W</td><td>Oh 0 1</td><td>下电参考缓冲区该位决定下电参考缓冲区状态。：下电内部参考缓冲区：使能内部参考缓冲区</td></tr><tr><td>6:5</td><td>Reserved</td><td>R/W</td><td>3h</td><td>预留写3h。Always</td></tr><tr><td></td><td>BIAS_MEAS</td><td>R/W</td><td>Oh 0</td><td>BIAS测量该位使能BIAS测量。偏置偏置信号可以用任何通道测量。Open 1 BIAS IN信号被路由到具有</td></tr><tr><td>3</td><td>BIASREF_INT</td><td>R/W</td><td>Oh 0</td><td>MUX Setting 010(VREF) BIASREF信号该位决定了BIASREF信号源。外部馈送的BIASREF信号1 BIASREF信号（AVDD + AVSS） 2内部产生的BIASREF信号</td></tr><tr><td>2</td><td>PD_BIAS</td><td>R/W</td><td>Oh 0 1</td><td>BIAS缓冲功率决定BIAS缓冲功率状态。BIAS buffer掉电BIAS buffer使能</td></tr><tr><td></td><td>BIAS_LOFF SENS</td><td>R/W</td><td>Oh 0 1</td><td>BIAS感测功能使能BIAS感测功能。BIAS检测值disabled: BlAS检测值使能</td></tr><tr><td></td><td>BIAS_STAT</td><td></td><td>Oh 0</td><td>BIAS导断状态决定BlAS状态。1：表示未连接</td></tr></table></body></html>

# 9.6.1.5 LOFF：引断控制寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 04h）（复位= 00h）

引线控制寄存器配置引线检测操作。

图54。LOFF：引线控制寄存器

<html><body><table><tr><td></td><td>6</td><td></td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td></td><td>COMP TH2[2:0]</td><td></td><td>0</td><td>ILEAD</td><td>OFF[1:0]</td><td>FLEAD</td><td>OFF[1:0]</td></tr><tr><td colspan="3">R/W-0h</td><td>R/W-0h</td><td colspan="2">R/W-0h</td><td>R/W-0h</td><td></td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表16所示。引线控制寄存器字段说明

表身体< html > < > < > < tr > < td >一点7:5 < / td > < td >字段类型COMP_TH [2:0] R / W < / td > < td >重置哦< / td > < td >描述< / td > < / tr > < tr > < td > < / td > < td > < / td > < td > < / td > < td >脱铅比较器阈值比较器积极的一面000 010 001 92.5% 95%:90% 011:100:87.5% 85% 101:80% 110:75% 111:70%比较负面000 5% 001:010:7.5% 10% 011:100 110 101 20% 15% 25% 12.5% < / td > < / tr > < tr > < td > 3:2 < / td > < td >保留R / W ILEAD_OFF 1:0 [] R / W < / td > < td >哦0 h < / td > < td > 111: 30%保留总是写0 h。引线电流大小这些位决定了电流</td></tr><tr><td>1:0</td><td>FLEAD_OFF[1:0] R/W</td><td>0h</td><td>引线模式的电流大小。00 6nA 01 24 nA 10:6 A 11 24A引线频率</td></tr><tr><td></td><td></td><td></td><td>这些位决定每个通道引线检测的频率。00直流导通检测01交流7.8 Hz导通检测10交流31.2 Hz导通检测（fcLk / 216） 11交流R / 4导通检测(fcLk /218</td></tr></table></body></html>

CHnSET：单个通道设置（cd:） $(n = 1$到8)（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 05h到0Ch） （reset = 6h）

CH[1:8]SET控制寄存器配置功率模式、PGA增益和多路复用器设置通道。有关详细信息，请参阅输入多路复用器部分。CH[2:8]SET与CH1SET相似，对应各自的通道。

图55。CHnSET：单个通道设置寄存器

<html><body><table><tr><td></td><td></td><td>5</td><td></td><td>3</td><td>2</td><td></td><td></td></tr><tr><td>PDn</td><td></td><td>GAINn[2:0]</td><td></td><td>SRB2</td><td></td><td>MUXn[2:0]</td><td></td></tr><tr><td>R/W-0h</td><td></td><td>R/W-6h</td><td></td><td>R/W-0h</td><td></td><td>R/W-0h</td><td></td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表17。单个通道设置（cd:） $\bf \Pi  \cdot \delta _ { n } = 1$到8)字段说明

<html><body><table><tr><td>Bit</td><td>字段</td><td>类型复位</td><td>描述</td></tr><tr><td>7</td><td>PDn</td><td>Oh</td><td> 电源 down该位决定了相应通道的通道功率模式。0：正常。1通道下电。当关闭通道电源时，TI建议通过设置相应的hnset寄存器的MUXn[2:0] = 001将通道设置为输入短。</td></tr><tr><td>6:4</td><td>增益[2:0]R/W</td><td>6h</td><td>PGA增益这些位决定PGA增益设置。000:1 001:2 010:4 011:6 100:8 101:12 110:24</td></tr><tr><td></td><td>SRB2</td><td>R/W Oh</td><td>111：不使用SRB2连接该位决定了相应通道的SRB2连接。0打开1关闭</td></tr><tr><td>2:0</td><td>MUXn[2:0] R/W</td><td>1h</td><td>通道输入这些位决定通道输入的选择。000：正常电极输入001：输入短路（用于偏移或噪声测量）010：与BIAS_MEAS位一起使用，用于BIAS测量。011:MVDD供电测量100：温度传感器101测试信号110 BIAS _DRP（正极为驱动器）111:BIAS DRN（负极为驱动器）</td></tr></table></body></html>

# 9.6.1.7 BIAS SENSP: BIAS驱动正导数寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 0Dh）（复位= 00h）

该寄存器控制来自每个通道的正信号的选择，用于偏置电压（bias）衍生。详细信息请参见偏置驱动（直流偏置电路）部分。

ADS1299-4寄存器位[5:4]不可用。寄存器位[7:6]不能用于ADS1299-4或ADS1299-6。当写入寄存器时，将相关设备的不可用位设置为0。

图56。偏置感测器：偏置正信号派生寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>BIASP8</td><td>BIASP7</td><td>BIASP6</td><td>BIASP5</td><td>BIASP4</td><td>BIASP3</td><td>BIASP2</td><td>BIASP1</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表18。BIAS正信号衍生场描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7 < / td > < td > BIASP8 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN8P偏见路线通道8积极信号到偏见推导0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > BIASP7 < / td > < td > R / W < / td > < td >哦< / td > < td > IN7P偏见路线频道7积极信号到偏见推导0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > BIASP6 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN6P偏见路线六频道正的信号偏差推导0禁用启用< / td > < / tr > < tr > < td > < / td > < td > BIASP5 < / td > < td > R / W < / td > < td >哦< / td > < td > IN5P偏见路线第5频道积极信号到偏见推导0禁用1启用< / td > < / tr > < tr > < td > 3 < / td > < td > BIASP4 < / td > < td > R / W < / td > < td >哦< / td > < td > IN4P偏见路线4频道积极信号到偏见推导0禁用1启用< / td > < / tr > < tr > < td > 2 < / td > < td > BIASP3 < / td > < td > R / W < / td > < td >哦< / td > < td > IN3P偏见路线通道3积极信号偏差推导0禁用1启用</td></tr><tr><td></td><td>BIASP2</td><td>R/W</td><td>Oh</td><td>IN2P到BIAS路由通道2正信号进入BIAS通道0禁用1</td></tr><tr><td></td><td>BIASP1</td><td>R/W</td><td>Oh</td><td>启用IN1P到BIAS路由通道1正信号进入BIAS通道0禁用1启用</td></tr></table></body></html>

# 9.6.1.8 BIAS_SENSN：偏置驱动负导数寄存器（address $\mathbf {\sigma} = \mathbf {\sigma}$ 0Eh） （reset = 00h）

该寄存器控制来自每个通道的负信号的选择，用于偏置电压（bias）衍生。详细信息请参见偏置驱动（直流偏置电路）部分。

ADS1299-4寄存器位[5:4]不可用。寄存器位[7:6]不能用于ADS1299-4或ADS1299-6。当写入寄存器时，将相关设备的不可用位设置为0。

图57。偏置感测器：偏置负信号派生寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>BIASN8</td><td>BIASN7</td><td>BIASN6</td><td>BIASN5</td><td>BIASN4</td><td>BIASN3</td><td>BIASN2</td><td>BIASN1</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表19。负信号派生域描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7 < / td > < td > BIASN8 < / td > < td > R / W < / td > < td >哦< / td > < td > IN8N偏差路线通道负面信号偏差推导0禁用启用1 < / td > < / tr > < tr > < td > < / td > < td > BIASN7 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN7N偏见路线频道7 -信号偏差推导0禁用1启用< / td > < / tr > < tr > < td > 5 < / td > < td > BIASN6 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN6N偏见路线通道负面信号进入BIAS派生0 Disabled 1 Enabled 6</td></tr><tr><td>3</td><td>BIASN5 </td><td>R/W</td><td>0h</td><td>IN5N到BIAS路由通道5负信号进入BIAS派生0 Disabled 1 Enabled</td></tr><tr><td>2</td><td>BIASN3</td><td>R/W</td><td>0h</td><td>IN4N到BIAS路由通道负信号进入BIAS派生0：启用禁用1 < / td > < / tr > < tr > < td > < / td > < td > BIASN2 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN3N偏见路线通道3 -信号偏差推导0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN2N偏见路线频道2 -信号偏差推导0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > BIASN1 < / td > < td > R / W < / td > < td > 0 h < / td > < td > IN1N偏差路线通道负面信号偏差推导0禁用启用1 < / td > < / tr > < /表> < /身体> < / html >

# 9.6.1.9 LOFF_SENSP：正信号引线检测寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 0Fh）（复位= 00h）

该寄存器从每个通道中选择正极进行引线检测。有关详细信息，请参阅引线检测部分。LOFF_STATP寄存器位只有在相应的LOFF SENSP位设置为1时才有效。

ADS1299-4寄存器位[5:4]不可用。寄存器位[7:6]不能用于ADS1299- 4或ADS1299-6。当写入寄存器时，将相关设备的不可用位设置为0。

图58。LOFF _SENSP：正信号引线检测寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>LOFFP8</td><td>LOFFP7</td><td>LOFFP6</td><td>LOFFP5</td><td>LOFFP4</td><td>LOFFP3</td><td>LOFFP2</td><td>LOFFP1</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表20。正信号引线检测领域描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > < / td > < td > LOFFP8 < / td > < td > R / W < / td > < td >哦< / td > < td > IN8P开始启用脱铅检测IN8P 0禁用1:启用< / td > < / tr > < tr > < td > 6 < / td > < td > LOFFP7 < / td > < td > R / W < / td > < td >哦< / td > < td > IN7P开始上启用脱铅检测IN7P 0:禁用启用1 < / td > < / tr > < tr > < td > < / td > < td > LOFFP6 < / td > < td > R / W < / td > < td >哦< / td > < td > IN6P开始启用脱铅检测IN6P 0禁用1:Enabled</td></tr><tr><td></td><td>LOFFP5</td><td>R/W</td><td>Oh</td><td>IN5P引线使能in5p0引线检测Disabled 1使能</td></tr><tr><td>3</td><td>LOFFP4</td><td>R/W</td><td>Oh</td><td>IN4P引线使能in4p0引线检测：Disabled 1使能</td></tr><tr><td>2</td><td> Oh</td><td>IN3P引线使能IN3P 0引线检测：Disabled 1 Enabled</td></tr><tr><td></td><td>LOFFP2</td><td>R/W</td><td>Oh</td><td>IN2P导断使能IN2P导断检测0:Disabled 1 Enabled</td></tr><tr><td></td><td>LOFFP1</td><td>R/W</td><td>Oh</td><td> IN2P导断使能IN2P导断检测0:Disabled 1 Enabled</td></tr></table></body></html>

# 9.6.1.10 loff_senn：负信号引线检测寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 10h）（复位= 00h）

该寄存器从每个通道中选择负极进行引线检测。有关详细信息，请参阅引线检测部分。LOFF_STATN寄存器位只有在相应的LOFF senn位设置为1时才有效。

ADS1299-4寄存器位[5:4]不可用。寄存器位[7:6]不能用于ADS1299- 4或ADS1299-6。当写入寄存器时，将相关设备的不可用位设置为0。

图59。LOFF _SENSN：负信号引线检测寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>LOFFM8</td><td>LOFFM7</td><td>LOFFM6</td><td>LOFFM5</td><td>LOFFM4</td><td>LOFFM3</td><td>LOFFM2</td><td>LOFFM1</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表21负信号引线检测领域描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7 < / td > < td > LOFFM8 < / td > < td > R / W < / td > < td >哦< / td > < td > IN8N开始上启用脱铅检测IN8N 0:启用禁用1 < / td > < / tr > < tr > < td > 6 < / td > < td > LOFFM7 < / td > < td > R / W < / td > < td >哦< / td > < td > IN7N开始上启用脱铅检测IN7N 0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > LOFFM6 < / td > < td > R / W < / td > < td >哦< / td > < td > IN6N开始上启用脱铅检测IN6N 0禁用1启用< / td > < / tr > < tr > < td > < / td > < td > LOFFM5 < / td > < td > R / W < / td > < td >哦< / td > < td > IN5N开始上启用脱铅检测IN5N 0禁用1启用< / td > < / tr > < tr > < td > 3 < / td > < td > LOFFM4 < / td > < td > R / W < / td > < td >哦< / td > < td > IN4N铅使能IN2N引线检测：关闭1：使能0</td></tr><tr><td>2</td><td>LOFFM3</td><td>R/W</td><td>0h</td><td>IN3N引线使能IN3N引线检测0：关闭1：使能</td></tr><tr><td></td><td>LOFFM2</td><td>R/W</td><td>0h</td><td>IN2N引线使能IN2N引线检测0：关闭1：使能IN2N引线检测0：关闭1：使能IN2N引线检测0：关闭1：使能IN2N引线检测0：关闭Enabled</td></tr><tr><td></td><td>LOFFM1</td><td>R/W</td><td>0h</td><td>IN1N导通IN1N导通检测0:Disabled Enabled 1</td></tr></table></body></html>

# 9.6.1.11 LOFF_FLIP：引线翻转寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 11h）（复位= 00h）

这个寄存器控制用于引线引出的电流的方向。有关详细信息，请参阅引线检测部分。

图60。LOFF _FLIP：引线触发寄存器

<html><body><table><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>LOFF_FLIP8</td><td>LOFF_FLIP7</td><td>LOFF_FLIP6</td><td>LOFF_FLIP5</td><td>LOFF_FLIP4</td><td>LOFF_FLIP3</td><td>LOFF_FLIP2</td><td>LOFF_FLIP1</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表22。引线翻转寄存器字段描述

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>复位</td><td>描述</td></tr><tr><td>Z</td><td>LOFF_FLIP8</td><td>R/W</td><td>Oh</td><td> 8通道LOFF极性翻转8通道电流源的上拉或下拉极性进行引线检测。0：无翻转=IN8P被拉到AVDD， IN8N被拉到AVSS 1：翻转=IN8P被拉到AVSS， IN8N'被拉到AVDD</td></tr><tr><td>6</td><td>LOFF_FLIP7</td><td>R/W</td><td>Oh</td><td>通道7 LOFF极性翻转通道7上电流源的上拉或下拉极性进行引线检测。0：无fip = IN7P被拉到AVDD， IN7N被拉到AVSS 1：翻转= IN7P被拉到AVSS， IN7N'被拉到AVDD</td></tr><tr><td>5</td><td>LOFF_FLIP6</td><td>R/W</td><td>Oh</td><td>通道6 LOFF极性翻转通道6上电流源的上拉或下拉极性进行引线检测。0：无翻转= IN6P被拉到AVDD， IN6N被拉到AVSS 1：翻转= IN6P被拉到AVSS， IN6N'被拉到AVDD</td></tr><tr><td></td><td>LOFF_FLIP5</td><td>R/W</td><td>Oh</td><td>通道5 LOFF极性翻转通道5上电流源的上拉或下拉极性进行引线检测。0：无翻转= IN5P被拉到AVDD， IN5N被拉到AVSS 1：翻转= IN5P被拉到AVSS， IN5N'被拉到AVDD</td></tr><tr><td>3</td><td>LOFF_FLIP4</td><td>R/W</td><td>Oh</td><td>通道4 LOFF极性翻转通道4上电流源的上拉或下拉极性进行引线检测。0:No fip = IN4P被拉到AVDD， IN4N被拉到AVSS 1: Flipped = IN4P被拉到AVSS， IN4N'被拉到AVDD</td></tr><tr><td>2</td><td>LOFF_FLIP3</td><td>R/W</td><td>Oh</td><td>通道3 LOFF极性翻转通道3上电流源的上拉或下拉极性进行引线检测。0：无翻转= IN3P被拉到AVDD， IN3N被拉到AVSS 1：翻转= IN3P被拉到AVSS， IN3N'被拉到AVDD</td></tr><tr><td></td><td>LOFF_FLIP2</td><td>R/W</td><td>Oh</td><td>通道2 LOFF极性翻转通道2上电流源的上拉或下拉极性，用于引线检测。0：不翻转= IN2P被拉到AVDD， IN2N被拉到AVSS 1：翻转= IN2P被拉到AVSS， IN2N被拉到AVDD </td></tr><tr><td></td><td>LOFF_FLIP1</td><td>R/W</td><td>Oh</td><td>通道1 LOFF极性翻转通道1上电流源的上拉或下拉极性，用于引线检测。0: No flip = IN1P被拉到AVDD， IN1N被拉到AVSS 1: Flipped = IN1P被拉到AVSS， IN1N'被拉到AVDD</td></tr></table></body></html>

# 9.6.1.12 LOFF_STATP：引开正信号状态寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 12h）（复位= 00h）

这个寄存器存储每个通道上的正极是开还是关的状态。有关详细信息，请参阅引线检测部分。如果对应的LOFF _SENSP位不为1，则忽略LOFF STATP值。

当LOFF SENSEP位为0时，应该忽略LOFF _STATP位。

图61。LOFF_STATP：引开正信号状态寄存器（只读）

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>IN8P OFF</td><td>IN7P OFF</td><td>IN6P OFF</td><td>IN5P OFF</td><td>IN4P OFF</td><td>IN3P OFF</td><td>IN2P OFF</td><td>IN1P_OFF</td></tr><tr><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；- $\mathsf {n} =重置后的$值

表23。引线正信号状态字段描述

<html><body><table><tr><td>位</td><td>字段</td><td> <td> <td>Reset</td><td>描述</td></ td>< tr><td>7</td><td>IN8P_OFF</td><td>R</td><td>Oh 0</td>< tr><td>6</td><td>IN7P_OFF</td><td>R</td><td>Oh</td><td>通道正通道导断状态IN7P电极导断状态0：电极闭合1电极闭合7</td></tr><tr><td></td><td>IN6P_OFF</td><td>R</td><td>Oh</td><td>通道6正通道引出状态IN6P电极闭合状态0：电极闭合1：电极闭合</td></tr><tr><td></td><td>IN5P_OFF</td><td>R</td><td>Oh 1</td><td>通道5正通道引出状态IN5P电极闭合状态0：电极闭合：电极关断</td></tr><tr><td>3</td><td>IN4P_OFF</td><td></td><td>Oh</td><td>通道4正通道引出状态IN4P电极开断状态0：电极开1：电极关断</td></tr><tr><td>2</td><td>IN3P_ _ _OFF</td><td>R</td><td>Oh</td><td>通道3正通道引出状态IN3P电极开断状态0：电极闭合1电极闭合</td></tr><tr><td></td>< /td><td>IN2P_ off</td>< td></td><td>Oh 0</td><td>通道2正通道导通状态IN2P电极闭合状态：电极闭合：电极闭合</td></tr><tr><td></td><td>IN1P_OFF</td><td>R</td><td>Oh 0</td><td>1是通道1正通道导通状态：电极闭合：电极闭合</td></tr></table></body></html>

# 9.6.1.13 LOFF_STATN：引开负信号状态寄存器（地址$\mathbf {\sigma} = \mathbf {\sigma}$ 13h）（复位= 00h）

这个寄存器存储每个通道上的负极是开还是关的状态。有关详细信息，请参阅引线检测部分。如果相应的LOFF_SENSN位不为1，则忽略LOFF_STATN值。

当LOFF _SENSEN位为0时，应该忽略LOFF _STATP位。

图62。LOFF _STATN：引开负信号状态寄存器（只读）

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>IN8N OFF</td><td>IN7N_OFF</td><td>IN6N_OFF</td><td>IN5N_OFF</td><td>IN4N_OFF</td><td>IN3N_OFF</td><td>IN2N_OFF</td><td>IN1N_OFF</td></tr><tr><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td><td>R-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；- $\mathsf {n} =重置后的$值

表24。引线负信号状态字段说明

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7 < / td > < td > IN8N_OFF < / td > < td > R < / td > < td >哦0 < / td > < td >通道8 -通道脱铅地位IN8N电极是否打开或关闭的状态:电极是1:电极从< / td > < / tr > < tr > < td > 6 < / td > < td > IN7N_OFF < / td > < td > R < / td > < td >哦< / td > < td >通道7 -通道脱铅状态是否IN7N电极或0的状态:电极电极1日离开了< / td > < / tr > < tr > < td > < / td > < td > IN6N_OFF < / td > < td > R < / td > < td >哦< / td > < td >通道6 -通道脱铅地位地位IN6N电极是否打开或关闭0:1:上电极电极是关闭的是< / td > < / tr > < tr > < td > < / td > < td > IN5N_OFF < / td > < td > R < / td > < td >哦< / td > < td >通道5 -通道脱铅地位IN5N电极是否打开或关闭状态0:电极是1:电极关断</td></tr><tr><td>3</td><td>IN4N_OFF</td><td></td><td>Oh</td><td>通道4负通道引出状态IN4N电极开断状态0：电极开1：电极关断</td></tr><tr><td>2</td><td>IN3N_OFF</td><td>R</td><td>Oh</td><td>通道3负通道引出状态IN3N电极开断状态0：电极闭合电极闭合</td></tr><tr><td></td>< /td> IN2N_OFF</td><td></td><td>Oh</td><td>1通道2负通道引出状态IN2N电极闭合状态0电极闭合：电极s</td></tr><tr><td></td><td>IN1N_OFF</td><td>R</td><td>Oh 0</td><td>1断开通道1负通道引出状态IN1N电极闭合状态：电极闭合1：电极闭合</td></tr></table></body></html>

GPIO：通用I/O寄存器（address $\mathbf {\sigma} = \mathbf {\sigma}$ 14h） （reset = 0Fh）

通用I/O寄存器控制三个GPIO引脚的动作。当RESP CTRL[1:0]处于模式01和11时，GPIO2， GPIO3和GPIO4引脚不可用。

图63。GPIO：通用I/O寄存器

<html><body><table><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td colspan="5">GPIOD[4:1]</td><td>GPIOC[4:1]</td><td></td></tr><tr><td colspan="5">R/W-0h</td><td colspan="2">R/W-Fh</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

#表25通用I/O字段描述

<html><body><table><tr><td>Bit</td><td>Field</td><td>Type Reset</td><td>Description</td></tr><tr><td>7:4</td><td>GPIOD[4:1] R/W</td><td>Oh</td><td>GPIO数据用于向GPIO端口读写数据。当读取寄存器时，返回的数据对应于GPIO外部引脚的状态，无论它们是编程GPIOD设置</td></tr><tr><td>3:0</td><td>GPIOC[4:1]</td><td>R/W Fh</td><td>作为输入还是作为输出。作为输出，写入输出值。作为输入，对GPIOD的写没有影响。GPIO在某些呼吸模式下不可用。GPIO控制（相应的GPIOD）这些位决定相应的GPIOD引脚是输入还是输出。0:输出1:输入< / td > < / tr > < /表> < /身体> < / html >

# 9.6.1.15 MISC1: Miscellaneous 1 Register (address $\mathbf {\sigma} = \mathbf {\sigma}$ 15h) （reset = 00h）

该寄存器提供控制，将SRB1引脚路由到四个、六个或八个通道（ADS1299-4、ADS1299-6或ADS1299）的所有反相输入。

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>0</td><td>0</td><td>SRB1</td><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；- $\mathsf {n} =重置后的$值

表26。1注册域描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 6 < / td > < td >保留< / td > < td > R / W < / td > < td >哦< / td > < td >保留总是写0 h < / td > < / tr > < tr > < td > < / td > < td > SRB1 < / td > < td > R / W < / td > < td >哦0:< / td > < td >刺激,参考,和偏见第一位连接SRB1 4, 6,开关打开</td></tr><tr><td>4:0</td><td>预留</td><td>R/W</td><td>Oh</td><td>1开关关闭总是写0h预留</td></tr></table></body></html>

$ $\mathbf {\sigma} = \mathbf {\sigma}$ 16h （reset $\mathbf {\sigma} = \mathbf {\sigma}$ 00h）

这个寄存器被保留以备将来使用。

图64。MISC1：杂项寄存器
图65。MISC1：杂项寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；${\sf R} =$只读；$- \mathsf {n} =重置后的$值

表27。1注册域描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td > 7:0 < / td > < td >保留< / td > < td > R / W < / td > < td >哦< / td > < td >保留总是写0 h < / td > < / tr > < /表> < /身体> < / html >

配置寄存器4 (address $\mathbf {\sigma} = \mathbf {\sigma}$ 17h) （reset = 00h）

该寄存器配置转换模式并启用引线比较器。

图66。CONFIG4：配置寄存器

<html><body><table><tr><td></td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>0</td><td>0</td><td>0</td><td>0</td><td>SINGLE_SHOT</td><td>0</td><td>PD LOFF COMP</td><td>0</td></tr><tr><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td><td>R/W-0h</td></tr></table></body></html>

LEGEND: R/W $\mathbf {\sigma} = \mathbf {\sigma}$ Read/Write；$\mathsf {R} =$只读；$- \mathsf {n} =重置后的$值

表28。配置寄存器4字段说明

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >重置< / td > < td >描述< / td > < / tr > < tr > < td >七道明> < /保留< td > < / td > < td > R / W < / td > < td >哦< / td > < td >保留总是写0 h < / td > < / tr > < tr > < td > 3 < / td > < td > SINGLE_SHOT < / td > < td > R / W < / td > < td >哦0 < / td > < td >单发转换这个位设置转换模式。连续转换模式1</td></tr><tr><td>2</td><td>保留</td><td>R/W</td><td>Oh</td><td>单发模式保留总是写0h</td></tr><tr><td></td><td>PD_LOFF_COMP</td><td>R/W</td><td>Oh 0</td><td>引路比较器掉电该位掉电引路比较器。导通比较器禁用</td></tr><tr><td></td><td>保留</td><td>R/W</td><td>Oh</td><td>1：导通比较器启用保留总是写入0h</td></tr></table></body></html>

# 10应用程序和实现

# NOTE

以下应用章节中的信息不是TI组件规范的一部分，TI不保证其准确性或完整性。TI的客户负责确定组件的适用性。客户应验证和测试其设计实现，以确认系统功能。

# 10.1申请信息

# 10.1.1未使用的输入和输出

关闭未使用的模拟输入并将其直接连接到AVDD。

如果未使用，请关闭偏置放大器，并浮动Bias out和Bias inv。如果未使用，BIASIN也可以浮动或直接绑定到AVSS。

将BIASREF直接连接到AVSS，如果未使用则保持浮动。

将SRB1和SRB2直接连接到AVSS，如果未使用，则将其浮动。

不要浮动未使用的数字输入，因为可能导致电源泄漏电流过大。通过≥10-kΩ电阻将两个状态模式设置引脚高置为DVDD或低置为DGND。

如果不使用，使用弱上拉电阻将DRDY拉到电源。

如果没有菊花链装置，将菊花链直接连接到DGND。

# 10.1.2设置基本数据抓取设备

图67概述了在基本状态下配置设备并捕获数据的过程。此过程将设备放入与规格部分中列出的参数相匹配的配置中，以检查设备是否在用户系统中正常工作。遵循这个过程，直到熟悉设备设置。验证完成后，可以根据需要对设备进行配置。有关命令计时的详细信息，请参见数据表中的相应部分。添加了ECG和eeg特定功能的示例编程代码。

#申请信息（续）

![](images/b2e7f0ff16b2971834f3c63d9af4d5ccaaca197b5c0af7d33d816d90d4bed8cd.jpg)  
图67。启动时的初始流程

#申请信息（续）

# 10.1.2.1引线

示例代码设置直流引线与上拉和下拉电阻在所有通道。

//比较器阈值为$ 9.5 \%$和$5 \%$，上拉或下拉电阻
//直流导通
//打开直流导通比较器
//打开所有通道的p侧进行引线检测
//打开所有通道的n侧进行引线感应

WREG CONFIG4 $0 \mathbf { x } 0 2$ WREG LOFF_SENSP 0xFF WREG LOFF_SENSN 0xFF

观察输出数据流的状态位来监控引线状态。

# 10.1.2.2偏置驱动

示例代码选择偏差作为前三个通道的平均值。

WREG RLD SENSN 0x07 // RLD感测选择1-3通道p侧WREG RLD SENSN 0x07 // RLD感测选择1-3通道n侧WREG CONFIG3 b 'x1xx 1100 //打开BIAS放大器，设置内部BIASREF电压

示例代码路由的偏置信号通过通道4 n侧和测量偏置与通道5。确保芯片BIASOUT的外部连接到BIASIN。

WREG CONFIG3 b 'xxx1 1100 //打开BIAS放大器，设置内部BIASREF电压，设置BIAS测量位WREG CH4SET b 'xxxx 0111 //将BIASIN路由到4通道n侧WREG CH5SET b 'xxxx 0010 //将BIASIN路由到5通道w.r.t BIASREF

# 10.1.3建立输入共模

ADS1299-x测量全差分信号，其中共模电压点是正模拟输入和负模拟输入的中点。由于操作所需的净空，内部PGA限制了共模输入范围。人体很容易发生共模漂移，因为噪声很容易耦合到人体上，就像天线一样。这些共模漂移可能使ADS1299-x输入共模电压超出ADC的可测量范围。

如果系统使用患者驱动电极，则ADS1299-x包括连接到患者驱动电极的片上偏置驱动（bias）放大器。偏置放大器的作用是使病人偏置，使其他电极的共模电压保持在有效范围内。当接通电源时，放大器使用模拟中电源电压或BIASREF引脚处存在的电压作为参考输入，将患者驱动到该电压。

ADS1299- $\cdot \mathsf {x}$提供了使用输入电极电压作为放大器反馈的选项，通过在BIAS SENSP和BIAS SENSN寄存器中设置相应的位，可以更有效地稳定输出到放大器参考电压。图68显示了一个利用这种技术的三电极系统的示例。

![](images/999f814aa49632b19bb1550990c20ce6c65658ff9fe14996713d9092eb7008b1.jpg)  
图68。使用BIAS电极设置共模

#申请信息（续）

# 10.1.4多设备配置

ADS1299-x设计用于在一个系统中使用多个设备时提供灵活的配置。串行接口通常需要4个信号：DIN、DOUT、SCLK和CS。每个设备有一个额外的芯片选择信号，多个设备可以连接在一起。连接n个设备所需的信号数为3 + n。

BIAS驱动放大器可以采用菊花链，如多器件的BIAS配置部分所述。要在菊花链配置中使用内部振荡器，必须将一个设备设置为时钟源的主设备，使内部振荡器启用（CLKSEL引脚$= 1$），并通过将$\overline {{\mathsf {CLK} \mathsf {E N}}}$寄存器位设置为‘1’，将内部振荡器时钟带出设备。此主设备时钟作为其他设备的外部时钟源。

当使用多台设备时，可以使用START信号同步设备。对于给定的数据速率，从START到DRDY信号的延迟是固定的（请参阅SPI接口部分的START小节，了解有关稳定时间的更多详细信息）。图69显示了两个设备在与START信号同步时的行为。

有两种方法可以用最佳的接口引脚数连接多个设备：级联模式和菊花链模式。

![](images/de4395ba20e5ebc6a78c5fdefa3d7bb587e1ab3182fe0b17192c0ecab6677338.jpg)  
图69。同步多个变换器

# 10.1.4.1级联模式

图70a说明了两个设备级联在一起的配置。这些设备一起创建了一个有16个通道的系统。DOUT、SCLK和DIN是共享的。每个设备都有自己的芯片选择。当一个器件没有被相应的CS驱动到逻辑1时，该器件的DOUT是高阻抗的。这种结构允许其他设备控制DOUT总线。这种配置方法适用于大多数应用程序。

#申请信息（续）

# 10.1.4.2雏菊链模式

通过在CONFIG1寄存器中设置DAISY EN位来启用DAISY -chain模式。图70b显示了菊花链的配置。在这种模式下，SCLK、DIN和$\overline {{\mathsf {C S}}}$在多个设备上共享。第二个设备的DOUT连接到第一个设备的DAISY_IN，从而创建一个链。当使用菊花链模式时，多重回读功能不可用。如果不使用，将DAISY_IN引脚短接到数字地。图2描述了图70的配置中所示的设备所需的时序。来自设备1的状态和数据首先出现在DOUT上，其次是来自设备2的状态和数据。ADS1299可以与第二个ADS1299、ADS1299-6或ADS1299-4进行菊花链连接。

![](images/03f1f82dc317136fceea4980961cb158468ab45ae60f92ef22c9fac76f3709da.jpg)  
图70。多设备配置

(1)为了减少引脚数，将START引脚设置为低，并使用START串行命令来同步和启动转换。

当链中的所有设备在相同的寄存器设置中运行时，DIN也可以共享。这种配置将SPI通信信号减少到4个，而不考虑设备的数量。BIAS驱动程序不能在多个设备之间共享，必须使用外部时钟，因为单个设备在共享公共DIN时无法编程。

注意，从图2中可以看出，SCLK上升沿在DOUT上将数据移出设备。SCLK负边用于将数据锁存到设备DAISY_IN引脚中。这种架构允许更快的SCLK速率速度，但也使接口对板级信号延迟敏感。链中的设备越多，遵守设置和保持时间就越具有挑战性。星形连接的SCLK到所有设备，最大限度地减少DOUT长度，和其他印刷电路板（PCB）布局技术的帮助。在DOUT和DAISY_IN之间放置延迟电路（如缓冲区）是缓解这一挑战的方法。另一种选择是在反向SCLK上的DOUT和DAISY_IN之间插入一个$D$触发器。还要注意，菊花链模式需要一些软件开销来重新组合跨字节边界分布的数据位。图71显示了此模式的时序图。

![](images/da849721d34f55cf80f57b87c6fbe6d806a0edfdaa275097b77592d52d1f39b9.jpg)  
图71。菊花链时间

#申请信息（续）

可以采用菊花链的设备的最大数量取决于设备运行的数据速率。最大装置数可由式10近似计算。

$$
(') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (') (')
$$

where:

NBITS =设备分辨率（取决于数据速率），NCHANNELS $\mathbf {\tau} = \mathbf {\tau}$设备中的通道数。

例如，当8通道ADS1299以2 ksps的数据速率和4 mhz的fSCLK工作时，10个设备可以被菊花链连接。

# 10.2典型应用

与其他类型的生物电位信号相比，脑电图（EEG）中测量的生物电位信号很小。由于其高性能内部PGA具有极低的输入参考噪声，因此ADS1299可以测量如此小的信号。图72和图73是典型EEG测量设置中如何配置ADS1299的示例。图72显示了如何在顺序蒙太奇中测量电极电位，而图73显示了参考蒙太奇测量连接。

![](images/b0816a5ab73b7ad90bf5a4da70cfa10ead7db20a5b75642b79c2b302d69abcd9.jpg)  
图72。ADS1299在脑电图数据采集应用中的示例原理图，顺序蒙太奇

#典型应用（续）

![](images/3e5662f5460c3391242b84dd6debf83275e953dc5d09a0ee252201500320eecb.jpg)

图73示例原理图使用ADS1299在脑电图数据采集应用，参考蒙太奇

10.2.1设计要求

表29显示了典型脑电图测量系统的设计要求。

表29。脑电数据采集设计要求

<html><body><table><tr><td>DESIGN PARAMETER</td><td>VALUE</td></tr><tr><td>Bandwidth</td><td>1 Hz 50 Hz</td></tr><tr><td>Minimum signal bandwidth</td><td>10μVpk</td></tr><tr><td>Input Impedance</td><td>> 10 MΩ</td></tr><tr><td>Coupling</td><td>dc</td></tr></table></body></html>

# 10.2.2详细设计程序

ADS1299上的每个通道都经过优化，可以测量单独的EEG波形。具体的连接取决于脑电图蒙太奇。顺序蒙太奇是一种配置，其中每个通道表示两个相邻电极之间的电压。例如，要测量ADS1299通道1上电极Fp1和F7之间的电位，将Fp1电极引到IN1P，将F7电极引到IN1N。顺序蒙太奇的连接如图72所示。

或者，可以在参考蒙太奇中测量EEG电极，其中每个电极相对于单个参考电极进行测量。这个蒙太奇也允许计算的波形，将被测量在一个连续蒙太奇找到两个电极波形之间的差异，这是相对于同一电极测量。ADS1299允许通过使用SRB1引脚进行这样的配置。通过在MISC1寄存器中设置SRB1位，ADS1299上的SRB1引脚可以在内部路由到每个通道负输入。当参考电极连接到SRB1引脚，所有其他电极连接到各自的正极通道输入时，可以用参考蒙太奇测量电极电压。参考蒙太奇如图73所示。通道输入多路复用器选项图见图18。

ADS1299被设计为EEG前端，因此在电极和ADS1299之间不需要额外的放大或缓冲级。ADS1299具有低噪声PGA，具有优异的输入参考噪声性能。对于某些数据速率和增益设置，ADS1299向信号链引入的输入参考噪声明显小于1 $\ mathsf {V} _ {\mathsf {R M S}}$，这使得该器件能够处理$ 10 \mathsf {-} \mu \mathsf {V} _ {\mathsf {P k}}$的最小信号幅度。表1、表2、表3和表4列出了不同PGA增益和数据速率设置下ADS1299的噪声性能。

传统的脑电图数据采集系统在前端对信号进行高通滤波，去除直流信号内容。这种拓扑结构允许信号通过大增益放大，因此信号可以通过12至16位ADC进行数字化。ADS1299 24位分辨率允许信号直流耦合到ADC，因为除了显著的直流偏移之外，还可以测量小的EEG信号信息。

ADS1299通道输入具有非常低的输入偏置电流，允许电极连接到ADS1299的输入端，而患者电缆上的漏电流非常小。当关闭导通电流源时，ADS1299的最小直流输入阻抗为$1 \ G \Omega$，当开启导通电流源时，通常为500 MΩ。

无源元件$\mathsf {R} _ {\mathsf {F ilt}}$和$\mathsf {C} _ {\mathsf {F ilt}}$构成低通滤波器。一般来说，建议使用分流输入的差分电容CFIlt而不是单独的RC滤波器，其电容分流到地。差分电容配置显著改善共模抑制，因为这种方法消除了对组件失配的依赖。

由于δ - σ ADC滤波器-然后抽取拓扑结构，滤波器的截止频率可以远远超过ADC的数据速率。注意防止在fMOD处第一次重复数字抽取滤波器响应时出现混叠。假设一个2.048 MHz的fCLK， fMOD $\mathbf {\sigma} = \mathbf {\sigma}$ 1.024 MHz。$\mathsf {R} _ {\mathsf {F i l t}}$的值具有医疗电子技术标准设定的最小值。必须设置电容值以安排合适的截止频率。

如果系统可能暴露于高频EMI，建议在输入端添加非常小值的共模电容来过滤高频共模信号。如果增加这些电容器，则电容器应比差分电容器小10或20倍，以确保其对CMRR的影响最小。

集成偏置放大器在ADS1299的脑电图数据采集系统中有两个用途。偏置放大器提供一个偏置电压，当施加到病人身上时，测量电极的共模电压保持在ADS1299的轨内。这个场景允许直流耦合。此外，偏置放大器可以配置为向患者提供负共模反馈，以消除出现在电极上的不需要的共模信号。这个特性特别有用，因为生物电位采集系统是出了名的容易受到主频共模干扰。

通过在CONFIG3寄存器中设置PD_BIAS位，偏置放大器上电。将CONFIG3寄存器中的BIASREF位设置为将内部生成的模拟中电源电压输入到偏置放大器的非反相输入。要使电极作为偏置放大器的输入，请在bias SENSP或BIAS_SENSN寄存器中设置相应的位。

偏置放大器的直流增益由$\mathsf {R} _ {\mathsf {B ias}}$和使能作为偏置放大器输入的通道输入数决定。偏置放大电路只通过共模信号。因此，每个PGA输出端的$ 3,30 \ mathm {-} \mathsf {k} \Omega$电阻并联用于共模信号。偏置放大器配置为反向增益方案。确定输入到偏置放大器的共模信号的直流增益的公式如公式11所示。电容器$\mathsf {C} _ {\mathsf {f}}$设置偏置放大器的带宽。确保放大器有足够的带宽输出所有预期的共模信号。

$$
\frac { \mathsf { V } _ { \mathsf { o u t } } } { \mathsf { V } _ { \mathsf { i n } } } = - \frac { \mathsf { R } _ { \mathsf { f } } \times \mathsf { N } } { 3 3 0 \mathsf { k } \Omega }
$$

直流耦合脑电图数据采集系统的另一个优点是能够检测电极何时不再与患者良好接触。ADS1299集成了引线检测电子器件。引线检测一节介绍如何在ADS1299上使用引线功能。请注意，在参考蒙太奇中配置时，仅使用一个导通电流源与参考电极。

10.2.3应用曲线

测试ADS1299在典型脑电图信号的频带和幅值附近测量信号的能力可以用精密信号发生器来完成。ADS1299在如图74所示的配置中进行了测试。

![](images/32d4ed603924a89e88e840c4cb87fe0b3c99c7d62cc70d829089d00694bd2699.jpg)

#图74示例原理图使用ADS1299在脑电图数据采集应用，参考蒙太奇

$ 95,2 - 1 < \ ω $和$ 10。3 \substack {- \mathsf {k} \Omega}$电阻器用于衰减来自信号源的电压，因为信号源无法直接达到所需的幅度。采用分压器后，输入端出现的信号为3.5 μ vrms， 10hz的正弦波。图75显示了校准偏移量后ADS1299的输入参考转换结果。所测量的信号与典型脑电图采集系统可以测量的一些最小的颅外脑电图信号相似。信号可以清楚地识别。考虑到该测量装置是无屏蔽的单端配置，测量装置会受到明显的市电干扰。采用数字低通滤波器消除干扰。

![](images/32952f2f65752d2c3c4a14b9c47673068fc13badb205bf7470d62d1d818175cb.jpg)  
图75。ADS1299 10hz输入信号结果

# 11电源建议

ADS1299- $\cdot \mathsf { x }$提供AVDD、AVDD1、DVDD三路电源。为了获得最佳性能，AVDD和AVDD1都必须尽可能安静。AVDD1为电荷泵块提供电源，并在$\mathsf { f } _ { \mathsf { C L K } }$处具有瞬态。因此，将AVDD1星形连接到AVDD引脚，AVSS1星形连接到AVSS引脚。必须消除与ADS1299-x操作不同步的AVDD和AVDD1噪声。用$1 0 \mathrm { - } \mu \ F$和$0 . 1 \mathrm { - } \mathrm { \dot { \mu } F }$固体陶瓷电容器旁路每个器件电源。为了获得最佳性能，请在系统中放置数字电路（DSP、微控制器、fpga等），以便这些设备上的返回电流不会穿过设备的模拟返回路径。ADS1299-x由单极或双极电源供电。

采用表面贴装、低成本、低轮廓的多层陶瓷电容器进行去耦。在大多数情况下，VCAP1电容器也是多层陶瓷；但是，在电路板遭受高频或低频振动的系统中，安装非铁电电容器，例如钽或1类电容器（C0G或NPO）。EIA第2类和第3类电介质（X7R、X5R、X8R等）是铁电介质。这些电容器的压电特性可以表现为来自电容器的电噪声。当使用内部引用时，VCAP1节点上的噪声会导致性能下降。

# 11.1开机顺序

在设备上电之前，所有数字和模拟输入必须为低电平。在上电时，将所有这些信号保持在低电平，直到电源稳定，如图76所示。

允许时间使电源电压达到其最终值，然后开始向CLK引脚提供主时钟信号。等待时间tPOR，然后使用reset引脚或reset命令发送复位脉冲来初始化芯片的数字部分。在tPOR或VCAP1电压大于$1后发出复位。1 \ V$，以时间较长的为准。注意:

tPOR如表30所示。
VCAP1引脚充电时间由VCAP1上电容值设定的RC时间常数设定；参见图25。

释放RESET引脚后，对配置寄存器进行编程。上电时序时序如表30所示。

![](images/f9f100abac8c5663d5c9d6255ca0d7560f9a90f215a61fe6c4bbb0f516a670ca.jpg)  
图76。上电时序图

(1)复位脉冲时间为t泊或tBG后，以时间较长的为准。
(2)当使用外部时钟时，直到CLK存在且有效，tPOR计时才开始。

表30。图76的时序要求

表身体< html > < > < > < tr > < td colspan = " 2 " > < / td > < td > MIN < / td > < td > MAX单位< / td > < / tr > < tr > < td > tPOR < / td > < td >等待直到电复位后< / td > < td > 218 < / td > < td > tCLK < / td > < / tr > < tr > < td > tRST < / td > < td >重置低时间< / td > < td > < / td > < td > tCLK < / td > < / tr > < /表> < /身体> < / html >

# 11.2将设备连接到单极（5 V和3.3 V）电源

图77显示了连接到单极电源的ADS1299- $\cdot \mathsf {x}$。在本例中，模拟电源（AVDD） T引用模拟地（AVSS），数字电源（DVDD） T引用数字地（DGND）。

#将设备连接到单极（5v和3.3 V）电源（续）

![](images/ccf9991154f299574581c18e9d2ed2a59cabf7bddcff1615c4ee69f9e4da42ab.jpg)  
图77。单电源操作

注意：将供电源、参考和VCAP1至VCAP4的电容器尽可能靠近封装。

# 11.3连接设备到双极$\pm5 \nu$和3.3 V)电源

图78显示了连接到双极电源的ADS1299- $\cdot \mathsf {x}$。在本例中，模拟电源连接到设备模拟电源(AVDD)。该电源参考设备模拟返回（AVSS），数字电源（DVDD）参考设备数字地返回（DGND）。

![](images/a2408091cc41aad6b3aa96c60f44e5992c33865c972415dfcf77b743f2a86c19.jpg)  
图78。双极供电操作

注意：将供电源、参考和VCAP1至VCAP4的电容器尽可能靠近封装。

# 12 Layout

# 12.1布局指南

TI建议在为模拟和数字组件布置印刷电路板（PCB）时采用最佳设计实践。该建议通常意味着该布局将模拟组件[如adc、放大器、参考、数模转换器（dac）和模拟mux]与数字组件[如微控制器、复杂可编程逻辑器件（cpld）、现场可编程门阵列（fpga）、射频（RF）收发器、通用串行总线（USB）收发器和开关稳压器]分开。图79显示了一个良好组件放置的示例。尽管图79提供了一个很好的组件放置示例，但每个应用程序的最佳放置位置取决于所采用的几何形状、组件和PCB制造能力。也就是说，没有一种布局对每一个设计都是完美的，在设计任何模拟组件时都必须仔细考虑。

![](images/52986a7ada68cb2e6cfa82df79274b3588fe76dd7554ffb67f4db4f0dc0dd06f.jpg)  
图79。系统组件放置

以下概述了ADS1299-x布局的一些基本建议，以获得ADC的最佳性能。一个好的设计可能会被一个糟糕的电路布局毁掉。

分离模拟和数字信号。首先，在布局允许的情况下，将电路板划分为模拟和数字部分。将数字线路与模拟线路分开。这种配置可以防止数字噪声耦合回模拟信号。
接地平面可以分为模拟平面（AGND）和数字平面（DGND），但不是必需的。将数字信号置于数字平面上，将模拟信号置于模拟平面上。作为布局的最后一步，模拟地和数字地之间的分离必须在ADC连接在一起。
用地面填充物填充信号层上的空洞区域。
提供良好的接地回路。信号返回电流沿阻抗最小的路径流动。如果接地平面被切断或有其他阻挡电流流过信号走线的走线，那么电流必须找到另一条路径返回源并完成电路。如果电流被迫进入较长的路径，信号辐射的机会就会增加。敏感信号更容易受到电磁干扰。
在电源上使用旁路电容器以降低高频噪声。不要在旁路电容器和有源器件之间放置过孔。将旁路电容器放置在靠近有源器件的同一层上可获得最佳效果。
具有差分连接的模拟输入必须在输入端有一个电容。差动电容器必须是高质量的。C0G （NPO）是最佳的陶瓷片式电容器，具有性能稳定、噪声低的特点。

# 12.2布局示例

图80是最少需要两个PCB层的ADS1299布局示例。示例电路显示为单模拟电源或双极电源连接。在这个例子中，多边形倾注被用作设备周围的供电连接。如果使用三层或四层PCB，则额外的内层可以专门用于路由电源走线。PCB划分为模拟信号从左走，数字信号从右走，电源从设备上下走。

![](images/4327defaa834a91c258440ab0d7b27e1811e6a6b6fe795714065e29e59e46660.jpg)  
布局示例（续）
图80。ADS1299布局样例

# 13 器件和文档支持

# 13.1 文档支持

# 13.1.1 相关文档

相关文档如下：

ADS129x is a low-电源, 8-channel, 24-bit analog front-end for physiological signal measurement. REF50xx features low noise, extremely low drift, and high-precision 电压 reference.
Improving Common-mode Suppression with Right Leg Drive Amplifier "ADS1299EEG-FE EEG Front-End Performance Demonstration Kit"

# 13.2 相关链接

以下表格列出了快速访问链接。 范围包括技术文档、 支持与社区资源、 工具和软件， 并且可通过快速访问立刻订购。

# 表 31. 相关链接

< HTML > < body > < table > < tr > < td > devices < / td > < td > product folder < / td > < td > immediately order < / td > < td > technical document < / td > < td > < / td > tools and software support and community < / td > < td > < / tr > < tr > < td > ADS12 99 < / td > < td > click here < / td > < td > click here < / td > < td > click here < / td > < td > click here < / td > < td > click here < / td > < / tr > < tr > < td > ADS1299-4 < / td > < td > click here < / td > < t D > click here < / td > < td > click here < / td > < td > click here < / td > < td > click here < / td > < / tr > < tr > < td > ADS1299-6 < / td > < td > click here < / td > < td > click here < / td > < td > click here < / Td > < td > click here < / td > < td > click here < / td > < / tr > < / table > < / body > < / HTML >

# 13.3 接收文档更新通知

To receive notifications of document updates, please visit the 器件 products folder on www.ti.com.cn. Click on "Alert me" in the upper right corner to register, and you can receive the updated product information regularly every week. For detailed information about the changes, please refer to the revision history included in the revised document.

# 13.4 社区资源

以下链接连接到TI社区资源。链接内容由各自的贡献者“按原样”提供。它们不构成TI规范，也不一定反映TI的观点；请参阅TI的使用条款。

TI E2E在线社区TI的工程师对工程师（E2E）社区。旨在促进工程师之间的合作。在e2e.ti.com上，你可以提问、分享知识、探索想法并帮助其他工程师解决问题。

快速找到有用的E2E论坛以及设计支持工具和技术支持的联系信息。

# 13.5 商标

E2E是德州仪器的商标。
所有其他商标均为其各自所有者的财产。

# 13.6 静电放电警告

![](images/0b0eaeedef623b1f93c44348004d5512ef39f744b066f959e9c60e146aadacbb.jpg)

ESD 可能会损坏该集成电路。 德州仪器 (TI) 建议通过适当的预防措施处理所有集成电路。 如果不遵守正确的处理措施和安装程序 可能会损坏集成电路。  
ESD 的损坏小至导致微小的性能降级 大至整个器件故障。 精密的集成电路可能更容易受到损坏 这是因为非常细微的参数更改都可能会导致器件与其发布的规格不相符。

13.7术语

SLYZ022 TI术语表。

本术语表列出并解释了术语、缩略语和定义。

# 14 机械、 封装和可订购信息

以下页中包括机械、封装和可订购信息。这些信息是针对指定器件可提供的最新数据。这些数据会在无通知且不对本文档进行修订的情况下发生改变。 欲获得该数据表的浏览器版本， 请查阅左侧的导航栏。

#包装信息

<html><主体><表><tr><td>可订购零件编号</td><td>状态(1)</td><td>封装|引脚</td><td>封装数量载具</td><td>RoHS (3)</td><td>引脚完成/球料</td><td>MSL额定/最高回流流量</td><td>Op温度（°C）</td><td>零件标记(6)</td></ td>< tr><td>ADS1299-4PAG</td><td>Active</td><td> <td>TQFP (PAG) | 64</td><td>160|JEDEC托盘(10+1)</td><td> <td>- 3- 260c -168 HR</td><td>-40 to85 < / td > < td > ADS1299-4 < / td > < / tr > < tr > < td > ADS1299-4PAG。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 160 |电平< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-4 < / td > < / tr > < tr > < td > ADS1299-4PAGR < / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) T64 < / td > < td >托盘(10 + 1)1500家大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-4 < / td > < / tr > < tr > < td > ADS1299-4PAGR。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-4 < / td > < / tr > < tr > < td > ADS1299-6PAG < / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 160 |电平托盘(10 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-6 < / td > < / tr > < tr > < td > ADS1299-6PAG。< / td > < td >活动< / td > < td > Productior < / td > < td > TQFP (PAG) | 64 < / td > < td > 160 |电平托盘(10 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-6 < / td > < / tr > < tr > < td > ADS1299-6PAGR < / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-6 < / td > < / tr > < tr > < td > ADS1299-6PAGR。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299-6 < / td > < / tr > < tr > < td > ADS1299IPAG < / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 160 |电平托盘(5 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299 < / td > < / tr > < tr > < td > ADS1299IPAG。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 160 |电平托盘(5 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299 < / td > < / tr > < tr > < td > ADS1299IPAGR < / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG)我64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > 40到85 < / td > < td > ADS1299 < / td > < / tr > < tr > < td > ADS1299IPAGR。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) I64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299 < / td > < / tr > < tr > < td > ADS1299IPAGRG4 < / td > < td >活动< / td > < td >生产< / td > < td > TQFP PAG) 64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299 < / td > < / tr > < tr > < td > ADS1299IPAGRG4。< / td > < td >活动< / td > < td >生产< / td > < td > TQFP (PAG) | 64 < / td > < td > 1500个大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > -40 - 85 < / td > < td > ADS1299 < / td > < / tr > < /表> < /身体> < / html >

(1)状态：有关状态的详细信息，请参阅我们的产品生命周期。

(2)材料类型：当指定时，预生产部件是原型/实验设备，尚未批准或释放全面生产。测试和最终过程，包括但不限于质量保证、可靠性性能测试和/或过程鉴定，可能尚未完成，本项目可能会进一步更改或可能停止。如果可以订购，购买将在结账时获得额外的豁免，并且仅用于早期内部评估目的。这些物品出售时没有任何形式的保证。

(3) RoHS值：Yes， No, RoHS Exempt。有关更多信息和价值定义，请参阅TI RoHS声明。

(4)铅饰面/球料：零件可能有多种材料饰面选择。完成选项由垂直直线分隔。如果光洁度值超过最大柱宽，铅光洁度/球材料值可能缠绕到两行。

(5) MSL额定值/峰值回流：湿度敏感等级额定值和峰值焊料（回流）温度。如果一个部件具有多个湿度敏感性等级，则仅显示JEDEC标准的最低等级。
请参阅发货标签，了解将用于将零件安装到印刷电路板上的实际回流温度。

6)零件标记：可能会有一个额外的标记，它涉及到标志，批号跟踪代码信息，或部件的环境类别。

多个部分标记将在括号内。一个零件上只会出现一个用“\~”分隔的用圆括号括起来的零件标记。如果一行是缩进的，那么它是前一行的延续，两者结合代表该设备的整个部分标记。

重要信息和免责声明：本页上提供的信息代表了TI在提供之日的知识和信念。TI的知识和信念基于第三方提供的信息，对此类信息的准确性不作任何陈述或保证。目前正在努力更好地整合来自第三方的信息。TI已经采取并将继续采取合理措施提供具有代表性和准确的信息，但可能没有对来料和化学品进行破坏性测试或化学分析。TI和TI供应商认为某些信息是专有的，因此CAS号码和其他有限的信息可能无法发布。

在任何情况下，由这些信息引起的TI的责任不应超过TI每年向客户出售的本文件中所述TI部件的总购买价格。

#磁带和卷轴信息

![](images/d10d22c50e1ba82a4cbbfd6ac3bee7b53c18e1f26f8b67bf2824a6dd83bb015d.jpg)

![](images/7f65741a6817667323e2e1d45b215b7072a872f2e9ff34a5094a81e71528e459.jpg)

<html><body><table><tr><td>A0</td><td>设计容纳元件宽度的尺寸</td></tr><tr><td>B0</td><td>设计容纳元件长度的尺寸</td></tr><tr><td>K0</td><td>设计容纳元件厚度的尺寸</td></tr><tr><td>W</td><td>载波带总宽度</td></tr><tr><td>P1</td><td>间距连续腔中心</td></tr></table></body></html>

#象限分配引脚在磁带中的方向

![](images/84b0d72641a566a5fe4d9ddb8ce6899ab084c93d1f219741a6b9856da7165e84.jpg)

\*所有尺寸均为标称尺寸

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> 器件 & lt; /戴利的小于戴利> > Package Type & lt; /戴利的小于戴利> > Package Drawing & lt; /戴利的小于戴利> > Pins & lt; /戴利的小于戴利> > SPQ & lt; /戴利的小于戴利> > Reel密度小于戴利(mm) & lt; /戴利> > Reel Width W1 (mm) & lt; /戴利的小于戴利> >亚当奥克福德(mm) & lt; /戴利的小于戴利> > B0 (mm) & lt; /戴利的小于戴利> >不小于戴利(mm) & lt; /戴利> > P1 (mm) & lt; /戴利的小于戴利> > W (mm) & lt; /戴利的小于戴利> > Pin1Quadrant & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-4PAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 330.0 & lt; /戴利的小于戴利> > 24.4 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > .5 & lt; /戴利的小于戴利> > 16.0 & lt; /戴利的小于戴利> > 24.0 & lt; /戴利的小于戴利> > Q2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-6PAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 330.0 & lt; /戴利的小于戴利> > 24.4 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 5 & lt; /戴利的小于戴利> > 16.0 & lt; /戴利的小于戴利> > 24.0 & lt; /戴利的小于戴利> > Q2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 330.0 & lt; /戴利的小于戴利> > 24.4 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 13。0 & lt; /戴利的小于戴利> > .5 & lt; /戴利的小于戴利> > 16.0 & lt; /戴利的小于戴利> > 24.0 & lt; /戴利的小于戴利> > Q2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAGRG4 & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 330.0 & lt; /戴利的小于戴利> > 24.4 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 13.0 & lt; /戴利的小于戴利> > 150 & lt; /戴利的小于戴利> > 16.0 & lt; /戴利的小于戴利> > 24.0 & lt; /戴利的小于戴利> > Q2 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

![](images/c82c1e997e8ecc36a01107cc186237cd9d3c5b2cf5120817e79e9fe6bbc4451d.jpg)

\*所有尺寸均为标称尺寸

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> 器件 & lt; /戴利的小于戴利> > Package Type & lt; /戴利的小于戴利> > Package Drawing & lt; /戴利的小于戴利> > Pins & lt; /戴利的小于戴利> > SPQ & lt; /戴利的小于戴利> > Length (mm) & lt; /戴利的小于戴利> > Width (mm) & lt; /戴利的小于戴利> >传递(mm) & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-4PAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 43.0 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-6PAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 43.0 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAGR & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 43.0 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAGRG4 & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于在1500戴利> > & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 350.0 & lt; /戴利的小于戴利> > 43.0 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

# TRAY

L-Outer tray length without tabs KO-Outer tray ten ten ten ten ten ten ten plus height +++++++++++++++ W -ten ten ten ten ten ten ten ten ten plus Outer Ten ten ten ten ten ten ten ten ten ten ten ten ++++++++ \$++++++++ P1-Tray unit pocket pitch CW-Measurement for tray edge (Y direction) to corner pocket center CL - Measurement for tray edge (X direction) to corner pocket center

托盘角上的倒角表示包装单元的引脚方向。

\*所有尺寸均为标称尺寸

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> 器件 & lt; /戴利的小于戴利> > Package Name & lt; /戴利的小于戴利> > Package Type & lt; /戴利的小于戴利> > Pins & lt; /戴利的小于戴利> > SPQ & lt; /戴利的小于戴利> > minutes array黑客帝国& lt; /戴利的小于戴利> >麦克斯temperature(丙)& lt; /戴利的小于戴利> > L (mm) & lt; /戴利的小于戴利> > W (mm) & lt; /戴利的小于戴利> >不小于戴利(μm) & lt; /戴利> > & lt; /戴利的小于戴利> >《作见证》(mm) & lt; /戴利的小于戴利> > CW (mm) & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-4PAG & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8×20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 15.2 & lt; /戴利的小于戴利> > 13。& lt; /戴利的小于戴利13 & lt; /戴利> > > & lt; / tr > & lt tr > & lt;戴利> ADS1299-4PAG。A & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8乘以20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 5.2 & lt; /戴利的小于戴利> > 13.1 & lt; /戴利的小于戴利> > 13 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-6PAG & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8乘以20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 15.2 & lt; /戴利的小于戴利> > 13.1 & lt; /戴利的小于戴利> > 13 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299-6PAG。A & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8乘以20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 15.2 & lt; /戴利的小于戴利> > 13.1 & lt; /戴利的小于戴利> > 13 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAG & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8乘以20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 15.2 & lt; /戴利的小于戴利> > 13.1 & lt; /戴利的小于戴利> > 13 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> ADS1299IPAG。A & lt; /戴利的小于戴利> > PAG & lt; /戴利的小于戴利> > TQFP & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > 160个& lt; /戴利的小于戴利> > 8×20 & lt; /戴利的小于戴利> > 150 & lt; / 315戴利的小于戴利> > & lt; /戴利的小于戴利> > 135.9 & lt; /戴利的小于戴利> > 7620 & lt; /戴利的小于戴利> > 15.2 & lt; /戴利的小于戴利> > 13.1 & lt; /戴利的小于戴利> > 13 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

#塑料四边形扁平包装

![](images/e985c59fa7970f2f8c79df46154003e7c6afbd82f7695dc25eec1d963415d07c.jpg)

注：A.所有线性尺寸均以毫米为单位。本图纸如有更改，恕不另行通知。C.属于JEDEC MS-026

![](images/3b2267c9d0e082bbe5c51b537c0b4efd5e4af6a96ebb6a1d008ffcc10b6abd0a.jpg)

A.所有的线性尺寸都以毫米为单位。
本图纸如有更改，恕不另行通知。
C.具有梯形壁和圆角的激光切割孔将提供更好的粘贴释放。客户应联系他们的电路板组装现场的模板设计建议。基于体积为50%的金属负载锡膏的模板设计示例。参考IPC-7525的其他模板建议。
客户应联系他们的电路板制造现场，了解信号垫之间和周围的阻焊容差。
A.所有的线性尺寸都以毫米为单位。
本图纸如有更改，恕不另行通知。
C.具有梯形壁和圆角的激光切割孔将提供更好的粘贴释放。客户应联系他们的电路板组装现场的模板设计建议。基于体积为50%的金属负载锡膏的模板设计示例。参考IPC-7525的其他模板建议。
客户应联系他们的电路板制造现场，了解信号垫之间和周围的阻焊容差。

![](images/d55ce13af6a1c47245351ead947f77f29f44200242291e68dad349779c5e9820.jpg)

# 重要通知和免责声明

TI“按原样”提供技术和可靠性数据 （包括数据表） 、 设计资源（包括参考设计）、 应用或其他设计建议、 网络工具、 安全信息和其他资源， 不保证没有瑕疵且不做出任何明示或暗示的担保， 包括但不限于对适销性、 与某特定用途的适用性或不侵犯任何第三方知识产权的暗示担保。

这些资源可供使用 TI 产品进行设计的熟练开发人员使用。 您将自行承担以下全部责任： (1) 针对您的应用选择合适的 TI 产品， (2) 设计、 验证并测试您的应用， (3) 确保您的应用满足相应标准以及任何其他安全、 安保法规或其他要求。

这些资源如有变更，恕不另行通知。TI 授权您仅可将这些资源用于研发本资源所述的 TI 产品的相关应用。严禁以其他方式对这些资源进行复制或展示。您无权使用任何其他 TI 知识产权或任何第三方知识产权。对于因您对这些资源的使用而对 TI 及其代表造成的任何索赔、损害、成本、损失和债务， 您将全额赔偿，TI 对此概不负责。

Products provided by TI are subject to the TI Sales Terms, TI General Quality Guidelines or other applicable terms on ti.com or other applicable terms accompanying TI products. The provision of these resources by TI does not extend or otherwise alter the applicable warranties or disclaimers of warranty issued by TI for TI products. Unless Texas Instruments (TI) explicitly designates a certain product as a custom product or a customer-specific product, all of its products are standard general-purpose devices included in the catalogue at a determined price.

TI 反对并拒绝您可能提出的任何其他或不同的条款。