# TFP410 TI PanelBus™数字变送器

# 1特点

支持高达165MHz的像素率（包括1080p和60Hz的WUXGA）

通用图形控制器接口：

12位，双边和24位，单边输入
modes   
可调1.1V至1.8V和标准3.3V
CMOS输入信号
全差分和单端输入
时钟模式
标准英特尔12位数字视频端口
与Intel™81x芯片组兼容

增强锁相环抗扰度：片上稳压器和旁路电容器，降低系统成本

增强的抖动性能：无HSYNC抖动异常可忽略不计的数据相关抖动

可编程使用$1 ^ {2}{\mathsf {C}}$串行接口
通过热插拔和接收器监测检测
detection   
单3.3V电源操作
64引脚TQFP采用TI的PowerPAD™封装
德州仪器涨了0美元。1 8 \mu \mathrm {m}$ EPIC- $。5 ^ {\mathsf {T M}}$ CMOS进程
技术
引脚兼容SiI164 DVI发射器1

# 2应用

DVD   
Blu-ray™   
高清投影仪
DVI/HDMI发射机

# 3描述

TFP410器件是德州仪器PanelBus™平板显示产品，是全面的端到端DVI 1.0兼容解决方案系列的一部分，针对PC和消费电子行业。

TFP410器件提供了一个通用接口，允许与最常用的图形控制器进行无胶连接。这种通用接口的一些优点包括可选总线宽度，可调信号电平，差分和单端时钟。可调1.1V至1.8V数字接口提供低emi高速总线，可与12位或24位接口无缝连接。DVI接口支持高达165MHz UXGA的平板显示分辨率，采用24位真彩色像素格式。

TFP410器件结合了PanelBus电路创新和TI先进的$0。EPIC-5 CMOS工艺技术和TI超低地电感PowerPAD封装。其结果是一个紧凑的64引脚TQFP封装提供可靠，低电流，低噪声，高速数字接口解决方案。

包信息

表身体< html > < > < > < tr > < td >零件号< / td > < td >包(1)< / td > < td >包大小(2)< / td > < / tr > < tr > < td > TFP410 < / td > < td > PAP (HTQFP, 64) < / td > < td > 12毫米×12毫米< / td > < / tr > < /表> < /身体> < / html >

(1)有关更多信息，请参见第10(2)节。封装尺寸（长度$\乘以$宽度）是标称值，在适用的情况下包括引脚。

![](images/390a49956178d0818a6ddd91284123689ab4f3a518a4399b15888e6466198856.jpg)  
典型HDMI接口

数字视觉接口（DVI）规范是由数字显示工作组（DDWG）开发的用于高速数字连接到数字显示器的行业标准，已被行业领先的PC和消费电子制造商采用。TFP410符合DVI Revision 1.0规范。

#目录

1功能。
2应用程序。
3描述. ....1
4引脚配置与功能
5产品规格.. .........................6
5.1绝对最大评级
5.2 ESD等级。6……
5.3推荐的操作条件 . ........................6
5.4热信息
5.5电气特性
5.6定时要求
5.7典型特征 ..........................9……
6详细描述
6.1概述……■。: 10
6.2原理框图 .........................................11
6.3特性描述
6.4设备功能模式。[12
6.5编程
6.6注册表. .........18

# 7应用与实现

7.1申请信息
7.2典型应用
7.3电源建议
7.4布局28

# 8设备和文档支持

8.1接收文档更新通知....
8.2支持资源
8.3商标
8.4静电放电注意事项. .. .
8.5术语33

# 9修改历史

# 10机械、包装和可订购

34信息。

# 4引脚配置和功能

![](images/a02a8aa8122ce429e1ff9df94a5cb2336bf856682494031294646bc65b0528b9.jpg)  
图4 - 1。PAP封装，64针HTQFP（俯视图）

表身体< html > < > < > < tr > < td colspan = " 2 " > < / td > < td colspan =“2”>表4 - 1。销功能< / td > < / tr > < tr > < td >销< / td > < td > < / td > < td >类型(1)< / td > < td >描述< / td > < / tr > < tr > < td colspan =“4”>的名字。</td></tr><tr><td>输入 DATA[23:12]</td><td>36-47</td><td> the在24位、单边12位、双边的</td><td> 24位像素总线输入模式的上12位（BSEL high）中，该总线输入24位像素总线的上半部分。在输入模式下（BSEL = low），这些位不用于输入像素数据。在这种模式下，DATA[23:16]的状态是I2C寄存器CFG。</td></tr><tr><td></td><td>50-55,</td><td></td><td>输入到允许图形控制器通过|2C接口读取8位用户配置数据（参见6.6节）。注：所有未使用的数据输入应连接到GND或VDD。24位像素总线的下12位/12位像素总线输入Ox</td></tr><tr><td>DATA[11:0]</td><td>58-63</td><td></td><td>在24位单边输入模式（BSEL high）下，该总线输入24位像素总线的下半部分。在12位双边缘输入模式（BSEL低）下，该总线在时钟的每个锁存器边缘（上升和下降）输入1/2个像素（12位）。差分时钟输入。TFP410支持单端和全差分时钟输入模式。在单端单端时钟源中，IDCK+输入端（引脚57）应连接到</td></tr><tr><td>IDCK- IDCK+</td><td>56 57</td><td></td><td>在单端单端时钟源中，IDCK-输入端（引脚56）应连接到GND。在差分时钟输入模式下，TFP410使用IDCK+和dck信号之间的交叉点作为锁存输入数据data[23:0]、DE、HSYNC和VSYNC的时序参考。差分时钟输入模式仅在低信号摆幅模式下可用。数据支持。根据DVI 1.0规范的定义，DE信号允许发射机在任意给定的输入时钟周期内编码像素</td></tr><tr><td>DE</td><td></td><td></td><td>数据或控制数据。在活动视频（DE高）期间，发射机编码像素数据，data[23:0]。在消隐间隔（DE = low）期间，发射机编码HSYNC、VSYNC和CTL[3:1]。</td></tr><tr><td>HSYNC VSYNC</td><td>4 5</td><td></td><td>水平同步输入垂直同步输入</td></tr><tr><td></td><td></td><td></td><td>这三个多功能输入的操作取决于ISEL（引脚13）和DKEN（引脚35）输入的设置。所有三个输入都支持3.3V CMOS信号电平，并包含弱下拉</td></tr><tr><td rowspan="2">A3/DK3 CTL2/A2/DK2 CTL1/A1/DK1</td><td rowspan="2">678</td><td rowspan="2"></td><td>电阻，因此，如果不连接，它们默认为所有低电平。当I²C总线禁用（ISEL = low）和模式禁用（DKEN low）时，引脚7和8成为控制输入CTL[2:1]，可用于在消隐间隔（de low）期间通过-skew</td></tr><tr><td> DVI链路发送额外信息。引脚6没有使用。当I²C总线被禁用(ISEL =和去倾斜模式被启用（DKEN =高），这三个输入成为去倾斜输入DK[3:1]，用于调整设置和保持时间的像素输入DATA[23:0]，相对于IDCK±。低)</td></tr><tr><td colspan="6">当I2C总线使能（ISEL高）时，这三个输入成为3个lsb地址，A[3:1]。</td></tr><tr><td>CONFIGURATION/PROGRAMMING MSEN/PO1</td><td></td><td></td><td>监控感/可编程输出1。该引脚的操作取决于I2C接口是否启用或禁用。这个引脚有一个漏极输出，只容忍3.3V。在此引脚上需要连接到VDD的外部5kΩ上拉电阻。开放- < / td > < / tr > < tr > < td > < / td > < td > < / td > < td > < / td > < td > dc-coupled系统有效。当|2C使能（ISEL = high）时，该输出可通过|2C接口编程（参见|2C寄存器说明章节）。12C接口select/I²C RESET (active low, asynchronous) Q</td></tr><tr><td>ISEL/ RST</td><td></td><td>the (BSEL)，改为f 1f</td><td>ISEL高，则|2C接口为active。I2C寄存器的默认值可以在章节6.6中找到。如果ISEL低，则I2C被禁用，芯片配置由配置引脚（DSEL， EDGE， VREF）和状态引脚（PD， DKEN）指定。将ISEL调低再调高，复位状态机。寄存器值是它们的默认值，在重置之前不会保留。12 c < / td > < / tr > < /表> < /身体> < / html >

表4 - 1。引脚函数（续）

<html><body><table><tr><td colspan="2">PIN</td><td>TYPE(1)</td><td>DESCRIPTION</td></tr><tr><td>NAME</td><td>NO.</td><td></td><td>bus select/I²C input. The operation of this pin depends on whether the 12C interface is Input clock</td></tr><tr><td>BSEL/SCL</td><td>15</td><td></td><td>enabled or disabled. This pin iS only 3.3V tolerant. When |2C is disabled (ISEL low), a high level selects 24-bit input, single-edge input mode. A low level selects 12-bit input, dual-edge input mode. When |2C is enabled (ISEL = high), this pin functions as the I2C clock input (see Section 6.6). In this configuration, this pin has an open-drain output that requires an external 5kΩ pullup resistor</td></tr><tr><td>DSEL/SDA</td><td>14</td><td>I/O</td><td>connected to VDD. DSEL/I2C data. The operation of this pin depends on whether the 12C interface is enabled or disabled. This pin is only 3.3V tolerant. When 12C is disabled (ISEL = low), this pin is used with BSEL and VReF to select the single-ended or differential input clock mode (see Table 6-1). When |2C is enabled (ISEL = high), this pin functions as the I2C bidirectional data line. In this configuration, this pin has an open-drain output that requires an external 5kQ pullup resistor connected to VDD.</td></tr><tr><td>EDGE/HTPLG</td><td></td><td></td><td>Edge select/hot plug input. operation this pin depends on whether the I2C interface is enabled or disabled. input is 3.3V tolerant only. When 12C is disabled (ISEL low), a high level selects the primary latch to occur on the rising edge of the input clock IDCK+. A low level selects the primary latch to occur on the falling edge of the input clock IDCK+ This is the case for both single-ended and differential input clock modes. When I²C is enabled (ISEL = high), pin is used to monitor the hot plug detect signal. When used for hot-plug detection pin requires a series The Of This this this</td></tr><tr><td>DKEN</td><td>35</td><td></td><td>1kΩ resistor. Data de-skew enable. The de-skew function can be enabled either through I²C or by this pin when I2C is disabled. When de-skew is enabled, the input clock to data setup/hold time can be adjusted in discrete trim increments. The amount of trim per increment is defined by t(sTEP). When I2C is disabled (ISEL = |ow), a high level enables de-skew with the trim increment determined by DK[3:1] (see Section 6.4.2). low level disables de-skew and the default trim setting is used. When |2C is enabled (ISEL = high), the value of DKEN and the trim increment are selected through pins 4</td></tr><tr><td>VREF</td><td></td><td></td><td>I2C. In this configuration, the DKEN pin should be tied to either GND or VDD to avoid a floating input. Input reference voltage. Selects the swing range of the digital data inputs (DATA[23:0], DE, HSYNC, VSYNC, and IDCK±). For high-swing 3.3V input signal levels, VReF should be tied to VDD. For low-swing input signal levels, VREF should be set to half of the maximum input voltage level. See Section 5.3 for the allowable range for VREF. The desired VREF voltage level is typically derived using simple voltage-divider circuit. Power down (active low). In the powerdown state, only the digital I/O buffers and |2C interface</td></tr><tr><td>PD</td><td>10</td><td>When When configuration, the Note: enabled or</td><td>remain active. 12C is disabled (ISEL low), high level selects the normal operating mode. A low level selects the powerdown mode. |2C is enabled (ISEL = high), the power-down state is selected through |2C. In this PD pin should be to GND. The default register value for PD is low, so the device is in powerdown mode when I2C is first after an I2C RESET. a tied</td></tr><tr><td colspan="4">RESERVED</td></tr><tr><td colspan="4">DVI DIFFERENTIAL SIGNAL OUTPUT PINS</td></tr><tr><td>TXO+ TXO-</td><td>25 24</td><td>Channel 0 DVI</td><td>differential output pair. TX0± transmits the 8-bit blue pixel data during active video andHSYNC and VSYNC during the blanking interval.</td></tr><tr><td>TX1+</td><td>28 27</td><td>Channel O</td><td>DVI differential output pair. TX1± transmits the 8-bit green pixel data during active video</td></tr><tr><td>TX1-</td><td>31</td><td>Channel 2</td><td>and CTL[1] during the blanking interval. DVI pair.</td></tr><tr><td>TX2+ TX2-</td><td>30</td><td>O CTL[3:</td><td>differential output TX2± transmits the 8-bit red pixel data during active video and during the blanking interval.</td></tr></table></body></html>

表4 - 1。引脚函数（续）

<html><body><table><tr><td colspan="2">PIN</td><td rowspan="2">TYPE(1)</td><td rowspan="2">DESCRIPTION</td></tr><tr><td>NAME</td><td>NO.</td></tr><tr><td>TXC+ TXC-</td><td>22 21</td><td>DVI</td><td>differential output clock.</td></tr><tr><td>TFADJ</td><td>19</td><td></td><td>Full-scale adjust. This pin controls the amplitude of the DVI output voltage swing, determined by the value of the pullup resistor connected to RTFADJ TVDD·</td></tr><tr><td colspan="4">POWER AND GROUND PINS</td></tr><tr><td>DVDD</td><td>1, 12, 33</td><td>Power</td><td>Digital power supply. Must be set to 3.3V nominal.</td></tr><tr><td>PVDD</td><td>18</td><td>Power</td><td>PLL power supply. Must be set to 3.3V nominal.</td></tr><tr><td>TVDD</td><td>23, 29</td><td>Power</td><td>Transmitter differential output driver power supply. Must be set to 3.3V nominal.</td></tr><tr><td>DGND</td><td>16, 48, 64</td><td>Ground</td><td>Digital ground</td></tr><tr><td>PGND</td><td>17</td><td>Ground</td><td>PLL ground</td></tr><tr><td>TGND</td><td>20, 26,32</td><td>Ground</td><td>Transmitter differential output driver ground</td></tr><tr><td>NC</td><td>49</td><td>NC No</td><td>connection required. If connected, tie high.</td></tr></table></body></html>

(1) $\mid \mid =$ input, $0 =$ output, NC $\mathbf { \tau } = \mathbf { \tau }$ no conection, ${ \sf P } =$ power, $\mathsf { G } =$ ground

# 5 Specifications 5.1 Absolute Maximum Ratings

超过自由空气工作温度范围（除非另有说明）(1)

表身体< html > < > < > < tr > < td colspan = " 2 " > < / td > < td > MIN < / td > < td > MAX < / td > < td >单位< / td > < / tr > < tr > < td > DVDD, PVDD, TVDD < / td > < td > < / td >电源电压范围-0.5 < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td >输入电压,逻辑/模拟信号< / td > < td > -0.5 < / td > < td > < / td > < td > V < / td > < / tr > < tr > < td > Rτ< / td > < td >外部DVI终止单端电阻< / td > < td > 0打开< / td > < td >电路< / td > < td >Ω< / td > < / tr > < tr > < td > < / td > < td >外部TFADJ阻力,RTFADJ < / td > < td > 300 < / td > < td >开路< / td > < td >Ω< / td > < / tr > < tr > < td > < / td > < td >情况下温度10秒钟< / td > < td > < / td > < td > 260 < / td > < td >°C < / td > < / tr > < tr > < td > < / td > < td >电平封闭(EIA / JESD78) < / td > < td > < / td > < td > 100 < / td >马< td > < / td > < / tr > < tr > < td >测试< / td > < td >存储温度< / td > < td > < / td > < td > 260 < / td > < td >°C < / td > < / tr > < /表> < /身体> < / html >

(1)超出绝对最大额定值的应力可能会对设备造成永久性损坏。这些仅是压力额定值，并不意味着设备在这些或超出推荐工作条件的任何其他条件下的功能操作。长时间暴露在绝对最大额定条件下可能会影响设备的可靠性。

# 5.2 ESD等级

表身体< html > < > < > < tr > < td colspan =“3”> < / td > < td >价值< / td > < td >单位< / td > < / tr > < tr > < td行宽=“2”> V (ESD)静电放电< / td > < td行宽=“2”>人体模型(HBM),每个ANSI / ESDA /电平js - 001 (1) < / td > < td > DVI别针< / td > < td >±4000 < / td > < td行宽=“2”> V < / td > < / tr > < tr > < td >其他别针< / td > < td >±2000 < / td > < / tr > < /表> < /身体> < / html >

(1) JEDEC文件JEP155指出，500-V HBM允许使用标准ESD控制过程进行安全制造。

# 5.3推荐操作条件

超过自由空气工作温度范围（除非另有说明）

表身体< html > < > < > < tr > < td colspan =“3”> < / td > < td > MIN < / td > < td >笔名< / td > < td > MAX < / td > < td >单位< / td > < / tr > < tr > < td colspan =“3”>电源电压(DVDD,VDD PVDD TVDD), < / td > < td > 3.0 < / td > < td > 3.3 < / td > < td > 3.6 < / td > < td > < / td > < / tr > < tr > < td > VREF < / td > < td行宽=“2”>输入参考电压< / td > < td >低振幅模式< / td > < td > 0.55 < / td > < td > VDDQ / 2 (1) < / td > < td > 0.9 < / td > < td > V < / td > < / tr > < tr > < td > < / td > < td >高振幅模式< / td > < td > < / td > < td > < / td > < td > DVDD < / td > < td > V < / td > < / tr > < tr > < td > AVDD < / td > < td > DVI终止电源电压(2)< / td > < td > DVI接收机< / td > < td > 3.14 < / td > < td > 3.3 < / td > < td > 3.46 < / td > < td > V < / td > < / tr > < tr > < td > Rτ< / td > < td > DVI单端终止电阻(3)< / td > < td > DVI接收机< / td > < td > 45 < / td > < td > 50 < / td > < td > 55 < / td > < td >Ω< / td > < / tr > < tr > < td > R (TFADJ) < / td > < td > TFADJ电阻器DVI-compliant V(摇摆)范围< / td > < td > 400 mv = 600 mv V(摇摆)< / td > < td > 505 < / td > < td > 510 < / td > < td > 515 < / td > < td >Ω< / td > < / tr > < tr > < td > TA < / td > < td colspan =“2”>操作自由空间温度范围< / td > < td > 0 < / td > < td > 25 < / td > < td > 70 < / td > < td >°C < / td > < / tr > < /表> < /身体> < / html >

(1) $V _ {\mathsf {D D Q}}$定义了最大低电平输入电压，它不是实际输入电压。(2) $\mathsf {A V} _ {\mathsf {D D}}$为DVI链路的终端电源电压。(3) $\mathsf {R} _ {\mathsf {T}}$为DVI链路接收端单端终端电阻。

# 5.4热信息

表身体< html > < > < > < tr > < td行宽=“2”>热< / td > < td行宽=“2”>指标(1)< / td > < td > TFP410 PAP < / td > < td行宽=“2”>单位< / td > < / tr > < tr > < td > 64针< / td > < / tr > < tr > < td >雷亚< / td > < td > Junction-to-ambient热阻< / td > < td > 26.6 < / td > < td行宽=“6”>°C / W < / td > < / tr > < tr > < td > ReJC(上)< / td > < td > Junction-to-case(上)热阻< / td > < td > 14.1 < / td > < / tr > < tr > < td > RθJB < / td > < td > Junction-to-board热阻< / td > < td > 11.3 < / td > < / tr > < tr > < td > 4 jt Junction-to-top < / td > < td >描述0.4参数< / td > < td > < / td > < / tr > < tr > < td > 4 jb < / td > < td > Junction-to-board表征参数< / td > < td > 11.2 < / td > < / tr > < tr > < td > ReJC (bot) < / td > < td > Junction-to-case(底部)热阻< / td > < td > 0.9 < / td > < / tr > < /表> < /身体> < / html >

(1)有关传统和新型热指标的更多信息，请参阅IC封装热指标应用报告（SPRA953）。

# 5.5电气特性

表身体< html > < > < > < tr > < td colspan =“2”>参数< / td > < td >测试条件< / td > < td > MIN < / td > < td > TYP < / td > < td > MAX < / td > < td >单位< / td > < / tr > < tr > < td colspan = " 2 " > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td行宽=“2”>直流规格HSYNC, < / td > < td行宽=“2”>高级输入电压(数据、DE VSYNC,和IDCK + / - CMOS输入)< / td > < td > VREF = DVDD < / td > < td > 0.7 vdd < / td > < td > < / td > < td > < / td > < td行宽= " 2 " > < / td > < / tr > < tr > < td > 0.5 V≤≤0.95 V < / td > < td > + 0.2 VREF < / td > < td > < / td > < td > < / td > < / tr > < tr > < td行宽=“2”> VH < / td > < td行宽=“2”>高级输入电压(所有其他CMOS输入)< / td > < td > < / td > < td > 0.7 vdd < / td > < td > < / td > < td > < / td > < td行宽= " 2 " > < / td > < / tr > < tr > < td > = DVDD VREF < / td > < td > < / td > < td > < / td > < td > 0.3 vdd < / td > < / tr > < tr > < td行宽= " 2 " > < / td > < td >低输入电压(数据、DE VSYNC HSYNC,和IDCK + / - CMOS输入)< / td > < td > 0.5 V≤≤0.95 V < / td > < td > < / td > < td > < / td > < td > 0.2 VREF < / td > < td行宽= " 2 " > < / td > < / tr > < tr > < td >低输入电压(所有其他CMOS输入)< / td > < td > < / td > < td > < / td > < td > < / td > < td > 0.3 VDD < / td > < / tr > < tr > < td > VoH < / td > < td >高级数字输出电压(明渠输出)< / td > < td > 3 V, IoH = 20μVDD < / td > < td > 2.4 < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td >卷< td >低级数字输出电压(明渠输出)< / td > < td > = 3.6 V,术后4马VDD < / td > < td > < / td > < td > < / td > < td > 0.4 < / td > < td > V < / td > < / tr > < tr > < td > IH < / td > < td >高级输入电流< / td > < td > V1 = 3.6 V < / td > < td > < / td > < td > < / td > < td > 25±< / td > < td >μ一个< / td > < / tr > < tr > < td > IL < / td > < td >低输入电流< / td > < td行宽=“5”> V = 0 AVDD = 3.3±5%,Rτ(1)= d 50Ω±10%,= 510Ω±1% RTFADJ < / td > < td > < / td > < td > < / td > < td > 25±< / td >马< td > < / td > < / tr > < tr > < td > VH < / td > < td > DVI单端高级输出电压< / td > < td > < / td > < td > -0.01 AVDD < / td > < td > AVDD 0.01×< / td > < td > V < / td > < / tr > < tr > < td >六世< / td > < td > DVI单端低输出电压< / td > < td > AVDD < / td > < td > -0.6 < / td > < td > AVDD 0.4 < / td > < td > V < / td > < / tr > < tr > < td >甚短波< / td > < td > DVI单端输出摆动电压< / td > < td > 400 < / td > < td > < / td > < td > 600 < / td > < td > mVp-p < / td > < / tr > < tr > < td > VoFF < / td > < td > DVI备用/关单端输出电压电流(3)< / td > < td > AVDD -0.01 < / td > < td > < / td > < td > AVDD + 0.01 < / td > < td > V < / td > < / tr > < tr > < td > IPD IDD < / td > < td >省电电源电流< / td > < td >最糟糕的模式(2)< / td > < td > < / td > < td > 200 < / td > < td > 500 < / td > < td >μ一个< / td > < / tr > < tr > < td >正常功率交流规范< / td > < td colspan = " 2 " > < / td > < td > < / td > < td > 200 < / td > < td > 250 < / td >马< td > < / td > < / tr > < tr > < td colspan =“6”> IDCK频率< / td > < td > < / td > < / tr > < tr > < td > tr f (IDCK) < / td > < td > DVI输出上升时间(20 - 80%) (4) < / td > < td > < / td > < td > 25 < / td > < td > < / td > < td > 165 < / td > < td > MHz < / td > < / tr > < tr > < td > tf < / td > < td > DVI输出下降时间(20 - 80%)(4)< / td > < td行宽= 4 > f (DCK) = 165 MHz < / td > < td行宽= " 4 " > < / td > < td > 75 < / td > < td > 240 < / td > < td > p < / td > < / tr > < tr > < td > DVI < / td > < td > < / td > < td > 75 < / td > < td > 240 < / td > < td > p < / td > < / tr > < tr > < td > (ct (D) D:) < / td > < td > intra-pair +输出微分见图5 - 4斜(5),< / td > < td > 50 < / td > < td > < / td > < td > p < / td > < / tr > < tr > < td > tojit < / td > < td > DVI输出时钟抖动,马克斯。(6) < / td > < td > < / td > < td > 150 < / td > < td > p < / td > < / tr > < tr > < td >(步骤)< / td > < td > De-skew修剪增量< / td > < td > DKEN = 1 < / td > < td > < / td > < td > 350 < / td > < td > < / td > < td > p < / td > < / tr > < /表> < /身体> < / html >

(1) ${\sf R} _ {\sf T}$为DVI链路接收端单端终端电阻（234）黑白棋盘图案，每个棋盘宽为1像素。假设发送器的所有输入都没有切换。上升和下降时间测量为信号幅度的20%和80%之间的时间。（56）使用$ 10 0 0 \mathsf {K} +$输入时钟作为触发器，在$50 \%$交叉点差分测量。相对于输入时钟（IDCK）。

# 5.6定时要求

表身体< html > < > < > < tr > < td colspan =“3”> < / td > < td > MIN < / td > < td >笔名< / td > < td > MAX < / td > < td >单位< / td > < / tr > < tr > < td colspan =“3”> t(像素))像素时期(1)< / td > < td > 6.06 < / td > < td > < / td > < td > 40 < / td > < td > ns < / td > < / tr > < tr > < td colspan =“3”> t (IDCK) IDCK责任周期< / td > < td > 30% < / td > < td > < / td > < td > 70% < / td > < td > < / td > < / tr > < tr > < td colspan =“2”> t (ji) IDCK时钟抖动容忍< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td >啧啧(cc) DVI输出inter-pair或< / td > < td > channel-to-channel斜(2)见图5 - 2f (IDCK) = 165 mhz < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td > tsu (DF) < / td > < td > Ptu,DEVSYNC, HSYNC IDCK +下降沿设置时间,看到< / td > < td > DSEL = 0, < / td > < td > 1.2 < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td > th (1 df) < / td > < td > HSYNC持有时间IDCK +下降沿,看< / td > < td >单边缘(BSEL = 1, DSEL = 0, < / td > < td > 1.3 < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td >数据,tsu (IDR) < / td > < td >, VSYNC, HSYNC设置时间IDCK +上升边,见图5 - 2 < / td > < td > DKEN = 0,边缘= 1)< / td > < td > 1.2 < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td >数据,th博士(1)< / td > < td >, VSYNC,HSYNC持有时间IDCK +上升边,见图5 - 2 < / td > < td > < / td > < td > 1.3 < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < /表> < /身体> < / html >

5.6时间要求（续）

<html><body><table><tr><td colspan="3"></td><td rowspan="2">MIN</td><td rowspan="2">NOM</td><td rowspan="2">MAX</td><td rowspan="2">UNIT</td></tr><tr><td>tsu(D))</td><td>数据、DE、VSYNC、HSYNC设置时间到IDCK+下降/上升沿，见图5-3</td><td>双边缘(BSEL=0， DSEL=1，DKEN = 0) < / td > < / tr > < tr > < td行宽=“2”> th (ID) < / td > < td > HSYNC持有时间IDCK +下降/上升边缘,< / td > < td > < / td > < td > 0.9 < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < tr > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > ns < / td > < / tr > < /表> < /身体> < / html >

（21） $\mathfrak {t} _ {(\mathsf {pixel})}$是定义为TXC输出时钟周期的像素时间。IDCK的周期为t（像素）。使用$ 10 0 0 \mathsf {K} +$输入时钟作为触发器，在$50 \%$交叉点差分测量。

DVI 80% VOD tf输出

![](images/fb01e5407d7a02ae0a0f913218029628eac47c326a9bfea479654f9e6f806ff7.jpg)  
图5 - 1。DVI输出的上升和下降时间

![](images/494880ec964cae70fc33a567eaeafc74496b5ac6e8624895b781186e6947d833.jpg)  
图5 - 2。控制和单侧数据设置/保持时间为IDCK±

![](images/818ec8158180fca8eb3f3f716d2db71ffded98c25e098e77302b1f20b1f4ea00.jpg)  
图5 - 3。双边缘数据设置/保持时间IDCK+
图5 - 4。模拟输出对内±差分倾斜
图盘中。模拟输出通道到通道的倾斜

TXN 50%××-×-X tsk(CC) TXM 50%

5.7典型特征

![](images/6aa7791af77406021c46525c5efebb96c60bf01cc42c28524ee7fecc56f625ed.jpg)  
图5 - 6。RTFDAJ vs . Vswing

6.1概述

TFP410是一款符合dvi标准的数字发射机，用于数字主机监控系统中，用于tmd编码和序列化RGB像素数据流。TFP410支持从VGA到WUXGA（和1080p）的分辨率，可以通过两种方式控制：

1. 配置和状态引脚
2. 可编程的$\mathsf {I} ^ {2} \mathsf {C}$串行接口（见表6-1）

数字显示系统中的主机，通常是PC或消费电子设备，包含一个dvi兼容的发射器，如TI TFP410，它接收24位像素数据以及适当的控制信号。TFP410将信号编码成高速、低电压、差分串行比特流，优化后通过双绞线电缆传输到显示设备。显示设备，通常是平板显示器，需要像TI TFP401这样的DVI兼容接收器，将串行比特流解码回相同的24位像素数据和来自主机的控制信号。解码后的数据可以直接应用于平板驱动电路，在显示器上产生图像。由于主机与显示器之间的距离可达5米或更远，因此首选串行传输像素数据（参见第6.3.1节、6.3.2节和6.3.3节）。

TFP410集成了一个高速数字接口、一个tm.d.s.编码器和三个差分tm.d.s.驱动器。数据通过12或24条数据线驱动到TFP410编码器，以及差分时钟对和同步信号。TFP410的灵活性允许多种时钟和数据格式，提高系统性能。

TFP410还具有增强的锁相环抗扰性，通过片上稳压器和旁路电容器实现了增强。

TFP410是多功能和高度可编程的，为用户提供最大的灵活性。提供$\mathsf {I} ^ {2} \mathsf {C}$主机接口，除了通过引脚带电阻编程的上电默认设置外，还允许增强配置。

TFP410通过接收器检测提供监视器检测，或者在启用$1 ^ {2}\mathsf {C}$时提供热插拔检测。监视器检测功能允许用户在连接数字显示器或接收器时增强灵活性（参见第6.4.3节和第6.6节）。

TFP410具有数据去斜特性，允许用户相对于IDCK±去斜输入数据（见第6.4.2节）。

＃ 6.2功能框图

![](images/a846376971ec2aa04f9e1d706aa28e399378029ccfaf06e09ad20565c00f02e4.jpg)

＃ 6.3特性描述

＃ 6.3.1 T.M.D.S.像素数据和控制信号编码

对于过渡最小化差分信号（T.M.D.S.），在给定的时间，对于给定的像素，只有两个可能的T.M.D.S.字符中的一个被传输。发送器保持先前发送的1和0的数量的运行计数，并发送使转换数量最小化并近似于传输线的直流平衡的字符。三个T.M.D.S.通道用于在活动视频间隔$( \mathsf { D } \mathsf { E } = \mathsf { H i g h } )$期间传输RGB像素数据。这些相同的三个通道也用于传输HSYNC， VSYNC和三个控制信号，CTL[3:1]，在非活动显示或消隐间隔$\begin{array} { r } { ( \mathsf { D } \mathsf { E } = \mathsf { L o w } , } \end{array}$。下表将传输的输出数据映射到dvi兼容系统中适当的T.M.D.S.输出通道。

<html><body><table><tr><td>输入 PINS (VALID FOR DE = High)</td><td>输出通道</td><td>传输像素数据主动显示（DE =高）</td></tr><tr><td>DATA[23:16]</td><td>通道2 （TX2±）</td><td>红色[7:0]</td></tr><tr><td>DATA[15:8]</td><td>绿色[7:0]</td></tr><tr><td>DATA[7:0]</td><td>蓝色[7:0]</td></tr><tr><td>输入pin (VALID FOR DE = Low)</td><td>T.M.D.S.输出通道</td></ td>传输控制数据空白间隔(DE = Low)</td></tr><tr><td>CTL2(1)</td><td>通道2（TX2±）</td><td>CTL[2]</td></tr><tr><td>CTL1 (1)</td><td>通道1 （TX1±）</td><td>CTL[1]</td></tr><tr><td>HSYNC， VSYNC</td></tr></table></ html>

(1) TFP410在垂直下料间隔期间对CTL[2:1]输入进行编码和传输。CTL3是为HDCP保留的，并且总是被编码为0。CTL[2:1]输入被保留以供将来使用。当DE $\mathbf {\tau} = \mathbf {\tau}$ high时，CTL和SYNC引脚必须保持不变。

# 6.3.2通用图形控制器接口电压信号电平

通用图形控制器接口可以在以下两种不同的电压模式下工作：

使用标准3.3V CMOS信号电平的高摆幅模式。
低摆幅模式，其中可调1.1V至1.8V信号电平使用。

要选择高摆幅模式，$V _ {\mathsf {R E F}}$输入引脚必须连接到3.3V电源。

要选择低摆幅模式，$V _ {\mathsf {R E F}}$必须为0.55 ~ 0.95V。

在低摆幅模式下，$V _ {\mathsf {R E F}}$用于设置可调信令电平的中点。VREF的允许取值范围从0.55V到$0。9 \mathsf {V}。典型的方法是通过使用简单的分压器电路从片外提供此功能。低摆幅模式下允许的最小输入信号摆幅为$\mathsf {V} _ {\mathsf {R E F}} \pm 0。2 \mathsf {V}。在低摆幅模式下，$V _ {\mathsf {R E F}}$输入对所有差分输入接收器是通用的。

# 6.3.3通用图形控制器接口时钟输入

TFP410的通用图形控制器接口支持全差分和单端时钟输入模式。在差分时钟输入模式下，通用图形控制器接口使用${\mathsf {IDCK}} +$和IDCK−信号之间的交叉点作为锁存输入数据（data [23:0]， DE， HSYNC和VSYNC）的时序参考。差分时钟输入提供更大的共模噪声抑制。差分时钟输入模式仅在低摆幅模式下可用。单端时钟输入模式下，${\mathsf {IDCK}} +$ 输入（引脚57）接单端时钟源，IDCK−输入（引脚56）接GND。

TFP410的通用图形控制器接口提供可选的12位双边缘和24位单边缘输入时钟模式。在12位双边缘中，12位数据锁存于输入时钟的每个边缘上。在24位单沿模式下，当edge $= 1$时，24位数据锁存于输入时钟的上升沿，当edge $= 0$时，24位数据锁存于输入时钟的下降沿。

DKEN和DK[3:1]允许用户补偿IDCK±与像素数据和控制信号之间的偏差。具体信息如表6-10所示。

# 6.4设备功能模式

# 6.4.1通用图形控制器接口模式

表6-1是通用图形控制器接口的不同模式的表格表示。当BSEL $_{。= 0}$和24位模式时，BSEL $_{。= 1}$。12位模式使用双边缘时钟，24位模式使用单边缘时钟。EDGE输入用于在24位模式下控制锁存边，或在12位模式下控制主锁存边。当EDGE $\ mathool {\mathop:} = 1$时，数据输入锁存于输入时钟的上升沿；当$E D G E = 0$时，数据输入锁存于输入时钟的下降沿。全差分输入时钟仅在低摆幅模式下可用。在低摆幅模式下不建议使用单端时钟，因为这会降低共模噪声抑制。

请注意，当$1 ^ {2}{\mathsf {C}}$启用时，BSEL， DSEL和EDGE由寄存器CTL_1_MODE决定（$\angle B EL = 1$），当$\mathsf {I} ^ {2} \mathsf {C}$禁用时，由输入引脚决定)$ (1 5 EL = 0)$。

表6 - 1。通用图形控制器接口选项（表格表示）

表身体< html > < > < > < tr > < td > VREF < / td > < td > BSEL < / td > < td >边缘< / td > < td > DSEL < / td > < td >总线宽度< / td > < td >锁模式td > < /时钟上升沿< td > < / td > < td >时钟模式< / td > < / tr > < tr > < td > 0.55 v - 0.9 v < / td > < td > 0 < / td > < td > < / td > < td > 0 < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >下降< td >微分(1)(2)< / td > < / tr > < tr > < td > 0.55 v -0.9 v < / td > < td > 0 < / td > < td > 00 < / td > < td > 1 < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >下降< td >单端< / td > < / tr > < tr > < td > 0.55 v-0.9 v < / td > < td > 0 < / td > < td > 1 < / td > < td > 0 < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >上升< td >微分(1)(2)< / td > < / tr > < tr > < td > 0.55 v一0.9 v < / td > < td > 0 < / td > < td > 1 < / td > < td > 1 < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >上升< td >单端< / td > < / tr > < tr > < td > 0.55 v - 0.9 v < / td > < td > 1 < / td > < td > 0 < / td > < td > 0 < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >下降< td >单端< / td > < / tr > < tr > < td > 0.55 v - 0.9 v < / td > < td > 1 < / td > < td > 0 < / td > < td > 1 < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >下降< td >微分(1)(3)< / td > < / tr > < tr > < td > 0.55 v0.9 v < / td > < td > 1 < / td > < td > 1 < / td > < td > 0 < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >上升< td >单端< / td > < / tr > < tr > < td > 0.55 v - 0.9 v < / td > < td > 1 < / td > < td > < / td > < td > 1 < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >上升< td >微分(1)(3)< / td > < / tr > < tr > < td > DVDD < / td > < td > 0 < / td > < td > 0 < / td > < td > X < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >下降< td >单教学(4)< / td > < / tr > < /表> < /身体> < / html >

表6 - 1。通用图形控制器接口选项（表格表示）（续）

表身体< html > < > < > < tr > < td > VREF < / td > < td > BSEL < / td > < td >边缘< / td > < td > DSEL < / td > < td >总线宽度< / td > < td >锁模式td > < /时钟上升沿< td > < / td > < td >时钟模式< / td > < / tr > < tr > < td > DVDD < / td > < td > 0 < / td > < td > < / td > < td > X < / td > < td > 12位< / td > < td > Dual-edge < / td > < td > < / td >上升< td >单端(4)< / td > < / tr > < tr > < td > DVDD < / td > < td > 1 < / td > < td > 0 < / td > < td > X < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >下降< td >单端(4)< / td > < / tr > < tr > < td > DVDD < / td > < td > < / td > < td > < / td > < td > X < / td > < td > 24位< / td > < td >单刃< / td > < td > < / td >上升< td >单教学(4)< / td > < / tr > < /表> < /身体> < / html >

(1)差分时钟输入方式只在低信号摆幅模式（即$\mathsf { V } _ { \mathsf { R E F } } \leq 0 . 9 \mathsf { V } ;$）下可用。
（23） TFP410不支持12位双时钟、单边输入时钟模式。TFP410不支持24位单时钟、双边输入时钟模式。
(4)在高摆模式下$( \mathsf { V } _ { \mathsf { R E F } } = \mathsf { D V } _ { \mathsf { D D } } ) ,$)， DSEL是一个不在乎；因此，设备始终处于单端锁存模式。

![](images/4298216265734f7cc5d6a5862961ae762a724f87dcef6c6a1f275c2008350e7e.jpg)  
12位，双边缘输入模式（BSEL $\mathbf { \sigma } = \mathbf { 0 }$）

![](images/1ba49c1fe6bcb87d99e4da1728fad2f78bf67d29864124cc029bc1525f8e5d39.jpg)  
图6-1通用图形控制器接口选项12位模式（图形表示）

Figure 6-2。24位模式通用图形控制器接口选项（图形表示）

桌子写成6-2。12位模式数据映射

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利rowspan =“3”> NAME & lt; /戴利电池的小于戴利colspan =“2”> > P0 & lt; /戴利的小于戴利colspan =“2”> > P1 & lt; /戴利的小于戴利>colspan =“2”> P2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> POL & lt; /戴利的小于戴利> > POH & lt; /戴利的小于戴利> > P1L & lt; /戴利的小于戴利> > P1H & lt; /戴利的小于戴利> > P2L & lt; /戴利的小于戴利> > P2H & lt; /戴利> & lt; / tr >小于戴利>灯光昏暗的小于tr > & lt; /戴利> & lt;戴利在曼哈顿高& lt; /戴利的小于戴利> > >灯光昏暗& lt; /戴利> & lt;戴利在曼哈顿高& lt; /戴利的小于戴利> > >灯光昏暗& lt; /戴利的小于戴利在曼哈顿高& lt; /戴利> > > & lt; / tr > & lt tr > & lt;戴利> D11 & lt; /戴利的小于戴利> > G0黑三铝& lt; /戴利的小于戴利> > R0黑七铝& lt; /戴利的小于戴利> > G1钠黑三铝& lt; /戴利的小于戴利> > R1黑七铝& lt; /戴利的小于戴利> > G2黑三铝& lt; /戴利的小于戴利> >,R2黑七铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D10 & lt; /戴利的小于戴利> > G0黑两个铝& lt; /戴利的小于戴利> > R0黑6铝& lt; /戴利的小于戴利> > G1钠黑两个铝& lt; /戴利的小于戴利> > R1黑6铝& lt; /戴利的小于戴利> > G2黑两个铝& lt; /戴利的小于戴利> > 6,R2黑铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D9 & lt; /戴利的小于戴利> > G0黑一个铝& lt; /戴利的小于戴利> > R0黑5铝& lt; /戴利的小于戴利> > G1钠黑一个铝& lt; /戴利的小于戴利> > R1黑5铝& lt; /戴利的小于戴利> > G2黑一个铝& lt; /戴利的小于戴利> >,R2黑5铝& lt; /戴利> & lt; /小于戴利tr > & lt tr > > D8 & lt; /戴利的小于戴利> > G0黑0铝& lt; /戴利的小于戴利> > R0黑4铝& lt; /戴利的小于戴利> > G1钠黑0铝& lt; /戴利的小于戴利> > R1黑4铝& lt; /戴利的小于戴利> > G2黑0铝& lt; /戴利的小于戴利> >,R2黑4铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D7 & lt; /戴利的小于戴利> > B0黑七铝& lt; /戴利的小于戴利> > R0黑三铝& lt; /戴利的小于戴利> > B1黑七铝& lt; /戴利的小于戴利> > R1黑三铝& lt; /戴利的小于戴利> > B2黑七铝& lt; /戴利的小于戴利> >,R2黑三铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D6 & lt; /戴利的小于戴利> > B0黑6铝& lt; /戴利的小于戴利> > R0黑两个铝& lt; /戴利的小于戴利> > B1黑6铝& lt; /戴利的小于戴利> > R1黑两个铝& lt; /戴利的小于戴利> > B2黑6铝& lt; /戴利的小于戴利> >,R2黑两个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D5 & lt; /戴利的小于戴利> > B0黑5铝& lt; /戴利的小于戴利> > R0黑一个铝& lt; /戴利的小于戴利> > B1黑5铝& lt; /戴利的小于戴利> > R1黑一个铝& lt; /戴利的小于戴利> > B2黑5铝& lt; /戴利的小于戴利> >,R2黑一个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D4 & lt; /戴利的小于戴利> > B0黑4铝& lt; /戴利的小于戴利> > R0黑0铝& lt; /戴利的小于戴利> > B1黑4铝& lt; /戴利的小于戴利> > R1黑0铝& lt; /戴利的小于戴利> > B2黑4铝& lt;/戴利的小于戴利> >,R2黑0铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D3 & lt; /戴利的小于戴利> > B0黑三铝& lt; /戴利的小于戴利> > G0黑七铝& lt; /戴利的小于戴利> > B1黑三铝& lt; /戴利的小于戴利> > G1钠黑七铝& lt; /戴利的小于戴利> > B2黑三铝& lt; /戴利的小于戴利> > G2黑七铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D2 & lt; /戴利的小于戴利> > B0黑两个铝& lt; /戴利的小于戴利> > G0黑6铝& lt; /戴利的小于戴利> > B1黑两个铝& lt; /戴利的小于戴利> > G1钠黑6铝& lt; /戴利的小于戴利> > B2黑两个铝& lt; /戴利的小于戴利> > G2黑6铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利的小于/戴利的小于戴利> > > D1 B0黑一个铝& lt; /戴利的小于戴利> > G0黑5铝& lt; /戴利的小于戴利> > B1黑一个铝& lt; /戴利的小于戴利> > G1钠黑5铝& lt; /戴利的小于戴利> > B2黑一个铝& lt; /戴利的小于戴利> > G2黑5铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D0 & lt; /戴利的小于戴利> > B0黑0铝& lt; /戴利的小于戴利> > G0黑4铝& lt; /戴利的小于戴利> > B1黑0铝& lt; /戴利的小于戴利> > G1钠黑4铝& lt; /戴利的小于戴利> > B2黑0铝& lt; /戴利的小于戴利> > G2黑4铝& lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

桌子写成9。24位模式数据映射

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> NAME & lt; /戴利的小于戴利> >电池P0 & lt; /戴利的小于戴利> > P1 & lt; /戴利的小于戴利> > P2 & lt; /戴利的小于戴利> >电池NAME & lt; /戴利的小于戴利> > P0 & lt; /戴利的小于戴利> > P1 & lt; /戴利的小于戴利> > P2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D23 & lt; /戴利的小于戴利> > R0黑七铝& lt; /戴利的小于戴利> > R1黑七铝& lt; /戴利的小于戴利> >,R2黑七铝& lt; /戴利的小于戴利> > D11 & lt; /戴利的小于戴利> > G0黑三铝& lt; /戴利的小于戴利> > G1钠黑三铝& lt; /戴利的小于戴利> > G2黑三铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D22 & lt; /戴利的小于戴利> > R0黑6铝& lt; /戴利的小于戴利> > R1黑6铝& lt; /戴利的小于戴利> > 6,R2黑铝& lt; /戴利的小于戴利> > D10 & lt; /戴利的小于戴利> > G0黑两个铝& lt; /戴利的小于戴利> > G1钠黑两个铝& lt; /戴利的小于戴利> > G2黑两个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D21 & lt; /戴利的小于戴利> > R0黑5铝& lt; /戴利的小于戴利> > R1黑5铝& lt; /戴利的小于戴利> >,R2黑5铝& lt; /戴利的小于戴利> > D9 & lt; /戴利的小于戴利> > G0黑一个铝& lt; /戴利的小于戴利> > G1钠黑一个铝& lt; /戴利的小于戴利> > G2黑一个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D20 & lt; /戴利的小于戴利> > R0黑4铝& lt; /戴利的小于戴利> > R1黑4铝& lt; /戴利的小于戴利> >,R2黑4铝& lt; /戴利的小于戴利> > D8 & lt; /戴利的小于戴利> > G0黑0铝& lt; /戴利的小于戴利> > G1钠黑0铝& lt; /戴利的小于戴利> > G2黑0铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D19 & lt; /戴利的小于戴利> > R0黑三铝& lt; /戴利的小于戴利> > R1黑三铝& lt; /戴利的小于戴利> >,R2黑三铝& lt; /戴利的小于戴利> > D7 & lt; /戴利的小于戴利> > B0黑七铝& lt; /戴利的小于戴利> > B1黑七铝& lt; /戴利的小于戴利> > B2黑七铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D18 & lt; /戴利的小于戴利> > R0黑两个铝& lt; /戴利的小于戴利> > R1黑两个铝& lt; /戴利的小于戴利> >,R2黑两个铝& lt; /戴利的小于戴利> > D6 & lt; /戴利的小于戴利> > B0黑6铝& lt; /戴利的小于戴利> > B1黑6铝& lt; /戴利的小于戴利> > B2黑6铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D17 & lt; /戴利的小于戴利> > R0黑一个铝& lt; /戴利的小于戴利> > R1黑一个铝& lt; /戴利的小于戴利> >,R2黑一个铝& lt; /戴利的小于戴利> > D5 & lt; /戴利的小于戴利> > B0黑5铝& lt; /戴利的小于戴利> > B1黑5铝& lt; /戴利的小于戴利> > B2黑5铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D16 & lt; /戴利的小于戴利> >风险黑0铝& lt; /戴利的小于戴利> > R1黑0铝& lt; /戴利的小于戴利> >,R2黑0铝& lt; /戴利的小于戴利> > D4 & lt; /戴利的小于戴利> > B0黑4铝& lt; /戴利的小于戴利> > B1黑4铝& lt; /戴利的小于戴利> > B2黑4铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D15 & lt; /戴利的小于戴利> > G0黑七铝& lt; /戴利的小于戴利> > G1钠黑七铝& lt; /戴利的小于戴利> > G2黑七铝& lt; /戴利的小于戴利> > D3 & lt; /戴利的小于戴利> > B0黑三铝& lt; /戴利的小于戴利> > B1黑三铝& lt; /戴利的小于戴利> > B2黑三铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D14 & lt; /戴利的小于戴利> > G0黑6铝& lt; /戴利的小于戴利> > G1钠黑6铝& lt; /戴利的小于戴利> > G2黑6铝& lt; /戴利的小于戴利> > D2 & lt; /戴利的小于戴利> > B0黑两个铝& lt; /戴利的小于戴利> > B1黑两个铝& lt; /戴利的小于戴利> > B2黑两个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D13 & lt; /戴利的小于戴利> > G0黑5铝& lt; /戴利的小于戴利> > G1钠黑5铝& lt; /戴利的小于戴利> > G2黑5铝& lt; /戴利的小于戴利> > D1 & lt; /戴利的小于戴利> > B0黑一个铝& lt; /戴利的小于戴利> > B1黑一个铝& lt; /戴利的小于戴利> > B2黑一个铝& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> D12 & lt; /戴利的小于戴利> > G0黑4铝& lt; /戴利的小于戴利> > G1钠黑4铝& lt; /戴利的小于戴利> > G2黑4铝& lt; /戴利的小于戴利> > D0 & lt; /戴利的小于戴利> > B0黑0铝& lt; /戴利的小于戴利> > B1黑0铝& lt; /戴利的小于戴利> > B2黑0铝& lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

# 6.4.2数据去斜特性

去倾斜功能允许调整输入设置/保持时间。具体来说，输入数据data[23:0]可以在时钟$1 0 0 \mathsf {K} \pm$的锁存边缘之前或之后进行锁存，这取决于所需的去斜量。当去倾斜使能（DKEN）被使能时，去倾斜的数量可以通过设置3位DK[3:1]来编程。当禁用时，将使用默认的反倾斜设置。为了允许最大的灵活性和易用性，当$\mathsf {I} ^ {2} \mathsf {C}$被禁用时，通过配置引脚直接访问DKEN和DK[3:1]，或者当$1 ^ {2}{\mathsf {C}}$被启用时，通过同名的寄存器访问DKEN和DK。当使用$\mathsf {I} ^ {2} \mathsf {C}$模式时，DKEN引脚应接地以避免浮动输入。

输入设置/保持时间相对于输入时钟可以变化t(CD)，如公式1所示。

$$
\mathfrak {t} _ {(\mathsf {C D})} = (\mathsf {D K} [3: 1] - 4) \times \mathfrak {t} （\mathsf {\Omega} _ {\mathsf {S t E P}}）
$$

# where

t（STEP）为调整增量量
DK[3:1]是一个从0到7的3位二进制数
$\mathfrak {t} _ {(\mathsf {C D})}$是累积的去偏量

（DK[3:1]-4）是t（STEP）在$\{-4,- 3,- 2,- 1,0,1,2,3 \}$范围内的一个乘数。因此，数据可以锁存，增量从时钟锁存边缘前的4倍t（STEP）到锁存边缘后的3倍t（STEP）。注意，输入时钟没有改变，只有相对于时钟的数据锁存的时间。

![](images/4f976de6bdd4561fceb39d7da3b1157f9ec590c6060e10ab6feb26477e17f7b8.jpg)  
图6 - 3。去偏函数的图形表示

# 6.4.3热插拔（自动连接/断开检测）

TFP410支持DVI链路的热插拔（自动连接/断开检测）。接收器感测输入（RSEN）位表示DVI接收器是否连接到${\mathsf {T X C}} +$和$T \乘以\mathsf {C} -$。HTPLG位反映通过DVI连接器连接到监视器的HTPLG引脚的当前状态。当$1 ^ {2}{\mathsf {C}}$被禁用时（ISEL $\scriptstyle = 0 ^ {\cdot}$）， RSEN值在MSEN引脚上可用。当I2C使能时，DVI链路和HTPLG检测引脚的连接状态由CTL_2_MODE寄存器提供。CTL_2_MODE寄存器的MSEL位可以用来编程MSEN输出HTPLG值、RSEN值、中断或禁用。

中断事件的源由TSEL在CTL_2_MODE寄存器中选择。中断是由所选信号的状态变化产生的。中断状态在CTL_2_MODE的MDI位中显示，可以通过MSEN引脚输出。中断继续被断言，直到1被写入MDI位，将位重置为0。将0写入MDI位没有效果。

# 6.4.4设备配置和$\mathsf {I} ^ {2} \mathsf {C}$ RESET说明

TFP410器件配置可以通过几种不同的方法进行编程，从而为用户的应用提供最大的灵活性。设备配置取决于ISEL/ RST引脚，配置引脚（BSEL， DSEL， EDGE, $V _ {\mathsf {R E F}}）$和状态引脚PD， DKEN)的状态。$\mathsf {I} ^ {2} \mathsf {C}$ bus select和$\mathsf {I} ^ {2} \mathsf {C}$ RESET （active low）是ISEL/ RST引脚上异步操作的共享函数。

保持ISEL/ RST低会导致设备配置由配置引脚（BSEL、DSEL、EDGE和VREF）和状态引脚（PD、DKEN）来设置。$\mathsf {I} ^ {2} \mathsf {C}$总线被禁用。

保持ISEL/ RST高会导致芯片配置基于$\mathsf {I} ^ {2} \mathsf {C}$寄存器中的配置位（BSEL， DSEL， EDGE）和状态位PD， DKEN)进行设置。$\mathsf {I} ^ {2} \mathsf {C}$总线已启用。

当设备在正常或断电模式下运行时，暂时将ISEL/ RST调低，然后调高，将重置${\mathsf {I}} ^ {2} {\mathsf {C}}$寄存器到其默认值。在启用$\mathsf {I} ^ {2} \mathsf {C}$时，设备配置将更改为默认上电状态。设备上电后必须复位。建议将此引脚连接到系统复位信号，该信号在上电期间为低电平，然后在所有电源完全正常后断言为高电平。

# 6.4.5 DE生成器

TFP410包含一个DE发生器，可用于在原始数据源不提供时生成内部DE信号。有几个$\mathsf {I} ^ {2} \mathsf {C}$可编程值控制DE生成器（见图6-4）。DE_CTL寄存器中的DE_GEN使能此功能。启用后，DE引脚将被忽略。

de_top和DE_LIN是行数，分别用于控制启用DE的VSYNC激活后的行数，以及DE保持活动的总行数。VSYNC的极性必须由DE_CTL寄存器中的VS_POL设置。

DE DLY和DE CNT分别是用于控制启用了DE的HSYNC激活后的像素数，以及DE保持活动的总像素数。HSYNC的极性必须由DE_CTL寄存器中的HS_POL设置。

TFP410还计算VSYNC脉冲之间的HSYNC脉冲总数，以及HSYNC脉冲之间的总像素数。这些值，即总垂直和水平分辨率，分别在V_RES和H_RES中可用。无论是否启用DE生成器，这些值在任何时候都是可用的。

![](images/59efe9b85fa77a52631bea68989e77b77ca47423a89a0ed6d74cca6f32604205.jpg)  
图6 - 4。DE生成器寄存器函数

# 6.5编程

# 6.5.1 $ 12 C$接口

$1 ^ {2} {\mathsf {C}}$接口用于访问内部TFP410寄存器。该双引脚接口由SCL时钟线和SDA串行数据线组成。基本的$\mathsf {I} ^ {2} \mathsf {C}$访问周期如图6-5和图6-6所示。

![](images/14cbd5f4bae7900a2a0389bf6adb4fd9928a6b14985a40e982c5c8ec750b4d3a.jpg)  
图6 - 5。I2C启停条件
图6尺6寸的大。交手I2C接入周期

基本访问写周期包括：

1. 启动条件
2. 目标地址周期
3. 子地址周期
4. 任意数量的数据周期
5. 停止条件

基本的访问读周期包括以下几个部分：

1. 启动条件
2.3.4. 目标写地址周期子地址周期重启条件
5. 一个目标读地址周期
6. 任意数量的数据周期
7. 停止条件

启动和停止条件如图6-5所示。当SCL高时，SDA从高到低的转变定义了启动条件。当SCL高时，SDA从低到高的转变决定了停止的条件。每个周期，数据或地址，由8位串行数据和一个接收设备生成的确认位组成。如图6-6所示，每个数据/地址周期包含9位。

SCL u SDA目标地址子地址数据停止

在启动条件之后，每个$1 ^ {2}\mathsf {C}$设备解码目标地址。如果TFP410将该地址解码为其地址，则在第九个时钟周期内通过拉低SDA线来响应确认。在随后的子地址和数据周期中，TFP410响应确认，如图6-7所示。子地址在每个数据周期后自动递增。

发送设备不能在确认周期内驱动SDA信号，以便接收设备可以将SDA信号驱动为低电平。控制器通过在断言停止条件(P)之前保持SDA信号高来指示不确认条件$(\ {\overline {{\mathsf {a}}}})$。依次结束一个读周期，如图6-8所示。

目标地址由7位地址和1位读写信息$（{\mathsf {read}} = 1$, write $= 0$）组成，如图6-6和图6-7所示。对于TFP410，使用A[3:1]的可选目标地址（包括R/W位）为0x70, 0x72, 0x74, 0x76, $0 {\times} 7.8 $, 0x7A， 0x7C和$0 {\times} 7 \mathsf E$用于写周期，$0 {\times} 7.1 $, $0 {\times} 7 3$, 0x75, 0x77, 0x79, 0x7B， 0x7D和$0 {\times} 7 {\mathsf {F}}$用于读周期。

<html><body><table><tr><td></td><td>目标地址</td><td>W</td><td></td><td>子地址</td><td></td><td>数据</td><td></td><td>数据</td><td></td><td></td></tr></table></body></html>

Where:

从控制器A确认从目标S开始条件P停止条件

<html><body><table><tr><td>S</td><td>目标地址</td><td></td><td></td><td>子地址</td><td></td><td></td><td>目标地址</td><td></td><td></td><td>数据</td><td></td></tr></table></body></html>

Where:

From控制器A Acknowledge From目标S启动条件/A不确认（SDA高）P停止条件R读条件${ \bf \delta } = 1$ Sr重启条件W写条件$\mathbf { \sigma } = \mathbf { 0 }$

＃ 6.6寄存器映射

TFP410是一个标准的$\mathsf {I} ^ {2} \mathsf {C}$目标器件。所有的寄存器都可以通过$1 ^ {2}{\mathsf {C}}$接口进行读写（除非另有说明）。TFP410目标机只支持字节读和写周期。不支持页面模式。$1 ^ {2} {\mathsf {C}}$机器的8位二进制地址是0111 A3A2A1X，其中A[3:1]是引脚可编程的或默认设置为000。TFP410的$\mathsf {I} ^ {2} \mathsf {C}$基址依赖于A[3:1]（分别为引脚6,7和8），如下所示。

表身体< html > < > < > < tr > < td > [3:1] < / td > < td >写地址(十六进制)< / td > < td >读地址(十六进制)< / td > < / tr > < tr > < td > 000 < / td > < td > 70 < / td > < td > 71 < / td > < / tr > < tr > < td > 001 < / td > < td > 72 < / td > < td > 73 < / td > < / tr > < tr > < td > 010 < / td > < td > 74 < / td > < td > < / td > < / tr > < tr > < td > 011 < / td > < td > < / td > < td > 577 < / td > < / tr > < tr > < td > 100 < / td > < td > 78 < / td > < td > 29日< / td > < / tr > < tr > < td > 101 < / td > 7 < td > < / td > < td > 7 b < / td > < / tr > < tr > < td > 110 < / td > < td > 7 c < / td > < td > 7 d < / td > < / tr > < tr > < td > 111 < / td > < td > 7 e < / td > < td > 7 f < / td > < / tr > < /表> < /身体> < / html >

表身体< html > < > < > < tr > < td >登记< / td > < td > RW < / td > < td >子地址< / td > < td > BIT7 < / td > < td > BIT6 < / td > < td > BIT5 < / td > < td >相加< / td > < td >第3位< / td > < td > BIT2 < / td > < td > BIT1 < / td > < td >鱼毒木< / td > < / tr > < tr > < td > VEN_ID < / td > < td > R < / td > < td > 00 < / td > < td > < / td > < td > < / td > < td > < / td > < td > VEN ID (7:0) < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td行宽= " 3 " > DEV_ID < / td > < td > R < / td > < td > 01 < / td > < td > < / td > < td > < / td > < td > < / td > < td > VEN_ID(很高)< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > R < / td > < td > 02 < / td > < td > < / td > < td > < / td > < td > < / td > < td > DEV ID (7:0) < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > R < / td > < td > 03 < / td > < td > < / td > < td > < / td > < td > < / td > < td > DEV_ID(很高)< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > REV_ID保留< / td > < td > RR < / td > < td > 04 < / td > < td > < / td > < td > < / td > < td > < / td > < td > REV_ID [7:0] < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > < / td > < td > 05-07 < / td > < td > < / td > < td > < / td > < td > < / td > < td >保留< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

表身体< html > < > < > < tr > < td >登记< / td > < td > RW < / td > < td >子地址< / td > < td > BIT7 < / td > < td > BIT6 < / td > < td > BIT5 < / td > < td >相加< / td > < td >第3位< / td > < td > BIT2 < / td > < td > BIT1 < / td > < td >鱼毒木< / td > < / tr > < tr > < td > CTL_1_MODE < / td > < td > RW < / td > < td > 08年< / td > < td > RSVD < / td > < td >今年< / td > < td > VEN < / td > < td >母鸡< / td > < td > DSEL < / td > < td > BSEL < / td > < td >边缘< / td > < td > PD < / td > < / tr > < tr > < td > CTL_2_MODE < / td > < td > RW < / td > < td > 09年< / td > < td > VLOW < / td > < tdcolspan = " 2 " >毫秒< / td > < td > < / td > < td > TSEL < / td > < td > RSEN < / td > < td > HTPLG < / td > < td > MDI < / td > < / tr > < tr > < td > CTL_3_MODE < / td > < td > RW < / td > < td > OA < / td > < td colspan =“3”> DK < / td > < td > DKEN < / td > < td colspan =“4”> CTL RSVD < / td > < / tr > < tr > < td > CFG < / td > < td > R < / td > < td > 0 b < / td > < td colspan =“5”> CFG < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >保留< / td > < td > RW < / td > < td > 0 c-31 < / td > < td colspan =“6”>保留< / td > < td > < / td > < td > < / td > < / tr > < tr > < td > de_de < / td > < td > RW < / td > < td > 32 < / td > < td colspan = " 2 " > < / td > < td > < / td > < tdcolspan = " 5 " > de_de [7:0] < / td > < / tr > < tr > < td > DE_CTL < / td > < td > RW < / td > < td > 33 < / td > < td > RSVD DE_GEN < / td > < td colspan =“6”> VS_POL HS_POL < / td > < td > de_de [8] < / td > < / tr > < tr > < td > DE_TOP < / td > < td > RW < / td > < td > 34 < / td > < td > RSVD < / td > < td colspan =“6”> de_de [6:0] < / td > < td > < / td > < / tr > < tr > < td >保留< / td > < td > RW < / td > < td > 35 < / td > < td colspan =“6”>保留< / td > < td > < / td > < td > < / td > < / tr > < tr > < td > DE_CNT < / td > < td > RW < / td > < td > 36 < / td > < td colspan =“6”>德问[7:0]< / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > RW < / td > < td > < / td > < td 37colspan = " 6 " >保留DE < / td > < td >问[8]< / td > < td > < / td > < / tr > < tr > < td > DE_LIN < / td > < td > RW < / td > < td > 38 < / td > < td colspan =“6”> DE_LIN [7:0] < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > RW < / td > < td > 39 < / td > < td colspan =“6”>保留< / td > < td >林[8]< / td > < td > < / td > < / tr > < tr > < td > H_RES < / td > < td > R < / td > 3 < td > < / td > < td colspan =“6”> H_RES [7:0] < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > R < / td > < td > 3 b < / td > < td colspan =“6”>保留< / td > < td > H_RES [8] < / td > < td > < / td > < / tr > < tr > < td > V_RES < / td > < td > R < / td > < td > 3 c < / td > < tdcolspan = " 6 " > V_RES [7:0] < / td > < td > V_RES [8] < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > R < / td > < td > 3 d < / td > < td colspan =“6”>保留< / td > < td > < / td > < td > < / td > < / tr > < tr > < td >保留< / td > < td > R < / td > < td > 3 e-ff < / td > < td colspan =“6”> < / td > < td > < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 6.6.1 VEN_ID 寄存器(子地址$\mathbf {\sigma} = 01 - 0 0$ [reset $\mathbf {\sigma} = \mathbf {\sigma}$ 0x014C]

表身体< html > < > < > < tr > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > 8 < / td > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > VEN_ID [7:0] < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“16”> VEN_ID(很高)< / td > < / tr > < /表> < /身体> < / html >

表6 - 4。VEN_ID字段说明

<html><body><table><tr><td>Bit</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>15:8</td><td>VEN_ID</td><td>R</td><td rowspan="2">这些只读寄存器包含16位德州仪器供应商ID。VEN_ID 0 x014c硬。< / td > < / tr > < tr > < td > 7:0 < / td > < td > VEN_ID < / td > < td > R < / td > < / tr > < /表> < /身体> < / html >

# 6.6.2 DEV_ID 寄存器（子地址$\mathbf {\lambda} = 0 3 \mathbf {-} 0 2$） [reset $\mathbf {\lambda} =$ 0x0410]

图6 - 10。DEV_ID注册

表身体< html > < > < > < tr > < td > 15 < / td > < td > 14 < / td > < td > 13 12 < / td > < td > < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > 8 < / td > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > 4 < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“16”> DEV_ID(很高)< / td > < / tr > < /表> < /身体> < / html >

图6 - 9。VEN_ID注册
表6 - 5。DEV_ID注册字段说明

<html><body><table><tr><td>Bit</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>15:8</td><td>DEV_ID</td><td>R</td><td rowspan="2">这些只读寄存器包含TFP410的16位设备ID。DEV_ID 0 x0410硬。< / td > < / tr > < tr > < td > 7:0 < / td > < td > DEV_ID < / td > < td > R < / td > < / tr > < /表> < /身体> < / html >

# 6.6.3 REV_ID 寄存器（子地址${\bf \delta} = {\bf 0 4}$） [reset = 0x00]

图6-11。REV＿ID注册表

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td></td></tr><tr><td colspan="8">REV＿ID[7:0]</td></tr></table></body></html>

表6-6REV＿ID注册字段说明

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>7:0</td><td>REV＿ID</td><td>(d:)</td><td>此只读寄存器包含修订ID。</td></tr></table></body></html>

＃ 6.6.4保留寄存器（子地址$\mathbf { \lambda } = \mathbf { 0 7 - 0 5 }$） [reset $\mathbf { \sigma } = \mathbf { \sigma }$ 0x641400]

图6-12。保留

<html><body><table><tr><td>15</td><td>14</td><td>13</td><td></td>< /td><td>8</td><td></td><td> 5</td><td></td><td>43</td><td> <td></td><td></td></ td></td></ td></td></ td></td></ table></body></html>

表6-7。保留字段说明

<html><body><table><tr><td>Bit</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>15:8</td><td>RESERVED</td><td> </td></ td>< tr><td>7:0</td><td>RESERVED</td><td>只读</td><td></td></tr></table></body></html>

＃ 6.6.5 CTL＿ 1 ＿MODE（子地址${ \bf \Omega } = { \bf 0 8 }$） [reset $\mathbf { \sigma } = \mathbf { \sigma }$ 0xBE]

图6-13。CTL_1_MODE注册

表身体< html > < > < > < tr > < td > 7 < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td > RSVD < / td > < td >今年< / td > < td > VEN < / td > < td >母鸡< / td > < td > DSEL < / td > < td > BSEL < / td > < td >边缘< / td > < td > PD < / td > < / tr > < /表> < /身体> < / html >

表6 - 8。CTL_1_MODE字段说明

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>7</td><td>RSVD</td><td>R/W</td>< /tr><tr><td></td><td>TDIS</td><td>R/W</td><td>这个读写寄存器包含tmd的禁用模式0:tmd电路使能状态由PD决定。1: tmd电路未使能。</td></tr><tr><td></td><td>VEN</td><td>R/W</td><td>这个读写寄存器包含垂直同步启用模式。0: VSYNC输入以固定低电平传输1:VSYNC输入以</td></tr><tr><td></td><td>HEN</td><td>R/W</td><td>这个读写寄存器包含水平同步启用模式。0: HSYNC输入以固定低电平发送1:HSYNC输入以其原始状态发送</td></tr><tr><td></td><td>DSEL</td><td>R/W</td><td>此读写寄存器与BSEL和VREF结合使用，选择单端或差分输入时钟模式。在高摆幅模式下，DSEL是不关心的，因为IDCK总是单端。</td></tr><tr><td></td><td>BSEL</td><td>R/W</td><td>这个读写寄存器包含输入总线选择模式。1:24位操作，单边时钟</td></tr><tr><td></td><td>EDGE</td><td>R/W</td><td>这个读写寄存器包含边缘选择模式。0：输入数据锁存到IDCK的上升沿+ 1：输入数据锁存到IDCK的上升沿+下降</td></tr><tr><td></td><td>PD</td><td>R/W</td><td>这个读写寄存器包含掉电模式。1：正常运行</td></tr></table></body></html>

# 6.6.6 CTL_2_MODE 寄存器（子地址${\bf \delta} = {\bf 0 9}$） [reset = 0x00]

图6日至14日。CTL_2 MODE寄存器

表身体< html > < > < > < tr > < td > 7 < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 2 < / td > < td > < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td > VLOW < / td > < td > < / td > < td >毫秒[3:1]< / td > < td > TSEL < / td > < td > RSEN < / td > < td > < / td > < td > HTPLG < / td > < td > MDI < / td > < / tr > < /表> < /身体> < / html >

表6-9CTL_2_ MODE字段描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >描述< / td > < / tr > < tr > < td > < / td > < td > VLOW < / td > < td > R / W < / td > < td >这只读寄存器指示VREF输入0级:这一点是逻辑电平(0)如果VREF模拟输入选择高振幅输入1:这一点是逻辑电平(1)VREF模拟输入选择低振幅输入如果< / td > < / tr > < tr > < td > 6:4 < / td > < td >毫秒[3:1]< / td > < td > R / W < / td > < td >的读/写寄存器包含源选择监视器输出接脚。000:禁用。MSEN输出高001：输出MDI位（中断）010：输出RSEN位（接收检测）011：输出HTPLG位（热插拔检测）</td></tr><tr><td></td><td>TSEL</td><td>R/W</td><td>这个读写寄存器包含中断产生源选择。0：监控RSEN产生的中断位（MDI） 1：监控HTPLG产生的中断位(MDI) </td></tr><tr><td></td><td>RSEN</td><td>R/W</td><td>此只读寄存器包含接收端感知输入逻辑状态，仅对直流耦合系统有效。0：未检测到接收端上电1：检测到接收端上电（即与DVI发送端输出相连）</td></tr><tr><td></td><td>HTPLG</td><td>R/W</td><td>这个只读寄存器包含热插拔检测输入逻辑状态。0：在EDGE/HTPLG引脚上检测到逻辑电平（引脚9）1：在EDGE/HTPLG引脚上检测到高电平（引脚9）</td></tr><tr><td></td><td>MDI</td><td>R/W</td><td>这个读写寄存器包含监视器检测中断模式。0：表示检测信号中检测到的逻辑电平发生了变化（要清除，向该位写1）。1：表示逻辑电平保持不变</td></tr></table></body></html>

# 6.6.7 CTL_3 MODE 寄存器（子地址${\bf \varepsilon} = {\bf 0} {\bf A}$） [reset = 0x80]

图6 - 15。CTL_3 MODE寄存器

表身体< html > < > < > < tr > < td > < / td > < td > 6 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td > < / td > < td > DK [3:1] < / td > < td > DKEN < / td > < td > RSVD < / td > < td > CTL [2:1] < / td > < td > < / td > < td > RSVD < / td > < / tr > < /表> < /身体> < / html >

表6 - 10。CTL_3 MODE寄存器字段说明

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>7:5</td><td>DK[3:1]</td><td>RW</td><td>这个读写寄存器包含去倾斜设置，每个增量调整倾斜t（STEP）。000：步骤（最小设置/最大保持）001:2 010：步骤3 011：步骤4 100：步骤5（默认）101：步骤6 110：步骤7步骤</td></tr><tr><td></td><td>DKEN</td><td>RW</td><td>111：步骤8（最大设置/最小保持）这个读写寄存器控制数据去倾斜启用。0：数据去偏使能，不使用DK[3:1]中的值</td></tr><tr><td>3</td><td>RSVD</td><td>RW</td><td>1：数据去偏使能，去偏设置通过DK[3:1]控制</td></tr><tr><td>2:1</td><td>CTL[2:1]</td><td>RW</td><td>这个读写寄存器包含两个CTL[2:1]位的值，它们在去偏间隔期间在DVI端口上输出</td></tr><tr><td>U</td><td> RW</td><td></td></tr></table></body></html>

# 6.6.8 CFG 寄存器（子地址$= 0 B$）

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“8”> CFG [7:0] < / td > < / tr > < /表> < /身体> < / html >

表6尺11寸。CFG寄存器字段说明

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >描述< / td > < / tr > < tr > < td > 7:0 (D [23:16]) < / td > < td > CFG < / td > < td >只读< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

图-。CFG注册

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >描述< / td > < / tr > < tr > < td > 7:0 < / td > < td >保留< / td > < td > R / W < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

＃ 6.6.9保留寄存器（子地址$\mathbf { \sigma } = \mathbf { 0 E } \mathbf { - } \mathbf { 0 C }$） [reset $\mathbf { \lambda } = \mathbf { \lambda }$ 0x97D0A9]

<html><body><table><tr><td></td><td>6</td><td>5</td><td></td><td>3</td><td></td><td></td><td>0</td></tr><tr><td colspan="8">RESERVED</td></tr></table></body></html>

图6-17。保留寄存器

<html><body><table><tr><td>Bit</td><td>Field</td><td>Type</td><td>Description</td></ td>< tr><td>7:0</td><td>DE DLY</td><td>R/W</td><td>这个读写寄存器定义在DE生成器启用时，生成DE的HSYNC激活后的像素数。该值必须小于或等于（2047 DE ＿CNT）灶台</td></tr></table></body></html>

＃ 6.6.10 DE＿DLY 寄存器(子地址$= 3 2$$\mathbf { \mu = 0 \times 0 0 }$

表6 - 12。保留寄存器字段说明
图6 - 18。de_de注册

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td colspan =“8”> de_de [7:0] < / td > < / tr > < /表> < /身体> < / html >

# 6.6.11 DE_CTL 寄存器（子地址$= 33 $）[reset = 0x00]

表6-13。DE_DLY字段描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >描述< / td > < / tr > < tr > < td > 7 < / td > < td >保留< / td > < td > R / W < / td > < td > < / td > < / tr > < tr > < td > < / td > < td > DE_GEN < / td > < td > R / W < / td > < td >这读/写寄存器允许内部发生器。0：关闭DE生成器。DE引脚1上需要信号：DE发生器已启用。DE引脚被忽略。</td></tr><tr><td></td><td>VS_POL</td><td>R/W</td><td>这个读写寄存器设置VSYNC极性。0：认为VSYNC处于低激活状态。1：认为VSYNC为主动高电平。在VSYNC活动边重置行数。</td></tr><tr><td></td><td>HS_POL</td><td>R/W</td><td>这个读写寄存器设置HSYNC极性。0：认为HSYNC处于低激活状态。1：认为HSYNC为主动高。在HSYNC活动边缘上重置像素计数。< / td > < / tr > < tr > < td > 1:3 < / td > < td >保留< / td > < td > R / W < / td > < td > < / td > < / tr > < tr > < td > 0 < / td > < td > de_de [8] < / td > < td > R / W < / td > < td >这个读/写寄存器包含顶部的DE_ _DLY。< / td > < / tr > < /表> < /身体> < / html >

图6-19。DE_CTL注册

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > 4 < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td >保留< / td > < td > DE_GEN < / td > < td > VS_POL < / td > < td > HS_POL < / td > < td > < / td > < td >保留< / td > < td > < / td > < td > de_de [8] < / td > < / tr > < /表> < /身体> < / html >

表6-14DE_CTL寄存器字段描述

# 6.6.12 DE_TOP 寄存器（子地址$= 34 $）[reset $\mathbf {\lambda} = \mathbf {0} \times \mathbf {0} \mathbf {0}]$]

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“8”> DE_TOP [7:0] < / td > < / tr > < /表> < /身体> < / html >

表6 - 15。DE_TOP寄存器字段描述

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></ td>< tr><td>7:0</td><td>DE TOP</td><td>R/W</td><td>这个读写寄存器定义了当DE生成器启用时，VSYNC激活后生成DE的像素数</td></tr></table></body></html>

# 6.6.13 DE_CNT 寄存器（子地址$= 37 - 36 $）[reset = 0x0000]

图6 20。DE_TOP注册

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></ td>< tr><td>10:8</td><td>DE CNT</td><td>R/W</td><td> 7:0</td><td>DE CNT</td><td>RW DE</td><td>生成器启用时，以像素为单位定义活动显示的宽度。</td></tr></table></body></html>

图6-21。DE_ CNT寄存器

表身体< html > < > < > < tr > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td colspan =“6”>德问[7:0]< / td > < td colspan = " 2 " > < / td > < / tr > < tr > < td colspan =“6”>保留< / td > < td colspan =“2”>德问[8]< / td > < / tr > < /表> < /身体> < / html >

表6-16DE_CNT寄存器字段描述

# 6.6.14 DE_LIN寄存器（子地址$\mathbf {\lambda} =$ 39-38）[复位$\mathbf {\lambda} = \mathbf {\lambda}$ 0x0000]

图第6 - 22。DE_LIN注册

表身体< html > < > < > < tr > < td colspan =“6”> 5 3 < / td > < / tr > < tr > < td colspan =“6”> 2 DE_LIN [7:0] < / td > < / tr > < tr > < td colspan = " 4 " > < / td > < / tr > < tr > < td colspan =“2”>保留< / td > < td colspan =“3”> DE_LIN [8] < / td > < / tr > < /表> < /身体> < / html >

表6-17DE_LIN寄存器字段描述

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></ td>< tr><td>10:8</td><td>DE_LIN</td><td>R/W</td>< tr><td>7:0</td><td>DE_LIN</td><td>R/W</td></tr></table></body></html>

# 6.6.15 H_RES寄存器（子地址$\mathbf {\lambda} =$ 3B−3A）

图6-23。H_RES注册

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“8”> H_RES [7:0] < / td > < / tr > < tr > < td colspan =“5”>保留< / td > < td colspan =“3”> H_RES [8] < / td > < / tr > < tr > < td colspan =“5”> < / td > < td colspan =“3”> < / td > < / tr > < /表> < /身体> < / html >

表6 - 18。H_RES寄存器字段描述

表身体< html > < > < > < tr > < td >位< / td > < td >字段< / td > < td >类型< / td > < td >描述< / td > < / tr > < tr > < td > 10:8 < / td > < td > H_RES < / td > < td >只读< / td > < td >这些只读寄存器返回之间的像素数量连续< / td > < / tr > < tr > < td > 7:0 < / td > < td > H_RES < / td > < td >只读< / td > < td > HSYNC脉冲。< / td > < / tr > < /表> < /身体> < / html >

# 6.6.16 V_RES寄存器（子地址$\mathbf {\lambda} = \mathbf {\lambda}$ 3D−3C）

图6日到24日。V_RES注册

表身体< html > < > < > < tr > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td colspan =“6”> V_RES [7:0] < / td > < td > < / td > < td > < / td > < / tr > < tr > < td colspan =“6”>保留< / td > < td colspan =“3”> V_RES [8] < / td > < / tr > < /表> < /身体> < / html >

表6-19。V_RES寄存器字段描述

<html><body><table><tr><td>位</td><td>字段</td><td>类型</td><td>描述</td></tr><tr><td>10:8</td><td>V_RES</td><td> <td rowspan="2">这些只读寄存器返回连续的行数</td></tr><tr><td>7:0</td><td>V_RES</td><td>只读VSYNC脉冲</td></tr></ body></html>

# 7应用和实现

# Note

以下应用章节中的信息不是TI组件规范的一部分，TI不保证其准确性或完整性。TI的客户负责确定组件对其用途的适用性，以及验证和测试其设计实现以确认系统功能。

# 7.1应用信息

TFP410是一款符合dvi标准的数字发射机，用于数字主机监控系统中，用于tmd编码和序列化RGB像素数据流。TFP410支持从VGA到WUXGA（和1080p）的分辨率。数字显示系统中的主机，通常是PC或消费电子设备，包含一个dvi兼容的发射器，如TI TFP410，它接收24位像素数据以及适当的控制信号。TFP410将信号编码成高速、低电压、差分串行比特流，优化后通过双绞线电缆传输到TFP401等显示设备。

# 7.2典型应用

![](images/b929558a651192e4d5403c201db481c3e93abecb371cda8d10e52b18d0c5bcb0.jpg)  
图7 - 1。TFP410器件的典型应用

# 7.2.1设计要求

表身体< html > < > < > < tr > < td >参数< / td > < td >价值< / td > < / tr > < tr > < td >电源< / td > < td > 3.3 v直流1 < / td > < / tr > < tr > < td >输入时钟< / td > < td >单端< / td > < / tr > < tr > < td >输入时钟频率范围< / td > < td > 25兆赫165 mhz < / td > < / tr > < tr > < td >输出格式< / td > < td > 24位/像素< / td > < / tr > < tr > < td >输入时钟闭锁< / td > < td >前沿< / td > < / tr > < tr > < td > I2C eepm支持< / td > < td >没有< / td > < / tr > < tr > < td > De-skew < / td > < td >没有< / td > < / tr > < /表> < /身体> < / html >

# 7.2.2详细设计程序

# 7.2.2.1数据和控制信号

从接收机发出的数据和控制信号的跟踪长度应尽可能保持接近相等。微量分离应约为高度的5倍。一般来说，如果可能的话，走线也应该小于2.8英寸（更长的走线也是可以接受的）。

$$
\mathsf {D e l a y} = 85 \times \mathsf {S Q R T} \times \mathsf {e R}
$$

where

$\mathsf {e r} = 4。3 5$；$ $ mathbf {\tau} = $ $ mathbf {\tau}$ 177 pS/in

上升沿长度$\mathbf {\tau} = \mathbf {\tau}$ Tr（ps）${\mathsf {T r}} = 3$ ns

# where

$= 3 0 0 0$ ps每英寸177 ps $= 169美元英寸

上升沿的长度=集总电路的最大走线长度。

16美元。9 / 6 = 2。8美元英寸

![](images/c7533446e8898f593adcb6058997a7d6262e1e0de20829a540185536fee4e057.jpg)  
图7 - 2)。数据信号

# 7.2.2.2配置选项

TFP410可以根据所需的输入格式配置为几种模式，例如1字节/时钟，2字节/时钟，下降/冲洗时钟边缘。

关于配置选项的更多信息请参见表6-1。

# 7.2.2.3电源去耦

数字、模拟和锁相环电源必须相互去耦，以避免锁相环和核心上的电气噪声。

![](images/bbbb63c95993515c34d7ed8d72bb32a0341a66d5aa124494a73f617693e7cbb2.jpg)  
图7。功率解耦

# 7.2.3应用曲线

有时面板不支持与GPU（图形处理器单元）相同的格式。在这些情况下，用户必须决定如何连接未使用的位。

图7-4和图7-5显示了18位GPU和24位LCD之间的不匹配，其中$“\bigtimes”$和“y”表示面板的2 LSB。

![](images/422977c5cf2cd0633429df97e63f91d1292c9df7179f3be33c70df7c40eb3553.jpg)

# 7.3电源建议

使用坚实的地平面。用尽可能多的通孔将地平面连接在一起。这将为电流提供一个理想的返回路径。每个电源应在单独的分割电源平面上，其中每个电源平面应尽可能大的面积。将PanelBus接收器电源和接地引脚以及所有旁路帽连接到适当的电源或接地平面上。过孔应该尽可能的长和短，目标是尽量减少电感。

# 7.3.1 DVDD

放一个0。0 1 \mu \mathsf {F}$电容器尽可能靠近每个DVDD设备引脚和地之间。在电源和0之间应放置一个$ 22 \mu \ F$钽电容器。0 1 \mu \mathsf {F}$电容器。在电源和$ 22 \mu \ F$电容器之间应使用铁氧体磁头。

# 7.3.2 TVDD

放一个0。0 1 \mu \mathsf {F}$电容器尽可能靠近每个TVDD器件引脚和地之间。在电源和0之间应放置一个$ 22 \mu \ F$钽电容器。0 1 \mu \mathsf {F}$电容器。在电源和$ 22 \mu \ F$电容器之间应使用铁氧体磁头。

# 7.3.3 PVDD

放三个0美元。1个\mu \mathsf {F}$电容并联，尽量靠近PVDD器件引脚和地之间。在电源和0之间应放置一个$ 22 \mu \ F$钽电容器。0 1 \mu \mathsf {F}$电容器。在电源和$ 22 \mu \ F$电容器之间应使用铁氧体磁头。

# 7.4布局

# 7.4.1布局指南

# 7.4.1.1层堆栈

德州仪器高速接口（HSI）器件的引脚具有差分信号对，其余信号包括电源轨、VCC和地，以及低速信号，如控制引脚。例如，考虑一个设备X，它是一个中继器/重新驱动程序，因此输入和输出都是高速差分信号。这些指导方针可以应用于其他高速设备，如驱动器、接收器、多路复用器等。

要完成低emi PCB设计，至少需要四层。层的堆叠顺序应从上到下依次为：高速差分信号层、接地层、电源层、控制信号层。

![](images/ce9efc88ce3ad6ca192fa31d67aeb2b43429517834e40114da6fef17edcc18cd.jpg)  
图7 - 6。PCB堆叠

# 7.4.1.2高速差分信号走线（RxC-、RxC+、Rx0-、Rx0+、Rx1-、Rx1+、Rx2-、Rx2+）

应控制走线阻抗以获得最佳性能。各差分对长度相等，对称，对地阻抗相等，走线间距为高度的2倍至4倍。4倍高度的差分走线分离对阻抗产生约6%的串扰效应。

我们建议差分走线布线应该并排，尽管差分走线紧密耦合在一起并不重要，因为在PCB走线上无法实现紧密耦合。pcb的典型比率仅为$ 20% $至$ 50% $；9美元。9 \%$是平衡良好的双绞线电缆的值。每个差分轨迹应该尽可能短（$\cdot < 2$英寸是最好的），没有$ 90 ^ {\circ}$角度。这些高速传输走线应该在第一层，也就是最顶层。

RxC - $ \ mathsf {x R C  + }$ , ${ \ mathsf {R}} {\ mathsf {x 0  - } }$ , ${ \ mathsf {R}}{\ *}{0} +美元,Rx1,美元\ mathsf {R} \ * 1  +$ , ${ \ mathsf {R}} {\ mathsf {x}} 2。$ -, ${\mathsf {R}} {\mathsf {x}} 2 +$信号都直接从DVI连接器引脚路由到设备，不需要外部组件。

# 7.4.1.3 DVI连接器

连接器引脚的清除孔应在引脚之间留出空间，以便连续接地通过引脚区。但是，在信号引脚过孔周围的接地平面上要有足够的间距，在过孔之间要保持足够的铜，以便地电流在过孔之间流动。避免在整个连接器周围创建一个大的接平面槽，因为最小化过孔电容是目标。

# 7.4.2布局示例

DVI连接器走线匹配如图7-7所示。

![](images/1eddb89c0195e004801d9d0958a7e4a438e63bbb6635dc55d7dd90d223cc8e3a.jpg)  
图7。DVI信号路由

数据线之间尽量保持远离，如图7-8所示。

![](images/07355b9ea6f3fff5f098716089d56e968cbf4c1c3d92ee358c4dd968d0258f42.jpg)  
图7 - 8。数据信号路由

如图7-9所示，将热垫接地。

![](images/61ea822cca3e3f7fec25ee1ef087154e8cec5787e1b50aff75d3a1299dba9e97.jpg)  
图7 - 9。地面的路由

# 7.4.3 TI PowerPAD 64针HTQFP封装

TFP410采用TI热增强型64引脚TQFP PowerPAD封装。PowerPAD包是$ 10 \mathrm {{m m} \乘以10 \mathrm {{m m} \乘以1。0 \ mathm {{mm}}}}$ TQFP轮廓与0.5mm引线间距。PowerPAD封装有一个特别设计的模具安装垫，与相同外形的典型TQFP封装相比，它提供了更好的热性能。TI 64引脚TQFP PowerPAD封装提供直接连接到芯片安装垫的背面焊料平面，以增强热传导。出于热方面的考虑，不需要将TFP410的背面焊接到应用板上，因为不焊接时器件功耗完全在封装能力范围内。

出于电气方面的考虑，建议将器件的背面焊接到PCB接地面上。由于芯片衬底与芯片衬底电连接，因此芯片接地，将PowerPAD封装的背面连接到PCG接地平面提供了低电感，低阻抗连接，有助于改善EMI，地反弹和电源噪声性能。

TI 64pin TQFP PowerPAD封装的热特性如表7-1所示。64引脚TQFP非powerpad封装仅供参考。

表7 - 1。TI 64针TQFP (10mm × 10mm × 1.0mm) 0.5mm引线间距

<html><body><table><tr><td rowspan="2">PARAMETER</td><td rowspan="2">WITHOUT PowerPADTM</td><td rowspan="2">PowerPADTM未连接到PCB THERMAL</td><td rowspan="2">PowerPAD Tm</td></tr><tr><td>连接到PCB PLANE THERMAL PLANE(1)</td></tr><tr><td>ROJA</td><td>热阻，结对环境(1)(2)</td><td> 42.20°C/W</td>< /tr><tr><td>ReJC</td><td>热阻，junction-to-case (1) (2) (1) (2) < / td > < td > 7.80°/ W < / td > < td > 0.38°C / W < / td > < td > 0.38°C / W < / td > < / tr > < tr > < td > PD < / td > < td >功率处理能力包< / td > < td > 0.92 W < / td > < td > 1.66 W < / td > < td > 3.26 W < / td > < / tr > < /表> < /身体> < / html >

(1)将包装背面指定的PowerPAD键合焊盘焊接到2盎司。铜板PCB热平面。(2)气流为0 LFM（无气流）(3)指定为$ 1.5 0 ^ {\circ} \mathsf {C}$结温和$8 0 ^ {\circ} \mathsf {C}$环境温度。

8.1接收文档更新通知

要接收文档更新通知，请导航到ti.com上的设备产品文件夹。单击Notifications注册并接收已更改的任何产品信息的每周摘要。有关变更细节，请查看任何修订文件中包含的修订历史。

8.2支持资源

TI E2E™支持论坛是工程师获得快速、经过验证的答案和设计帮助的首选来源-直接来自专家。搜索现有答案或提出自己的问题，以获得所需的快速设计帮助。

链接内容由各自的贡献者“按原样”提供。它们不构成TI规范，也不一定反映TI的观点；请参阅TI的使用条款。

# 8.3商标

英特尔™是英特尔公司的商标。
PowerPAD EPIC- $。5 ^ {\mathtt {T M}}$和PanelBus™是TI的商标。
蓝光™是蓝光光盘协会的商标。
TI E2E™是德州仪器的商标。
所有商标均为其各自所有者的财产。

# 8.4静电放电

![](images/6d3756081974b39b2fb0093d91f7903b4aaf3ae4cde300cef84ed8e9e3633d6e.jpg)

这种集成电路会被ESD损坏。德州仪器建议所有集成电路都要采取适当的预防措施。不遵守正确的操作和安装程序可能会造成损坏。

ESD损坏的范围可以从细微的性能下降到完全的设备故障。精密集成电路可能更容易受到损坏，因为非常小的参数变化可能导致器件不符合其公布的规格。

8.5术语表

# TI术语表

本术语表列出并解释了术语、缩略语和定义。

9 .修订历史

注意：以前版本的页码可能与当前版本的页码不同。

从版本C（2014年11月）到版本D（2024年2月）的变化

Page

在整个数据表中添加了包容性术语。
将引脚22从：$T V C +$改为：$T X C +$
更新了MSEN引脚描述，从：当$I ^ {2} C$被禁用$\ rangle I SE L = I ow \rangle$时，低电平表示在差分输出检测到一个上电的接收器。高电平表示未检测到接收器上电：当$I ^ {2} C$ I disabled $\prime I S E L = I ow)$时，高电平表示在差分输出检测到接收器上电。低电平表示未检测到上电的接收器。2将引脚6的名称从CTL3/A3/DK3更改为A3/DK3. .....…2更改自：当I2C总线禁用$\prime I S E L = I o w)$和去斜模式禁用$(D K E N = I o w)$时，这三个输入成为控制输入，$C T L [3；1],$可用于在空白间隔$\left(D E = I o w \right)$期间通过DVI链路发送额外信息。CTL3输入保留用于HDCP兼容的DVI TXs (TFP510)， CTL[2:1]输入保留用于将来使用。当$I ^ {2} C$总线被禁用$\prime | S E L = I o w)$和去斜模式被禁用$(D K E N = I o w)$时，引脚7和8成为控制输入，CTL[2:1]，可用于在空白间隔$\左（D E = I o w \右）$期间通过DVI链路发送额外信息。引脚6不使用. .........
更新了$\mathsf {V} _ {\mathsf {I H}}$和$\mathsf {V} _ {\mathsf {l}}$用于Data， DE， VSYNC， HSYNC和$ 10 0 0 \mathsf {K} + I -$ - CMOS输入和剩余的CMOS输入。…7在非活动显示或下料间隔期间，将三个用户可定义的控制信号CTL[3:1]更改为：在非活动显示或下料间隔期间，三个控制信号CTL[3:1]

将TFP410在垂直落料间隔期间编码并传输CTL[3:1]输入更改为：TFP410在垂直落料间隔期间编码并传输CTL[2:1]输入................................11更改了CTL3输入为HDCP兼容的DVI TXs保留，CTL[2:1]输入保留以供将来使用：CTL3为HDCP保留，并且始终编码为0。CTL[2:1]输入被保留以供将来使用...........11改变RW字段从RW sub-address 0 b R . ......... ......................................................18将CTL_1_MODE寄存器复位值由：0xFE修改为：0xBE。20将CTL_3_MODE寄存器的第3位从CTL3修改为RSVD .......…21在DE - DLY寄存器中添加的值必须小于等于（2047 DE_ CNT） ...........................22在DE_CNT寄存器中添加的值必须小于等于（2047 - DE_DLY） ....23更改申请信息摘要，重点关注TFP410而不是TDP401。... ... ... 25

从修订B（2011年5月）到修订C（2014年11月）的变化

Page

新增ESD额定值表、热信息表、典型特性、特性描述、设备功能模式、应用与实现、电源建议、布局、设备与文档支持、机械、封装和可订购信息. .......................................

# 10机械、包装和可订购信息

以下页面包括机械、包装和可订购信息。此信息是指定设备可用的最新数据。这些数据如有更改，恕不另行通知或本文档进行修订。有关此数据表的基于浏览器的版本，请参阅左侧导航。

#包装信息

<html><主体><表><tr><td>可订购零件编号</td><td>状态(1)</td><td>封装|引脚</td><td>封装数量载具</td><td>RoHS (3)</td><td>引脚完成/球料(4)</td><td>额定/最高回流密度MSL</td><td>额定温度（℃）</td><td> <td>零件标记(6)</td></ td><td>活性</td><td>生产</td><td>HTQFP (PAP)| </td><td>Yes</td><td>NIPDAU</td><td>等级-3- 260c -168 HR</td><td>0至70 < / td > < td > TFP410PAP < / td > < / tr > < tr > < td > TFP410PAP。< / td > < td >活动< / td > < td >生产< / td > < td > HTQFP (PAP) | 64年< / td > < td > 160 |电平托盘(10 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > 0到70 < / td > < td > TFP410PAP < / td > < / tr > < tr > < td > TFP410PAPG4 < / td > < td >活动< / td > < td >生产< / td > < td > HTQFP (PAP) | 64年< / td > < td > 160 |电平托盘(10 + 1)< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > 0到70 < / td > < td > TFP410PAP < / td > < / tr > < tr > < td > TFP410PAPR < / td > < td >活动< / td > < td >生产< / td > < td > HTQFP (PAP) 64年< / td > < td > 1000收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > 0到70 < / td > < td > TFP410PAP < / td > < / tr > < tr > < td > TFP410PAPR。< / td > < td >活动< / td > < td >生产< / td > < td > HTQFP (PAP) 64年< / td > < td > 1000 |大型收发< / td > < td >对< / td > < td > NIPDAU < / td > < td >水平- 3 - 260 - c - 168人力资源< / td > < td > 0到70 < / td > < td > TFP410PAP < / td > < / tr > < /表> < /身体> < / html >

(1)状态：有关状态的详细信息，请参阅我们的产品生命周期。

(2)材料类型：当指定时，预生产部件是原型/实验设备，尚未批准或释放全面生产。测试和最终过程，包括但不限于质量保证、可靠性性能测试和/或过程确认，可能尚未完成，此项目可能会进一步更改或可能停止。如果可以订购，购买将在结账时附加豁免，并且仅用于早期内部评估目的。这些物品出售时没有任何形式的保证。

(3) RoHS值：Yes， No, RoHS Exempt。有关更多信息和价值定义，请参阅TI RoHS声明。

(4)铅饰面/球料：零件可能有多种材料饰面选择。完成选项由垂直直线分隔。如果光洁度值超过最大柱宽，铅光洁度/球材料值可能缠绕到两行。

(5) MSL额定值/峰值回流：湿度敏感等级额定值和峰值焊料（回流）温度。如果一个部件具有多个湿度敏感性等级，则只显示JEDEC标准的最低等级。
请参阅发货标签，了解将用于将零件安装到印刷电路板上的实际回流温度。

(6)零件标记：可能会有一个额外的标记，它涉及到标志，批号跟踪代码信息，或零件的环境类别。

多个部分标记将在括号内。零件上只会出现一个用圆括号括起来并以分隔的零件标记。如果一行是缩进的，那么它是前一行的延续，两者结合代表该设备的整个部件标记。

重要信息和免责声明：本页上提供的信息代表了TI在提供之日的知识和信念。TI的知识和信念基于第三方提供的信息，对此类信息的准确性不作任何陈述或保证。目前正在努力更好地整合来自第三方的信息。TI已经采取并将继续采取合理措施提供具有代表性和准确的信息，但可能没有对来料和化学品进行破坏性测试或化学分析。TI和TI供应商认为某些信息是专有的，因此CAS号码和其他有限的信息可能无法发布。

在任何情况下，由这些信息引起的TI的责任不得超过TI每年向客户出售的本文件中所述TI部件的总购买价格。

tfp410的其他合格版本：

•增强产品TFP410-EP

注：合格版本定义：•增强产品支持国防，航空航天和医疗应用

#磁带和卷轴信息

![](images/f8a66383dfd64b417848dd063b026e9955f503af31b5524b301b67be29ee65c9.jpg)

![](images/43467095de74ebc63f7dfae927c39107574806c9267d56d9a3a80ce3ecf5ad5a.jpg)

<html><body><table><tr><td>A0</td><td>设计容纳元件宽度的尺寸</td></tr><tr><td>B0</td><td>设计容纳元件长度的尺寸</td></tr><tr><td>K0</td><td>设计容纳元件厚度的尺寸</td></tr><tr><td>W</td><td>载波带总宽度</td></tr><tr><td>P1</td><td>间距连续腔中心</td></tr></table></body></html>

#象限分配引脚在磁带中的方向

![](images/d945afbc523094d3f3e363cef4d0369f99e7c073b0866d6a08b02b6f6113467b.jpg)

\*所有尺寸均为标称尺寸

表身体< html > < > < > < tr > < td >设备< / td > < td >类型< / td > < td >包包装图< / td > < td >针< / td > < td > SPQ < / td > < td >卷筒直径(毫米)< / td > < td > W1卷筒宽度(毫米)< / td > < td > AO (mm) < / td > < td > B0 (mm) < / td > < td > KO (mm) < / td > < td > P1 (mm) < / td > < td > W (mm) < / td > < td > Pin1象限< / td > < / tr > < tr > < td > TFP410PAPR < / td > < td > HTQFP < / td > < td > PAP < / td > < td > 64 < / td > < td > 1000 < / td > < td > 330.0 < / td > < td > 24.4 < / td > < td > 13.0 < / td > < td > 13.0 < / td > < td > 1.5 < / td > < td > 16.0 < / td > < td > 24.0 < / td > < td > Q2 < / td > < / tr > < /表> < /身体> < / html >

![](images/491fe0827367656ed80a8fd52c2883259aa189e08f92d1a948c38e87e2a57db9.jpg)

\*所有尺寸均为标称尺寸

<html><body><table><tr><td>器件</td><td>Package Type</td><td>Package Drawing</td><td>Pins</td><td>SPQ</td><td>Length  (mm)</td><td>Width (mm)</td><td>Height  (mm) < / td > < / tr > < tr > < td > TFP410PAPR < / td > < td > HTQFP < / td > < td > PAP < / td > < td > 64 < / td > < td > 1000 < / td > < td > 350.0 < / td > < td > 350.0 < / td > < td > 4 3.0 < / td > < / tr > < / table > < / body > < / HTML >

# TRAY

L - Outer tray length without tabs KO - Outer tray
Ten, ten, ten, ten, ten, ten + height
十十十十十十十十十十十十十十十 W -   
Ten, ten, ten, ten, ten + Outer
++ ten ten +++++ ten +++++ tray width
十十十十十十十十十十十十十十+   
+++++++\$++++++++ P1 -托盘单元口袋间距
cm -测量托盘边缘（Y方向）到角袋中心CL -测量托盘边缘（X方向）到角袋中心

托盘角上的倒角表示包装单元的引脚方向。

\*所有尺寸均为标称尺寸

表身体< html > < > < > < tr > < td >设备< / td > < td >包名称< / td > < td >类型包< / td > < td >针< / td > < td > SPQ < / td > < td >单元阵列矩阵< / td > < td > Max温度(°C) < / td > < td > L (mm) < / td > < td > W (mm) < / td > < td > K0(μm) < / td > < td >毫米)< / td > < td > CL (mm) < / td > < td >连续波(mm) < / td > < / tr > < tr > < td > TFP410PAP < / td > < td > PAP < / td > < td > HTQFP < / td > < td > 64 < / td > < td > 160 < / td > < td > 8×20 < / td > < td > 150 < / td > < td > 315 < / td > < td > 135.9 < / td > < td > 7620 < / td > < td > 15.2 < / td > < td > 13.1 < / td > < td > 13 < / td > < / tr > < tr > < td > TFP410PAP。< / td > < td > PAP < / td > < td > HTQFP < / td > < td > 64 < / td > < td > 160 < / td > < td > 8 x 20 < / td > < td > 150 < / td > < td > 315 < / td > < td > 135.9 < / td > < td > 7620 < / td > < td > 15.2 < / td > < td > 13.1 < / td > < td > 13 < / td > < / tr > < tr > < td > TFP410PAPG4 < / td > < td > PAP < / td > < td > HTQFP < / td > < td > 64 < / td > < td > 160 < / td > < td > 8×20 < / td > < td > 150 < / td > < td > 315 < / td > < td > 135.9 < / td > < td > 7620 < / td > < td > 15.2 < / td > < td > 13.1 < / td > < td > 13 < / td > < / tr > < /表> < /身体> < / html >

# 10 × 10, 0.5 mm间距

此图像是包装系列的代表，实际包装可能会有所不同。有关包装细节，请参阅产品说明书。

![](images/7aa6cf3a775b69441d9a14a4a719a105f583ff653e53c26e787f39a7d178f689.jpg)

# PowerPAD TM塑料四平板包

![](images/450aabec3d1ddac5e10e060e740126da355278610a5c6f753c89940239558272.jpg)  
人民行动党(S-PQFP-G64)

08/03

NOTES:

A.所有的线性尺寸都以毫米为单位。
本图纸如有更改，恕不另行通知。
阀体尺寸不包括模具闪光或突出部分
D.该封装被设计为焊接到电路板上的热垫上。参见技术简介，PowerPad热增强封装，德州仪器文献编号：SLMA002获取有关推荐电路板布局的信息。本文档可从www.ti.com <http://www.ti.com>获得。
E.属于JEDEC MS-026

#热信息

这款“PowerPAD”封装包含一个外露的热垫，设计用于连接到印刷电路板（PCB）。热垫必须直接焊接到PCB上。焊接后，PCB可以用作“散热器”。此外，通过使用热通孔，热垫可以直接连接到设备电气原理图中所示的适当铜平面上，或者可以连接到设计成PCB的特殊散热器结构上。该设计优化了集成电路（IC）的传热。

有关PowerPAD封装的更多信息以及如何利用其散热能力，请参阅技术简介，PowerPAD热增强封装，德州仪器文献编号：SLMA002和应用简介，PowerPAD Made Easy, Texas InstrumentsSLMA004。这两份文件都可以在www.ti.com上找到。

该包装的外露热垫尺寸如下图所示。

![](images/52767b5f8bc098badc4418137e65f82e3c5c801e4d311aa0efae5ccab94c46a0.jpg)

注：A线性尺寸以毫米为单位，表带特征可能不存在。PowerPAD是德州仪器的商标

![](images/b64e0aad660fb414cb7ec24a5927707e47a9d7eb4b2f4fa7e5a192ef4deb8018.jpg)

A.所有的线性尺寸都以毫米为单位。PowerPAD是德州仪器的商标
本图纸如有更改，恕不另行通知。客户应在电路板制造图上注明不得更改中心阻焊板定义的焊盘。
D.该封装被设计为焊接到电路板上的热垫上。参见技术简介，PowerPad热增强封装，德州仪器文献编号：SLMA002, SLMA004，以及关于特定热信息的产品数据表，通过要求和推荐的电路板布局。这些文档可从www.ti.com <http://www.ti.com>获得。出版物IPC-7351推荐用于替代设计。
E.具有梯形壁和圆角的激光切割孔将提供更好的粘贴释放。客户应联系他们的电路板组装现场的模板设计建议。基于体积为50%的金属负载锡膏的模板设计示例。参考IPC-7525的其他模板建议。
客户应联系他们的电路板制造现场，了解信号垫之间和周围的阻焊容差。

#重要通知和免责声明

Ti提供的技术和可靠性数据（包括数据表）、设计资源（包括参考设计）、应用程序或其他设计建议、网络工具、安全信息和其他资源“按原样”提供，不含任何错误，并不承担所有明示和默示的保证，包括但不限于任何关于适销性、适合特定用途或不侵犯第三方知识产权的默示保证。

这些资源是为熟练的开发人员设计的TI产品。您全权负责(1)为您的应用选择合适的TI产品，(2)设计、验证和测试您的应用，以及(3)确保您的应用符合适用的标准，以及任何其他安全、安保、法规或其他要求。

这些资源如有更改，恕不另行通知。TI允许您仅将这些资源用于开发使用资源中描述的TI产品的应用程序。禁止以其他方式复制和展示这些资源。未授予任何其他TI知识产权或任何第三方知识产权许可。对于因您使用这些资源而产生的任何索赔、损害、成本、损失和责任，TI不承担任何责任，您完全赔偿TI及其代表。

TI的产品是根据TI的销售条款、TI的一般质量指南或其他适用条款提供的，这些条款可以在ti.com上找到，也可以与TI产品一起提供。TI提供的这些资源不会扩大或以其他方式改变TI对TI产品的适用保证或保证免责声明。除非TI明确指定产品为定制或客户指定，否则TI产品是标准的，目录的，通用设备。

TI反对并拒绝您可能提出的任何附加或不同条款。