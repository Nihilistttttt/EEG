# SSD2828QN4

高级信息mipi主桥

本文档包含新产品的信息。规格及资料如有更改，恕不另行通知。

http://www.solomon-systech.com

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 1/168</td><td> 2013年3月版权©2013 Solomon Systech Limited</td></tr></table></body></html>

附录1:SSD2828规范IC修订历史

表身体< html > < > < > < tr > < td >版本< / td > < td >改变项目< / td > < td >生效日期< / td > < / tr > < tr > < td > 1.0 < / td > < td >进步信息的最初版本< / td > < td > 16-Oct-12 < / td > < / tr > < tr > < td > 1.1 < / td > < td >修改的描述,公司(8.1.38节)< / td > < td > 13-Dec-12 < / td > < / tr > < tr > < td > 1.2 < / td > < td >修改时间在RGB数据锁存器(14.4节)指定前缀T在RGB时机(Section14.4)时间< / td > < td > 09-Jan-13 < / td > < / tr > < tr > < td > 1.3 < / td > < td >修改RGB颜色安排(表6 - 3)更新上、下电顺序(Section 15 & Section 16)</td><td>27- 3 -13</td></tr></table></body></html>

# CONTENTS

#总则.....

# 2功能. ...10

2.1引用。10
2.2定义。0

3订购信息11

4框图1

# 5功能描述。14

5.1功能块。
5.2时钟和复位模块
5.3外部接口…
5.4协议控制单元pcu。···
5.5包处理单位（ppu）
5.6纠错码/循环冗余校验（ecc / crc）
长缓冲区和命令缓冲区。…
5.8中断信号·
5.9 d-phy控制器
5.10模拟收发器
5.11内部pl

# 6 ssd2828qn4引脚分配。18

# 7引脚描述

# 8命令表23

8.1注册说明……·
8.1.1设备标识寄存器。
8.1.2 RGB接口控制寄存器
8.1.3 RGB接口控制寄存器
8.1.4 RGB接口控制寄存器
8.1.5 RGB接口控制寄存器
8.1.6 RGB接口控制寄存器
8.1.7 RGB接口控制寄存器
8.1.8配置寄存器......
8.1.9 VC控制寄存器。
8.1.10锁相环控制寄存器  ....................................
8.1.11锁相环配置寄存器  ......................... .
8.1.12时钟控制寄存器。
8.1.13报文大小控制寄存器……
8.1.14包大小控制寄存器
8.1.15包大小控制寄存器
8.1.16通用丢包寄存器
8.1.17运行控制寄存器
8.1.18最大返回大小寄存器
8.1.19返回数据计数寄存器........
8.1.20 ACK响应状态表
8.1.21线路控制寄存器....................
8.1.22中断控制寄存器
8.1.23中断状态寄存器......
8.1.24错误状态寄存器
8.1.25延迟调整寄存器
8.1.26延时调整寄存器
8.1.27延迟调整寄存器3 . ..... ........ 56
8.1.28延时调整寄存器
8.1.29延时调整寄存器
8.1.30延时调整寄存器
8.1.31 HS TX定时器寄存器
8.1.32 HS TX定时器寄存器
8.1.33 LP RX定时器寄存器
8.1.34 LP RX定时器寄存器
  
8.1.35 TE状态寄存器
8.1.36 SPI读寄存器
8.1.37锁相锁寄存器……
8.1.38测试寄存器
8.1.39 TE计数寄存器。
8.1.40模拟控制寄存器
8.1.41模拟控制寄存器
8.1.42模拟控制寄存器3  ....................
8.1.43模拟控制寄存器
8.1.44中断输出控制寄存器
8.1.45 RGB接口控制寄存器
8.1.46车道配置寄存器 .................................
8.1.47延时调整寄存器
  
8.1.48拉控制寄存器
8.1.49拉控制寄存器
8.1.50拉控制寄存器
8.1.51 CABC亮度控制寄存器
8.1.52 CABC亮度控制寄存器
8.1.53 CABC亮度状态注册
8.1.54编码器控制寄存器  ................................
8.1.55视频同步延迟注册  .................. .
8.1.56微调寄存器
8.1.57 GPIO1 Register. . . …   
8.1.58 GPIO2 Register.........................................   
8.1.59 DLYA01 Register   
8.1.60 DLYA23 Register   
8.1.61 DLYB01 Register   
8.1.62 DLYB23注册......
8.1.63 DLYC01寄存器··
8.1.64 DLYC23寄存器。
8.1.65模拟控制寄存器cid。·。
8.1.66读寄存器.....................

# 0配置102

9.1车道管理。102
9.2用例。9.2.1 RGB + SPI接口9.2.2 MIPI DC特性。9.2.3时钟高速传输。9.2.4数据通道状态流9.2.5数据高速传输9.2.6数据通道双向翻转09·9.2.7逃逸模式......... . 069.2.8低功耗数据传输9.2.9复位9.2.10撕裂效果……9.2.11确认..........9.2.12包传输9.2.13海关传输的例子  .........................9.2.14通用报文结构9.2.15长报文格式9.2.16短报文结构

9.2.17 DI (Data Identifier)..............117
9.2.18虚拟通道标识。18
9.2.19数据类型（DT） ...............18
9.3工作模式。22
9.3.2状态机操作...........135
9.3.3 D-PHY操作............. ............36
9.3.4模拟收发器........137
9.3.5锁相环...............137
9.3.6时钟源示例138
10外部接口139
10.1 spi接口8位4线。39
10.2 spi接口8位3线。, 141年
10.2.1 3 or 4 wires 8bit SPI read back sequence for 0xFF register which is stored MIPI read back data ......... 143   
10.3 SPI INTERFACE 24 BIT 3 WIRE...... ............... 145   
10.3.1 3 wires 24bit SPI read back sequence for 0xFF register which is stored MIPI read back data. .............. 147   
11 MAXIMUM RATINGS ............................................................... 149   
12 RECOMMENDED OPERATING CONDITIONS 150   
13直流特性  ..................................151
14交流特性153
14.1 8位4线SPI接口时机 ..................................... ............................................154
14.2 8位3线SPI接口时机 . ..... .................155
14.3 24位3线SPI接口时机 . ........... ...................................56
14.4 RGB接口定时
14.5复位定时158
14.6 tx_clk定时。158
15上电序列159
16关闭顺序160
17系统休眠进出............举例161
18串口链路数据顺序............162
19包信息●● · •• ••• · · · • ●●•165
19.1尺寸为ssd2828qn4 . ..........165

# TABLES

表订购信息。
表6- ssd2828qn4引脚图- 68qfn - ep（俯视图）n4引脚分配68qfn - ep（俯视图）abn4 RGB数据安排abl mm IO RGB射频控制寄存器ab8 -6: RGB接口控制寄存器
表7:RGB接口控制寄存器描述
表8-8:RGB接口控制寄存器描述
表8- 9：配置寄存器描述
表8- 10:vc控制寄存器描述表1：锁相环控制寄存器描述表ab1：锁相环控制寄存器描述表ab1：锁相环控制寄存器描述表ab1：锁相环控制寄存器描述表ab1：锁相环大小控制寄存器描述表ab1：锁相环大小控制寄存器描述表ab1 8：包大小控制寄存器描述表ab1：包大小控制寄存器描述表ab1：包大小控制寄存器描述表ab1：包大小控制寄存器描述表ab1操作控制寄存器des rip最大返回大小寄存器es描述b返回轮数寄存器描述响应状态寄存器描述行控制寄存器描述on tert控制寄存器描述rip中断状态寄存器描述错误状态寄存器描述abl 26 del每天调整寄存器描述。延时调整寄存器描述。一种可调节的寄存器，它有一个可调节的$4 0 / 1$ 1，可调节的寄存器，它有一个可调节的-43：模拟控制寄存器
表8-44：模拟控制寄存器描述
表8-45：中断输出控制寄存器描述
表8-46:RGB接口控制寄存器描述
表8-47:lane配置寄存器描述。
表8-48：延迟调整寄存器描述。
表8-49：拉控制寄存器描述
表8-50：拉控制寄存器描述
表8-51：拉控制寄存器描述表8-52:cabc亮度控制寄存器1描述。
表8-53:cabc亮度控制寄存器2的描述。
表8-54:cabc亮度状态寄存器说明
表8-55：编码器控制寄存器描述
表8-56：视频同步延时寄存器说明。·
表8-57：修整寄存器描述。
表8-58:gpio1寄存器描述
表8-59:gpio1寄存器描述。
表8-60:dlya01寄存器描述
表8-61:dlya23寄存器描述
表8-62:dlyb01寄存器描述
表8-63:dlyb23寄存器描述
表8-64:dlyc01寄存器描述
表8-65:dlyc23寄存器描述
表8-66:acr5寄存器描述。
表8-67：读寄存器描述。
表9-1:ssd2828通道管理
表9-2：视频模式BLLP期间的操作
表9-3:dsi状态码和直流特性
表9-4：数据通道模式进入/退出序列
表9-5：开始传输顺序。
表9-6：传输结束顺序。
表9-7:mipi转义模式输入码·
表9-8：处理器源数据包的数据类型。
表9-9给出了外设源报文的数据类型。
表9-11：非突发模式的PLL设置（使用tx _clk引用PLL）表9-12：突发模式的PLL设置............
表9-13 mipi错误报告
表11-1：最大额定值（电压参考$\ mathm {V _ {S S}}$）…表12-1：推荐工况表13-1：直流特性
表13-2:hs变送器直流特性
表13-3:lp变送器直流特性
表13-4:lp接收机直流特性
表14-1:8位4线spi接口时序特性
表14-2:8位3线spi接口时序特性
表14-3:24位3线spi接口时序特性
表14-4:RGB接口时序特性
表14-5：复位时间。
表14-6:tx_clk定时特性

# FIGURES

图图
ssd2828的时钟方案如图5-1所示。
图8-1延时计算时序图……
图8-2：计算时延的时序
图8-3：唤醒周期延时计算。
图8-4：计算时延的时序
图9-1：带有RGB和spi接口的ssd2828
图9-2:mipi线电平
图9-3：时钟通道在高速模式和低功耗模式之间切换
图9-4：高速突发数据传输
图9-5：周转流程……
图9-6：低功率数据传输…
图9-7:escape模式下的trigger reset命令。
图9-8:escape模式下的撕裂效果命令...............
图9-9:承认命令在逃脱模式 ..................
图9-10：双路数据传输方式（分离、单路）
图9-11：单通道数据传输示例
图9-12：双通道hs传输示例............
第四图:尾数法(长包示例 ) .....................
图9-14：长报文结构。
图9-15：短报文结构。
图9 -16：数据标识符结构…
图9-17:16位/像素RGB颜色格式，长数据包用于mipi接口。
图9-18:18bit / pixel - RGB颜色格式，长数据包用于mipi接口。
图9-19:18bit / pixel（3字节）- RGB颜色格式，长数据包用于mipi接口。图9-20:24bit / pixel - RGB颜色格式，长数据包用于mipi接口
图9-21：具有同步脉冲. .......的非突发模式的RGB接口参数说明22图9-22：具有同步事件和突发模式的非突发模式RGB接口参数示意图.....23图9 -23：非突发模式mipi结构......
图9-24：突发模式mipi结构
图9-25：非读命令后的确认处理。30图9-26：读取后的确认处理命令。
图9-27：中断延迟示意图。
图10-1:8位4线接口写操作示意图…
图10-2:8位4线接口读操作示意图
图10-3 8位三线接口写操作示意图
图10-4:8位3线接口.......的读操作示意图
图10-5:24位三线接口写操作示意图
图10-6:24位三线接口读操作示意图
图14-1:8位4线spi接口时序图
图14-2:8位3线spi接口时序图........
图14-3:24位3线spi接口时序图。
图14-4:RGB接口时序图
图甚佳:TX_CLK时间图  .................
图18-1包装信息…
图18-2标记信息·66图18-3托盘信息。67

# 1总体描述

SSD2828 IC是一款MIPI主桥接芯片，用于连接具有传统并行LCD接口的应用处理器和具有MIPI从接口的LCD驱动程序。2828支持高达1Gbps的每通道速度，最多4通道使用并行RGB接口和串行SPI接口。

# 2特点

通过串行链路支持高达4Gbps的总速率
•支持多达4个数据通道
•与传统的RGB传输相比，信号数量显着减少
•支持高达1920像素每显示行在视频模式，高达60hz刷新率
•支持高达2560像素每显示行在视频模式下，高达$ 30 \ mathm {{h z}}$刷新率降低功耗和减少EMI通过差分对串行数据使用低幅度信号。支持并行RGB接口（DPI 2.0）高达24位支持串行SPI接口（DBI 2.0）高达16位支持MIPI DSI标准中的命令模式和视频模式每个DSI支持独立的双向数据传输（高速低功耗模式下的正向链路和低功耗模式下的反向链路）支持每个DSI在空闲状态下的超低功耗模式支持视频模式下的CABC功能片上PLL具有可变输出频率MIPI模拟和数字电源：（MVDD） 1美元。2 \ mathm {V} + / \ mathm {-} 1 0 \%$
•IO电源：（VDDIO） 1美元。8 \sim 3；3 \ mathm {V} + / \ mathm{-} 1 0 \%$支持MIPI标准DSI(v1.01.00)， DCS(v1.02.00), D-PHY （v1.00）

# 2.1参考文献

MIPI显示串行接口联盟标准，版本1.01
MIPI联盟标准显示命令集，版本1.02
MIPI联盟标准D-PHY，版本1.00
MIPI显示总线接口联盟标准，2.0版
MIPI显示像素接口联盟标准，2.0版

# 2.2定义

HS高速
MIPI显示总线接口联盟标准v2.0 （DBI-2）的SPI Type C接口选项
低功耗
超低功耗状态
RGB MIPI显示像素接口联盟标准2.0 （DPI-2）
虚拟信道

表3-1：订购信息

<html><body><table><tr><td>订购件号</td><td>封装形式</td></tr><tr><td>SSD2828QN4</td><td>68 QFN-EP（托盘形式）</td></tr></table></body></html>

# 4框图

SSD2828集成电路由以下模块组成：

时钟和复位模块
外部接口
协议控制单元
分组处理单元
ECC/CRC   
长缓冲区和命令缓冲区
D-PHY控制器
模拟MIPI收发器
内部锁相环

SSD2828的用法如下图所示。

![](images/ea5dbcbb85cc59cc4553473a31af11a7a68c364250a70983158df34147927d9a.jpg)  
图4-1采用SSD2828的显示系统全景图

下面是SSD2828驱动MIPI从机面板的接口图。支持三种类型的接口：RGB接口和SPI接口。可以通过ps[1:0]引脚选择接口。

![](images/6c2f6aa9891ddccb1d2878abe05bd6192dd361481eb12e9641ec96f0917b377c.jpg)  
图4-2:SSD2828接口示意图

![](images/d9da42cf818b540ad16189a50a8e0ca85901547eac5712722879fa3039ae77df.jpg)  
图4-3框图

# 5功能描述

# 5.1功能模块

# 5.2时钟和复位模块

时钟和复位模块控制整个系统的运行时钟的生成。锁相环有两个参考时钟源。一个来自tx_clk，另一个来自pclk。应用处理器可以通过编程CSS来选择锁相环的参考时钟。锁相环输出时钟用于在HS模式下产生串行链路上的时钟和数据。锁相环频率与1条数据通道上的数据速率相同。因此，锁相环需要根据HS速度进行编程。关于如何编程锁相环，请参考9.3.5。

说明：CSS默认值为0，表示选择tx时钟。因此，在上电之后，tx_clk必须存在，以便可以对寄存器进行编程。如果应用程序处理器想要切换时钟源，必须首先提供tx_clk，以便对CSS字段进行编程。CSS编程完成后，可以关闭tx时钟。

上电后，锁相环处于休眠模式。在使能PLL之前，主机需要对PLL设置进行编程。如果主机需要切换PLL的时钟源，需要先将PLL设置为休眠模式。然后，主机需要用新的设置对锁相环进行编程，使能锁相环。在这两种情况下，锁相环在使能后都需要一定的时间来锁定输出时钟频率。因此，当锁相环处于休眠模式或当锁相环使能但未锁定时，整个系统使用参考时钟运行。锁相环锁定后，系统使用锁相环输出时钟运行。详细的时钟方案请参见下图。由于参考时钟比锁相环输出时钟慢得多，主机也需要在锁相环被锁定之前以低速运行。低速和正常转速的要求请参见14。

![](images/f717f97067c4d4ac1e488d3b93470a13c07e48cb161df53bbf855d8d7aeaaae9.jpg)  
图5-1 SSD2828的时钟方案

提供输出锁信号以供指示。这个信号被连接到一个中断源。主机可以使用中断信号int来决定是低速运行还是正常运行。主机也可以轮询状态位PLS来获取锁的状态。

本文档中提到了各种时钟。下面是对它们的解释。

Bit clock

它是锁相环的输出时钟。是SSD2828中所有时钟的时钟源。

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 14/168 < / td > < td > 2013年3月所罗门Systech < / td > < / tr > < /表> < /身体> < / html >

啃时钟

它是一个频率为1/4位时钟的时钟。

字节时钟

它是一个频率为比特时钟的1/8的时钟。

低功耗时钟

它是由字节时钟生成的时钟。分法器值由字段LPD给出。请参考8.1.12。低功耗时钟周期对应于MIPI D-PHY规范中定义的$2 \ mathm {~ x ~ T _ {L P x}}$。

# 5.3外部接口

外部接口负责与应用程序处理器的通信。支持RGB和SPI两种接口。

哑显示控制器的并行RGB接口。数据总线宽度可以是16位、18位和24位。串行SPI接口的智能显示控制器。SPI接口支持8位3线、8位4线和24位3线三种模式。8位3线模式是MIPI DBI 2.0中指定的C型选项接口。8位4线模式是MIPI DBI 2.0中指定的C型选项3接口。

SPI接口是一个完全独立的接口。具体方案请参见引脚表说明。

SSD2828支持单接口配置。

RGB和SPI接口的组合

该配置主要用于通过MIPI链路驱动哑显示面板。RGB接口将显示数据输入哑显。SPI接口输入用于配置哑显示的数据。如果MIPI从机能够同时控制哑显示板和智能显示板，SPI接口也可以输入驱动智能显示板的数据。

协议控制单元（PCU）

PCU负责处理传出和传入的数据流。它有一个状态机来决定当事件进入时要发送什么包，以及如何对接收到的包做出反应。

# 5.5包处理单元（PPU）

PPU负责分组的组装和拆卸。在传输过程中，它将根据来自PCU的指令形成数据包。在接收过程中，它将从数据包中提取必要的信息并传递给PCU。

纠错码/循环冗余校验（ECC/CRC）

在传输过程中，ECC/CRC模块会对出码流生成ECC或CRC。

在接收过程中，ECC/CRC模块将检查传入流的ECC和CRC字段的正确性。

如果数据和ECC字段中有1位错误，则ECC模块会对该错误进行校正。如果数据和ECC字段的误差大于1位，ECC模块将检测并报告错误。如果数据和CRC字段中至少有1位错误，CRC模块将检测并报告错误。

# 5.7长缓冲区和命令缓冲区

在正向方向上，SSD2828支持DCS短写、DCS长写、通用短写、通用长写和所有视频报文。内部缓冲区用作传入数据的临时存储，因此应用程序处理器在写入下一个数据包之前不需要等待数据包传输。除DCS命令2C/3C外，所有命令报文都存储在命令缓冲区中。所有视频模式下的长数据包和命令模式下DCS命令2C/3C的长数据包将被存储在长缓冲区中。当一个完整的数据包被写入缓冲区后，SSD2828将发送该数据包。

命令缓冲区可以包含一个或多个报文，最大1024字节。只要收到一个完整的数据包，状态机就会指示D-PHY控制器发送数据包。

每个长缓冲区最多可以包含2个数据包。

对于每个缓冲区，有2个状态位相关联。一个是缓冲区空的，另一个是缓冲区可用的。缓冲区空意味着缓冲区中没有包。缓冲区可用意味着有空间可以容纳至少一个数据包。缓冲区状态可以通过中断信号反映到应用处理器。

# 5.8中断信号

提供一个中断信号来触发应用处理器在SSD2828中的某些事件。这些事件包括内部长缓冲区或命令缓冲区空、内部长缓冲区或命令缓冲区可用、准备好回读的数据、来自MIPI slave的确认响应、来自MIPI slave的BTA响应、超时和包操作就绪。请参阅中断寄存器描述和9.3.1.6了解更多细节。

# 5.9 D-PHY控制器

D-PHY控制器负责与模拟收发器的通信。在传输过程中，它接收来自PPU的数据并通知模拟发射机如何传输。在接收过程中，它从模拟接收器接收数据并将数据传递给PPU进行进一步处理。与此同时，它还在执行握手过程，如公交车转弯和不同模式之间的切换。

# 5.10模拟收发器

它由4个数据通道控制器和时钟通道控制器组成。数据通道控制器能够提供反向传输。

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P16/168 < / td > < td > 2013年3月所罗门Systech < / td > < / tr > < /表> < /身体> < / html >

5.11内部锁相环

内部PLL将生成整个系统运行所需的高速时钟。输入参考时钟可以来自tx_clk（_xin，_xio）或pclk。

![](images/8ca0e5b34952d19100cb4aea5e84444a7269135b8ffe3147a215cfc61deffd4d.jpg)  
表6-1:SSD2828QN4 Pinout图- 68 QFN-EP(顶部视图)

表6-2:SSD2828QN4 Pin码配置- 68 QFN-EP(顶视图)

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> QFN电池# & lt; /戴利的小于戴利> >是网络name & lt; /戴利的小于戴利> > QFN电池# & lt; /戴利能够熟练的小于戴利> >name & lt; /戴利> & lt; / tr > & lt小于戴利> 1 & lt; / tr >戴利的小于戴利> > DATAPO & lt; /戴利的小于戴利> > 35 & lt; /戴利的小于戴利> > DATAO & lt; /戴利> & lt; / tr > & lt小于戴利> 2 & lt; / tr >戴利的小于戴利> > DATANO & lt; /戴利的小于戴利> > 36 & lt; /戴利的小于戴利> > DATA1 & lt; /戴利> & lt; / tr > & lt小于戴利> 3 & lt; / tr >戴利的小于戴利> > GND & lt; /戴利的小于戴利> > 37 & lt; /戴利的小于戴利> > DATA2 & lt; /戴利> & lt; / tr > & lt小于戴利> 4 & lt; / tr >戴利的小于戴利> > DATAP1 & lt; /戴利的小于戴利> > 38 & lt; /戴利的小于戴利> > DATA3 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利五& lt; /戴利的小于戴利> > > DATAN1 & lt; /戴利的小于戴利> > 39 & lt; /戴利的小于戴利> > DATA4 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利的小于戴利> > 6 & lt; /戴利> GND & lt; /戴利的小于戴利> > 40 & lt; /戴利的小于戴利> > DATA5 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利的小于戴利> > 7 & lt; /戴利> CLKP & lt; /戴利的小于戴利> > 4 & lt; /戴利的小于戴利> > DATA6 & lt; /戴利> & lt; / tr > & lt小于戴利> 8 & lt; / tr >戴利的小于戴利> > CLKN & lt; /戴利的小于戴利> > 42颗& lt; /戴利的小于戴利> > DATA7 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利9 > & lt; /戴利的小于戴利> > GND & lt; /戴利小于戴利> > 43 & lt; /戴利的小于戴利> > DATA8 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利10 & lt; /戴利的小于戴利> > > DATAP2 & lt; /戴利的小于戴利> > 44 & lt; /戴利的小于戴利> > DATA9 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利11 & lt; /戴利的小于戴利> > > DATAN2 & lt; /戴利的小于戴利> > 45 & lt; /戴利的小于戴利> > DATA10 & lt; /戴利> & lt; / tr > & lt小于戴利> 12 & lt; / tr >戴利的小于戴利> > GND & lt; /戴利的小于戴利> > 46个& lt; /戴利的小于戴利> > DATA11 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利13 & lt; /戴利的小于戴利> > > DATAP3 & lt; /戴利的小于戴利> > 47 & lt; /戴利的小于戴利> > DATA12 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利14 & lt; /戴利的小于戴利> > > DATAN3 & lt; /戴利的小于戴利> > 48 & lt; /戴利的小于戴利> > DATA13 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利15 & lt; /戴利的小于戴利> > > MVDD & lt; / 49戴利的小于戴利> > & lt; /戴利的小于戴利> > DATA14 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利16 & lt; /戴利的小于戴利> > > VDDIO & lt; /戴利的小于戴利> > 50 & lt; /戴利的小于戴利> > DATA15 & lt; /戴利> & lt; / tr > & lt小于戴利> 17 & lt; / tr >戴利的小于戴利> > PSO & lt; /戴利的小于戴利> > 51篇& lt; /戴利的小于戴利> > DATA16 & lt;/戴利> & lt; / tr > & lt tr > & lt;戴利18个& lt; /戴利的小于戴利> > > PS1 & lt; /戴利的小于戴利> > 52 & lt; /戴利的小于戴利> > DATA17 & lt; /戴利> & lt; / tr > & lt小于戴利19 > & lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> >国家博茨瓦纳在内的小于戴利& lt; /戴利> > DATA18 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利的小于戴利> > 20 & lt; /戴利> & lt; /戴利的小于戴利> > 54个国家博茨瓦纳在内的小于戴利& lt; /戴利> > DATA19 & lt; /戴利> & lt; / tr > & lt小于戴利21 > & lt; / tr >队队员造成戴利的小于戴利> > & lt; /戴利的小于戴利> > 55 & lt; /戴利的小于戴利> > DATA20 & lt; /戴利> & lt; / tr > & lt小于戴利> 22 & lt; / tr >戴利的小于戴利> > RESET & lt; /戴利的小于戴利> > 55 & lt; /戴利的小于戴利> > DATA21 & lt; /戴利> & lt; / tr > & lt小于戴利> 23 & lt; / tr >戴利的小于戴利> > SDO & lt; /戴利的小于戴利> > 57 & lt; /戴利的小于戴利> > DATA22 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利24 & lt; /戴利的小于戴利> > > SDI & lt; /戴利的小于戴利> > 58 & lt; /戴利的小于戴利> > DATA23 & lt; /戴利> & lt; / tr > & lt小于戴利> 25 & lt; / tr >戴利的小于戴利> > SCK & lt; /戴利的小于戴利> > 59 & lt; /戴利的小于戴利> > CSXO & lt; /戴利> & lt; / tr > & lt小于戴利> 26 & lt; / tr >戴利的小于戴利> > SDC 6 & lt; /戴利的小于戴利> >0 & lt; /戴利的小于戴利> > MVDD & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > 27 SHUT & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 61 SYS CLK绿恶魔& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利,28 & lt; /戴利的小于戴利> > >穴& lt; /戴利的小于戴利> > 62 & lt; /戴利的小于戴利> > TX CLK_XIO & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; / 29戴利的小于戴利> > HSYNC & lt; / 63戴利的小于戴利> > & lt; /戴利的小于戴利> > TX CLK请& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利30 & lt; /戴利的小于戴利> > > PCLK & lt; /戴利的小于戴利> > 64 & lt; /戴利的小于戴利> > VDDIO & lt; /戴利> & lt; / tr > & lt小于戴利> 31 & lt; / tr >戴利的小于戴利> > MVDD & lt; /戴利的小于戴利> > 65 & lt; /戴利的小于戴利> > GND & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利32 & lt; /戴利的小于戴利> > > GND & lt; /戴利& lt; / 66的小于戴利> >戴利的小于戴利> > MVDD & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > 33 VDDIO & lt; /戴利> & lt;戴利67 & lt; /戴利的小于戴利> > > GND & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利34 & lt; /戴利的小于戴利> > > VSYNC & lt; /戴利的小于戴利> > 68号& lt; /戴利的小于戴利> > VDDIO & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

表6-3：ssd2828qn4 RGB数据安排

<html><body><table><tr><td></td><td>D23</td><td>D22</td><td>D21</td><td>D20</td><td>D19</td><td>D18</td><td>D17</td><td>D16</td><td>D15</td><td>D14 D13</td><td>D12</td><td>D11</td><td>D10</td><td></td><td>D9</td><td>D8</td><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td>D3</td><td>D2</td><td></td><td>D1</td><td>D0</td></tr><tr><td>24bpP</td><td>R7</td><td></td><td></td><td>R4</td><td></td><td></td><td></td><td></td><td>G7</td><td>6</td><td></td><td></td><td></td><td></td><td></td><td>G0</td><td>B7</td><td>B6</td><td>B5</td><td>B4</td><td>B3</td><td></td><td>B2</td><td>B1</td><td>B0</td></tr><tr><td>18bpp</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>G1</td><td>G0</td><td>B5</td><td>B4</td><td>" B3</td><td>"</td><td>B1</td><td></td><td>B0</td></tr><tr><td>16bpp</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>R4</td><td></td><td></td><td></td><td>R0</td><td></td><td></td><td></td><td>G2</td><td>G</td><td>G0</td><td>B4</td><td>B3</td><td>B2</td><td></td><td>B1</td><td>B0</td></tr></table></body></html>

# 7 Pin Description

# SSD2828 Pin Function Description

Key: $\mathrm { { I } = \mathrm { { I n p u t } } }$ $\scriptstyle \mathrm { O = O u t p u t }$ $\mathrm { I } / \mathrm { O } = \mathrm { B i }$ -directional (input/output) ${ \bf P } =$ Power pin $\mathrm { G N D } =$ System VSS

Table 7-1: Power Supply Pins   

<html><主体><表><tr><td>名称</td><td>类型</td><td>连接到</td><td>功能</td><td>< td>不使用时</td></ td></td><td>GND</td><td> </td><td>GND</td><td>电源接地</td><td></td></ td>< tr><td>MVDD</td><td></td><td>电源</td><td>内部模拟/数字和PLL电路电源。(1.2 v + / - -10%) < / td > < td >电路< / td > < / tr > < tr > < td > VDDIO < / td > < td > < / td > < td > < / td > < td >电路< / td > < td > P1.8 ~ 3 + /和数字接口电路。< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

表7-1:MIPI引脚

表身体< html > < > < > < tr > < td >名称< / td > < td >类型< / td > < td >连接到< / td > < td >函数< / td > < td >描述< / td > < td >不使用时< / td > < / tr > < tr > < td > CLKP0 < / td > < td行宽=“5”> < / td > < td行宽=“6”> MIPI Rx < / td > < td行宽=“6”> MIPI < / td > < td >积极微分时钟信号DSI_0 < / td > < / tr > < tr > < td > CLKN0 < / td > < td >负微分时钟信号DSI_0 < / td > < / tr > < tr > < td > DATAPO < / td > < td >积极DSI_0微分数据信号)< / td > < / tr > < tr > < td > DATANO < / td > < td >负微分数据信号0DSI_0</td></tr><tr><td>DATAP1</td><td> DSI_0的正差分数据信号开放</td></tr><tr><td>DATAN1</td><td>信号负差分数据信号</td></tr><tr><td>DATAP2</td><td >DATAN2</td><td> DSI_0的正差分数据信号</td></tr><tr><td>DATAP3</td><td> DSI_0的正差分数据信号</td></tr>< /td><td>的负差分数据信号</td><td>forDSI_0 3 < / td > < / tr > < /表> < /身体> < / html >

表7-2：接口逻辑引脚

表身体< html > < > < > < tr > < td >名称< / td > < td >类型< / td > < td >连接到< / td > < td >函数< / td > < td >描述< / td > < td >不使用时< / td > < / tr > < tr > < td >数据(23:0)< / td > < td > 1 / O < / td > < td行宽=“7”>美联社< / td > < td行宽=“5”>界面< / td > < td > RGB数据为RGB接口< / td > < td >打开< / td > < / tr > < tr > < td > VSYNC < / td > < td > RGB < / td > < td > VSYNC为RGB接口< / td > < td > VDDIO或接地< / td > < / tr > < tr > < td > PCLK < / td > < td行宽= " 4 " > < / td > < td > PCLK为RGB接口< / td > < td > VDDIO或接地< / td > < / tr > < tr > < td >窝< / td > < td > RGB的巢穴界面< / td > < td > VDDIO或接地< / td > < / tr > < tr > < td > HSYNC < / td > < td > HSYNC为RGB接口< / td > < td > VDDIO或接地< / td > < / tr > < tr > < td >署< / td > < td >数据或命令的SPI接口(8位4线)< / td > < td > VDDIO或接地< / td > < / tr > < tr > < td > CSX0 < / td > < td行宽=“4”> < / td > < td行宽=“4”> SPI接口< / td > < td > DSI 0 SPI接口的芯片选择< / td > < td > VDDIO < / td > < / tr > < tr > < td > SCK < / td > < td >串行时钟的SPI接口(8位3线,8位4线,24位3线)</td><td>VDDIO或GND</td></tr><tr><td>SDI</td><td> SPI接口的串行数据输入（适用于8位3线，8位4线，24位3线）</td><td>VDDIO或GND</td></tr><tr><td>0</td><td> SPI接口的串行数据输出（适用于8位3线，8位4线，24位3线）</td><td>Open</td></tr></table></body></html>

Table 7-3: Miscellaneous Pins   

<html><body><table><tr><td>Name</td><td>Type</td><td>Connect to</td><td>Function</td><td>Description</td><td>When not in use</td></tr><tr><td>SHUT</td><td rowspan="3"></td><td rowspan="3">VDDIO GND</td><td rowspan="3">or</td><td>Shutdown signal of RGB interface (to put the driver into sleep mode). - 1: The panel is shut down (Sending 22h packet when SHUT from "0" ) "1" in video mode) - 0: The panel is operating (Sending 32h packet at the beginning of video mode automatically) changes</td><td>VDDIO</td></tr><tr><td>PS[1:0]</td><td>Interface selection signal PS[1:0] is for SPI interface 00: 3 wire 24 bit SPI interface 01: 3 wire 8 bit SPI interface 10: 4 wire 8 bit SPI interface</td><td>GND</td></tr><tr><td>TX_CLK_XIN</td><td>Control 11: SSL internal test mode Signal Input system clock. 8 ~ 30MHz oscillator input.</td><td></td></tr><tr><td></td><td>I/O</td><td>Exte </td><td>Input crystal range for the crystal 30Mhz Input system clock. Open.</td><td>8Mhz to</td><td></td></tr><tr><td>TX_CLK_XIO</td><td></td><td></td><td>Input crystal range for 30Mhz</td><td>the crystal oscillator input. 8Mhz to</td><td>Open</td></tr><tr><td>SYS_CLK_OUT RESET</td><td></td><td>VDDIO or GND</td><td>Active low reset signal</td><td>Output system clock for MIPI slave to the chip</td><td>Open VDDIO</td></tr></table></body></html>

Table 8-1: SSD2828 Register Summary   

<html><body><table><tr><td colspan="4"></td></tr><tr><td>Offset</td><td>Name</td><td>Mnemonic</td><td>Reset Value</td></tr><tr><td>0xB0</td><td>Device Identification Register</td><td>DIR</td><td>0x2828</td></tr><tr><td>0xB1</td><td>RGB Interface ontrol Register</td><td>VICR1</td><td>0x020A</td></tr><tr><td>0xB2</td><td>RGB Interface Control Register</td><td>VICR2</td><td>0x0214</td></tr><tr><td>0xB3</td><td>RGB Interface Control Register</td><td>VICR3</td><td>0x0428</td></tr><tr><td>0xB4</td><td>RGB Interface Control Register 4</td><td>VICR4</td><td>0x0780</td></tr><tr><td>0xB5</td><td>RGB Interface Control Register</td><td>VICR5</td><td>0x0438</td></tr><tr><td>0xB6</td><td>RGB Interface Control Register 6</td><td>VICR6</td><td>0x0024</td></tr><tr><td>0xB7</td><td>Configuration Register</td><td>CFGR</td><td>0x0301</td></tr><tr><td>0xB8</td><td>VC Control Register</td><td>VCR</td><td>0x0045</td></tr><tr><td>0xB9</td><td>PLL Control Register</td><td>PCR</td><td>0x0000</td></tr><tr><td>0xBA</td><td>PLL Confi guration Register</td><td>PLCR</td><td>0x8120</td></tr><tr><td>0xBB</td><td>Clock Control Register</td><td>CCR</td><td>0x0003</td></tr><tr><td>0xBC</td><td>Packet Size Control Register</td><td>PSCR1</td><td>0x0000</td></tr><tr><td>0xBD</td><td>Packet Size Control Register 2</td><td>PSCR2</td><td>0x0000</td></tr><tr><td>0xBE</td><td>Packet Size Control Register 3</td><td>PSCR3</td><td>0x0100</td></tr><tr><td>0xBF</td><td>Packet Drop Register</td><td>PDR</td><td>0x0000</td></tr><tr><td>0xC0</td><td>Operation Control Register</td><td>OCR</td><td>0x0000</td></tr><tr><td>0xC1</td><td>Maximum Return Size Register</td><td>MRSR</td><td>0x0001</td></tr><tr><td>0xC2</td><td>Return Data Count Register</td><td>RDCR</td><td>0x0000</td></tr><tr><td>0xC3</td><td>ACK Response Register</td><td>ARSR</td><td>0x0000</td></tr><tr><td>0xC4</td><td>Line Control Register</td><td>LCR</td><td>0x0000</td></tr><tr><td>0xC5</td><td>Interrupt Control Register</td><td>ICR</td><td>0x0080</td></tr><tr><td>0xC6</td><td>Interrupt Status Register</td><td>ISR</td><td>0xCF06</td></tr><tr><td>0xC7</td><td>Error Status Register</td><td>ESR</td><td>0x0000</td></tr><tr><td>0xC9</td><td>Delay Adjustment Register</td><td>DAR1</td><td>0x1402</td></tr><tr><td>0xCA</td><td>Delay Adjustment Register 2</td><td>DAR2</td><td>0x2803</td></tr><tr><td>0xCB</td><td>Delay Adjustment Register 3</td><td>DAR3</td><td>0x0416</td></tr><tr><td>0xCC</td><td>Delay Adjus Register 4 tment</td><td>DAR4</td><td>0x0A0A</td></tr><tr><td>0xCD</td><td>Delay Adjustment Register 5</td><td>DAR5</td><td>0x1000</td></tr><tr><td>0xCE</td><td>Delay Adju stment Register 6</td><td>DAR6</td><td>0x0405</td></tr><tr><td>0xCF</td><td>HS TX Timer Register</td><td>HTTR1</td><td>0x0000</td></tr><tr><td>0xD0</td><td>HS TX Timer Register 2</td><td>HTTR2</td><td>0x0010</td></tr><tr><td>0xD1</td><td>LP RX Timer Register</td><td>LRTR1</td><td>0x0000</td></tr><tr><td>0xD2</td><td>LP RX Timer Register 2</td><td>LRTR2</td><td>0x0010</td></tr><tr><td>0xD3</td><td>TE Status Register</td><td>TSR</td><td>0x0000</td></tr><tr><td>0xD4</td><td>SPI Read Register</td><td>LRR</td><td>0x00FA</td></tr><tr><td>0xD5</td><td>PLL Lock Register</td><td>PLLR</td><td>0x1450</td></tr><tr><td>0xD6</td><td>Test Register</td><td>TR</td><td>0x0005</td></tr><tr><td>0xD7</td><td>TE Count Register</td><td>TECR</td><td>0x0001</td></tr><tr><td>0xD8</td><td>Analog Control Register</td><td>ACR1</td><td>0x2020</td></tr><tr><td>0xD9</td><td>Analog Control Register</td><td>ACR2</td><td>0x64A0</td></tr><tr><td>0xDA</td><td>Analog Control Register</td><td>ACR3</td><td>0x99A4</td></tr><tr><td>0xDB</td><td>Analog Control Register</td><td>ACR4</td><td>0x8098</td></tr><tr><td>0xDC</td><td>Interrupt Output Control Register</td><td>IOCR</td><td>0x0000</td></tr><tr><td>0xDD</td><td>RGB Interface Control Register</td><td>VICR7</td><td>0x0000</td></tr><tr><td>0xDE</td><td>Lane Configuration Register</td><td>LCFR</td><td>0x0000</td></tr><tr><td>0xDF</td><td>Delay Adjustment Register</td><td>DAR7</td><td>0x0010</td></tr><tr><td>OxE0</td><td>Pull Control Register</td><td>PUCR1</td><td>0x5556</td></tr><tr><td>0xE1</td><td>Pull trol Register Cont</td><td>PUCR2</td><td>0x6656</td></tr><tr><td>0xE2</td><td>Pull Control Register</td><td>PUCR3</td><td>0x0159</td></tr></table></body></html>

<html><body><table><tr><td>Offset</td><td>Name</td><td>Mnemonic</td><td>Reset Value</td></tr><tr><td>0xE9</td><td>CABC Brightness Control Register</td><td>CBCR1</td><td>0x0000</td></tr><tr><td>OxEA</td><td>CABC Brightness Control Register 2</td><td>CBCR2</td><td>0x6900</td></tr><tr><td>0xEB</td><td>CABC Brightness Status Register</td><td>CBSR</td><td>0x0000</td></tr><tr><td>0xEC</td><td>Encoder Control Register</td><td>ECR</td><td>0x7800</td></tr><tr><td>0xED</td><td>Video Sync Delay Register</td><td>VSDR</td><td>0x0002</td></tr><tr><td>0xEE</td><td>Trimming Register</td><td>TMR</td><td>0x0000</td></tr><tr><td>OxEF</td><td>GPIO Register 1</td><td>GPIO1</td><td>0x0000</td></tr><tr><td>0xF0</td><td>GPIO Register 2</td><td>GPIO2</td><td>0x0000</td></tr><tr><td>OxF1</td><td>DLYA01 Register</td><td>DLYA01</td><td>0x2020</td></tr><tr><td>0xF2</td><td>DLYA23 Register</td><td>DLYA23</td><td>0x2020</td></tr><tr><td>0xF3</td><td>DLYB01 Register</td><td>DLYB01</td><td>0x2020</td></tr><tr><td>0xF4</td><td>DLYB23 Register</td><td>DLYB23</td><td>0x2020</td></tr><tr><td>0xF5</td><td>DLYC01 Register</td><td>DLYC01</td><td>0x2020</td></tr><tr><td>0xF6</td><td>DLYC23 Register</td><td>DLYC23</td><td>0x2020</td></tr><tr><td>0xF7</td><td>Analog Control Register 5</td><td>ACR5</td><td>0x0000</td></tr><tr><td>0xFF</td><td>Read Register</td><td>RR</td><td>0x0000</td></tr></table></body></html>

# 8.1注册说明

# 8.1.1设备标识寄存器

偏移地址

DIR

#设备标识寄存器

表8-2：设备标识寄存器描述

<html><body><table><tr><td>BIT</td><td>15</td><td>4</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">DIR[15:8]</td></tr><tr><td>TYPE</td><td colspan="8">RO</td></tr><tr><td>RESET</td><td colspan="8">0x28</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">DIR[7:0]</td></tr><tr><td>TYPE</td><td colspan="8">RO</td></tr><tr><td>RESET</td><td colspan="8">0x28</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>DIR Bit 15-0</td><td>Device Identification Number</td><td>0x2828</td></tr></table></body></html>

# 8.1.2 RGB Interface Control Register 1

偏移地址

VICR1

# RGB Interface Control Register 1

0xB1

表8-3:RGB接口控制寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> VSA RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x02 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HSA < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x0a < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VSA Bit 15-8</td><td>垂直同步激活周期这些位指定垂直同步激活周期。垂直同步活动是从垂直同步下降沿到上升沿，在同步脉冲方面。它仅用于同步脉冲的非突发模式。详情请参阅9.3。period</td><td>最小值为1。</td></tr><tr><td>HSA Bit 7-0</td><td>水平同步激活周期-这些位指定Hsync激活周期。Hsync活动周期是从Hsync下降沿到沿，以pclk表示。它仅用于同步脉冲的非突发模式。详情请参阅9.3。< / td上升> < td >最小值是1。< / td > < / tr > < /表> < /身体> < / html >

# 8.1.3 RGB接口控制寄存器

VICR2

# RGB接口控制寄存器

表8-4:RGB接口控制寄存器2描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> VBP < / td > < / tr > < tr > < td >类型< / td > < td colspan =“9”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“9”> 0 x02 < / td > < / tr > < tr > < td > < / td > < td colspan =“9”> < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HBP < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 9 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“9”> 0 x14 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VBP Bit 15-8</td><td>垂直后门廊周期这些位以Hsync脉冲的形式指定垂直后走廊周期。垂直后廊周期取决于视频模式，如果模式为同步脉冲的非突发模式，则从Vsync上升沿到第一行有源显示的Hsync。如果模式为带有Sync事件的非突发模式，则从Vsync下降沿到Hsync的第一行主动显示。如果为burst模式，则与具有Sync事件的非burst模式相同。详情请参阅9.3。水平后廊时期这些钻头设置。</td><td></td></tr><tr><td>HBP Bit 7-0</td><td>以pclk表示水平后廊时段。水平后廊周期取决于非突发模式，如果模式为同步脉冲的非突发模式，则从Hsync上升沿到有效显示像素的开始。如果模式是具有同步事件的非突发模式，则从Hsync下降沿到有效显示像素的开始。如果为burst模式，则与配置了Sync事件的-burst模式相同。详情请参阅9.3。设置。非< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 8.1.4 RGB接口控制寄存器

偏移地址

表8-5:RGB接口控制寄存器3描述

表身体< html > < > < > < tr > < td colspan =“9”> VICR3 RGB接口控制Register3 0 xb3 < / td > < / tr > < tr > < td >位< / td > < td > 15 < / td > < td > < / td > < td > < / td > < td > 12 < / td > < td > < / td > < td > 0 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”> VFP RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x04 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>六< / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x28 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VFP Bit 15-8</td><td>垂直前厅周期这些位指定Hsync脉冲的垂直前厅周期。垂直前廊周期是从主动显示的最后一行之后的第一个垂直同步到下一个垂直同步下降沿。详情请参阅9.3。</td><td></td></tr><tr><td>HFP Bit 7-0</td><td>水平前廊周期这些位以pclk表示水平前廊周期。水平前廊周期是从有效显示结束到下一个Hsync下降沿。详情请参阅9.3。像素< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 8.1.5 RGB接口控制寄存器

偏移地址

VICR4

# RGB接口控制寄存器

0xB4

表8-6:RGB接口控制寄存器4描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HACT(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x07 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HACT [7:0] < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x80 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>HACT Bit 15-0</td><td>水平活动周期这些位以pclk表示水平周期。在水平活跃期，den信号应该一直是高的。active</td><td>最大值为0x0A00 </td></tr></table></body></html>

# 8.1.6 RGB接口控制寄存器

偏移地址

VICR5

# RGB接口控制寄存器

0xB5

表8-7:RGB接口控制寄存器

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> VACT(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x04 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> VACT [7:0] < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x38 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VACT Bit 15-0</td><td>垂直活跃周期这些位以Hsync脉冲表示垂直活跃周期。< / td > < td >最小值< / td > < / tr > < /表> < /身体> < / html >

# 8.1.7 RGB Interface Control Register 6

偏移地址

VICR6

# RGB Interface Control Register6

0xB6

Table 8-8: RGB Interface Control Register 6 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td>VS_P</td><td>HS_P</td><td>PCLK_P</td><td></td><td></td><td></td><td></td><td>CBM</td></tr><tr><td>TYPE</td><td>RW</td><td>RW</td><td>RW</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td>NVB</td><td>NVD</td><td>BLLP</td><td>VCS</td><td colspan="2">VM</td><td colspan="2">VPF</td></tr><tr><td>TYPE</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td colspan="2">RW</td><td colspan="2">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x1</td><td>0x0</td><td colspan="2">0x1</td><td colspan="2">0x0</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VS_P Bit 15</td><td>VS_P – This bit control the polarity of the Vsync pulse input.</td><td>0 − Vsync Pulse is active low 1 − Vsync Pulse is active high</td></tr><tr><td>HS_P Bit 14</td><td>HS_P – This bit control the polarity of the Hsync pulse output.</td><td>0 – Hsync Pulse is active low 1 – Hsync Pulse is active high</td></tr><tr><td>PCLK_P Bit 13</td><td>PCLK_P This bit control the polarity of the CM output.</td><td>Data is launch at falling edge, SSD2828 latch data at rising edge 1 – Data is launch at rising edge, SSD2828 latch data at falling 0-</td></tr><tr><td>Reserved Bit 12-9</td><td></td><td>edge</td></tr><tr><td>CBM Bit 8</td><td>Compress Burst Mode Control – If the mode is burst and this bit is 1, MPITX will send video packet in compressed burst mode (i.e. no after horizontal sync packet) blanking packet</td><td>0 – Video with blanking packet. 1- Video with no blanking packet.</td></tr><tr><td>NVB Bit 7</td><td>Non Video Data Burst Mode Control – This bit specifies how non video data will be interleaved with video data transmission in burst mode.</td><td>0−Non video data will be transmitted during any BLLP period. 1- Non video data will only be transmitted during vertical blanking period.</td></tr><tr><td>NVD Bit 6</td><td>Non Video Data Transmission Control –This specifies how non video data will be interleaved with video data transmission. Please refer to 9.2.1 for more details. The SSD2828 will send non video data (written from the SPI interface) during the vertical blanking period (non burst mode) or any BLLP period in burst mode (depends on NVB setting). The data can be sent either in high speed mode or low power mode. This bit selects which mode to use. If LP mode is selected, the data bit</td><td>0−Non video data will be transmitted using HS mode. 1-Non video data will be transmitted using LP mode.</td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P31/168</td><td>Mar 2013</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td></td><td>lane will enter LP mode for BLLP period, even if there is no non-video data to send. Please note that sending data in LP mode is much slower than HS mode. It is the responsibility of the host processor to make sure that the duration is long enough to finish the data transfer and the timing of Hsync and Vsync is not affected.</td><td></td></tr><tr><td>BLLP Bit 5</td><td>BLLP Control - This bit specifies the SSD2828 operation during BLLP period. This bit takes effect only for non burst mode and NVD being 0. When the video mode is burst mode, the SSD2828 will not send any blanking packet during BLLP. It will enter LP mode. When NVD is in non burst mode, the SSD2828 will stay in LP mode after sending the non video data (if there is any), until the BLLP period ends. When NVD is 0 in non burst mode, the SSD2828 will use this bit to decide whether to send blanking packet or enter LP mode after sending non video data (if there i any), until the BLLP ends. Please note that entering and exiting from LP mode needs more time, as the speed of LP mode is slow. It is the responsibility of the host processor to make sure that the period is long enough to finish the data transfer and the timing period</td><td>0 Blanking packet will be sent during BLLP period. 1 − LP mode will be used during BLLP period.</td></tr><tr><td>VCS Bit 4</td><td>of Hsync and Vsync is not affected. Video Clock Suspend – This bit specifies the clock lane behavior. This bit is only applicable for burst mode. When the video mode non burst mode, the clock lane will remain in HS mode all the time.</td><td>0 The clock lane remains in HS mode, when there is no data to transmit. — The clock lane enters LP mode when there is no data to transmit. — Non burst mode with sync</td></tr><tr><td>VM Bit 3-2</td><td>Video Mode These bits specify the video mode the SSD2828 will use, when RGB interface is selected. Please refer to MIPI DSI for the definition of different modes. Video Pixel Format These bits specify the</td><td>00 pulses 01 Non burst mode with sync events 10 Burst mode 11 Reserved</td></tr><tr><td>VPF Bit 1-0</td><td>pixel format for video mode.</td><td>00 01 packed 10 loosely 11 16bpp 18bpP, 18bpp, packed 24bpp</td></tr></table></body></html>

<html><body><table><tr><td>24bpp</td><td>D23</td><td>D22</td><td>D21</td><td>D20</td><td>D19 D18</td><td>D17</td><td>D16</td><td>D15</td><td>D14</td><td>D13</td><td>D12</td><td>D11</td><td>D10</td><td>D9</td><td>D8</td><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td>D3</td><td>D2</td><td>D1</td><td>D0</td></tr><tr><td>18bpp</td><td>X</td><td></td><td></td><td></td><td></td><td>X</td><td>D17</td><td>D16 D15</td><td>D14</td><td>D13</td><td>D 12</td><td></td><td>D10</td><td>D9</td><td>D8</td><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td>D3</td><td>D2</td><td>D1</td><td>D0</td></tr><tr><td>16bpp</td><td>X</td><td></td><td></td><td></td><td></td><td>X</td><td>X</td><td>D15</td><td>D14</td><td>D13</td><td>D12</td><td>D11</td><td>D10</td><td>D9</td><td>D8</td><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td>D3 D2</td><td>D1</td><td></td><td>D0</td></tr></table></body></html>

# 8.1.8 Configuration Register

偏移地址

# Configuration Register

CFGR   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td>TXD</td><td>LPE</td><td>EOT</td><td>ECD</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x1</td><td>0x1</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td>REN</td><td>DCS</td><td>CSS</td><td>HCLK</td><td>VEN</td><td>SLP</td><td>CKE</td><td>HS</td></tr><tr><td>TYPE</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x1</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

0xB7

Table 8-9: Configuration Register Description   

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-12</td><td></td><td></td></tr><tr><td>TXD Bit 11</td><td>Transmit Disable This bit specifies whether the SSD2828 will disable the sending of MIPI Packets stored in the buffers. Software can enable TXD, fill out the buffers and then disable it to send all packets out in burst.</td><td>0- Transmit on 1- Transmit halt</td></tr><tr><td>LPE Bit 10</td><td>Long Packet Enable This bit specifies whether the SSD2828 will send out a Generic Long Write Packet or Generic Short Write Packet when the payload is no more than 2 bytes. It also specifies whether the SSD2828 will send out a DCS Long Write Packet or DCS Short Write Packet when the payload is no more than byte.</td><td>0– Short Packet 1 − Long Packet</td></tr><tr><td>EOT Bit 9</td><td>EOT Packet Enable This bit specifies whether the SSD2828 will send out the EOT packet at the end of HS transmission or not.</td><td>0 – Do not send 1–Send</td></tr><tr><td>ECD Bit 8</td><td>ECC CRC Check Disable - This bit specifies whether SSD2828 will perform ECC and CRC checking for the packets received from the MIPI slave.</td><td>0 Enable 1 Disable</td></tr><tr><td>REN Bit 7</td><td>Read Enable –This bit specifies whether the next operation is a write or read operation.</td><td>0– Write operation 1 – Read operation</td></tr><tr><td>DCS Bit 6</td><td>DCS Enable This bit specifies whether the packet to be sent is DCS packet or generic packet. This bit applies for both write and read operation.</td><td>0 Generic packet (The packet can be any one of Generic Long Write, Generic Short Write, Generic Read packet, depending on the configuration.) 1 – DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)</td></tr></table></body></html>

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 33/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>CSS Bit 5</td><td>时钟源选择-此位为PLL选择时钟源。切换时钟源时的系统行为请参见5.2。CSS设置只需要编程</td><td>0 -时钟源为tx_clk 1 -时钟源为pclk</td></tr><tr><td>HCLK Bit 4</td><td>当PEN为1时不生效。HS时钟使能在反向通信时控制时钟通道的行为。此位仅在CKE为0且VEN为0时生效。0.</td><td>0−HS时钟开启1 - HS时钟关闭</td></tr><tr><td>VEN Bit 3</td><td>Video Mode Enable视频模式开启。只有将该位设置为1后，才能启用视频模式。该位仅在接口设置为RGB SPI时生效。视频模式操作请参考0。</td><td>0−关闭视频模式1 -开启视频模式</td></tr><tr><td>SLP Bit 2</td><td>休眠模式使能该位控制休眠模式操作。休眠模式操作请参见9.3.2。当该位设置为1时，HS位将自动清除为0。</td><td>0−禁用休眠模式1-启用休眠模式。只有注册接口是活动的。Sleep</td></tr><tr><td>CKE Bit 1</td><td>时钟通道使能-当数据通道进入LP模式时，该位控制时钟通道模式。</td><td>0 -时钟通道将进入LP模式，如果它不是在反向通信。如果是反向通信，时钟通道将遵循HCLK的设置。1 -时钟通道对所有in</td></tr><tr><td>HS比特0</td><td>HS模式-这个比特控制SSD2828是使用HS还是LP模式发送数据。在</td><td>的情况下，该位会受到SLP位值的影响。0−LP模式1小时模式< / td > < / tr > < /表> < /身体> < / html >

# 8.1.9 VC控制寄存器

VCR

# VC控制寄存器

表8-10:VC控制寄存器描述

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td></td><td></td><td></td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="2">VCM</td><td colspan="2">VCE</td><td colspan="2">VC2</td><td colspan="2">VC1</td></tr><tr><td>TYPE</td><td colspan="2">RW</td><td colspan="2">RW</td><td colspan="2">RW</td><td colspan="2">RW</td></tr><tr><td>RESET</td><td colspan="2">0x1</td><td colspan="2">0x0</td><td colspan="2">0x1</td><td colspan="2">0x1</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-8</td><td></td><td></td></tr><tr><td>VCM Bit 7-6</td><td>Virtual Channel ID for Maximum Return Size Packet – These bits specify the VC ID for the Maximum Return Size Packet sent by SSD2828. This register field is included as the VC ID for this packet might be different from the VC ID actual data.</td><td></td></tr><tr><td>VCE Bit 5-4</td><td>for the packets carrying the Virtual Channel ID for EOT Packet – These bits specify the VC ID for the EOT Packet sent by SSD2828. This register field is included as the VC ID for this packet might be different from the VC ID for the packets carrying the actual data.</td><td></td></tr><tr><td>VC2 Bit 3-2</td><td>Virtual Channel ID for SPI Interface These bits specify the VC ID for the packets written in through the SPI interface, when the interface setting is RGB + SPI This register field is included as the RGB 十 SPI interface can address two different LCD panels at the same time. The VC ID for the is different. two panels</td><td></td></tr><tr><td>VC1 Bit 1-0</td><td>Virtual Channel ID for RGB Interface - These bits specify the VC ID for the packets written in through the RGB interface, when the interface is RGB + SPI</td><td></td></tr></table></body></html>

# 8.1.10 PLL Control Register

PCR

# PLL Control Register

0xB9

Table 8-11: PLL Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td colspan="2">SYSD</td><td>SYS_DIS</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td colspan="2">RW</td><td>RW</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td colspan="2">0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>PEN</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>SYSD Bit 15-14</td><td>SYS_clk Divider These bits give the divider value for generating the sys_clk output from the tx_clk or crystal input.</td><td>00 Divide by 1 01 Divide by 2 10 Divide by 4 11 Divide by 8</td></tr><tr><td>SYS_DIS Bit 13</td><td>SYS_clk DISable This bit will shut off the Sys clk signal output when enabled.</td><td>0– Enable Sys_clk output 1 – Disable Sys_clk output</td></tr><tr><td>Reserved Bit 12-1</td><td></td><td></td></tr><tr><td>PEN Bit 0</td><td>PLL Enable This bit controls the PLL operation.</td><td>0 – PLL power down 1 – PLL enable</td></tr></table></body></html>

Remark: Frequency of PLL can only be changed during $\mathrm { P E N } = 0$

# 8.1.11 PLL Configuration Register

PLCR

# PLL Configuration Register

0xBA

表8-12:PLL配置寄存器说明

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“2”> FR < / td > < td > < / td > < td > < / td > < td colspan =“4”>女士< / td > < / tr > < tr > < td >类型< / td > < td colspan =“2”> RW < / td > < td > RO < / td > < td colspan =“5”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“2”> 0 x2 < / td > < td > 0 x0 < / td > < td > < / td > < td > < / td > < td > 0 x01 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 9 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > 5 < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> NS < / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x20 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>FR Bit 15-14</td><td>频率范围这些位选择输出时钟的范围。只有当PEN为0时，才能对FR设置进行编程。当PEN为1时，该参数无效。</td><td>00 62.5 < fouT< 125 01 - 126 <250 10-251 < fouT< 500 11-501 < fouT< 1000 < fouT</td></tr><tr><td>保留位13 MS</td><td>分频器这些位指定PLL pre-</td><td>0x00: MS=1</td></tr><tr><td>Bit 12-8</td><td>PLL分频器的值，MS.鉴相器的频率，f REF由fpre /MS决定，输入频率，f N和鉴相器频率，f REF应在5Mhz到100Mhz之间。当PEN为0时，只有当I</td><td>0x01: MS=1 0x02 MS=2 0x1F MS=31</td></tr><tr><td>NS Bit 7-0</td><td>时，MS设置才需要编程。当PEN 1时，它不起作用。锁相环乘法器这些位指定锁相环输出频率乘法器值NS。输出频率，fouT = f RE * NF</td><td>0x00: NS=1 0x01: NS=1 0x02: NS=2 0xFF: NS=255</td></tr></table></body></html>

e.

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 37/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

# 8.1.12时钟控制寄存器

CCR

偏移地址

# Clock Control Register

0xBB

Table 8-13: Clock Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">LPD</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td colspan="5">0x03</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-6</td><td></td><td></td></tr><tr><td>LPD Bit 5-0</td><td>LP Clock Divider These bits give the divider value for generating the LP mode clock from the</td><td>0x0 Divide by 1 0x1− Divide by 2 0x3F Divide by 64</td></tr></table></body></html>

备注：例如$\ mathm {L P D} = 0 \ mathm {x} 4$
PLL $= 4 0 0 \mathrm {M b p s}$
LP时钟$\ l =$ 400Mbps / LPD $8 = 400 / 5 / 8 = 1 0 \ mathm {M H z}$

# 8.1.13包大小控制寄存器

偏移地址

表8-14：包大小控制寄存器描述

表身体< html > < > < > < tr > < td colspan =“9”> PSCR1 acket大小控制寄存器0 xbc < / td > < / tr > < tr > < td >位< / td > < td > 15 < / td > < td > < / td > < td > < / td > < td > 2 < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td > < / td > < td > < / td > < td > < / td > < td > TDC(很高)< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 7 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“7”> TDC [7:0] < / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td rowspan="5">TDC Bit 15-0</td><td rowspan="2">发送数据计数这些位设置了SSD2828在下次操作中发送的数据字节总数。SSD2828将使用该字段中的值来决定发送哪种类型的数据包。当命令为0x2C或0x3C时，TDC和PST（0xBE）的设置会将传输模式设置为分区模式和非分区模式。分区模式（TDC > PST）用于DCS时DCS</td><td> TDC > PST时的分区模式。非分区模式当TDC <= PST。Maximum TDC = 4096(0x1000)</td></tr><tr><td></td></tr><tr><td rowspan="2">命令为0x2C或0x3C时，写入传输的最大字节数没有限制。PST最大可设置为4096字节。SSD2828将自动插入0x3C命令。最大MCU速度的链路频率。非分区模式（TDC <= PST） For</td><td></td></tr><tr><td>在这些边界。当DCS命令为0x2C或0x3C时，写入时传输的最大字节数为</td></tr></table></body></html>

# 8.1.14包大小控制寄存器

偏移地址

PSCR2

#包大小控制寄存器

0xBD

表8-15：包大小控制寄存器2描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> TDC [31:24] < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x0 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> TDC (23:16) < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x0 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TDC[31:16] Bit 31-0</td><td>发送数据计数</td></ table></body></html>

# 8.1.15包大小控制寄存器

偏移地址

PSCR2

# Packet Size Control Register 3

Table 8-16: Packet Size Control Register 3 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td>PST[12:8]</td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td colspan="5">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td></td><td></td><td>0x1</td><td></td><td></td></tr><tr><td></td><td colspan="6"></td><td colspan="2"></td></tr><tr><td>BIT</td><td></td><td></td><td>b</td><td></td><td></td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="6">PST[7:0]</td><td></td><td></td></tr><tr><td>TYPE</td><td colspan="7">RW</td></tr><tr><td>RESET</td><td colspan="6">0x00</td><td></td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-13</td><td></td><td></td></tr><tr><td>PST Bit 12-0</td><td>Packet Size Threshold These bits give the threshold value for partitioning the incoming long packet data into smaller packets. The partitioning only applies to the DCS Long Write packet with DCS command being 0x2C or 0x3C in Command mode(if_sel=1). The payload will be partitioned into multiple packets. The PST represents the threshold in term of bytes. The maximum MCU speed at the input is 1/10 of the link frequency. Please refer to 9.3.1.1 for more details.</td><td>The maximum value allowed is 4096 bytes. Program PST<TDC will allows auto insertion of 0x3C at the PST boundary(Partition mode). If the user tries to program a larger value than the maximum allowed value into this field, SSD2828 will cap the value to the maximum value. When the interface setting is 16- bit, the value in this field must be multiple of 2 bytes, or even number. If an odd number is written in, it will e automatically truncated to an even number. For example, 0x5 will be truncated to 0x4. When the interface setting is 24- bit, the value in this field must be in multiple of 3 bytes</td></tr></table></body></html>

# 8.1.16通用丢包寄存器

表8-17：通用丢包寄存器描述

表身体< html > < > < > < tr > < td colspan =“9”> GPDR通用丢包注册0 xbf < / td > < / tr > < tr > < td >位< / td > < td > 15 < / td > < td > < / td > < td > 3 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>加仑日(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“6”>我们< / td > < td > < / td > < / tr > < tr > < td >重置< / td > < td colspan =“6”> 0 x0 < / td > < / tr > < tr > < td > < / td > < td colspan =“6”> < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td名称colspan = " 7 " >加仑日[7:0]< / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”>我们< / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x0 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td> <td>描述该寄存器</td><td>Setting</td></tr><tr><td>GPD Bit 15-0</td><td>Generic Packet Drop不是一个真正的寄存器。它是内部缓冲区的入口点。通用包的有效负载（通用短写、通用长写和通用读），或者如果DCS包应该写入这个寄存器，则命令和有效负载。SSD2828将使用相应的通用或DCS数据包发送它们。CFGR寄存器的DCS字段将用于确定进入该寄存器的数据是用于通用还是DCS包生成。应用处理器可以把这个寄存器当作一个FIFO，并不断地向它写入数据。当接口为16位时，该字段的宽度为16位。当接口为8位时，该字段的宽度为8位。由于寄存器只是内部缓冲区的入口点，所以应用程序处理器不能读取写入缓冲区的数据</td><td></td></tr></table></body></html>

# 8.1.17运行控制寄存器

OCR

# Operation Control Register

0xC0

Table 8-18: Operation Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>RST</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RWAC</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>COP</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RWAC</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-9</td><td></td><td></td></tr><tr><td>RST Bit 8</td><td>Software Reset Writing a '1' to this bit will reset the entire module except SSD2828 local register setting. This bit will be cleared after the reset is completed. Wrting a '1' to this bit will cause the MIPI link enters TX stop state immediately and any outgoing MIPI packet will be terminated</td><td>0–NOP 1 – Software Reset</td></tr><tr><td>Reserved Bit 7-1</td><td>immediately.</td><td>0- NOP</td></tr><tr><td>COP Bit 0</td><td>Cancel Operation This bit is cancel the current operation. When this bit is set to 1, the SSD2828 will still finish transmitting the current packet. (Otherwise, the serial link operation will lose sync.) Afterwards, the SSD2828 will stop any further transmission. It will clear its internal buffer such that all the data being written in and not sent out yet will be cleared. It will also bring the state machine to its initial state. Once this process is finished, the COP bit will be automatically set to At the same time, the PO bit of the status register will be set to too. At this stage, there is no data in the internal buffer. The application processor can start a new operation. This operation is not valid in video mode(VEN=1). to 0.</td><td>1 – Cancel the current operation</td></tr></table></body></html>

# 8.1.18 Maximum Return Size Register

MRSR

偏移地址

0xC1

表8-19：最大返回值寄存器描述

表身体< html > < > < > < tr > < td colspan =“9”> MnSn Ia < / td > < / tr > < tr > < td >位< / td > < td > 15 < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td >夫人< td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td名称colspan = " 8 " > < / td夫人> < / tr > < tr > < td >类型< / td > < td colspan =“7”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x01 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>MRS Bit 15-0</td><td>最大返回大小-这些位设置MIPI从机返回的读响应报文的最大返回大小。在每次读取操作之前，SSD2828将根据该字段的值自动发送Set Maximum Return Size报文。它通知MIPI从机关于SSD2828的限制。应用处理器不需要在每次读取</td><td></td></tr></table></body></html>之前对寄存器进行编程

# 8.1.19返回数据计数寄存器

偏移地址

表8-20：返回数据计数寄存器描述

表身体< html > < > < > < tr > < td colspan =“8”> RDCR返回数据计数寄存器0 xc2 < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> RDC < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td名称colspan = " 8 " > RDC < / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”> RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>RDC Bit 15-0</td><td>返回数据计数表示从MIPI读响应报文接收到的数据字节数。这个寄存器只能通过</td><td></td></tr></table></body></html>来更新

# 8.1.20 ACK响应状态寄存器

ARSR

# ACK响应状态寄存器

表8-21:ACK响应状态寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“7”> AR < / td > < / tr > < tr > < td >类型< / td > < td colspan =“7”> RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“7”> AR < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 7 " > RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>AR Bit 15-0</td><td>ACK Response来自MIPI slave的ACK响应。当收到带有错误报告包的ACK时，将更新寄存器。否则，该值将被设置为0。这个寄存器中的位遵循MIPI DSI的定义。</td><td></td></tr></table></body></html>

# 8.1.21线路控制寄存器

LCR

# Line Control Register

Table 8-22: Line Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td></td><td>IBC</td><td>RT</td><td>RTB</td><td>FBC</td><td>FBT</td><td>FBW</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RW</td><td>RWAC</td><td>RWAC</td><td>RWAC</td><td>RW</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-6</td><td></td><td></td></tr><tr><td>IBC Bit 5</td><td>Ignore Bus Contention This bit is to detect bus contention reported by the Analog Phy. If this bit is disabled, whenever bus contention is detected, the state machine will ignore it and continue sending new packet if available. If this bit is enabled, the state machine will halt further transmission.</td><td>0 − Detect Bus Contention from Analog Phy 一 Ignore Bus Contention detected by Analog Phy</td></tr><tr><td>RT Bit 4</td><td>Reset Trigger This bit is to send a Reset Trigger When this bit is set to 1, the SSD2828 will send a Reset Trigger Message. It is recommended to enter LP mode and send this trigger message. If this bit is programmed during vertical active data is sent on MIPI link, the reset trigger will be delayed to next vertical period so that the reset trigger message will not disturb the video on the MIPI link. Once the Reset Trigger Message is sent out, RT bit will be automatically set to 0. Message. being blanking timing</td><td>0−NOP 1 – Send a Reset Trigger Message</td></tr><tr><td>RTB Bit 3</td><td>Register Triggered BTA This bit automatically perform Bus Turnaround(BTA) when link is not used. When bus is returned back from the slave, the link will remains in Low Power state until a new request come in where HS bit determination the transfer mode.</td><td>0−NOP 1 – Automatically perform BTA when link is available. In video mode, it will be sent at the next vertical blanking period.</td></tr><tr><td>FBC Bit 2</td><td>Force Bus Contention This bit controls whether to force a bus contention on the data lane. This bit will be changed to 0, after the bus contention is not detected.</td><td>0–NOP 1 – Drive the data lane to LP11 to force a bus contention.</td></tr><tr><td>FBT</td><td>Force BTA TE This bit controls whether to</td><td>0 – No BTA after previous BTA</td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P47/168</td><td>Mar 2013</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Bit 1</td><td>在之前的BTA之后进行自动BTA，以获得MIPI slave的TE响应。</td><td>1- BTA previous</td></tr><tr><td>FBW Bit 0</td><td>强制写后BTA控制写后是否自动生成BTA。它只对写操作有效。执行BTA后，总线机构</td><td>0−下一个写报文后不执行BTA。1 -自动在下一个写报文后进行BTA。< / td > < / tr > < tr > < td > < / td > < td > MIPI奴隶。在总线权威被传回之前，SSD2828不能向MIPI从站发送任何数据。应用程序处理器负责在发送任何数据之前检查总线的状态。通过< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 8.1.22中断控制寄存器

ICR

#中断控制寄存器

0xC5

表8-23：中断控制寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > 8 < / td > < / tr > < tr > < td >名称< / td > < td > CBEE < / td > < td > CBAE < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > MLEE < / td > < td > MLAE < / td > < / tr > < tr > < td >类型< / td > < td > RW < / td > < td > RW < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RW < / td > < td > RW < / td > < / tr > < tr > < td >重置< / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td >名称< / td > < td > PLSE < / td > < td > LPTOE < / td > < td > HSTOE < / td > < td > < / td > < td > ARRE < / td > < td > BTARE < / td > < td >坡< / td > < td > RDRE < / td > < / tr > < tr > < td >类型< / td > < td > RW < / td > < td > RW < / td > < td > RW < / td > < td > RO < / td > < td > RW < / td > < td > RW < / td > < td > RW < / td > < td > RW < / td > < / tr > < tr > < td >重置< / td > < td > 0 x1 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>CBEE Bit 15</td><td>命令缓冲区空使能该位使能CBE中断映射到int引脚。</td><td>0 -未使能1 -使能</td></tr><tr><td>CBAE位14</td><td>命令缓冲区可用使能该位使能CBA中断映射到int引脚。</td><td>0 -未使能1 -使能</td></tr><tr><td>保留位13-10</td><td></td><td></td></tr><tr><td>MLEE位9</td><td>MCU长缓冲区空使能该位使能MLE中断映射到int引脚。</td><td>0 -未使能1 -使能</td></tr><tr><td>MLAE位8</td><td>MCU长缓冲区可用使能该位使能MLA中断映射到int引脚。</td><td>0 -未使能1 -使能</td></tr><tr><td>PLSE位7</td><td>PLL锁状态使能使能PLS中断映射到int引脚。</td><td> 0-Not enabled 1- enabled </td></tr><tr><td>LPTOE Bit 6</td><td>LP RX Time Out Enable使能LPTO中断映射到int引脚。</td><td>0 -未使能1使能</td></tr><tr><td>HSTOE Bit 5</td><td>HS TX Time Out使能-该位使能HSTO中断映射到int引脚。</td><td>0未启用已启用</td></tr><tr><td>保留位4</td><td></td><td></td></tr><tr><td>ARRE位3</td><td>ACK响应就绪使能该位使能ARR中断映射到int引脚。</td><td>0 -未使能1 -使能</td></tr><tr><td>BTARE比特2</td><td>BTA响应使能该比特使能BTAR中断映射到int引脚。</td><td>0 -未启用1 -启用</td></tr><tr><td>POE位1</td><td>Packet Operation Enable该位使能PO中断映射到int引脚。</td><td>0 -Not enabled 1 enabled </td></tr><tr><td>RDRE Bit 0</td><td>Read Data Ready Enable该位使能RDR中断映射到int引脚。< / td > < td > 0-Not启用1 -启用< / td > < / tr > < /表> < /身体> < / html >

# 8.1.23中断状态寄存器

ISR

#中断状态寄存器

0xC6

表8-24：中断状态寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > 8 < / td > < / tr > < tr > < td >名称< / td > < td > CBE < / td > < td > CBA < / td > < td > < / td > < td > < / td > < td >中科< / td > < td > DST < / td > < td >初速< / td > < td > MLA < / td > < / tr > < tr > < td >类型< / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < td > RO < / td > < / tr > < tr > < td >重置< / td > < td > 0 x1 < / td > < td > 0 x1 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x1 < / td > < td > 0 x1 < / td > < td > 0 x1 < / td > < td > 0 x1 < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > 6 < / td > < td > 5 < / td > < td > < / td > < td > 3 < / td > < td > 2 < / td > < td > < / td > < td > 0 < / td > < / tr > < tr > < td >名称< / td > < td >请< / td > < td > LPTO < / td > < td > HSTO < / td > < td > ATR < / td > < td > ARR < / td > < td > BTAR < / td > < td >阿宝< / td > < td > RDR < / td > < / tr > < tr > < td >类型< / td > < td > RO < / td > < td > RW1C < / td > < td > RW1C < / td > < td > RW1C < / td > < td > RW1C < / td > < td > RW1C < / td > < td > RO < / td > < td > RO < / td > < / tr > < tr > < td >重置< / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x0 < / td > < td > 0 x1 < / td > < td > 0 x1 < / td > < td > 0 x0 < / td > < / tr > < tr > < td > < / td > < tdcolspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>CBE Bit 15</td><td>Command Buffer Empty该位表示SPI接口内部命令缓冲区的状态。如果命令缓冲区为空，则该位将被设置为1。应用程序处理器可以写入的最大大小为</td><td>0 -命令缓冲区不为空。1—命令缓冲区为空。is</td></tr><tr><td>CBA Bit 14</td><td>命令缓冲区。Command Buffer Available -该位反映SPI接口内部命令缓冲区的状态。如果命令缓冲区未满，则该位将被设置为1。应用程序处理器至少可以将数据包写入</td><td>0 -命令缓冲区已满。1 -命令缓冲区未满。< / td > < / tr > < tr > < td >保留一些13-12 < / td > < td >缓冲区。命令</td><td></td></tr><tr><td>CST比特11</td><td>时钟通道状态-该比特反映MIPI时钟通道的状态。</td><td>0 -时钟通道不在LP-11中。1 -时钟在LP-11。lane</td></tr><tr><td>DST Bit 10</td><td>数据通道状态反映MIPI数据通道的状态。</td><td>0数据通道不在LP-11中。1-数据通道在LP-11。</td></tr><tr><td>MLE Bit 9</td><td>Reserved</td>< /td><td>Reserved</td></tr><tr><td>MLA Bit 8</td><td>Reserved</td><td>Reserved</td></tr><tr><td>PLS Bit 7</td><td>PLL锁状态-该位反映PLL的状态。在锁相环被锁定之前，整个系统运行在锁相环的参考时钟输入，因为锁相环在被锁定之前没有输出。因此，应用处理器必须以慢速</td><td>0 PLL未被锁定1 - PLL已被锁定</td></tr><tr><td>LPTO位6</td><td>速度访问寄存器。LP RX超时-这个位反映了LP RX定时器的状态。</td><td>0 - LP RX定时器已过期。1−LP RX定时器未过期。</td></tr><tr><td>HSTO Bit 5</td><td>HS TX Time Out -该位反映HS TX定时器的状态。</td><td>0 - HS TX定时器已过期。1 - HS TX定时器未过期</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>ATR Bit 4</td><td>ACK触发响应−该位反映是否收到ACK触发消息。</td><td>0-未收到ACK触发消息。1 -已收到ACK触发消息。</td></tr><tr><td>ARR比特3</td><td>ACK响应就绪-该比特反映是否收到ACK响应。ACK响应可以是ACK触发消息，也可以是带有错误报告包的ACK。</td><td>0 -未收到响应。1 -已收到回应。</td></tr><tr><td>BTAR比特2</td><td>BTA响应-该比特反映SSD2828进行BTA后的数据通道状态。</td><td>0 - MIPI从端没有通过通道权限。1 - MIPI slave已经通过了lane authority back。</td></tr><tr><td>PO位1</td><td>包操作-这个位反映SSD2828是否准备好从应用处理器接收更多的数据。</td><td>0−未准备好1−准备好</td></tr><tr><td>RDR Bit 0</td><td>Read Data ready -该位表示MIPI从站的数据是否准备好供应用处理器读取。该位仅在读取操作时有效。当所有接收到的数据被读出时，这个位将被自动清除。< / td > < td > 0−没有准备好1−准备< / td > < / tr > < /表> < /身体> < / html >

# 8.1.24错误状态寄存器

ESR

#错误状态寄存器

0xC7

表8-25:Error Status Register描述

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td>CRCE</td><td>ECCE2</td><td>ECCE1</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW1C</td><td>RW1C</td><td>RW1C</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td>5</td><td></td><td></td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td>SO</td><td></td><td></td><td>MLO</td><td></td><td>CONT</td><td></td><td>VMM</td></tr><tr><td>TYPE</td><td>RW1C</td><td>RO</td><td>RO</td><td>RW1C</td><td>RO</td><td>RO</td><td>RO</td><td>RW1C</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-11</td><td></td><td></td></tr><tr><td>CRCE Bit 10</td><td>CRC Error - This bit reflects the status of CRC checking for the packets received from the MIPI slave. The status is valid only when the ECD bit is set to 0. Once CRC error occurs, this bit will be set to 1. It will remain as until the application processor writes to clear it. a</td><td>0−No CRC error since this bit is cleared 1 – At least CRCerror since this bit is cleared</td></tr><tr><td>ECCE2 Bit 9</td><td>ECC Multi Bit Error This bit reflects the status of ECC checking for the packets received from the MPI slave. The status is valid only when the ECD bit is set to 0. Once an ECC multi-bit error occurs, this bit will be set to 1. It will remain as until the application processor</td><td>0 –No ECC multi-bit error since this bit is cleared 1 – At least ECC multi-bit error since this bit is cleared</td></tr><tr><td>ECCE1 Bit 8</td><td>writes to clear it. ECC Single Bit Error This bit reflects the status of ECC checking for the packets received from the MIPI slave. The status is valid only when the ECD bit is set to Once an ECC single-bit error occurs, this bit will be set to 1. will remain as until the application to clear it. 0.</td><td>0 – No ECC single-bit error since this bit is cleared 1 – At least ECC single-bit error since this bit is cleared</td></tr><tr><td>CBO Bit 7</td><td>processor writes Command Buffer Overflow This bit reflects the status of internal command buf fer of the SPI interface. If the command buffer has overflowed, this bit will be set to 1. It will remain as until the application processor writes to clear it.</td><td>0 – Overflow has not occurred 1 – Overflow has occurred</td></tr><tr><td>Reserved Bit 6-5</td><td></td><td></td></tr><tr><td>MLO Bit 4</td><td>Reserved</td><td>Reserved</td></tr><tr><td>Reserved Bit 3</td><td></td><td></td></tr><tr><td>CONT</td><td>Contention — This bit reflects the status of the</td><td>0−No contention</td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 52/168</td><td>Mar 2013</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Bit 2</td><td>data lane contention detector.</td><td>1- Contention has occurred</td></tr><tr><td>Reserved Bit 1</td><td></td><td></td></tr><tr><td>VMM Bit 0</td><td>VC Mis Match This bit reflects whether there is a mismatch between the VC ID transmitted by the SSD2828 and the VC ID received from the MIPI slave</td><td>0−No mismatch 1 – Mismatch has occurred</td></tr></table></body></html>

# 8.1.25 Delay Adjustment Register 1

DAR1

#延迟调整寄存器

表8-26：延迟调整寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > 9 < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HZD < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x14 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HPD < / td > < / tr > < tr > < td >重置< / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td > < / td > < td colspan =“8”> 0 x02 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>HZD Bit 15-8</td><td>HS零延迟周期HS- Zero的啃食时钟个数。</td><td></td></tr><tr><td>HPD Bit 7-0</td><td>HS Prepare Delay HS Prepare -PREPARE的啃食时钟个数。指定延迟解说- < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

它定义了多少个蚕食时钟是hs - zero周期（图9-1）。
例如，如果锁相环运行在600Mbps，则蚕食时钟频率将为$ 15 0 \ mathm {{M h z}}$，或$6。6 7 \ mathm {ns}$默认值为20表示有133ns用于THS-ZERO周期。
s - zero周期将为$ 20 ^ {*}$ nibble_c $1 \ mathm {k} = $ 20 \AA ^{*} 6。6 7 \ mathm {n s} = 1 3 3。4 \ mathm {n s}$。

# HPD

它定义了多少个蚕食时钟是hs - prepare周期（图9-1）。从LP-00的边缘有大约4 nibble_clk的固有延迟。因此，实际的hs - prepare周期将是4个nibble clk $\boldsymbol {\cdot} + \ mathm {hp D} ^ {*}$ nibble_clk周期。

例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，即$6。7 \ mathm {n s}$。此- prepare周期为$4 ^ {*}$ nibble clk $+ 2 ^ {*}$ nibble $\ mathm {clk} = 4 \ mathm{~} ^{*} 6。6 . 7 \ mathm {n s} + 2 \ mathm {~} ^ {*}6 7 \ mathm {n s} = 4 0。0 2 \ mathm {n s}$

CLK   
xxx . xxx . xxx . xxx . xxx . xxx . Dp/Dn - tlpxthsppare -THSZERO -THSSYNC断开7终止器
-VIH(min)   
-Vll (max)-   
- verwen (max) x X000000C f Capture +TREOT TD-TERM-EN 1TData Bit THS-SKIP- LP-11 LP-11 !LP-01 LP-00 - TEOT - THSSETTLE - THS-TRAIL THsExIT

#8.1.26延迟调整寄存器2

DAR2

延迟调整寄存器2

表8-27:延迟调整寄存器2说明

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 & lt; /戴利的小于戴利> > 12 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“8”> > CZD & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“8”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“8”> > 0x28 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“8”> > & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“8”> > CPD & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE RESET & lt; /戴利的小于戴利>colspan =“8”> rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“8”> > 0x03 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“8”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></ td>< tr><td>CZD Bit 15-8</td><td> <td>< td>CPD Bit 7-0</td><td>CLK Prepare Delay - Prepare·period TCLK-</td><td></td></tr></table></body></html>

它定义了多少个蚕食时钟是TCLK -ZERO周期（图9-2）。
例如，如果锁相环运行在600bps，则蚕食时钟频率将为$ 15 0 \ mathm {{M h z}}$，或$6。7 \ mathm {n s}$。
TCLK-ZERO周期将为$4 0 ^ {*}$ nibble_c $\mathbf {k} = 4 0 \ast 6。6 7 \ mathm {n}$ s $\ mathm {\sim} 2 6 68 \ mathm {n s}$。

# CPD

它定义了多少个蚕食时钟是TCLK- PREPARE周期（图9-2）。从LP-00的边缘有一个大约3 nibble_clk的固有延迟。因此，实际的TCLK-PREPARE周期将是3 nibble_ $\ mathm {{clk + c P D} ^ {*}}$ nibble_ clk周期。

例如，如果锁相环运行600bps，则蚕食时钟频率将为$ 15 0 \ mathm {M h z}$，或$6。7 \ mathm {n s}$。TCLK-PREPARE周期将为$3 ^ {*}$ nibble_ clk $+ 3 ^ {*}$ nibble ${\ maththrm {clk}} = 3 * 6。6 7 {\ mathm {n s}} + 3 * 6。6 7 {\ mathm {n s}} \sim = 4 0。0 2 {\math {n s}} .$

注意：$\ O =$ PLL / 4

![](images/d0ee375a59ef41860173e9d3a48e04b8fb6a49286915ce87a871d8a9524a349f.jpg)  
图8-2：延迟计算的时序

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 55/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

# 8.1.27延迟调整寄存器

偏移地址

DAR3

#延迟调整寄存器

表8-28：延迟调整寄存器3描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > 9 < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> cp < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x04 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> CPTD < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x16 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></ td>< tr><td>CPED Bit 15-8</td><td>CLK Pre Delay -这些Bit指定了CLK Pre Delay period的啃食时钟个数TCLK-PER·</td>< /td></ td>< tr><td>CPTD Bit 7-0</td><td>CLK Post Delay -这些Bit指定了CLK Post Delay period的啃食时钟个数TCLK-POST.</td><td></td></tr></table></body></html>

CPED

它定义了多少个蚕食时钟是TCLK-PRE周期（图9-2）。TCLK-PR E周期的固有延迟为0.1 lp＿clk。

例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，或$6 . 6 7 \mathrm { n s }$。如果lpdiv为7，则lp＿clk为$9 . 3 7 5 \mathrm { M h z }$或$1 0 6 . 6 7 \mathrm { n s }$。

TCLK-PRE周期（最小）为$4 ^ { * }$ nibble clk $+ \underline { { 0 ^ { * } } }$ lp＿ $\mathrm { c l k } \ = 4 \ ^ { * } \ 6 . 6 7 \mathrm { n s \sim } 2 6 . 6 8 \mathrm { n s }$ TCLK-PRE周期（最大）为$^ 4 { } ^ { * }$ nibble＿clk $\mathrm { ~ \frac ~ { ~ 1 ~ } ~ { ~ 4 ~ } ~ l p ~ \frac ~ { ~ c ~ l ~ k ~ } ~ } = 4 ~ ^ { \ast } ~ 6 . 6 7 \mathrm { n s } + \mathrm { ~ 1 ~ } ^ { \ast } ~ 1 0 6 . 6 7 \mathrm { n s } \sim 1 3 3 . 3 5 \mathrm { n }$ s

# CPTD

它定义了多少个蚕食时钟是TCLK-POST周期（图9-2）。
例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，即$6。7 \ mathm {n s}$。
TCLK-POST期间将为$2 2 ^ { * }$ nibble ${ \mathrm { c l k } } = 2 2 * 6 . 6 7 { \mathrm { n s } } \sim = 1 4 6 . 7 4 { \mathrm { n s } }$。 {

$$
\mathsf N o t e{:}}{\mathsf p ＿ c l k{ =}}{\mathsf p l l{ /}}\left （8 ^ {\star}\left (| {\mathsf p d{ + 1}}\right) \right） 
$$

＃ 8.1.28延迟调整寄存器 

DAR4

＃延迟调整寄存器 

表8-29：延迟调整寄存器$4 0 / 1$

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> CTD < / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x0a < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> HTD < / td > < / tr > < tr > < td >重置< / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td > < / td > < td colspan =“8”> 0 x0a < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>CTD Bit 15-8</td><td>CLK Trail Trail的啃食时钟个数。</td><td></td></tr><tr><td>HTD Bit 7-0</td><td>HS Trail Delay的最小值。黑色- < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

它定义了多少个蚕食时钟是TCLK-TRAIL周期（图9-2）。例如，如果锁相环运行在600bps，则蚕食时钟频率将为$ 15 0 \ mathm {{M h z}}$，或$6。7 \ mathm {n s}$。TCLK-TRAIL周期将为$ 10 ^ {*}$ nibble_clk $= $ 10 \AA ^{*} 6。6 7 \mathord {n s \mathord {\sim}}7 \ mathm {n s}$

HTD   
它定义了多少个蚕食时钟是这个- trail周期（图9-2）。
例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，即$6。7 \ mathm {n s}$。

The THS-TRAIL period will be $1 0 ^ { \star }$ nibble c $\lvert \mathsf { k } = 1 0 ^ { \star } 6 . 6 7 \mathsf { r }$ s $\mathrm { \sim } 6 6 . 7 \mathrm { n s }$ .

# 8.1.29 Delay Adjustment Register 5

DAR5

# Delay Adjustment Register 5

Table 8-30: Delay Adjustment Register 5 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td></td></tr><tr><td>NAME</td><td colspan="8">WUD[15:8]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x10</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">WUD[7:0]</td></tr><tr><td>TYPE RESET</td><td colspan="8">RW</td></tr><tr><td></td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>WUD Bit 15-0</td><td>唤醒延迟唤醒延迟周期的时钟周期数用于将MIPI从ULPS状态唤醒。时钟是低功耗时钟。指定TwAKEUP.</td><td></td></tr></table></body></html>

它定义了从LP-10到LP-11的TWAKEUP周期中有多少低功耗时钟（lp＿clk）（图9-3）。例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，或$6 . 6 7 \mathrm { n s }$。如果lpdiv $= 7$， lp＿clk将为9.375Mhz，即106.6ns。

TWAKEUP周期为$4 0 9 6 * \mathrm { l p \_ c l k } = 4 0 9 6 * 1 0 6 . 6 \mathrm { n s } \sim 4 3 6 6 3 3$ ns

$$
\mathsf N o t e: l p ＿ c l k = p l l l: {}\left （8 ^ * {}\left (\mathsf l p d {}\mathrm + 1 {}\right) \right）
$$

![](images/d10e675594dd5a8d55b1e4b7960c0080d19f48f55ffa3a1252dffb397a79e3da.jpg)  
图8-3:TWAKEUP周期延迟计算

# 8.1.30 Delay Adjustment Register 6

DAR6

# Delay Adjustment Register 6

Table 8-31: Delay Adjustment Register 6 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td colspan="4">TGO</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td colspan="4">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td></td><td colspan="3">0x4</td></tr><tr><td></td><td colspan="9"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td colspan="4">TGET</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td colspan="4">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td colspan="4">0x5</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-12</td><td></td><td></td></tr><tr><td>TGO Bit 11-8</td><td>TA Go Delay These bits specifies the number for TA go delay period of TLpx TTA-GO.</td><td></td></tr><tr><td>Reserved Bit 7-4</td><td></td><td></td></tr><tr><td>TGET Bit 3-0</td><td>TA Get Delay These bits specifies the number of TLpx for TA get delay period TTA-GET.</td><td></td></tr></table></body></html>

它定义了有多少$\ mathm {T _ {L P X}}$是TTA-GO周期（图9-4）。$\ mathm {T _ {L P X}}$是lp_clk的一半周期。
例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，即$6。7 \ mathm {n s}$。如果lpdiv $= 7$，则lp_clk将为$9。3 7 5 \ mathm {M h z}$，或106.6ns。
$\ mathm {T _ {T A - G O}}$将是$4 \ mathm {~ ^ * ~ l p \_ c l k / 2} \ mathm {~ =} 4 \ mathm{~ ^ * ~} 1 0 6。6 / 2 \ mathm {\sim} 2 1 3。3 3 \ mathm {n s}$

# TGET

它定义了有多少个$\ mathm {T _ {L P X}}$是TTA-GET周期（图9-4）。$\ mathm {T _ {L P X}}$是lp_clk的一半周期。例如，如果锁相环运行600bps，则蚕食时钟频率将为150Mhz，即$6。7 \ mathm {n s}$。如果lpdiv $= 7$，则lp_clk为9.375Mhz，即106.6ns。

$\ mathm {T _ {T A - G E T}}$将是$5 * \ mathm {l p \_ c l k} / 2 = 5 * 1 0 6。6 / 2 \sim = 2 6 6。5 \ mathm {n s}$；

$$
\ mathm {N ot e: l p \_ c l k = p l l \cdot / \left（8 \ ^ {*} \left（l p d + 1 \右）\右）}
$$

![](images/700e19480888356cc035d6815141fda45e77fba20a97f6f221ae482cef479ea9.jpg)  
图8-4：延迟计算的时序

# 8.1.31 HS TX定时器寄存器

HTTR1

# HS TX定时器寄存器

表8-32:HS TX定时器寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>计画(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>计画[7:0]< / td > < / tr > < tr > < td >重置< / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td > < / td > < td colspan =“8”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>HTT Bit 31-0</td><td>HS TX Timer这些位指定HS TX Timer的超时间值。锁相环参考时钟用于增加内部计时器。当SSD2828进入HS发送模式时开始计时。当SSD2828退出HS发送模式时，定时器复位。如果定时器在HS传输结束前过期，SSD2828将发出错误信号并切换</td><td></td></tr></table></body></html>

# 8.1.32 HS TX定时器寄存器

偏移地址

HTTR2

# HS TX定时器寄存器

0xD0

Table 8-33: HS RX Timer Register 2 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td></td></tr><tr><td>NAME</td><td colspan="8">HTT[31:24]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">HTT [23:16]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x10</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>HTT Bit 31-0</td><td>Please see the description of HS TX Timer Register</td><td></td></tr></table></body></html>

# 8.1.33 LP RX Timer Register 1

LRTR1

# LP RX Timer Register 1

0xD1

表8-34:LP TX定时器寄存器说明

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > 9 < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>“轻轨(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>“轻轨(7:0)< / td > < / tr > < tr > < td >重置< / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td > < / td > < td colspan =“8”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>LRT Bit 31-0</td><td>LP RX定时器这些位指定LP RX定时器的超时间值。锁相环参考时钟用于增加内部计时器。当SSD2828进入LP接收模式时开始计时。当SSD2828退出LP接收模式时，定时器复位。如果定时器在从LP退出前到期，接收</td><td></td></tr></table></body></html>

# 8.1.34 LP RX定时器寄存器

偏移地址

LRTR2

# LP RX Timer Register 2

0xD2

Table 8-35: LP TX Timer Register 2 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td></td></tr><tr><td>NAME</td><td colspan="8">LRT[31:24]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">LRT[23:16]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x10</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>LRT Bit 31-0</td><td>Please see the description of LP RX Timer Register</td><td></td></tr></table></body></html>

# 8.1.35 TE Status Register

TSR

# TE Status Register

0xD3

Table 8-36: TE Status Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>0</td><td>5</td><td></td><td></td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>TER</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW1C</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-1</td><td></td><td></td></tr><tr><td>TER Bit</td><td>TE Response − This bit reflects whether a TE response has been received or not. Once a TE response is received, this bit will be set to 1. At the same time, the output te signal will go high. The host processor can write 1 to clear this bit. Once the bit is cleared, the te signal will go low.</td><td>0 –TE response has not been received. 1 – TE response has been received.</td></tr></table></body></html>

# 8.1.36 SPI Read Register

偏移地址

LRR

# SPI Read Register

0xD4

Table 8-37: SPI Read Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">RRA</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">OxFA</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-8</td><td></td><td></td></tr><tr><td>RRA Bit 7-0</td><td>Register Read Address (− These bits specify the address of the register to be read through the SPI interface, when the interface is SPI 8-bit (ither 3 wire or 4 wire).</td><td></td></tr></table></body></html>

# 8.1.37锁相锁寄存器

#锁锁寄存器

PLLR   

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > < / td > < td > 13 < / td > < td > 12 < / td > < td > < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>锁< / td > < / tr > < tr > < td >类型< / td > < td colspan =“8”> RW < / td > < / tr > < tr > < td >重置< / td > < td colspan =“8”> 0 x14 < / td > < / tr > < tr > < td > < / td > < td colspan = " 8 " > < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”>锁< / td > < / tr > < tr > < td >重置< / td > < td类型colspan = " 8 " > RW < / td > < / tr > < tr > < td > < / td > < td colspan =“8”> 0×50 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

表8-38:PLL锁寄存器描述

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>LOCK Bit 15-0</td><td>LOCK这些位指定PLL锁定范围，以PLL参考频率表示，f FIN最大PLL锁定周期为500us，默认设置为参考时钟，f FIN为10Mhz。LOCK设置应该只编程为</td><td>0x1450 * 520us f FIN /</td></tr></table></body></html>

# 8.1.38 Test Register

TR

Table 8-39: Test Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td colspan="2">TM/FLO</td><td></td><td></td><td colspan="3">EIC</td><td>FLM</td></tr><tr><td>TYPE</td><td colspan="2">RW</td><td></td><td>RW</td><td colspan="3"></td><td>RW</td></tr><tr><td>RESET</td><td colspan="2">0x0</td><td></td><td></td><td>0x00</td><td></td><td></td><td>0x0</td></tr><tr><td></td><td colspan="7"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>0</td></tr><tr><td>NAME</td><td colspan="6">PNB</td><td>END</td><td>CO</td></tr><tr><td>TYPE</td><td colspan="6">RW</td><td>RW</td><td>RW</td></tr><tr><td>RESET</td><td colspan="6">0x01</td><td>0x0</td><td>0x1</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TM/FL0 Bit 15-14</td><td>Test Mode These bits selects whether to CRC/ECC error for the outgoing streams. are used for debugging purpose only. should be set to 00 in normal mode! Force Lane 0 一 These bits are valid when FLM is 1. During this mode, user can write Low Power value to the MIPI lane 0 data using these bits. SSD2828 will not respond to the normal</td><td>inject 00 Normal mode They 01 一 Inject CRC error 10 Inject bit ECC error 11– Inject 2 bit ECC error 2</td></tr><tr><td>EIC Bit 13-9</td><td>FLo[1] controls MIPI_DP0 and FL0[0] controls MIPI DNO. Error Injection Control – These bits controls the position of the error being injected for</td><td></td></tr><tr><td>FLM Bit 8</td><td>testing. It is only applicable when TM is 01. Force Lane Mode This bit enable user to write to FL0 bits to directly control the analog lane DP0 and DP1. The lane should be in Low Power mode(HS=0)</td><td>0 –Normal Mode 1– Force Lane Mode</td></tr><tr><td>PNB Bit 7-2</td><td>when write using FL0. Packet Number in Blanking Period These bits controls the number of packet to send</td><td></td></tr><tr><td>END Bit 1</td><td>during video mode blanking period. Endianess − Internal test only. Default as 0</td><td></td></tr><tr><td>CO Bit 0</td><td>Color Order This bit specifies the order of the color component in the pixel. command mode transmission, this bit takes effect only when the IFS bit is 1 and the transmitted packet is DCS write memory packet, 0x2C or 0x3C. During video mode transmission, this bit must be set to 1 so as to follow the MIPI DSI specification. During</td><td>1–RGB. R is in the higher portion of the pixel. 0– BGR. B is in the higher portion of the pixel.</td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 67/168</td><td>Mar 2013</td></tr></table></body></html>

Remark: 24 bits color format   

<html><body><table><tr><td>CO</td><td>D23</td><td>D22</td><td>D21</td><td>D20</td><td>D19</td><td>D18</td><td>D17</td><td>D16</td><td>D15</td><td>D14</td><td>D13</td><td>D12</td><td>D11</td><td>D10</td><td>D9</td><td>D8</td><td>D7</td><td>D6</td><td>D5</td><td>D4</td><td></td><td>D</td><td></td><td>D1</td><td>D0</td></tr><tr><td></td><td>R7</td><td>R6</td><td>R5</td><td>R4</td><td>R3</td><td>R2</td><td></td><td>R0</td><td>G</td><td></td><td></td><td></td><td></td><td></td><td>GJ</td><td>G0</td><td>B7</td><td>B6</td><td>B5</td><td></td><td>B4</td><td>B3</td><td>B2</td><td>B1</td><td>B0</td></tr><tr><td></td><td>B7</td><td>B6</td><td>B5</td><td>B4</td><td>B3</td><td>B2</td><td>B1</td><td>B0</td><td>G</td><td></td><td></td><td>T</td><td></td><td></td><td></td><td>G0</td><td>R7</td><td>R6</td><td>R5</td><td></td><td></td><td></td><td></td><td></td><td></td></tr></table></body></html>

# 8.1.39 TE Count Register

# TE Count Register

0xD7

TECR   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td></td><td>10</td><td>9</td><td></td></tr><tr><td>NAME</td><td colspan="8">TEC[15:8]</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">TEC[7:0]</td></tr><tr><td>TYPE</td><td colspan="7">RW</td></tr><tr><td>RESET</td><td colspan="7">0x01</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

表8-40:TE计数寄存器描述

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TEC Bit 15-0</td><td>TE Count这些位决定了输出信号的脉宽。TE信号为1后开始计数。当计数器达到TEC字段的值时，信号将被置为0。计数器使用锁相环参考时钟进行计数。< / td > < td >最小值是1。< / td > < / tr > < /表> < /身体> < / html >

# 8.1.40模拟控制寄存器

ACR1

模拟控制1寄存器

表8-41:Analog Control 1寄存器描述

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 & lt; /戴利的小于戴利> > 12 & lt; /戴利的小于戴利> > 11 & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > D3 DELAY塞吉黑3:2铝& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > D1_DELAY_SEL & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“7”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“8”> > 0x0 0x20 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利的小于小于戴利/戴利> >colspan =“7”> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > 6 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 3 & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > D3_DELAY塞吉黑1:0铝& lt; /戴利的小于戴利colspan =“6”> > D0_DELAY_SEL & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“6”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x0 & lt; /戴利的小于戴利colspan =“6”> > 0x20 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

数据行3延时选择[3:2]这些位控制序列化器输出到HS发射机的延时。数据行1延时选择这些位控制序列化器输出到HS发射机的延时。数据行3延时选择[1:0]这些位控制序列化器输出到HS发射机的延时。选择这些位控制序列化器输出到HS发射机的延迟。

#8.1.41模拟控制寄存器2

ACR2

#模拟控制寄存器2

0xD9

表8-42模拟控制寄存器2描述

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 12 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 11 & lt; /戴利的小于戴利> > 9 & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > 8 & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > HSTX_Z & lt; /戴利的小于戴利colspan =“3”> > LPTXDS & lt; /戴利的小于戴利colspan =“3”> > HSTX_DS & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“3”> > rsw & lt; /戴利的小于戴利colspan =“3”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“6”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x4 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“6”> > & lt; /戴利的小于戴利>colspan =“2”> & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 3 & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > D3 DELAY_SEL黑5:4铝& lt; /戴利的小于戴利colspan =“6”> > D2 DELAY塞吉& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“6”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x2 & lt; /戴利的小于戴利colspan =“6”> > 0x20 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

HSTX_Z位15-14</td>高速传输控制这些位控制用于输出阻抗控制的微调。微调</td></td>lptxds位13-11</td><td>lptxds位选择这些位在低功率模式下控制TX的驱动强度。高速TX电流驱动程序选择这些位控制高速模式下TX的驱动电流。数据通道延时选择[5:4]-这些位控制序列化器输出到HS发射机的延时。3 </td></td>d2_delay SEL Bit 5-0</td><td>d2_29数据通道延时选择这些位控制序列化器输出到HS发射机的延时。2 & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

#8.1.42模拟控制寄存器3

偏移地址

#模拟控制寄存器3

ACR3

0xDA

Table 8-43: Analog Control Register 3 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td>TLFT1</td><td>TLFTO</td><td colspan="3">PREEM</td><td colspan="2">PREEM_MOD</td><td>PREEM E</td></tr><tr><td>TYPE</td><td>RW</td><td>RW</td><td></td><td>RW</td><td></td><td>RW</td><td></td><td>RW</td></tr><tr><td>RESET</td><td>0x1</td><td>0x0</td><td></td><td>0x3</td><td></td><td>0x0</td><td></td><td>0x1</td></tr><tr><td></td><td colspan="6"></td><td colspan="2"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>0</td><td>4</td><td></td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td>THFT1</td><td>THFTO</td><td></td><td>TC</td><td></td><td></td><td>ISEL</td><td></td></tr><tr><td>TYPE</td><td>RW</td><td>RW</td><td colspan="3">RW</td><td colspan="3">RW</td></tr><tr><td>RESET</td><td>0x1</td><td>0x0</td><td></td><td>0x04</td><td></td><td></td><td>0x04</td><td></td></tr><tr><td></td><td colspan="6"></td><td colspan="2"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TLFT1 Bit 15</td><td></td><td></td></tr><tr><td>TLFT0 Bit 14</td><td></td><td></td></tr><tr><td>PREEM_SE L Bit 13-11</td><td>PREEM_SEL Used for tap selection for preemphasis.</td><td></td></tr><tr><td>PREEM_M OD Bit 10-9</td><td>PREEM_MOD Preemphasis mode selection.</td><td></td></tr><tr><td>PREEM_E Bit 8</td><td>PRE-EMphasis Enable Used for HSTX.</td><td></td></tr><tr><td>THFT1 Bit 7</td><td>ThresHold 1 – This bit controls the low power receiver schmitt trigger high to low threshold selection1.</td><td></td></tr><tr><td>THFTO Bit 6</td><td>ThresHold 0 – This bit controls the low power receiver schmitt trigger high to low threshold selection 0.</td><td></td></tr><tr><td>TC Bit 5-3</td><td>Temperature Coefficient Select – These bits control the temperature coefficient of the bandbap regulator.</td><td></td></tr><tr><td>ISEL Bit 2-0</td><td>Current Output Trim These bits control current output trim of the bandgap regulator. /</td><td></td></tr></table></body></html>

# 8.1.43 Analog Control Register 4

ACR4

# Analog Control Register 4

偏移地址

0xDB

Table 8-44: Analog Control Register 4 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td colspan="4">CLK_DELAY_SEL</td><td>CKF</td><td></td></tr><tr><td>TYPE</td><td colspan="5">RW</td><td>RW</td><td>RO</td></tr><tr><td>RESET</td><td colspan="5">0x20</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="7"></td></tr><tr><td>BIT</td><td></td><td>6</td><td></td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td colspan="3">TCI</td><td>ENLV</td><td>CD_EN</td><td>GFFT1</td><td>GFFTO</td><td>GF_E</td></tr><tr><td>TYPE</td><td colspan="3">RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td></tr><tr><td>RESET</td><td colspan="3">0x4</td><td>0x1</td><td>0x1</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>CLK_DELA Y_SEL Bit 15-10</td><td>CLocK lane DELAY SELect - These bits control the delay for serializer output to the HS transmitter.</td><td></td></tr><tr><td>CKF Bit 9</td><td>ClocK Flip – This bit controls whether to flip the TX_LP_CP and TX_LP_CN to the analog block. It should be set to 0 for normal operation.</td><td></td></tr><tr><td>Reserved Bit 8</td><td></td><td></td></tr><tr><td>TCI Bit 7-5</td><td>Current Temperature Coefficient Select</td><td></td></tr><tr><td>ENLV Bit 4</td><td>BandGap Reference Enable</td><td></td></tr><tr><td>CD_EN Bit 3</td><td>Contention Detection Enable</td><td></td></tr><tr><td>GFFT1 Bit 2</td><td>Glitch Filter Selection 1</td><td></td></tr><tr><td>GFFT0 Bit 1</td><td>Glitch Filter Selection 0</td><td></td></tr><tr><td>GF_E Bit 0</td><td>Glitch filter Enable</td><td></td></tr></table></body></html>

# 8.1.44 Interrupt Output Control Register

IOCR

# Interrupt Output Control Register

Table 8-45: Interrupt Output Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td>IOT</td><td></td><td>IAS</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RW</td><td></td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td></td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-3</td><td></td><td></td></tr><tr><td>IOT Bit 2-1</td><td>Interrupt Output Type . These bits specify the type of output for the int. Write to this bit is only valid when all the enable bits in the Interrupt Control Registers(0xC5) are</td><td>00 CMOS output 01 − Open Drain active low output(Wired-AND). The IAS should be 0 when selected. 1x − Open Drain active high output(Wired-OR). The IAS should be when selected.</td></tr><tr><td>IAS Bit 0</td><td>0. Interrupt Active State - This bit selects the polarity of the int pin at the chip IO. Write to this bit is only valid when all the enable bits in the Interrupt Control Registers(0xC5) are 0.</td><td>11-NA 0− int pin is active 1ow, when there is no interrupt events, it is normally high. 1 – int pin is active high, when there is no interrupt events, it is normally low.</td></tr></table></body></html>

# 8.1.45 RGB Interface Control Register 7

偏移地址

VICR7

# RGB Interface Control Register7

0xDD

Table 8-46: RGB Interface Control Register 7 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td colspan="2"></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="4">VBN</td><td colspan="4">VFN</td></tr><tr><td>TYPE</td><td colspan="4">RW</td><td colspan="4">RW</td></tr><tr><td>RESET</td><td colspan="4">0x00</td><td colspan="4">0x00</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-8</td><td></td><td></td></tr><tr><td>VBN Bit 7-4</td><td>Vertical Front Porch Non Video Data Window These fields specify the number of vertical back counting backward from the first vertical active line in which non-video data is not allowed to be sent via MIPI link. This field is only valid when VEN is 1 and the interface setting is RGB + SPI. This field should not larger than VBP. If it is larger, the internal logic will cap this field to VBP. porch</td><td></td></tr><tr><td>VFN Bit 3-0</td><td>Vertical Back Porch Non Video Data Window These fields specify the number of vertical front porch from the last vertical active line in which non-video data is not allowed to be sent via MIPI link. This field is only valid when VEN is 1 and the interface setting is RGB + SPI. This field should not larger than VFP. If it is larger, the internal logic will cap this field to VFP.</td><td></td></tr></table></body></html>

注意：同时设置$\mathsf { V B N } \geq \mathsf { V B P }$和${ \mathsf { V F N } } \geq { \mathsf { V F P } }$，在视频模式为非突发模式或与$N V B = 1$设置突发模式时，会导致非视频数据不通过MIPI链路发出。

＃ 8.1.46 Lane Configuration 寄存器

偏移地址

LCFR

＃ Lane配置寄存器

0xDE

表8-47:Lane Configuration Register描述

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td>5</td><td></td><td></td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td colspan="2">LS</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td colspan="2">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0×0</td><td colspan="2">0x00</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-2</td><td></td><td></td></tr><tr><td>LS0 Bit 1-0</td><td>Lane Select These bits define the number of lane to be used for SSD2828.</td><td>00–1 lane mode 01 –2 lane mode 10 – 3 lane mode 11 – 4 lane mode</td></tr></table></body></html>

# 8.1.47 Delay Adjustment Register 7

DAR7

# Delay Adjustment Register 7

偏移地址

0xDF

Table 8-48: Delay Adjustment Register 7 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">HED</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RW</td></tr><tr><td>RESET</td><td>0x0 0x0</td><td colspan="6">0x10</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-6</td><td></td><td></td></tr><tr><td>HED Bit 5-0</td><td>HS Exit These bits specifies the number of nibble clock for HS exit delay period for data and clock lane. Delay</td><td></td></tr></table></body></html>

# 8.1.48 Pull Control Register 1

# Pull Control Register 1

PUCR1   

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 12 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 11 & lt; /戴利的小于戴利> > 9 & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > 8 & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利> > XTAL_PULL & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > PS4_PULL & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > PS3_PULL & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > PS2_PULL & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利>colspan =“2”> 0x1 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“6”> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > 5 & lt; /戴利的小于戴利> > 6 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > PS1_PULL & lt; /戴利的小于戴利colspan =“2”> > PSO_PULL & lt; /戴利的小于戴利colspan =“2”> > IS_PULL & lt; /戴利的小于戴利colspan =“2”> > MR_PULL & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利>colspan =“2”> 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

表8-49：拉控制寄存器描述

<td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td></td><td>10 - 75k上下拉75k上下拉</td></tr><tr><td>XTAL_PUL</td><td>Xtal模式拉这些位选择Xtal模式引脚的拉状态。</td><td>10 - 75k上下拉75k保存器</td></tr><tr><td>PS4_PULL Bit 13-12</td><td>引脚选择3拉这些位选择引脚的状态[4]。</td><td>75k上下拉器</td></tr>< td>PS3_PULL Bit 11-10</td><td>引脚选择3拉这些位选择引脚的拉状态[3]。</td><td>10 - 75k上下拉75k保存器</td></tr><tr><td>PS2_PULL Bit 9-8</td><td></td><td>75k下拉75k保存器00 -无拉</td></tr>< td>位7-6</td><td>引脚选择1拉这些位选择引脚ps[1]的拉状态。</td>75k下拉器</td></td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td><td>& lt; /戴利的小于戴利> > Keeper & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利>比特3-2 & lt; /戴利的小于戴利> > the if_sel电池。& lt; /戴利的小于戴利> > 75k Keeper & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> SSD2828QN4 & lt; /戴利的小于戴利> > Rev 130 & lt; /戴利的小于戴利> > P 78/168 & lt; /戴利的小于戴利> >名词& lt; / 2013是戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

小于桌子写成的小于医学上的小于html > > > & lt小于戴利> Name & lt; / tr >戴利的小于戴利> >字母表Description & lt; /戴利的小于戴利> >行传说& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 11一75k Keeper & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

#8.1.49拉动控制寄存器2

#拉控制寄存器2

0xE1

PUCR2   

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 12 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 11 & lt; /戴利的小于戴利> > 9 & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > 8 & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > DEN_PULL & lt; /戴利的小于戴利colspan =“2”> > HS_PULL & lt; /戴利的小于戴利colspan =“2”> > PC_PULL & lt; /戴利的小于戴利colspan =“2”> > VS_PULL & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x2 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利>colspan =“2”> 0x2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“8”> > & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 5 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利colspan =“2”> > DH_PULL & lt; /戴利的小于戴利colspan =“2”> > DM_PULL & lt; /戴利的小于戴利colspan =“2”> > DL_PULL & lt; /戴利的小于戴利colspan =“2”> > CSX_PULL & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利的小于戴利colspan =“2”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESET & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利colspan =“2”> > 0x1 & lt; /戴利的小于戴利>colspan =“2”> 0x2 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

表8-50:拉控制寄存器2描述

小于桌子写成的小于医学上的小于html > > > & lt小于戴利> Name & lt; / tr >戴利的小于戴利> >字母表Description & lt; /戴利的小于戴利> >行传说& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> DEN_PULL比特15-14 & lt; /,戴利的小于戴利> >穴都能这些比特是select the都能礼物,滨州的电池》一书穴。</td><td>75k上拉75k下拉</td></tr><t>hs_pull Bit 13-1210 - 01 - </td><td>75k上下拉75k下拉</td></tr>< td><td> PC_PULL Bit 11-10</td><td>11 Pixel_clk拉这些位选择pclk pin的拉状态00。1001 - </td><td>75k上下拉75k </td></tr>< td>VS_PULL Bit 9-8</td><td>Vsync01</td><td>75k上下拉75k上下拉75k </td></tr><t>dh_pull Bit 7-6</td><td>11数据高字节拉这些位选择引脚数据的拉00状态[23:16]。</td><td> 1075k拉上01 - 75k拉下1175k位5-4</td><td>数据介质字节拉这些位选择引脚数据的拉状态[15:8]。</td><td>01 - 75k向下拉11 - 75k Keeper</td></tr>< td>Bit 3-2</td><td>数据低字节拉这些位选择引脚数据的拉00状态[7:0]。10</td><td>01 - 75k下拉11 - 75k管理员</td></tr>< td>CSX_PULL Bit 1-0</td><td>& lt; /戴利(小于戴利> > 0个饱,都能10−75k pull-up哈上尉75k pull-down 11 75k Keeper / & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

# 8.1.50 Pull Control Register 3

# Pull Control Register 3

PUCR3   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td colspan="2">SDI_PULL</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td colspan="2">RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td colspan="2">0x1</td></tr><tr><td></td><td colspan="9"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="2">SCK_PULL</td><td colspan="2">SDC PULL</td><td colspan="2">SHUT_PULL</td><td colspan="2">CM_PULL</td></tr><tr><td>TYPE</td><td colspan="2">RW</td><td colspan="2">RW</td><td colspan="2">RW</td><td colspan="2">RW</td></tr><tr><td>RESET</td><td colspan="2">0x1</td><td colspan="2">0x1</td><td colspan="2">0x2</td><td colspan="2">0x1</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

Table 8-51: Pull Control Register 3 Description   

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-10</td><td></td><td></td></tr><tr><td>SDI_PULL Bit 9-8</td><td>SDI Pull These bits select the pull state of the pin sdi.</td><td>10 − 75k pull-up 75k pull-down</td></tr><tr><td>SCK_PULL Bit 7-6</td><td>the pin sck.</td><td>10– 75k pull-up 01- 75k pull-down</td></tr><tr><td>Bit 5-4</td><td></td><td>10– 75k pull-up 01 75k pull-down</td></tr><tr><td>Bit 3-2</td><td></td><td>10 75k pull-up 01– 75k pull-down</td></tr><tr><td></td><td>pin cm.</td><td>10 − 75k pull-up 01– 75k pull-down</td></tr></table></body></html>

# 8.1.51 CABC Brightness Control Register 1

CBCR1

# CABC Brightness Control Register 1

Table 8-52: CABC Brightness Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td colspan="8">WDBV</td></tr><tr><td>TYPE</td><td colspan="8">RW</td></tr><tr><td>RESET</td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td></td><td>GAM18</td><td>BL</td><td>DD</td><td>BCTR</td><td colspan="2">BP_MODE</td><td>CABC EN</td></tr><tr><td>TYPE</td><td>RO</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td colspan="2">RW</td><td>RW</td></tr><tr><td>RESET</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td colspan="2">0x0</td><td>0x0</td></tr><tr><td></td><td colspan="8"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>WDBV Bit 15-8</td><td>Write Display Brightness Value These bits control the brightness setting.</td><td></td></tr><tr><td>Reserved Bit 7</td><td></td><td></td></tr><tr><td>GAM18 Bit 6</td><td>Gamma 18 — This bit controls gamma 18 select.</td><td></td></tr><tr><td>BL Bit 5</td><td>BackLight Control This bit controls the backlight on/off at BC line.</td><td>0– Off (Completely turn off backlight circuit) 1-On</td></tr><tr><td>DD Bit 4</td><td>Display Dimming This bit controls the display dimming feature.</td><td>0– Display Dimming Off 1– Display Dimming On</td></tr><tr><td>BCTR Bit 3</td><td>Brightness Control This bit controls the On/Off state of the Brightness Control block.</td><td>0–Off 1-On</td></tr><tr><td>BP_MODE Bit 2-1</td><td>BP MODE These bits control the Brightness Preservation of the CABC.</td><td>00 Disable DABC 01 Conservation Mode (Or User Interface Mode) 10 Normal Mode (Or Still Picture Mode) 11 —Aggressive Mode</td></tr><tr><td>CABC_EN Bit 0</td><td>CABC ENable — This bit enable the CABC This bit is valid only when if sel is 0. feature.</td><td>(Or Moving Image Mode) 0 − CABC is disable 1- CABC is enable</td></tr></table></body></html>

# 8.1.52 CABC Brightness Control Register 2

CBCR2

# CABC Brightbess Control Register 2

偏移地址

0xEA

Table 8-53: CABC Brightness Control Register 2 Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>3</td></tr><tr><td>NAME</td><td colspan="4">PWM_PS</td><td colspan="4">BCB_PS</td></tr><tr><td>TYPE</td><td colspan="4">RW</td><td colspan="4">RW</td></tr><tr><td>RESE</td><td colspan="4">0x6</td><td colspan="4">0x9</td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td>CABC</td><td>_MB</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8"></td></tr><tr><td>TYPE RESE</td><td colspan="8">RW</td></tr><tr><td></td><td colspan="8">0x00</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>PWM_PS Bit 15-12</td><td>PWM precale这些位控制PWM信号的频率。</td><td>这取决于像素时钟速度（pclk）。例如，10MHz = " 0111 “或” 1000“ 20MHz “ 0110”或” 0101"</td></tr><tr><td>BCD_PS Bit 11-8</td><td>BCB precale -这些位控制BCB单元每秒执行的迭代数。</td><td>这取决于像素时钟速度（pclk）。例如，5MHz = " 0101 " 10MHz = " 0110" 25MHz = "1001" 50MHz = " 1011 " 75MHz = "1100"</td></tr><tr><td>CABC_MB位7-0</td><td>CABC最小亮度这些位控制CABC最小亮度级别</td><td></td></tr></table></body></html>

# 8.1.53 CABC亮度状态寄存器

CBSR

偏移地址

# CABC亮度状态寄存器

0xEB

表8-54:CABC亮度状态寄存器描述

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td colspan="3">VGA_SEL</td><td></td><td></td><td></td><td>BCL</td></tr><tr><td>TYPE</td><td colspan="4">RW</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESE</td><td colspan="4">0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td></td><td colspan="2"></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>BIT</td><td></td><td>6</td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="8">RDBV</td></tr><tr><td>TYPE</td><td colspan="7">RO</td></tr><tr><td>RESE</td><td colspan="7">0x00</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>VGA_SEL Bit 15-12</td><td>VGA SELect These bits select the type of resolution of the RGB frame. Panel resolutions need not be matched exactly. For example, "0111" will work for any panel with 384000 pixels (+/- 15%). In other words, the range is 326400 to 441600 pixels. Hence, 800x480, 480x800, 512x750 and 750x512 will be supported by VGA_SEL= "0111".</td><td>0001 400x240 (WQVGA) 0010-400x300 (SQVGA) 0011-480x320 (FVGA) 0100-540x360 (FVGA+) 0101-600x400 (FVGA++) 0110-640x480 (VGA) 0111 800x480 (WVGA) 1000 800x600 (SVGA) 1001 1024x600 (WSVGA) 1010 - 1024x768 (XGA) 1011 - 1280x768 (WXGA) 1100 1280x960 (SXGA) 1101 Reserved 1110 Reserved</td></tr><tr><td>Reserved Bit 11-9</td><td></td><td>1111 - Reserved</td></tr><tr><td>BCL Bit 8</td><td>Brightness Control Line This bit is the pulse width modulation signal output to control external backlight power source.</td><td></td></tr><tr><td>RDBV Bit 7-0</td><td>This output is used for Architecture I. Read Display Brightness Value − These bits show the brightness value of the CABC. This output is used for Architecture II.</td><td></td></tr></table></body></html>

# 8.1.54 Encoder Control Register

ECR

# Encoder Control Register

Table 8-55: Encoder Control Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>3</td></tr><tr><td>NAME</td><td colspan="6">ENC_LW</td><td></td><td></td></tr><tr><td>TYPE</td><td colspan="7">RW</td><td></td></tr><tr><td>RESE T</td><td colspan="8">0x78</td></tr><tr><td></td><td colspan="8"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>NAME</td><td colspan="4">ENC_LW</td><td></td><td></td><td>ENC_MO DE</td><td>ENC_EN</td></tr><tr><td>TYPE RESE</td><td colspan="3">RW</td><td>RO</td><td>RO</td><td></td><td>RW</td><td>RW</td></tr><tr><td></td><td colspan="4">0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="9"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>ENC_LW Bit 15-4</td><td>LineWidth 一 These bits define line width of the Encoder input.</td><td></td></tr><tr><td>Reserved Bit 3-2</td><td></td><td></td></tr><tr><td>ENC_MODE Bit 1</td><td>ENCoder Mode This bit select the</td><td>0 − Type 1 1- Type 2</td></tr><tr><td>Bit 0</td><td>ENCoder ENable This bit enable the Smartlink Encoder feature. This bit is valid only when if sel is 0.</td><td>Encoder is disable 1 – Encoder is enable</td></tr></table></body></html>

# 8.1.55 Video Sync Delay Register

VSDR

# Video Sync Delay Register

偏移地址

0xED

表8-56:视频同步延迟寄存器描述

小于桌子写成的小于医学上的小于html > > > & lt小于戴利>比特& lt; / tr >戴利的小于戴利15 & lt; /戴利> > > & lt;戴利14 & lt; /戴利的小于戴利> > > 13 12 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 11 & lt; /戴利的小于戴利> > 9 & lt; /戴利的小于戴利> > 10 & lt; /戴利的小于戴利> > 3 & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > VSD & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“8”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESE & lt; /戴利的小于戴利colspan =“8”> > 0x00 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“8”> > & lt; /戴利> & lt; / tr > & lt小于戴利>比特& lt; / tr >戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt小于戴利> NAME & lt; / tr >戴利的小于戴利>colspan =“8”> HSD & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> TYPE & lt; /戴利的小于戴利colspan =“8”> > rsw & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> RESE & lt; /戴利的小于戴利colspan =“8”> > 0x02 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

<html> body ><td> name</td><td>description </td>setting</td>vsd Bit 15:8</td><td>vsync延迟这些位控制VSync输入的内部管道延迟。HSync延时这些位控制HSync输入的内部管道延时。

#8.1.56微调寄存器

#微调寄存器

偏移地址

TMR   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td>TRIM_ DONE</td><td>TRIM PASS</td><td>XORC DONE</td><td>XORC SEL</td><td>XORC EN</td><td>VBIST SRT</td><td>VBIST_ EN</td><td>TRIM_EN</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RW</td><td>RWAC</td><td>RW</td><td>RW</td><td>RWAC</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="6"></td><td></td><td></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td></td></tr><tr><td>NAME</td><td colspan="4">TRIM CMD[7:4]</td><td colspan="4">TRIM CMD[3:0]/XORC FILTER[3:0]</td></tr><tr><td>TYPE</td><td colspan="4">RW</td><td colspan="4">RW</td></tr><tr><td>RESE</td><td colspan="4">0x0</td><td colspan="4">0x0</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

0xEE

Table 8-57: Trimming Register Description

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TRIM_DON E Bit 15</td><td>TRIMming DONE This bit indicates if the trimming process is done.</td><td></td></tr><tr><td>TRIM_PASS Bit 14</td><td>TRIMming PASS This bit indicates if the trimming process is successful. It is valid when bit 15 is set to 1.</td><td></td></tr><tr><td>XORC_DON</td><td>the trimming process is done. cleared when read.</td><td></td></tr><tr><td>XORC_SEL Bit 12</td><td>XOR Calibration SELect This bit select the internal byte data for calibration to be 8'hAA or 8'h55.</td><td>1 = 8'hAA</td></tr><tr><td>Bit 11</td><td>calibration is done, this bit will be auto clear and</td><td></td></tr><tr><td>Bit 10</td><td>stop at the frame The software should wait for at boundary.</td><td></td></tr><tr><td></td><td>automatically send the color, red, white repeatedly base on</td><td></td></tr><tr><td></td><td>trimming process and when the trimming is this bit will be auto clear and the status bits at bit 14 and bit 15 indicates</td><td></td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 87/168</td><td>Mar 2013</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>TRIM _CMD</td><td>该位在正常模式下应为0。</td><td></td></tr><tr><td>Bit 7:4</td><td>TRIM命令这些位定义命令头字节的上4位用于自动修剪模式，当TRIM EN=1时，SSD2828向MIPI接收器发送读请求。</td><td></td></tr><tr><td>TRIM_CMD /XORC_FIL TER Bit 3:0</td><td>TRIM命令这些位定义命令头字节的下4位在自动模式下使用，当TRIM EN=1时，SSD2828向MIPI接收器发送读请求。当XORC EN=1时，这些位定义滤波器以符合XOR校准脉冲宽度。修剪< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

# 8.1.57 GPIO1寄存器

GPIO1

# GPIO1注册

表8-58:GPIO1寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > 8 < / td > < / tr > < tr > < td >名称< / td > < td > GPIO1统计< / td > < td colspan =“7”> GPIO1_CTR < / td > < / tr > < tr > < td >类型< / td > < td > RO < / td > < td colspan =“7”> RW < / td > < / tr > < tr > < td >复位< / td > < td > 0 x0 < / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < tr > < td >位< / td > < td > GPIO0_ < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> GPIOO_CTR统计< / td > < / tr > < tr > < td >类型复位< / td > < td > RO < / td > < td colspan =“7”> RW < / td > < / tr > < tr > < td > < / td > < td > 0 x0 < / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>GPIO1_CTR Bit 15</td><td>GPIO Status该位提供该引脚配置为输入时的状态。</td><td></td></tr><tr><td>GPIO1_CTR位14:8</td><td>GPIO 1控制控制引脚的输出行为。</td><td>请参见6-0位的描述。</td></tr><tr><td>GPIO0_CTR Bit ></td><td>GPIO 0状态—该位提供dbcl引脚配置为输入时的状态。</td><td></td></tr><tr><td>GPIO0_CTR位6:0</td><td>GPIO 0控制-这些位控制dbcl引脚的输出行为。</td><td>Bit 0模块或寄存器该位控制模块或寄存器控制的引脚。0 -模块控制1 -寄存器控制位1方向当位0为1时，该位控制引脚的方向。当第0位为0时，没有影响。0 - Pin为输入1 - Pin为输出比特2输出状态当比特0为1时，控制引脚的输出状态。当第0位为0时，没有影响。0 -输出状态为0 1 -输出状态为1 Bit 4-3: Pull state当引脚处于输入状态时，这些位控制引脚的Pull状态。00无拉10 75k上拉01- 75k下拉11 - 75k Keeper /</td></tr></table></body></html>

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 89/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td></td><td></td><td>Bit 5 Interrupt Enable该位使能GPIO0输入（Bit 1 = 0）中断到int引脚。当第0位为0时，没有影响。比特6中断极性选择比特5使能时GPIO0输入的极性（比特= 0）。当第0位为0时，没有影响。0−Active low</td></tr></table></body></html>

# 8.1.58 GPIO2寄存器

GPIO2

# GPIO2寄存器

Table 8-59: GPIO1 Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>BIT</td><td></td><td>6</td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td>GPIO2 STAT</td><td colspan="7">GPIO2_CTR</td></tr><tr><td>TYPE</td><td>RO</td><td colspan="6">RW</td></tr><tr><td>RESE</td><td>0x0</td><td colspan="6">0x00</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-8</td><td></td><td></td></tr><tr><td>GPIO2_CTR Bit 7</td><td>GPIO 2 Status − This bit provide the status of the sys_clk_out pin when it is configured as input.</td><td></td></tr><tr><td>GPIO2_CTR Bit 6:0</td><td>GPIO 2 Control These bits control the output behavior of the sys_clk_out pin.</td><td>Bit 0 - Module or Register This bit controls the pin to be module controlled or register controlled. 0 – Module controlled 1- Register controlled Bit 1 – Direction This bit controls the direction of the pin when bit 0 is 1. When bit 0 is 0, it has no effect. 0 – Pin is input 1 – Pin is output Bit 2 Output State This bit controls the output state of the pin when bit 0 is 1. When bit 0 is 0, it has no effect. 0 − Output state is 0 1- Output state is 1 Bit 4-3 : Pull State These bits control the pull state of the pin when it is in input state. 00 – No pull 10 – 75k pull-up 01 – 75k pull-down 11–75k Bit 5 : Interrupt Enable Keeper</td></tr></table></body></html>

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 91/168</td><td>Mar 2013</td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td></td><td></td><td>This bit enables the GPIO0 input(bit = 0) interrupt to the int pin. When bit 0 is 0, it has no effect. Bit 6 : Interrupt Polarity This bit selects the polarity of the GPIO0 input(bit = when bit 5 is enabled. When bit 0 is 0, it has no effect. Active low 0) 0-</td></tr></table></body></html>

# 8.1.59 DLYA01 Register

DLYA01

# DLYA01 Register

0xF1

Table 8-60: DLYA01 Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_A_1</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_A_0</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_A_1 Bit 13-8</td><td>DELAY A for lane 1– These bits provide the status of the XOR calibration results when XORC SEL is set to 0.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_A_0 Bit 5-0</td><td>DELAY A for lane 0 These bits provide the status of the XOR calibration results when XORC_ _SEL is set to 0.</td><td></td></tr></table></body></html>

# 8.1.60 DLYA23 Register

# DLYA23 Register

0xF2

DLYA23   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_A_3</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_A_2</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

Table 8-61: DLYA23 Register Description   

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_A_3 Bit 13-8</td><td>DELAY A for lane 3 – These bits provide the status of the XOR calibration results when XORC SEL is set to 0.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_A_2 Bit 5-0</td><td>DELAY A for lane 2 These bits provide the status of the XOR calibration results when XORC_SEL is set to 0.</td><td></td></tr></table></body></html>

# 8.1.61 DLYB01 Register

DLYB01

# DLYB01 Register

0xF3

Table 8-62: DLYB01 Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_B_1</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_B_0</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_B_1 Bit 13-8</td><td>DELAY B for lane 1– These bits provide the status of the XOR calibration results when XORC SEL is set to 1.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_B_0 Bit 5-0</td><td>DELAY B for lane 0 These bits provide the status of the XOR calibration results when XORC_ _SEL is set to 1.</td><td></td></tr></table></body></html>

# 8.1.62 DLYB23 Register

# DLYB23 Register

0xF4

DLYB23   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_B_3</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6"></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">DELAY_B_2 RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

Table 8-63: DLYB23 Register Description   

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_B_3 Bit 13-8</td><td>DELAY B for lane 3 These bits provide the status of the XOR calibration results when XORC SEL is set to 1.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_B_2 Bit 5-0</td><td>DELAY B for lane 2 These bits provide the status of the XOR calibration results when XORC_SEL is set to 1.</td><td></td></tr></table></body></html>

# 8.1.63 DLYC01 Register

DLYC01

# DLYC01 Register

0xF5

Table 8-64: DLYC01 Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_C_1</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6"></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">DELAY_C_0 RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_C_1 Bit 13-8</td><td>DELAYC for lane 1 – These bits provide the status of the average XOR calibration results from DELAY_A_1 and DELAY_B_1.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_C_0 Bit 5-0</td><td>DELAY C for lane 0– These bits provide the status of the average XOR calibration results from DELAY A 0 and DELAY B 0.</td><td></td></tr></table></body></html>

# 8.1.64 DLYC23 Register

DLYC23

# DLYC23 Register

0xF6

Table 8-65: DLYC23 Register Description   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td></td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6">DELAY_C_3</td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr><tr><td>BIT</td><td></td><td></td><td></td><td></td><td>3</td><td></td><td></td><td></td></tr><tr><td>NAME</td><td></td><td></td><td colspan="6"></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td colspan="6">DELAY_C_2 RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td colspan="6">0x20</td></tr><tr><td></td><td colspan="6"></td></tr></table></body></html>

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>Reserved Bit 15-14</td><td></td><td></td></tr><tr><td>DELAY_C_3 Bit 13-8</td><td>DELAYC for lane 3 – These bits provide the status of the average XOR calibration results from DELAY_A_3 and DELAY_B_3.</td><td></td></tr><tr><td>Reserved Bit 7-6</td><td></td><td></td></tr><tr><td>DELAY_C_2 Bit 5-0</td><td>DELAY C for lane 2 – These bits provide the status of the average XOR calibration results from DELAY A_2 and DELAY B_2.</td><td></td></tr></table></body></html>

# 8.1.65 Analog Control Register 5

偏移地址

# ACR5 Register

ACR5   

<html><body><table><tr><td>BIT</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td></tr><tr><td>NAME</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>TYPE</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td><td>RO</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td>BIT</td><td></td><td>6</td><td>5</td><td></td><td>3</td><td>2</td><td></td><td>0</td></tr><tr><td>NAME</td><td>DEC_XO</td><td>DEC_FB</td><td>D3_FB_E</td><td>D2_FB_E</td><td>D1_FB_E</td><td>D0_FB_E</td><td>XOR_TU</td><td>REG</td></tr><tr><td>TYPE</td><td>R_E RW</td><td>E RW</td><td>RW</td><td>RW</td><td>RW</td><td>RW</td><td>NE_EN RW</td><td>CTR RW</td></tr><tr><td>RESE</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td><td>0x0</td></tr><tr><td></td><td colspan="7"></td></tr></table></body></html>

0xF7

Table 8-66: ACR5 Register Description   

<html><主体><表><tr><td>名称</td><td>描述</td><td>设置</td></tr><tr><td>保留位15-8</td><td></td><td></td></tr><tr><td>DEC_XOR_ E位7</td><td>解码器XOR使能−设置[0]位时，该位使能模拟中的XOR功能。该位用于内部测试，对于正常</td><td></td></tr><tr><td>DEC_FB_E位6</td><td>使用，应该为0。解码器反馈使能当[0]位被设置时，使能模拟解码器功能。该位用于内部测试，应编程为</td><td></td></tr><tr><td>D3_FB_E位5</td><td>至0才能正常使用。D3 FeedBack Enable当[0]位设置时，该位使能通道3的异或反馈调谐。该位用于内部测试，应编程为</td><td></td></tr><tr><td>D2_FB_E位4</td><td>至0才能正常使用。D2反馈使能当[0]位设置时，使能2通道的异或反馈调谐。该位用于内部测试，应编程为</td><td></td></tr><tr><td>D1_FB_E位3</td><td>至0才能正常使用。D1 FeedBack Enable当[0]位设置时，开启通道的异或反馈调谐。该钻头用于内部测试，并应用于正常编程的编程使用。0</td><td></td></tr><tr><td>DO_FB_E Bit 2</td><td>D0 FeedBack Enable当Bit[0]设置时，使能0车道的XOR反馈调谐。</td><td></td></tr></table></body></html>

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 99/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td></td><td>该位用于内部测试，正常使用时应为0。programmed</td><td></td></tr><tr><td>XOR_TUNE _EN Bit 1</td><td>XOR TUNE ENable设置Bit[0]时，使能XOR反馈调谐。该位用于内部，正常使用时应该为0。位测试程序</td><td></td></tr><tr><td>REG_CTR位0</td><td>模拟寄存器控制该位允许通过寄存器位[7:1]或通过模拟测试模式4控制XOR反馈调谐。该位用于内部，应编程为0正常使用。Test </td><td>0 -由模拟测试模式控制1 -控制从[7:1]。< / td > < / tr > < /表> < /身体> < / html >

# 8.1.66读寄存器

RR

表8-67：读寄存器描述

表身体< html > < > < > < tr > < td >位< / td > < td > 15 < / td > < td > 14 < / td > < td > 13 < / td > < td > 12 < / td > < td > 11 < / td > < td > 10 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> RD(很高)< / td > < / tr > < tr > < td >类型< / td > < td colspan =“9”> RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“9”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“9”> < / td > < / tr > < tr > < td >位< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >名称< / td > < td colspan =“8”> RD (7:0) < / td > < / tr > < tr > < td > < / td > < td类型colspan = " 7 " > RO < / td > < / tr > < tr > < td >重置< / td > < td colspan =“7”> 0 x00 < / td > < / tr > < tr > < td > < / td > < td colspan =“7”> < / td > < / tr > < /表> < /身体> < / html >

<html><body><table><tr><td>Name</td><td>Description</td><td>Setting</td></tr><tr><td>RD Bit 15-0</td><td>Read Data−该寄存器不是真正的寄存器。它是内部缓冲区的入口。用于读取MIPI从机返回的数据。应用处理器可以把这个寄存器当作先进先出（FIFO），并不断地从中读取数据。当接口为16位时，该字段的宽度为16位。当接口为8位时，该字段的宽度为8位。这个寄存器的读只有在RDY位为1时才有效。换句话说，只有当接收到MIPI从机返回的数据时，应用处理器才能读取这个寄存器以返回数据。点得到< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

![](images/e12860b7e512f2a4faaa6ffd30b17988340f5dcaac0cdcbbd4f0fbebfca137d9.jpg)

# 9配置

SSD2828可以通过配置支持多种操作。所涉及的信号是ps[1:0]引脚，LS寄存器位。这些信号定义了前端支持的接口类型以及MIPI链路上的通道分布。定义SSD2828操作的特性如下：

每个MIPI链路RGB和SPI接口在前端选择的通道数转储和智能配置

# 9.1 Lane管理

SSD2828中使用的通道数由LS（0xDE[1:0]）位决定。SSD2828 DPHY通道由时钟通道（CL）和4个数据通道（DL0、DL1、DL2、DL3）组成。

下表列出了所有的通道使用组合和每个DPHY通道的活动状态。

表9-1:SSD2828 Lane Management

表身体< html > < > < > < tr > < td > LS < / td > < td > SSD2828 < / td > < td > CL < / td > < td > DL0 < / td > < td > DL1 < / td > < td > DL2 < / td > < td > DL3 < / td > < td > Max。速度< / td > < / tr > < tr > < td > 00 < / td > < td > 1巷< / td > < td >在< / td > < td >在< / td > < td >从< / td > < td >从< / td > < td >从< / td > < td > 1.0 gbps < / td > < / tr > < tr > < td > 01 < / td > < td > 2车道< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td >从< / td > < td >从< / td > < td > 2.0 gbps < / td > < / tr > < tr > < td > 10 < / td > < td > 3车道< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td >从< / td > < td > 3.0 gbps < / td > < / tr > < tr > < td > 11 < / td > < td > 4车道< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td >在< / td > < td > 4.0 gbps < / td > < / tr > < /表> < /身体> < / html >

# 9.2用例

SSD2828支持单接口配置。RGB和SPI接口的组合。下面列出了支持的配置的详细信息。

# 9.2.1 RGB $\dagger$ SPI接口

应用处理器可以使用此配置来驱动哑显示面板或智能显示面板或两者。用户需要通过设置ps[1:0]来选择所需的SPI接口。当不使用SPI接口时，csx引脚需要保持高位。

由于RGB和SPI接口是完全分离的，所以这两个接口可以独立工作。RGB接口用于为视频模式提供显示数据。SPI接口用于对SSD2828的本地寄存器进行编程。如果驱动哑显示面板，需要对显示的寄存器进行编程，也可以使用SPI接口进行寄存器编程。如果驱动智能显示面板，也可以使用SPI接口在命令模式下配置智能显示和发送显示数据。下面是这个用例的一些插图。

![](images/1c8fddb20085d0ef7fd8903d7d8e72500486bcf773799b1e4f6b923afab31f32.jpg)  
图9-1：带有RGB和SPI接口的SSD2828

不同目的地的数据由数据包的VC字段分隔。用户可以对寄存器录像机进行编程来设置VC字段。使用此配置时，串行链路的主要用途是将显示视频数据发送到哑板。如果需要通过SPI接口并发发送非视频数据，SSD2828可以将其放入通用写包或DCS写包中，并与视频包交织。

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P103/168 < / td > < td > 2013年3月所罗门Systech < / td > < / tr > < /表> < /身体> < / html >

如果选择非突发视频模式，则只在垂直消隐期间发送非视频数据包。如果选择突发视频模式，则在水平和垂直空白时间段（如BLLP时间段）都可以发送非视频数据包。

在水平和垂直消隐期间，串行链路可以保持HS模式（发送消隐报文），也可以进入LP模式。非视频数据也可以以HS或LP方式发送。在消隐期间提供了使用HS或LP模式的选项，以及是否以HS或LP模式发送非视频数据。寄存器VICR6中的NVD和BLLP位就是为此目的而提供的。详情请参阅下表。

表9-2“视频模式BLLP”时间段的操作

<html><body><table><tr><td>NVD</td><td>BLLP</td><td>非突发模式</td><td>突发模式</td></tr><tr><td>0</td><td></td><td>如果没有非视频数据要发送，则在BLLP期间以HS模式发送空白。如果有非视频数据需要发送，则以HS模式发送非视频数据。之后，在BLLP周期的剩余时间内，串行链路将以HS模式发送消隐报文。packet</td><td>如果没有非视频数据要发送，则在BLLP期间串行链路进入LP模式。如果有非视频数据需要发送，则以HS模式发送非视频数据。之后，在BLLP周期的剩余时间内，串口链路将进入LP模式。</td></tr><tr><td></td><td></td><td>若无非视频数据发送，则在BLLP期间串口链路进入LP模式。如果有非视频数据需要发送，则以HS模式发送非视频数据。之后，在</td><td>的剩余时间内，串行链路将进入LP模式，与非突发模式相同。< / td > < / tr > < tr > < td > < / td > < td > < / td > < td > BLLP时期。如果是BLLP模式，串口链路将进入LP模式。如果有非视频数据需要发送，则在BLLP周期开始时以LP模式发送数据。期< / td > < td >一样non-burst模式。< / td > < / tr > < /表> < /身体> < / html >

# 9.2.2 MIPI直流特性

DSI的不同状态码表示数据通道和时钟通道中不同的直流电压水平，如下表所示。

表9-3:DSI状态码和DC特性

表身体< html > < > < > < tr > < td行宽=“2”>状态代码< / td > < td colspan =“2”>线电压水平< / td > < / tr > < tr > < td > Dp-Line < / td > < td > Dn-Line < / td > < / tr > < tr > < td > HS-0 < / td > < td > HS低< / td > < td > HS高< / td > < / tr > < tr > < td > HS-1 < / td > < td > HS高< / td > < td > HS低< / td > < / tr > < tr > < td > LP-00 < / td > < td > LP低< / td > < td > LP低< / td > < / tr > < tr > < td > LP-01 < / td > < td > LP低< / td > < td > LP高< / td > < / tr > < tr > < td > LP-10 < / td > < td > LP高< / td > < td > LP低< / td > < / tr > < tr > < td > LP-11 < / td > < td > LP高< / td > < td > LP高< / td > < / tr > < /表> < /身体> < / html >

如图9-2所示，不同状态下的逻辑级别不同。

![](images/45da73452e6d02ecd8953252a863dcf4eff847dd1b64a08dda492b8ef788b682.jpg)  
图9-2:MIPI线路电平

# 9.2.3高速时钟传输

在高速模式下，时钟通道为高速数据传输提供从主到从的低摆幅差分DDR（半速率）时钟信号。时钟启动和停止过程如下图所示。

![](images/8172575d6ba68296227d870540869f943aff4421119effd02162097e8080c7a8.jpg)  
图9-3：时钟通道在高速模式和低功耗模式之间切换

# 9.2.4数据通道状态流

数据通道有三种驱动模式：

12. 高速数据传输双向数据通道周转3。逃避方式

表9-4：数据通道模式进入/退出序列

表身体< html > < > < > < tr > < td >模式< / td > < td >进入模式序列< / td > < td >退出模式序列< / td > < / tr > < tr > < td >高速数据传输< / td > < td > LP-11 = > LP-10 = > LP-00 = > LP-01 = > LP - 00 < / td > < td > LP-00 = > LP-10 = > LP - 11 < / td > < / tr > < tr > < td >双向数据巷周转< / td > < td > LP-11 = > LP-01 = > LP-00 = > HS-0 < / td > < td > (HS-0或HS-1) = > LP-11 < / td > < / tr > < tr > < td >逃脱< / td > < td > LP 11 = > LP−−10 = > LP−00 = > LP 10 = > LP−−< / td > < td >高红移< / td > < / tr > < /表> < /身体> < / html >

# 9.2.5高速数据传输

高速数据传输以突发形式发生。传输从停止状态开始，并以停止状态结束。在爆发之间的中间时间内，数据通道应保持停止状态，除非在通道上提出周转或逃避请求。在HS数据突发期间，时钟通道应处于高速模式，向从端提供DDR时钟。

发送请求后，数据通道离开停止状态，通过开始传输（SoT）过程为高速模式做准备。
TX侧和RX侧的事件处理顺序如表9-5所示。

表9-5：传输开始顺序

<html><body><table><tr><td>观察到停止状态</td></tr><tr><td>观察到线路上从LP-11到LP-01的过渡</td></tr><tr><td>观察到线路在时间线td之后从LP-01到LP-00的过渡。TERM-EN</td></tr><tr><td>启用HS-RX并等待超时以忽略过渡效应。hs - settle </td></tr><tr><td>开始查找Leader-Sequence</td></tr><tr><td>识别Leader Sequence“011101”</td></tr></table></body></html>

在数据突发结束时，数据通道通过EoT （end -Transmission）过程离开高速传输模式，进入停止状态。EoT过程中可能出现的事件顺序如表9-6所示。注意，EoT处理可以由协议或D-PHY处理。

表9-6传输结束顺序

<html><body><table><tr><td>接收有效负载数据</td></tr><tr><td>检测离开LP-00状态进入停止状态（LP-11）并禁用Termination</td></tr><tr><td b>忽略最后周期位以隐藏转换效果THS-SKIP</td></tr><tr><td>检测有效数据中的最后一个转换，确定最后一个有效数据字节并跳过预告序列</td></tr></table></body></html>

下面显示了高速数据传输的顺序，包括SoT数据。

CLK   
xxxxx□xxxxx□XXXXXX□XXXXXX xxxx Dp/Dn TLPX thspprepare this - zero X this - sync断开连接终止器
-VIH(min)   
VIL(max)-   
- vtermen (max) VDTH(max) XO xxoxoooox捕获-这个-跳过这个- term - en 1STData位TEOT LP-11 LP-11 LP-01 LP-00这个- trail -这个- sexit -这个- settle

# 9.2.6双向数据通道转换

双向数据通道的传输方向可以通过链路转换过程进行交换。这个过程可以使信息在与当前方向相反的方向上传递。从正向到反向方向或从反向到正向方向的更改过程是相同的。

![](images/3896a767d06db2eeed090dcb4052bfe44a6b7abfdcff96cfae087f31a00d2440.jpg)  
图9-5：周转过程

# 9.2.7逃离模式

逃逸模式是低功耗状态下数据通道的一种特殊操作模式。有了这种模式，一些额外的功能就可用了。正向应支持Escape模式操作，反向可选。如果受支持，Escape模式不必包括所有可用的功能。

数据通道通过Escape模式进入程序（LP-11、LP-10、LP-00、LP-01、LP-00）进入Escape模式。一旦在线路上观察到最终的桥梁状态（LP-00），车道将在空间状态（LP-00）下进入逃生模式。如果在最终桥接状态（LP-00）之前的任何时间检测到LP-11，则逃逸模式进入过程将被中止，接收端将等待或返回到停止状态。

对于数据通道，一旦进入Escape模式，发送器将发送一个8位的进入命令来指示请求的操作。表9-7列出了当前所有可用的Escape模式命令和操作。保留所有未分配的命令以供将来扩展。

停止状态用于退出转义模式，并且由于空格- one - hot编码，在转义模式操作期间不能发生停止状态。停止状态立即将车道返回到控制模式。如果输入命令与支持的命令不匹配，则忽略特定的Escape模式动作，接收端等待，直到发送端返回到停止状态。

表9-7:MIPI Escape Mode Entry Code

<html><body><table><tr><td>逃避模式动作</td><td>命令类型</td><td> <td>低功耗数据传输</td><td>模式</td><td>11100001</td></tr><tr><td>超低功耗状态</td><td>模式</td><td>00011110</td></tr><tr><td>未定义-1</td>< /td>< tr><td>未定义-2</td><td> 11011110</td></tr><tr><td>Reset-Trigger [Remote .应用程序)< / td > < td >触发< / td > < td > 01100010 < / td > < / tr > < tr > < td >影响撕裂< / td > < td >触发< / td > < td > 01011101 < / td > < / tr > < tr > < td >承认< / td > < td >触发< / td > < td > 00100001 < / td > < / tr > < tr > < td > Unknown-5 < / td > < td >触发< / td > < td > 10100000 < / td > < / tr > < /表> < /身体> < / html >

# 9.2.8低功耗数据传输

低功耗数据传输的启动顺序如下：Start: LP-11 Escape Mode Entry: LP-11、LP-10、LP-00、LP-01、LP-00低功耗数据传输命令：11100001 <一个或多个字节（8位）数据通道停止时暂停模式Exit Escape Mode: LP-00、LP-10、LP-11 Stop State: LP-11 DT MMN v Escape LPDT First Data Byte Pause: Second Data Byte Exit Mode command 01110101 Asynchronous 11010000 Escape Entry no transition LP Clk = EXOR(Dp,Dn) www

图9-6：低功率数据传输

# 9.2.9复位触发器

当数据通道进入Escape模式时，AP可以通知显示模块应该在Reset触发中进行复位。

远程应用程序重置（RAR）使用以下顺序：

开始:LP-11
逃生模式入口：LP-11， LP-10, LP-00, LP-01, LP-00
Escape模式下的RAR （Remote Application Reset）命令：0110 0010 （First to Last bit）
标记1:LP-00， LP-10, LP-11
停止状态：LP-11

10 # LP-11 > > 00 > > 00 > > 10 > 00 00 > >。

aA！   
Escape Mode Entry进入命令Mark-1和停止状态
LP CLK $\mathbf {\tau} = \mathbf {\tau}$ EXOR（Dp, Dn）

# 9.2.10撕裂效果

当显示模块发生撕裂效应事件（新v同步）时，显示模块可以通过撕裂效应（TEE）通知AP。

撕裂效果（TEE）使用以下顺序：

开始:LP-11
逃逸模式入口（EME）： LP-11, LP-10, LP-00, LP-01, LP-00
撕裂效果：0101 1101（第一个到最后一个）
标记1:LP-00， LP-10, LP-11
停止状态：LP-11

![](images/07ac0d9f04940eb6f149959f82978a949bbd92c3f72414736a082a8fe504b58a.jpg)  
图9-8：逃离模式下的撕裂效果命令

# 9.2.11确认

当一个错误没有被确认（ACK）识别时，显示模块可以通知AP。确认（ACK）使用以下顺序：

$\bullet$ Start: LP-11
逃脱模式入口：LP-11， LP-10, LP-00, LP-01, LP-00
$\bullet$确认（ACK）命令：0010 0001（从第一位到最后一位）
$\bullet$ Mark-1: LP- $0 0 {=} {>} \mathrm {LP-} 1 0 {=} {>} \mathrm {LP-} 11 $停止状态：LP-11

# lp-11 >10±00>01>00>01>00>10>00>

C命令Mark-1和innnnnnnnnnnnrc LP CLK= EXOR（Dp, Dn）

# 9.2.12分组传输

SSL MIPI CORE支持MIPI DSI规范中定义的两种数据传输。

图9-10:Two Data Transmission Mode （Separate, single）

![](images/698eafa8bcd5071b7be6d898b45cd88ea4fdf47a85f5e71b8583ad175198909e.jpg)

# 9.2.13 HS传输示例

![](images/986897cee9cb49cffb9f20041b329e81171b111add51f7dae26a30b40fa694ab.jpg)  
图9-11：单通道数据传输示例

传输的字节数N为通道数的整数倍：

![](images/eb05abafb97a9ea5f5d3711b722d4d669a8b32d7c512c6a2e39aba6e505e0716.jpg)  
图9-12：双通道HS传输示例

# 9.2.14通用报文结构

为低级协议通信定义了两种数据包结构：长数据包和短数据包。对于这两种包结构，数据标识符总是包的第一个字节。所有包数据都以字节的形式通过接口。发送器按顺序先发送LSB数据，后发送MSB数据。对于具有多字节字段的数据包，除非另有规定，否则应首先传输最低有效字节。

图9-13:Endian示例（长报文）

表身体< html > < > < > < tr > < td > DI < / td > < td > WC (LS字节)< / td > < td > WC(女士字节)< / td > < td > ECC < / td > < td > < / td > < td >数据< / td > < td > CRC (LS字节)< / td > < td > CRC(女士字节)< / td > < / tr > < tr > < td > 0 x29 < / td > < td > 0 x01 < / td > < td > 0 x00 < / td > < td > 0 x06 < / td > < td > 0 x01 100101001000000000000000011000001000000001 < / td > < td > 0 < / td >人头马xoe特优香槟干邑< td > 1 1000001 111 000 < / td > < td > 0 x1e < / td > < / tr > < tr > < td colspan =“8”> L毫升< / td > < / tr > < tr > < td colspan =“2”> S党卫军B < / td > < td >毫升党卫军BB < / td > < td > < / td > < td >毫升党卫军BB < / td > < td >毫升党卫军BB < / td > < td > < / td > < td > M S B < / td > < / tr > < tr > < tdcolspan = " 8 " > BB < / td > < / tr > < tr > < td colspan =“8”>时间< / td > < / tr > < /表> < /身体> < / html >

# 9.2.15长报文格式

长报文的结构如图9-14所示。一个长包应该由三个元素组成：一个32位的包头（PH），一个可变字节数的特定于应用程序的数据负载，和一个16位的包脚（PF）。包头进一步由三个元素组成：一个8位的数据标识符，一个16位的单词计数，和8位的ECC。包页脚有一个元素，一个16位校验和。长报文的长度为6 ~ 65535字节。

数据标识符（DI）：包含虚拟通道标识符和数据类型信息数据类型表示应用程序特定的有效载荷数据的格式和内容16位字数（WC）：字数表示数据包有效载荷中有多少字（字节）接收方使用WC确定包端（在payload $^ +$ Checksum之后）8位纠错码（ECC）报头：报头8位ECC，报头最大保护8字节启用报头1位错误纠错和2位错误检测应用负载校验和CS (APPLICATION SPECIFIC PAYLOAD CHECKSUM) LPS SoT C EoT LPS 32位16位Packet Packet Header FOOTER (PH) (PF) Packet DATA (PAYLOAD): Length = WC DATA Word size（8位）PAYLOAD中数据字不受值限制

# 9.2.16短报文结构

报文结构如图9-15所示。短数据包应包含一个8位数据ID，后跟两个命令或数据字节和一个8位ECC；包页脚不应该存在。短数据包的长度应为4字节。ECC （Error Correction Code）字节允许在短报文中进行单比特错误纠错和2比特错误检测。

![](images/9ea574d739210b2cdc343471f30c901d353c224c1917c4a213e4a622648ab5db.jpg)  
图9-15：短报文结构

所有包数据都以字节的形式通过接口。发送器按顺序先发送LSB数据，后发送MSB数据。对于具有多字节字段的数据包，除非另有规定，否则应首先传输最低有效字节。

完整的长包数据传输如图9-13所示。注意，该图以标准位置表示法显示了字节值，即MSB在左边，LSB在右边，而位是按时间顺序显示的，LSB在左边，MSB在右边，时间从左到右递增。

# 9.2.17数据标识符（DI）

数据标识符为数据定义虚拟通道，为特定于应用程序的有效负载数据定义数据类型。

![](images/93b14e3a97eca544980409dfa4b247e8587781a9a5d5cb4dc9e126ff9ffade9a.jpg)  
图9-16：数据标识符结构

9.2.18虚拟通道标识符（VC）

VC是AP和显示模块之间的通道地址。在数据处理过程中，AP和显示模块将使用同一个VC进行通信。在SSD2085中，命令模式的VC为$0 \mathrm {x} 0 2 \mathrm {H}$，视频模式的VC为$0 \mathrm {x} 0 1 \mathrm {H}$。

9.2.19数据类型（DT）

有两组数据类型：

处理器到显示模块，显示模块到处理器

表9-8：处理器源数据包的数据类型

表身体< html > < > < > < tr > < td >数据类型,十六进制< / td > < td >二进制数据类型,< / td > < td >描述< / td > < td >数据包大小< / td > < / tr > < tr > < td > 01 H < / td > < td > 000001 < / td > < td >同步事件,V同步开始< / td > < td >短< / td > < / tr > < tr > < td > 11 H < / td > < td > 01 0001 < / td > < td >同步事件,V同步结束< / td > < td >短< / td > < / tr > < tr > < td > 21 H < / td > < td > 0001 < / td > < td >同步事件,H同步开始< / td > < td >短< / td > < / tr > < tr > < td > 31 H < / td > < td > 11 0001 < / td > < td >同步事件,同步结束< / td > < td >短< / td > < / tr > < tr > < td > 08年h < / td > < td > 001000 < / td > < td >传输结束(测试结束)包< / td > < td >短< / td > < / tr > < tr > < td > 02 h < / td > < td > 00 0010 < / td > < td >颜色模式(CM)命令< / td > < td >短< / td > < / tr > < tr > < td > 12 h < / td > < td > 010010 < / td > < td >颜色模式(CM)命令< / td > < td >短< / td > < / tr > < tr > < td > 22 h < / td > < td > 100010 < / td > < td >关闭周边命令< / td > < td >短< / td > < / tr > < tr > < td > 32 h < / td > < td > 110010 < / td > < td >打开外围命令</td><td>Short</td></tr><tr><td>03h</td><td>000011</td>< /td><td> 13h</td><td>01 0011</td><td>通用Short WRITE，参数</td><td>Short</td></tr><tr><td>23h</td><td> 100011 </td><td>通用Short WRITE， 2参数</td><td>Short</td></tr><tr><td> 000100</td><td>通用READ，无参数</td><td>Short</td></tr><tr><td>14h</td><td>010100</td><td>通用READ，参数< / td > < td >短< / td > < / tr > < tr > < td > 24 h < / td > < td > 100100 < / td > < td >一般读,两个参数< / td > < td >短< / td > < / tr > < tr > < td > 05 h < / td > < td > 000101 < / td > < td > DCS写,没有参数< / td > < td >短< / td > < / tr > < tr > < td > 15 h < / td > < td > 010101 < / td > < td > DCS写参数< / td > < td >短< / td > < / tr > < tr > < td > 06 h < / td > < td > 000110 < / td > < td > DCS阅读,没有参数< / td > < td >短< / td > < / tr > < tr > < td > 37 h < / td > < td > 110111 < / td > < td >设置最大返回数据包大小< / td > < td >短< / td > < / tr > < tr > < td > 09 h < / td > < td > 00 1001 < / td > < td >零包,没有数据< / td > < td >长< / td > < / tr > < /表> < /身体> < / html >

表9-9外设源报文的数据类型

<html><body><table><tr><td>数据类型，十六进制</td><td>数据类型，二进制</td><td>描述</td><td> 19h</td>< /tr><tr><td>19h</td><td>01 1001</td>< tr><td>29h</td><td>101001</td>< /td>< tr><td>39h</td><td>111001</td><td>DCS长写/write_LUT命令包</td><td> </td>< tr><td>0Eh</td><td>001110</td><td>压缩像素流，16位RGB，5-6-5格式</td><td>长</td></tr><tr><td>1Eh</td><td>011110</td>< /td><td> 2Eh</td><td>101110</td><td> 3Eh</td><td>111110</td><td>压缩像素流，24位RGB， 8-8-8格式</td><td>长</td></tr><tr><td>xOh和xFh，未指定</td><td>xx 0000 xx1111</td><td>请勿使用所有未指定的代码均保留</td><td></td></tr></table></body></html>

<html><body><table><tr><td>数据类型，hex</td><td>数据类型，描述二元< / td > < td > < / td > < td >数据包大小< / td > < / tr > < tr > < td > 00 h-01h < / td > < td > 00000 x < / td > < td >保留< / td > < td >短< / td > < / tr > < tr > < td > 02 h < / td > < td > 00 0010 < / td > < td >承认和错误报告< / td > < td >短< / td > < / tr > < tr > < td > 03 h - 07年h < / td > < td > 00 0011 000111 < / td > < td >保留< / td > < td > < / td > < / tr > < tr > < td > 08年h < / td > < td > 00 1000 < / td > < td >传输结束(测试结束)包< / td > < td >短< / td > < / tr > < tr > < td > 09 h 10 h一< / td > < td > 00 1001 01 0000 < / td > < td >保留< / td > < td > < / td > < / tr > < tr > < td > 11 h < / td > < td > 010001</td><td>通用短读响应，返回字节</td><td>Short</td></tr><tr><td>12h</td><td>01 0010</td><td>通用短读响应，2字节返回< / td > < td >短< / td > < / tr > < tr > < td > 13 h - 19 h < / td > < td > 01 0011 01 1001 < / td > < td >保留< / td > < td > < / td > < / tr > < tr > < td > 1啊< / td > < td > 011010 < / td > < td >通用长阅读响应< / td > < td >长< / td > < / tr > < tr > < td > 1 bh < / td > < td > 011011 < / td > < td >保留< / td > < td > < / td > < / tr > < tr > < td > 1 ch < / td > < td > 011100 < / td > < td > DCS长阅读响应< / td > < td >长< / td > < / tr > < tr > < td > 1 dh 20 h < / td > 011101 < td > 0000 < / td > < td >保留< / td > < td > < / td > < / tr > < tr > < td > 21 h < / td > < td > 0001 < / td > < td > DCS短阅读反应,返回字节< / td > < td >短< / td > < / tr > < tr > < td > 22 h < / td > < td > 0010 < / td > < td > DCS短阅读反应,2字节返回< / td > < td >短< / td > < / tr > < tr > < td > 23 h-3fh < / td > < td > 10 0011 111111 < / td > < td >保留< / td > < td > < / td > < / tr > < /表> < /身体> < / html >

![](images/aef1ee132c95161f69428edc849550e35ef2fc8c65df9cdb08eea5f6de878277.jpg)  
图9-17:16bit / pixel RGB Color Format， Long packet for MIPI 接口

![](images/72c3dfeb49877b76eddc056257b18a324f36e7b1f3e79d65ea84436b9519e78d.jpg)  
图9-18:18bit / Pixel - RGB Color Format， Long packet for MIPI 接口

![](images/0a745034686c50ed2b61ef209323dcff0ececd81afad2080b7a127eed1fc072a.jpg)  
图9-19:18bit / Pixel in Three Bytes - RGB Color Format， Long packet for MIPI 接口

![](images/618c4aa5fc1f8801f9d011e0ef5f04d124d41d7d86b9a011e798bf53d03440a5.jpg)  
图9-20:24bit / Pixel - RGB Color Format， Long packet for MIPI 接口

# 9.3工作模式

视频数据来自RGB接口，配置通过SPI接口完成。为了支持不同的bpp设置，使用以下数据引脚。对于所有情况，R应该在上位，B应该在下位。

数据[15:0]为16 bpp。
数据[17:0]为18 bpp，已打包。
数据[17:0]为18 bpp，松散打包。
数据[23:0]为24bpp。

首先，用户需要用正确的值对寄存器VICR1到VICR6进行编程。用户还需要将END和CO位分别编程为0和1。在对这些寄存器字段进行编程后，用户可以打开RGB接口，使VEN位开始传输。支持MIPI DSI规范中定义的所有三种视频模式序列。

在非突发模式下，CSS（寄存器0xB7位5）可以设置为0或1。当设置为1时，选择pclk作为PLL参考时钟，则PLL倍增系数应该等于bpp的值。当设置为0选择tx_clk作为PLL参考时钟时，应设置PLL倍增系数，使串行链路的数据速率大于输入数据速率。请参考下表的锁相环设置。寄存器VICR1到VICR6 （0xB1到$0 \times \mathsf {B} 6 ^ {\prime}$）需要编程。（VICR1不用于同步事件的非突发模式）。（VICR1不用于同步事件的非突发模式。）下面的图表说明了所有字段的定义。

![](images/d50a2c4cbb0149755739d9836b180fe3a5dd0df137639ef24b9bd89df697fdc5.jpg)  
图9-21：带有同步脉冲的非突发模式的RGB接口参数说明

表9-10：非突发模式的锁相环设置（使用pclk作为锁相环参考）

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利>像素每日分之一的BPP(位)& lt; /戴利的小于戴利colspan =“2”> > PLL Multiplication Factor & lt; /戴利的小于戴利colspan =“2”> > PLL产量一年的Frequency & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> >一个data lane & lt; /戴利的小于戴利> > 2的小于戴利data lane & lt; /戴利> >一个data lane & lt; /戴利的小于戴利> >两data lane & lt; /戴利> & lt; / tr > & lt tr小于戴利rowspan =“4”> > 16 18,packed 18,loosely packed & lt; /戴利的小于戴利> > 16 & lt; /戴利的小于戴利> > 8 & lt; /戴利的小于戴利> > 16 x pclk & lt; /戴利的小于戴利> > 8乘以pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> 9 & lt; /戴利的小于戴利> > 18 & lt; /戴利的小于戴利> > 18 x pclk & lt; / x pclk 9戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利24 & lt; /戴利的小于戴利> > > 12 & lt; /戴利的小于戴利> > 24 x pclk & lt; /戴利的小于12戴利> > x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利24 & lt; /戴利的小于戴利> > > 12 & lt; /戴利的小于戴利> > 24x pclk & lt; /戴利的小于12戴利> > x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利rowspan = 16 18个,“4”>packed & lt; /戴利的小于戴利> >三data lane & lt; /戴利的小于戴利> > 4 data lane & lt; /戴利的小于戴利> >三data lane & lt; /戴利的小于戴利> > 4 data lane & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> 5.33 & lt; /戴利的小于戴利> > 4 & lt; /戴利的小于戴利> > 5.33 x pclk & lt; /戴利的小于戴利> > 4 x pclk & lt; /戴利> & lt; / tr > & lt小于戴利tr > > 6 & lt; /戴利的小于戴利> > 45 & lt; /戴利的小于戴利> >六x pclk目标& lt; /戴利的小于戴利> > 45 x pclk & lt; /戴利> & lt; / tr > & lt小于戴利> 8 8 & lt; / tr > & lt; /戴利66戴利的小于戴利> > > & lt;戴利> 8乘以pclk 8乘以pclk & lt; /戴利的小于戴利> > 6 x pc1k 6 x pc1k & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

表9-11：非突发模式PLL设置（使用tx_clk的PLL引用）

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利>像素每日分之一的BPP(位)& lt; /戴利的小于戴利colspan =“2”> > PLL Multiplication Factor & lt; /戴利的小于戴利colspan =“2”> > PLL产量一年的Frequency & lt; /戴利> & lt; / tr > & lt小于戴利> 1 & lt; / tr >戴利的小于戴利> > data lane & lt; /戴利的小于戴利> > 2的小于戴利data lane & lt; /戴利> >一个data lane & lt; /戴利的小于戴利> >两data lane & lt; /戴利> & lt; / tr > & lt tr小于戴利rowspan =“4”> > 16 18,packed 18,loosely packed & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 16 x pclk & lt; /戴利的小于戴利> > > = 8 pclk x & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 18 x pclk & lt; /戴利的小于戴利> > > = 9 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = x 24 pclk & lt; /戴利的小于戴利> > > = 12 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = x 24 pclk & lt; /戴利的小于戴利> > > = pclk 12x & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利16 & lt; /戴利的小于戴利> > >三data lane & lt; /戴利的小于戴利> > 4 data lane & lt; /戴利的小于戴利> >三data lane & lt; /戴利的小于戴利> > 4 data lane & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利rowspan 18个,packed 18 =“4”>,loosely packed 24 & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 5.33 x pclk & lt; /戴利的小于戴利> > > = 4 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 6 x pclk & lt; /戴利的小于戴利> > > = 45 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > & lt; / 8 = x pclk戴利的小于戴利> > > = 6 pclk x & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 8乘以pclk & lt; /戴利的小于戴利> > > = 6 x pclk & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

在突发模式下，需要将css（注册$0\ *\ mathsf {B 7} $bit 5）设置为0，以选择tx_clk作为PLL引用时钟。设置PLL乘法系数，使串行链路数据速率快于输入数据速率。请参考下表进行PLL设置。将VICR2注册到VICR6 $\ mathtt{0\ mathtt{x} \ mathtt{b} 1$到$0\乘以\ maths{b} 6 cdot}需要编程。这种模式不使用vicr1。所有字段的定义与具有同步事件的非突发模式相同。

![](images/8bb09155cb0d519c711f4e99de2f16c42f38ce86b4618ee443d38e10646b8c29.jpg)  
图9-22：带同步事件和突发模式的非突发模式的RGB接口参数说明

表9-12：突发模式PLL设置

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利>像素每日分之一的BPP(位)& lt; /戴利的小于戴利colspan =“2”> > PLL Multiplication Factor & lt; /戴利的小于戴利colspan =“2”> > PLL产量一年的Frequency & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> >一个data lane & lt; /戴利的小于戴利> > 2的小于戴利data lane & lt; /戴利> >一个data lane & lt; /戴利的小于戴利> >两data lane & lt; /戴利> & lt; / tr > & lt tr小于戴利rowspan =“3”> > 16 18,packed 18,loosely packed & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 16 x pclk & lt; /戴利的小于戴利> > > = 8乘以pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 18 x pclk & lt; /戴利的小于戴利> > > = 9 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = x 24 pclk & lt; /戴利的小于戴利> > > = 12 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利24 & lt; /戴利的小于戴利> > > NA data lane 3 & lt; /戴利的小于戴利> > NA 4 data lane & lt; /戴利的小于戴利> > > = 3 x pclk 24 data lane & lt; /戴利的小于戴利> > > = 12 x pclk 4 data lane & lt; /戴利> & lt; / tr > & lt小于戴利tr > > 16 18, packed & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = 5.33 x pclk & lt; /戴利的小于戴利> > > = 4 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利rowspan = 18个,“4”>loosely packed 24 & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > > = & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; / x pclk 6戴利的小于戴利> > > = 8乘以pclk & lt; /戴利的小于戴利> > > = 45 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> NA & lt; /戴利的小于戴利> > NA & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > > = 6 x pclk & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > > = 8乘以pclk & lt; /戴利的小于戴利> > > = 6 x pclk & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

\ *：应设置该值，使串行链路数据速率快于输入数据速率。

SSD2828还将监测CM和关闭信号的状态。当这些信号发生变化时，它会发出适当的数据包。在CM的上升边缘，cm上的包会被发送。在CM下降的边缘，cm将被发送出去。在关闭的上升边缘，关闭的外围数据包将被发送。在关闭的下降边缘，将发送打开外围包的开关。有了这些数据包，mipi奴隶将能够重建RGB接口信号。

![](images/df71aa145c5f9d4400faf7df63c281b6b2a5ff9e88917f981ab656087801abd3.jpg)  
图9-23:非突发模式MIPI结构

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 124/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

![](images/71879ce6c3382a937c3e4fb81dc9b95b6e81281b76b5bbb6e8f23c881cfc40eb.jpg)  
图9-24突发模式MIPI结构

# 9.3.1.1写操作

如果要进行写操作，用户需要将REN位置为0。SSD2828可以发出四种写操作报文：通用短写报文、通用长写报文、DCS短写报文和DCS长写报文。位DCS控制是发送通用写包还是DCS写包。VC1或VC2字段决定出报文的VC ID。（VC1和VC2的区别请参见8.1.9）

SSD2828需要知道出方向报文的负载大小。因此，用户需要对相应的控制寄存器进行编程。PSCR1和PSCR2组成TDC字段，表示负载字节总数。

发送DCS Write Packet时，用户需要将DCS命令/报头和负载写入寄存器PDR，且DCS位设置为1。如果TDC字段不大于2，SSD2828将发送正确类型的DCS短写包。否则，将发送DCS长写包。

发送Generic Write Packet时，用户需要将负载写入寄存器PDR，并且DCS位设置为0。如果TDC字段不大于2，SSD2828将发送正确类型的通用短写包。否则，将发送Generic Long Write Packet。

对于“DCS Write Packet”，仅当DCS命令为“$0 \mathrm {x} 2 \mathrm {C}$”或“$0 \mathrm {x} 3 \mathrm {C}$”时才启用分区。否则，SSD2828不会进行自动分区。（这是因为DCS命令$0 \mathrm {x} 2 \mathrm {C}$和$0 \mathrm {x} 3 \mathrm {C}$是将显示数据写入LCD面板显示存储器。）有效载荷将被划分为几个数据包，每个数据包的有效载荷是PST字节。第一个字节是DCS命令，下面的PST字节是有效负载。只有最后一个数据包可能包含较少的有效载荷，因为总有效载荷可能不是PST的整数倍。如果输入的DCS命令为$0 \mathrm {x} 2 \mathrm {C}$，则第一个数据包的DCS命令为$0 \mathrm {x} 2 \mathrm {C}$，其他所有数据包的DCS命令为0x3C。如果接收到的DCS命令为$0 \mathrm {x} 3 \mathrm {C}$，则所有报文的DCS命令为0x3C。

例如，在原始数据模式$\mathbf {I F C}{=} 0 \$)下，如果TDC字段为200，PST字段为80，则发送3个数据包。前两个有80字节的有效载荷。最后一个数据包有40字节的有效载荷。

在执行写操作后，用户可以选择进行BTA，让MIPI从机报告其状态。这是通过将FBW位设置为1来完成的。每次写操作后，SSD2828会自动生成一个BTA。有关如何处理收到的确认，请参阅0。

![](images/6f999aa18d0a34e86808e999777dfc95cb63f395a1ab186b063fbe0778926e23.jpg)

# 9.3.1.2读操作

如果要进行读操作，用户需要将REN位设置为1。SSD2828可以发出两种读操作报文：Generic read Packet和DCS read Packet。位DCS控制是发送通用读包还是DCS读包。VC1或VC2字段决定出报文的VC ID。（VC1和VC2的区别请参见8.1.9）

在发送读报文之前，SSD2828始终会发送Set Maximum Return Size报文。这是为了限制MIPI从机发送的读响应包，这样就不会有溢出。两个因素决定了最大尺寸。一个是SSD2828的限制，另一个是应用处理器的限制。用户应在这两个限制中选择较小的一个作为最大返回尺寸。

设置最大返回大小包中的参数取自寄存器MRSR。用户可以在每次读取之前对MRSR进行编程，以便通过设置最大返回大小包发送正确的值。如果MRSR中的值已经是期望的值，用户可以选择不对它进行编程。SSD2828将始终自动发送使用MRSR中的值设置最大返回大小的数据包。

发送DCS读包时，用户只需要将DCS命令（因为没有DCS读参数）写入PDR寄存器，并将DCS位设置为1。

发送Generic Read Packet时，用户需要将负载写入寄存器PDR，并且DCS位设置为0。

与写操作类似，TDC字段用于确定出包的有效负载大小。对于DCS读包，有效负载只是DCS命令。没有关联的参数。对于Generic Read Packet， SSD2828将根据TDC值发送正确的报文类型。

SSD2828发送读报文后，会自动进行BTA，等待MIPI从机的读响应报文。返回的数据将存储在寄存器RR中。无论发送哪个读包，都只有一个包返回数据。因此，无论读是DCS读还是Generic读，无论在DCS读中使用什么命令，返回的数据总是存储在寄存器RR中。当RDR位设置为1时，用户可以读出数据。在看到RDR位被设置为1后，用户应该首先读取寄存器RDCR，其中包含MIPI从机返回的字节数。通过使用这些信息，用户将知道应该从寄存器RR中读出多少数据。所有返回数据被读出后，RDR位被SSD2828置为0。

当RDR位设置为1后，用户可以选择不从寄存器RR读取数据。用户可以继续执行其他操作。一旦用户这样做，RDR位将被SSD2828设置为0。

MIPI从机可能同时发送了确认报文和错误报告报文。确认处理的操作在0中描述。

在某些情况下，MIPI从机可能只发回确认和错误报告包，而不发回任何数据。因此，RDR位不会被设置。因此，建议用户先检查BTAR位。BTAR用于指示MIPI从站是否已将总线权限传递回SSD2828。只有当BTAR为1时，才可能有返回数据。如果没有返回数据，用户应该跟随0来处理确认。

![](images/b78391d24a3b97142865097c694bf51b91d9e40917289427bfdd04ff48e41c4f.jpg)

# MIPI回读

![](images/7b780af0340303f70f8929e2aed5dd116e2022d4a1bcc8ade10d74c0ee77c8a3.jpg)

9.3.1.3确认操作

SSD2828可以执行BTA，将总线权限授予MIPI从站，并让它报告其状态。BTA可以设置为“FBW位”为1，并进行写操作，也可以只进行读操作。在MIPI从站传回总线授权后，SDD2828将把BTAR位设置为1。

如果从端没有错误，如果BTA之前的数据包是写数据包，MIPI从端将返回ACK触发消息。如果BTA之前的数据包是读数据包，MIPI从机将返回读响应数据包。在这种情况下，SSD2828收到MIPI从机的响应后，会将ARR位和ATR位设置为1。ARR表示已经从MIPI slave接收到响应。ATR表示MIPI从机没有报告ACK触发消息错误。因此，寄存器ARSR将被清除为0。

如果从端出现错误，如果BTA之前的报文是写报文，则MIPI从端返回“确认和错误报告”报文。如果BTA之前的报文是读报文，MIPI slave将返回读响应报文（取决于错误类型）和承认和错误报告报文。在这种情况下，SSD2828收到MIPI从机的响应后，会将位ARR位置为，将位ATR位置为0。ARR表示已经从MIPI slave接收到响应。ATR表示MIPI slave发送的是确认和错误报告报文，而不是ACK触发消息。因此，MIPI从机报告了错误。MIPI从机报告的错误将存储在寄存器ARSR中。用户可以读取这个寄存器来查看MIPI从机遇到了什么错误。

对于每个错误位的详细描述，请参考MIPI DSI规范。下面是处理MIPI从属确认的流程图。仅供参考。

![](images/e551acd13d1ca9bb3c955e1e1dd4ff99b114e0f18ccc7fb1c958de398a6abea7.jpg)  
图9-25非读命令后的确认处理

![](images/539ecd316034131ffb7a579d9da148d6fed3e72ec6d0d5afafe436341b028199.jpg)  
图9-26读取命令后的确认处理

![](images/4c4f725ad663b4f0d1f8b4d6411351aa05b450939683613fbc987d9ec43b4a71.jpg)  
注：Rx的LP时钟必须在Tx LP时钟的$10 \%$范围内

表9-13 MIPI错误报告

表身体< html > < > < > < tr > < td >位< / td > < td >描述< / td > < / tr > < tr > < td > 0 < / td > < td >说误差< / td > < / tr > < tr > < td > < / td > < td >说同步误差< / td > < / tr > < tr > < td > 2 < / td > < td >测试结束同步误差< / td > < / tr > < tr > < td > 3 < / td > < td >逃脱模式输入命令错误< / td > < / tr > < tr > < td > < / td > < td >低功耗传输同步误差< / td > < / tr > < tr > < td > < / td > < td >外围超时错误< / td > < / tr > < tr > < td > < / td > < td >假控制误差< / td > < / tr > < tr > < td > < / td > < td >争用检测到< / td > < / tr > < tr > < td > < / td > < td > ECC错误,单比特（检测和校正）</td></tr><tr><td></td><td>ECC错误，多比特（检测和校正）</td></tr><tr><td>10</td><td>校验错误（仅限长包）</td></ td></td><td>DSI数据类型不可识别</td></tr><tr><td>12</td><td>DSI VC ID无效</td></tr><tr><td>14</td><td> DSI协议违反</td></tr></table></body></html>

# 9.3.1.4撕裂效果（TE）操作

TE操作是在前一个BTA之后执行一个BTA，而不传输中间的任何内容。总线被交给MIPI从站来提供TE信息。从显示驱动程序获得TE事件后，MIPI从站将使用BTA触发消息将总线权限传递回SSD2828。

在向MIPI slave写入最后一条命令之前，将FBT位和FBW位设置为1，可以使能TE操作。然后，应用程序处理器可以指示SSD2828在写包中发出最后一条命令。由于FBW为1，SSD2828将在写操作后自动执行BTA。MIPI从站将响应并将总线权威传递回来。由于FBT为1，SSD2828将执行另一次BTA，而不发送任何数据。这使得MIPI从机进入TE模式。

当MIPI从机收到TE事件时，它将返回一个TE触发消息。收到触发消息后，SSD2828将TE引脚置为1，表示收到TE事件。DATA[16]作为TE引脚。同时，位TER将被设置为1。应用程序处理器可以写这个位来清除它。由于TE触发消息仅决定何时将TE引脚设为1，因此使用计数器来确定何时将TE引脚设为0。一旦计数器达到TEC的值，TE引脚将被设置为0。计数器使用参考时钟进行计数。

如果MIPI从站没有发送回TE触发消息，而只是执行BTA将总线传递回，SSD2828将自动执行另一个BTA将总线再次传递给MIPI从站。它将继续这样做，直到MIPI从机响应TE触发消息，或者FBT位被设置为0，或者LP RX定时器过期。

如果MIPI slave没有发回TE触发消息，并且仍然持有总线，用户可以将位FBC设置为1来强制总线争用。总线争用解决后，从站将把总线传回SSD2828。

# 9.3.1.5争用检测和定时器操作

在SSD2828中定义了两个计时器来解决总线上潜在的争用问题。这两个定时器分别是HS TX定时器和LP RX定时器。请参阅寄存器描述以了解详细用法。

只要SSD2828看到检测到争用，它就会重置状态机并进入默认模式，即LP TX空闲模式。数据线将保存在LP11。

# 9.3.1.6中断操作

提供了一个中断信号int $\smash {0 / \ maththrm {int} _{-} 1}$来中断应用程序处理器，以便它不需要一直轮询状态。这将节省应用程序处理器的处理时间。Int_ $\ mathm {0 / I nt \_ 1}$是一个活动的低信号，换句话说，当事件发生时，将变为低信号。

有许多源可以映射到中断信号。用户可以选择不同的源来执行不同的任务。如果选择了多个源，当其中一个源的事件发生时，int_0/int_1信号将变低。在这种情况下，用户需要读取寄存器ISR来确定发生了什么事件。可以通过注册ICR启用/禁用不同的源。下面是可用中断源的列表和它们的用法。

# RDR

表示从MIPI slave返回的数据可供读取。

# BTAR

指示SSD2828是否具有总线权限。SSD2828制作BTA后即可使用。如果MIPI从站已经将总线权限返回给SSD2828，中断将被设置为指示这样做。请注意，在上电时，总线授权已经在SSD2828上。因此，SSD2828将显示它具有总线权限。

# ARR

指示SSD2828是否收到MIPI从机的确认响应。确认响应可以报告错误，也可以不报告错误。这是由ATR位决定的。

上述三个中断提供给用户来处理从MIPI slave读取数据或从MIPI slave获得确认响应。

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 133/168 < / td > < td > 2013年3月< / td > < / tr > < /表> < /身体> < / html >

# PLS

指示锁相环是否被锁定。如果锁相环没有锁定，那么外部接口的编程速度一定很慢。在改变PLL设置或改变参考时钟源后，用户还需要使用该中断来确定PLL状态。

上电时，只启用PLS中断。这是为了让用户在配置SSD2828之前确定编程速度。

# LPTO

表示有LP RX超时。

# HSTO

表示有HS TX超时。

上述两个中断提供给用户用于错误处理。

# PO

指示SSD2828是否准备好接受来自用户的任何数据。SSD2828有几个内部缓冲区来保存用户写入的数据。当用户写入速度超过串行链路速度时，这些缓冲区将被填满。如果用户仍然向SSD2828写入数据，这些数据将丢失。用户将要写入的下一个数据包的有效负载长度由TDC、PST和DCS字段决定。SSD2828将使用这些字段来决定用户是否可以写入下一个数据包。因此，在编写了上述字段之后，用户需要在写入之前检查中断状态。

# se, sa, sle, sla, le, mla

所有这些中断都是用来指示内部数据缓冲区的状态。如果用户熟悉SSD2828的缓冲区管理，则可以使用它们。否则，建议使用PO中断。

需要注意的一个重要问题是中断延迟。输出中断信号在操作后不会立即改变。这是由于SSD2828的内部处理。例如，在将中断源从一个更改为另一个之后，输出int_ $\underline {{0 /}} \mathrm {int \underline {{{1}}}}$ level将在编程完成后的短时间内保持在旧水平。另一个例子是，在编程TDC字段之后，中断将需要很短的时间来反映int $\underline {{0 / \ maththrm {int}}} \underline{{1}}$上的正确PO状态。在实际事件和中断之间总是有一个延迟。

为了保证用户能够获得正确的中断，建议用户在接收中断信号或轮询中断状态位之前，对任何SSD2828本地寄存器执行一次读取。读取操作将覆盖中断延迟期。或者，用户可以等待一定的时间来确保中断反映真实状态。下面是一个图解。

![](images/2f79e520740cbf8fc8af9a4cc14df1c851ce914bd086ef18d90f4d0e242e8334.jpg)  
图9-27中断延迟示意图

# 9.3.1.7内部缓冲区状态

SSD2828QN4内部有完整的数据缓冲区，即SPI命令接口缓冲区（CB）。

对于CB缓冲区，所有数据包都将存储在其中。它们可以存储多个数据包，每个数据包的大小可以设置为1023字节。下面是可能的数据包列表

通用短写报文通用读报文DCS短写报文DCS读报文通用长写报文DCS长写报文

在自动分区的情况下，报文长度由PST字段决定。不建议将PST字段设置得这么小。

用户可以通过SPI接口写入数据。所有的数据包将被写入CB缓冲区。因此，用户需要检查相应的中断。下面列出了中断的用法。

# CBE

指示命令缓冲区为空。

# CBA

指示命令缓冲区至少可以容纳更多的数据包。用户可以将这样的报文写入CB缓冲区。

这里提到的中断可以用作应用程序处理器和SSD2828之间的流控制。但是，它要求用户很好地了解缓冲区操作。PO中断是这八种中断的组合。它根据用户提供的参数来决定下一个要写入的数据包。因此，用户不需要知道将要使用哪个缓冲区以及缓冲区状态如何。

# 9.3.2状态机操作

状态机通过串行链路控制数据包的发送和接收。它由来自应用程序处理器或接收到的数据的事件触发。一旦一个完整的数据包被写入SSD2828缓冲区，它将通过串行链路发送出去。用户可以随时将1写入位COP，以取消当前所有操作。说明请参见8.1.17。

当SSD2828处于高速模式时，串行链路主要用于发送显示数据。如果没有数据要发送，它将发送空包以保持串行链路时序。如果应用处理器在很长一段时间内没有显示数据要发送，它可以通过将寄存器位HS设置为0来将串行链路转换为低功耗模式。

当SSD2828处于低功耗模式时，串行链路主要用于发送命令和配置数据。如果没有数据要发送，SSD2828将以LP TX停止模式空闲。

用户也可以通过将1写入SLP位来进入休眠模式。一旦SLP位设置为1，SSD2828将自动进入LP模式。如果HS位为1，则SSD2828将HS位清除为0，并从HS模式切换到LP模式。之后，SSD2828会向MIPI从机发出ULPS触发消息，进入超低功耗状态。在此状态下，可以关闭到SSD2828的时钟，使SSD2828只消耗泄漏电流。这将节省整个系统的功耗。用户退出ULPS时，可以将0写入SLP位。但是，用户应该意识到退出ULPS的时间相对较长（请参考MIPI DPHY规范）。因此，在系统退出ULPS之前，用户无法进行任何数据传输。

在接收过程中，状态机将入站数据包拆解，并将接收到的寄存器内容放入内部缓冲区中读出。一旦所有数据都放入缓冲区，它将把寄存器位RDY设置为1，表示SSD2828已准备好读取。接收到的字节总数也将存储在RDCR中。

接收完成后，SSD2828将进行一次总线翻转，进入传输模式。
在它进入任何其他模式之前，它总是会回到LP TX停止模式。

# 9.3.3 D-PHY操作

D-PHY控制模拟收发器的操作。它控制串行链路是处于高速还是低功耗模式，是处于发送还是接收模式。

在传输模式下，D-PHY将根据PCU的控制在LP模式和HS模式之间切换时执行握手程序。在HS模式下，D-PHY将向模拟发射机提供并行数据和时钟，用于差分信号的串行传输。在LP模式下，D-PHY将直接驱动Datap和Datan线输出。它将向模拟发射机提供串行数据。

在接收模式下，D-PHY将检测LP模式下的握手序列并通知PCU。一旦进入转义模式，它将从模拟接收器收集串行数据并将其转换成字节形式供PCU处理。

在MIPI DPHY规范中定义了各种时序参数。计时参数是绝对时间和周期计数的混合。因此，对于不同的运行速度，有不同的时序要求。为此目的提供了寄存器DAR1至DAR6。用户可以调整这些寄存器中的值以获得不同的DPHY时序参数。这为不同的操作速度提供了最大的灵活性。

＃ 9.3.4模拟收发器

# 9.3.5 PLL

锁相环输出频率由下式计算：

$$
\begin{array} { l } { \displaystyle f _ { P R E } = \frac { f _ { I N } } { M S } } \\ { \displaystyle f _ { O U T } = f _ { P R E } \ast N S } \end{array}
$$

其中$f _ { I N }$为输入参考时钟频率，$f _ { O U T }$为锁相环的输出时钟频率。

时钟频率需要满足下面的约束。

$$
\begin{array} { r l } & { 5 M H z > f _ { I N } \ge 1 0 0 M H z } \\ & { 5 M H z > f _ { R E F } \ge 1 0 0 M H z } \\ & { 6 2 . 5 M H z > f _ { O U T } \ge 1 0 0 0 M H z } \end{array}
$$

FR、MS和NS的值由寄存器PLCR控制。

FR、MS和NS的所有值只有在锁相环关闭时才能修改。因此，修改的顺序是关闭PLL，修改寄存器值，打开PLL。

# 9.3.6时钟源示例

![](images/ead5b5da06a3116ecd69956b3b4459fc924aea2abeeebcb3b1702d728fd5c624.jpg)

# 10外部接口

SSD2828支持三种SPI接口，

8位3线（C型选项1，DBI 2.0）
8位4线（C型选项3，DBI 2.0）
24位三线制

和RGB接口。

选择由ps[1:0]引脚控制。

RGB接口支持4bpp设置。下面是用于每个接口的数据引脚。对于所有情况，R应该在上位，B应该在下位。

数据[15:0]为16 bpp。
数据[17:0]为18 bpp，已打包。
数据[17:0]为18 bpp，松散打包。
数据[24:0]为24bpp。SPI接口支持8位数据总线。应该首先写入最低有效位字节。
下面是每个接口的操作和时序图。

# 10.1 SPI接口8位4线

该接口由sdcx、sck、sdin、sdout和csx组成。它只支持8位数据。每个周期包含8位数据。第一个周期应该是一个命令写周期，用来指定要访问的寄存器地址。后面的周期是读或写操作的读或写周期。

csx应该从1到0驱动开始操作，从0到结束操作。在操作过程中，应用处理器可以写入或读取多个字节。

SDCX表示该操作是数据操作还是命令操作。当sdcx = 1时，表示对数据进行操作。当sdcx为0时，表示执行命令操作。在1次操作中，SDCX在SCK的每${8}^ {\ mathm {th}}$上升沿采样。

在写操作期间，stin将由SSD2828在sck的上升沿采样。csx下降沿之后sck的第一个上升沿采样8位数据的位。sck的第二个上升沿采样8位数据的第6位，以此类推。sdcx的值与8位数据的0位一起在sck的第8个上升沿采样。请看下面的图表来说明。可选地，csx可以被驱动到两个周期之间。

![](images/87c7a76f445efc5cb459ea9914ce7cee9e442a0eb8785ee5f074b1ecf4d8216f.jpg)  
图10-1 8位4线接口写操作示意图

备注：发送LSB 8bit后发送MSB 8bit

在读操作期间，因为没有rwx信号来指示操作是读还是写。在csx被驱动为低电平之后，第一个周期总是一个命令写周期，它指定要访问的寄存器。第二个周期仍然是命令写周期。如果此周期中的命令与寄存器LRR中的命令匹配，则SPI接口进入读模式。接下来的周期将是读周期。如果该命令不匹配，SPI接口将保持写模式。

进入读模式后，返回数据在sout上提供，在sck的下降沿上。应用程序处理器应该使用sck的上升沿对数据进行采样。在读取周期中，SDCX应该被驱动为1。请看下面的图表来说明。可选地，csx可以在两个周期之间被驱动到1。

![](images/2a1b64a8b895904bc55cc4586027e6468a69a0b0e8dc41e03ea3b06ebb79989d.jpg)  
图10-2 8位4线接口读操作示意图

# 10.2 SPI接口8位3线

该接口由sck、sdin、sout和csx组成。它只支持8位数据。每个周期包含8位数据。第一个周期应该是写周期，用来指定要访问的寄存器地址。后面的周期是读或写操作的读或写周期。

csx应该从0开始操作，从0结束操作。在操作过程中，应用处理器可以写入或读取多个字节。

不是sdcx，而是sdcx位来指示操作是针对数据还是针对命令。每个字节与一个sdcx位相关联。当sdcx为1时，表示该操作为显示数据。当sdcx为0时，表示执行命令操作。sdcx位被预先发送到每个字节。换句话说，sdcx位是操作过程中每9位的第一个位。

在写操作期间，stin将由SSD2828在sck的上升沿采样。csx下降沿之后sck的第一个上升沿采样sdcx位。8位数据的第二个上升沿采样位（cid:）。sck的第三个上升沿采样8位数据的第6位，以此类推。请看下面的图表来说明。可选地，csx可以被驱动到两个周期之间。

![](images/285fcb0899406b8af410288d4cd90e3ce9cf728719cdc0fcc76369e5ddced849.jpg)  
图10-3 8位三线接口写操作示意图

备注：先发送LSB 8bit数据后发送MSB 8bit

在读操作期间，因为没有rwx信号来指示操作是读还是写。在csx被驱动为低电平之后，第一个周期总是一个命令写周期，它指定要访问的寄存器。第二个周期仍然是命令写周期。如果此周期中的命令与寄存器LRR中的命令匹配，则SPI接口进入读模式。接下来的周期将是读周期。如果该命令不匹配，SPI接口将保持写模式。

进入读模式后，返回数据在sout上提供，在sck的下降沿上。应用程序处理器应该使用sck的上升沿对数据进行采样。请注意，没有sdcx位可以从SSD2828读出。因此，每个读周期由8位而不是9位组成。这是读周期和写周期之间的区别。请看下面的图表来说明。可选地，csx可以被驱动到两个周期之间。

SCK   
SDI   
Sdo d7 d6 d5 d4 d3 d2 d1 d d d d d d d d d d3 d2 d1 d0
CSX0 Command：命令返回数据返回数据写周期写周期读周期读周期写实际写特殊地址读命令，进入读模式

![](images/9d4762c5f9850802c4127f3ad2e2c1a9a6b29275e3ddbe40b4b73f6ac25785b4.jpg)  
图10-4:8位3线接口读操作示意图

# 10.2.1用于存储MIPI回读数据的0xFF寄存器的3或4线8bit SPI回读序列

![](images/12337c22ea49c1c79b3163b005ff7e325218dfbd661ca35b3a6a6ca4d5cb2162.jpg)

![](images/e417e681865010b3074c754615e34a6a65d65c27d2b8fae1ab1376be5c6452e8.jpg)

#读取寄存器FFh （0xXX的第xx字节数据）的序列

![](images/e9cfba5b74926e08f3b404b17a99b985b02c58c4a358748a8c29a2e977b7e277.jpg)

# 10.3 SPI接口24位3线

该接口由sck、sdin、sout和csx组成。它只支持16位数据。每个周期包含16位数据。第一个周期应该是写周期，用来指定要访问的寄存器地址。后面的周期是读或写操作的读或写周期。

csx应该从1到0驱动开始一个循环，从0到结束一个循环。在操作期间，应用处理器可以有多个写或读周期。但是，csx必须在每个周期结束时从0到。

每个周期包含24位数据。在24位数据中，前8位是控制数据，后16位是实际数据。前6位为SSD2828的ID位，必须为01 100。如果这个字段不匹配，循环将不被接收。第7位是sdcx位，与8位3线接口相同。第8位是RW位，表示当前周期是读还是写。当RW为1时，该周期为读周期。当RW为0时，该周期为一个写周期。

在写操作期间，stin将由SSD2828在sck的上升沿采样。请看下面的图表来说明。以将数据0x1264写入注册地址0x28为例。

![](images/c8ca205f3aa1fa1f85cf6933c136e7df7a4387f6c7941415483884367af3b3bc.jpg)  
图10-5:24位三线接口写操作示意图

备注：先发送MSB 8bit数据后发送LSB 8bit数据

在读操作期间，前8位仍然由应用处理器写入，以指定后面的16位是用于命令还是用于数据。之后，SSD2828将提供sout上的返回数据，在sck的下降沿上。应用处理器应该使用sck的上升沿进行采样。请看下面的图表来说明。

![](images/c552dfd85f677e2f22ac2580d7d9717dac1701728736322649965ef7e133f70c.jpg)  
图10-6:24位三线接口读操作示意图

![](images/c39fd9a30cf857d71c037ae9e3517fdbfd6e0d00b30b3d38c4df9b146b0fe613.jpg)

10.3.1用于存储MIPI回读数据的0xFF寄存器的3线24bit SPI回读序列

![](images/00c17d2367120dd937c4e65950e295b7309c54d9b66b2389270fa17df9d02a44.jpg)

![](images/784c0ec854ee10282f46388c96cfe61c56a3ec74140619707d5bbc1bcbaac7a2.jpg)

表11-1：最大额定值（参考电压$\mathbf {V _ {S S}}$）

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td >价值< / td > < td >单位< / td > < / tr > < tr > < td > MVDD < / td > < td >核心电源< / td > < td > -0.3 - 1.44 < / td > < td > V < / td > < / tr > < tr > < td > VDDIO < / td > < td > I / O电源< / td > < td > -0.3 - 4.0 < / td > < td > V < / td > < / tr > < tr > < td > TsoL < / td > < td >焊接温度时间< / td > < td > 225 40秒马克斯在锡球< / td > < td >℃< / td > < / tr > < tr > < td >测试< / td > < td >存储温度< / td > < td > -40 - 100 < / td > < td >°C < / td > < / tr > < /表> < /身体> < / html >

最大额定值是那些超过可能对设备造成损坏的值。功能操作应限制在电气特性表和引脚说明部分规定的范围内

本设备包含保护输入免受高静电电压或电场损坏的电路；然而，建议采取正常的预防措施，以避免任何高于最大额定电压的电压应用于这种高阻抗电路。未使用的输出必须保持打开状态。这个装置可能对光敏感。在正常操作时，应注意避免将本设备暴露在任何光源下。这个装置没有防辐射。

# 12推荐操作条件

表12-1建议的运行条件

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td > MVDD < / td > < td >数字核心电源< / td > < td > 1.08 < / td > < td > 1.2 < / td > < td > < / td > 1.32 V < td > < / td > < / tr > < tr > < td行宽=“2”> VDDIO < / td > < td行宽=“2”> IO和数字电源< / td > < td > 2.97 < / td > < td > 3.3 < / td > < td > < / td > 3.63 V < td > < / td > < / tr > < tr > < td > 1.62 < / td > < td > 1.8 < / td > < td > < / td > 1.98 V < td > < / td > < / tr > < tr > < td > TA < / td > < td >操作温度< / td > < td > -30 < / td > < td > 25 < / td > < td > 85 < / td > < td >°℃< / td > < / tr > < /表> < /身体> < / html >

# 13直流特性

条件：参考GND $\ mathm {M V D D}的电压= 1。2 \ mathm {V}$ $\ mathm {\Delta V D D I O} = 1。8 \ mathm {V}$帧频$= 6 0 \ mathm {{H z}}$通道数$= 4$显示模式$= 1 0 8 0 \乘以1 9 2 0，$ 8颜色竖条$\ mathm {T _ {A}} = 2 5 ^ {\circ} \ mathm {C}$

表13-1：直流特性

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td >测试条件< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td > HS IMVDD_ < / td > < td行宽=“3”>高速模式< / td > < td行宽=“3”> 1 gbps < / td > < td > < / td > < td > 46.8 < / td > < td > 76.2 < / td >马< td > < / td > < / tr > < tr > < td > (1.8 IvpD101 < / td > < td > < / td > < td > 0.36 < / td > < td > 0.75 < / td >马< td > < / td > < / tr > < tr > < td > (3.3 v) IVDDIO_HS < / td > < td > < / td > < td > 0.79 < / td > < td > 1.64 < / td >马< td > < / td > < / tr > < tr > < td > IMVDD_LP < / td > < td行宽=“3”>低< / td > < td阿宝er模式行宽= " 3 " > 10 mbps的< / td > < td > < / td > < td > 16.00 < / td > < td > 39.90 < / td >马< td > < / td > < / tr > < tr > < td > (1.8 v) IvDDIO_LP < / td > < td > < / td > < td > 0.17 < / td > < td > 0.43 < / td >马< td > < / td > < / tr > < tr > < td > (3.3 v) IvDDIO_LP < / td > < td > < / td > < td > 0.37 < / td > < td > 0.94 < / td >马< td > < / td > < / tr > < tr > < td > _ULPS IMVDD < / td >是< td行宽= 3 > < / td > < td行宽=“3”>πI f翁奇< / td > < td > < / td > < td > 292.4 < / td > < td > 435 < / td > < td > < / td > < / tr > < tr > < td > (1.8 v) IVDDIO_ULPS < / td > < td > < / td > < td > 75.6 < / td > < td > 150 < / td > < td > < / td > < / tr > < tr > < td >就ulp进行(3.3 v)IvDDI0 < / td > < td > < / td > < td > 165.2 < / td > < td > 327.8 < / td > < td >μ一个< / td > < / tr > < tr > < td > (CMOS) VOH < / td > < td >电压< / td > < td > IoH = 2∼-16毫安< / td > < td > VDDIO x 0.8 < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >卷(CMOS) < / td > < td >电压< / td > < td >人工= 2 ~ 16马< / td > < td > < / td > < td > < / td > < td > x 0.15 VDD10 < / td > < td > < / td > < / tr > < tr > < td > VIH (CMOS) < / td > < td >输入高电压(CMOS) < / td > < td > < / td > < td > x0.7 VDDIO < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > (CMOS)维尔< / td > < td >输入低(CMOS)电压< / td > < td > < / td > < td > < / td > < td > < / td > < td > x0.2 VDDIO < / td > < td > < / td > < / tr > < tr > < td > I0z < / td > < td > Tri-ktage utputt < / td > < td > < / td > < td > < / td > < td > + / - 1 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > < / td > < td >输入Leagee < / td > < td > VIN = VDDIO或接地< / td > < td > < / td > < td > + / - 1 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > CIN < / td > < td >输入电容< / td > < td > < / td > < td > < / td > < td > 2.2 < / td > < td > < / td > < td > pF < / td > < / tr > < /表> < /身体> < / html >

表13-2:HS变送器直流特性

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td > VcMTx < / td > < td >海关传输静态模电压< / td > < td > 150 < / td > < td > < / td > < td > 250 < / td > < td > mV < / td > < / tr > < tr > < td > IVoD | < / td > < td >海关传输差分电压< / td > < td > 140 < / td > < td > < / td > < td > 270 < / td > < td > mV < / td > < / tr > < tr > < td > |∆VoD | < / td > < td > HS微分不匹配< / td > < td > < / td > < td > < / td > < td > 10 < / td > < td > mV < / td > < / tr > < tr > < td > VOHHS < / td > < td >商品输出高电压< / td > < td > < / td > < td > < / td > < td > 360 < / td > < td > mV < / td > < / tr > < /表> < /身体> < / html >

表13-3:LP变送器直流特性

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td > VOH < / td > < td > LP等效发电机输出高水平< / td > < td > 1.1 < / td > < td > 1.2 < / td > < td > < / td > 1.3 V < td > < / td > < / tr > < tr > < td > < / td >卷< td > LP等效发电机输出低电平< / td > < td > -50 < / td > < td > < / td > < td > 50 < / td > < td > mV < / td > < / tr > < tr > < td > ZoLP < / td > < td > LP发射机输出阻抗< / td > < td > 110 < / td > < td > < / td > < td > < / td > < td >欧姆< / td > < / tr > < /表> < /身体> < / html >

表13-4:LP接收器DC特性

表身体< html > < > < > < tr > < td >符号< / td > < td >参数< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td > VIH < / td > < td > LP逻辑输入电压< / td > < td > 880 < / td > < td > < / td > < td > < / td > < td > mV < / td > < / tr > < tr > < td > Vn < / td > < td > LP逻辑0输入电压< / td > < td > < / td > < td > < / td > < td > 550 < / td > < td > mV < / td > < / tr > < /表> < /身体> < / html >

# 14交流特性

# NOTE:

锁相环锁定后，除非指定，否则T为锁相环输出时钟的周期。锁相环锁定前，T为锁相环输入参考时钟的周期。参考时钟可以是tx_clk或pclk，具体取决于CSS位。

1 / t = PLL / 2，
例如，当锁相环Off <0xB9 0x0000>时，
TX CLK = 10MHz，
PLL = TX CLK × 2 = 20Mbps
1 / T = 10MHz

2. W是显示的宽度，例如，水平线的像素数。

3. 交流特性规定了输入接口处输入信号的最大速度。然而，串行链路上的数据吞吐量是影响速度的另一个因素。如果用户接收INT信号，将会有自动流量控制。如果用户不接受INT信号，则用户需要确保输出吞吐量大于输入数据速率。

# 14.18 8位4线SPI接口时序

Table 14-1: 8 Bit 4 Wire SPI Interface Timing Characteristics   

<html><body><table><tr><td>Symbol</td><td>Parameter</td><td>Min</td><td>Typ</td><td>Max</td><td>Unit</td></tr><tr><td>tcycle</td><td>Clock Cycle Time</td><td>8T</td><td></td><td></td><td>ns</td></tr><tr><td>fcLK</td><td></td><td></td><td></td><td>1/8T</td><td>M</td></tr><tr><td>tAS</td><td> e</td><td></td><td></td><td></td><td></td></tr><tr><td>tAH</td><td>Register select Hold Time</td><td></td><td></td><td></td><td>ns</td></tr><tr><td>tcss</td><td>Chip Select Setup Time</td><td></td><td></td><td></td><td>ns</td></tr><tr><td>tcSH</td><td>Chip Select Hold Time</td><td></td><td></td><td></td><td>ns</td></tr><tr><td>tDsW</td><td>Write Data Setup Time</td><td></td><td></td><td></td><td>ns</td></tr><tr><td>tDHW</td><td>Write Data Hold Time</td><td>404040</td><td></td><td></td><td>ns</td></tr><tr><td>tACC</td><td>Read Data Access Time</td><td></td><td></td><td>4.4+6T</td><td>ns</td></tr><tr><td>tDHR</td><td>Read Data Hold Time</td><td>1.2+4T</td><td></td><td>4.4+6T</td><td>ns</td></tr><tr><td>tCLKL</td><td>Clock Low Time</td><td>4T</td><td></td><td></td><td>ns</td></tr><tr><td>tCLKH</td><td>Clock High Time</td><td>4T</td><td></td><td></td><td>ns</td></tr><tr><td>tCsWD</td><td>Chip Select Write Delay Time</td><td>8T</td><td></td><td></td><td>ns</td></tr><tr><td>tCSRD</td><td>Chip Select Read Delay Time</td><td>16T</td><td></td><td></td><td>ns</td></tr><tr><td></td><td>Rise time</td><td></td><td></td><td>2</td><td>ns</td></tr><tr><td>F</td><td>Fall time</td><td></td><td></td><td>2</td><td>ns</td></tr></table></body></html>

注：所有的时间都是基于20%到80%的电源电压。

# Write

![](images/921193638fc94557c4a1b37bbb11f62ec280a96113d4aba620a9f0f74dd3b7e1.jpg)  
Figure 14-1: 8 Bit 4 Wire SPI Interface Timing Diagram

<html><body><table><tr><td>SSD2828QN4</td><td>Rev 1.3</td><td>P 154/168</td><td>Mar 2013</td></tr></table></body></html>

8位3线SPI接口计时

表14-2:8位3线SPI接口定时特性

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> Symbol & lt; /戴利的小于戴利> > Parameter & lt; /小敏戴利的小于戴利> > & lt; /戴利的小于戴利> > Typ & lt; /戴利的小于戴利> >马克斯& lt; /戴利的小于戴利> > minutes & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcycle & lt; /戴利的小于戴利> >一年的《时代》Cycle & lt; /戴利的小于戴利> > 8T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> fCLK & lt; /戴利《时代》的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 1/8T & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcss & lt; /戴利的小于戴利> > Seipl谈谈性e ime & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > M & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcSH & lt; /戴利的小于戴利> >《时代》Select芯片坚持& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tDsw & lt; /戴利的小于添加Data Setup戴利> >时代& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> toHW & lt; /添加Data戴利的小于戴利> >《时代》坚持& lt; /戴利的小于戴利> > -4040 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tACC & lt; /戴利的小于戴利> > Read Data系统与时代& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 4.4 + 6T & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tDHR & lt; /戴利的小于戴利> >《时代》Read Data坚持& lt; /戴利的小于戴利> > 120 + 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 4.4 + 6T & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCLKL & lt; /戴利的小于戴利> >《时代》《灯光昏暗的小于戴利& lt; /戴利> > 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCLKH & lt; /戴利的小于戴利> >《高》时代& lt; /戴利的小于戴利> > 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcsWD & lt; /戴利的小于戴利> >《时代》添加Delay Select芯片& lt; /戴利的小于戴利> > 8T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCSRD & lt; /戴利的小于戴利> > Select芯片,如果我不读《时代》Delay & lt; /戴利的小于戴利> > 16T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tr & lt; /戴利《时代》的小于戴利> > Rise酒吧& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tF & lt; /《时代》过戴利的小于戴利> >抵抗& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

注：所有的时间都是基于20%到80%的电源电压。

![](images/eea96eac0f54f5218fd1a2fd0d92bdd33bb073d024cd8421f4754f2bce51819b.jpg)  
图14-2:8位3线SPI接口时序图

#14.3 24位3线SPI接口计时

表14-3：24位3线SPI接口定时特性

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> Symbol & lt; /戴利的小于戴利> > Parameters & lt; /小敏戴利的小于戴利> > & lt; /戴利的小于戴利> > Typ & lt; /戴利的小于戴利> >马克斯& lt; /戴利的小于戴利> >国防军及& lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcycle & lt; /戴利的小于戴利> >一年的《时代》Cycle & lt; /戴利的小于戴利> > 8T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> fcLK & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 1/8T & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tcss & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > M & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCSH & lt; /戴利的小于戴利> >《时代》Select芯片坚持& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tDsW & lt; /戴利的小于添加Data Setup戴利> >时代& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> toHW & lt; /添加Data戴利的小于戴利> >《时代》坚持& lt; /戴利的小于戴利> > 4040 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tACC & lt; /戴利的小于戴利> > Read Data系统与时代& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 4.4 + 6T & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tDHR & lt; /戴利的小于戴利> >《时代》Read Data坚持& lt; /戴利的小于戴利> > 120 + 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 4.4 + 6T & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCLKL & lt; /戴利的小于戴利> >《时代》《灯光昏暗的小于戴利& lt; /戴利> > 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCLKH & lt; /戴利的小于戴利> >《高》时代& lt; /戴利的小于戴利> > 4T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCsWD & lt; /戴利的小于戴利> >《时代》添加Delay Select芯片& lt; /戴利的小于戴利> > 8T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tCSRD & lt; /戴利的小于戴利> > Select芯片,如果我不读《时代》Delay & lt; /戴利的小于戴利> > 16T & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt小于戴利> R & lt; / tr >戴利的小于戴利> >《时代》Rise酒吧& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> tF & lt; /《时代》过戴利的小于戴利> >抵抗& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > 2 & lt; /戴利的小于戴利> > ns & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

注：所有的时间都是基于20%到80%的电源电压。

![](images/4cc848430b24f7c854f46d4731ecd7672956294145a5f73f29007a6fee6a247c.jpg)

![](images/bfd23888b5ff640e0e714d319d7fc22c956a543c1ff03e834e4f23b730aae1ad.jpg)  
图14-3：24位3线SPI接口计时图

# 14.4 RGB Interface Timing

Table 14-4: RGB Interface Timing Characteristics   

<html><body><table><tr><td>Symbol</td><td>Parameters</td><td>Min</td><td>Typ</td><td>Max</td><td>Units</td></tr><tr><td>tpclk</td><td>Period pclk</td><td>16/18/24T</td><td>16/18/24T</td><td></td><td>ns</td></tr><tr><td>tysys</td><td>Vertical Sync Setup Time</td><td>5</td><td></td><td></td><td>ns</td></tr><tr><td>tysyh</td><td>Vertical Sync Hold Time</td><td>(5d:</td><td></td><td></td><td>ns</td></tr><tr><td>thsys</td><td>Horizontal Sync Setup Time</td><td>5</td><td></td><td></td><td>ns</td></tr><tr><td>thsyh</td><td>Horizontal Sync Hold Time</td><td>5</td><td></td><td></td><td>ns</td></tr><tr><td>thy</td><td>Phase difference of Sync Signal Falling Edge</td><td>0</td><td></td><td>W</td><td>tpck</td></tr><tr><td>tcKL</td><td>Low Period pclk</td><td>8/9/12T</td><td>8/9/12T</td><td></td><td>ns</td></tr><tr><td>tcKH</td><td>High Period pclk</td><td>8/9/12T</td><td>8/9/12T</td><td></td><td>ns</td></tr><tr><td>tds</td><td>Data Setup Time</td><td>3.3</td><td></td><td></td><td>ns</td></tr><tr><td>tdh</td><td>Data hold Time</td><td>3.3</td><td></td><td></td><td>ns</td></tr></table></body></html>

# Note:

1. T represents the total bit rate Tmax $= 1$ / (PLL $\mathbf { x }$ number of lane) $\mathrm { T m a x } = 1 ⁄ 4 \mathrm { G } = 2 5 0 \mathrm { p s }$

2. All timings are based on $20 \%$ to $80 \%$ of supply voltage

3. W是水平线上的像素数

4. pclk周期取决于bpp （bit per pixel）设置和视频模式是burst还是non-burst模式。在突发模式下，应遵循Min列中的值。在非突发模式下，应遵循type列中的值。

![](images/c04116d49f31bf6ca02da1fc0f3bd5a2ca25c3a6686f5ace4594305d1a1b887b.jpg)  
图14-4:RGB接口时序图

14.5复位时间

表14-5:RESET定时

<html><body><table><tr><td>Symbol</td><td>Parameters</td><td>Min</td><td>Typ</td><td>Max</td><td>Units</td></tr><tr><td>TRESET</td><td>RESET “Low" Pulse Width</td><td>10</td><td></td><td></td><td>ms</td></tr></table></body></html>

# 14.6 TX_CLK Timing

Table 14-6: TX_CLK Timing Characteristics   

<html><body><table><tr><td>Symbol</td><td>Parameters</td><td>Min</td><td>Typ</td><td>Max</td><td>Units</td></tr><tr><td>fTxCLK</td><td>TX_CLK Frequency</td><td>8</td><td></td><td>30</td><td>MHz</td></tr><tr><td>tr</td><td>Rise Time</td><td></td><td></td><td>10</td><td>ns</td></tr><tr><td>tF</td><td>Fall Time</td><td></td><td></td><td>10</td><td>ns</td></tr></table></body></html>

![](images/4b53ff04589ad96b1ce8dfeda97546cbe568eee523fd5526fc01fe8b04c86700.jpg)  
Figure 14-5: TX_CLK Timing Diagram

# 15上电顺序

![](images/2f17b71c837540cfca1dd30786cc615caec81d354ac92470b2bc9fd85dd8e023.jpg)

# 16断电顺序

![](images/6345e556f897fc1c883f9f11758701a9e1eff8f9d001abc2fdd7a3c671fa3bdd.jpg)

# 17系统睡眠进出的例子

注：以下示例仅供参考，应用必须与AP和驱动IC的特定信息相关，例如等待时间和AP的TX_CLK （_XIN, _XIO）控制

![](images/33a2d3f7b548602f19d916664ac46587a66e9f43b4efc7dc255566652133281e.jpg)

# 18串行链路数据顺序

有许多可能的方式做并行串行转换。SSD2828通过编程两个寄存器位END和CO提供了灵活性。在视频模式下，它们必须分别编程为0和。

下面是通过串行链路接收显示数据的顺序，当END位为，CO位为0时。

对于16位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

表身体< html > < > < > < tr > < td行宽=“2”> MSB < / td > < td colspan =“5”>字节< / td > < td行宽=“2”> LSB < / td > < td行宽=“2”> MSB < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> Byte2 < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> MSB < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> Byte3 < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td行宽=“2”> MSB < / td > < td colspan =“2”行宽= " 2 " > < / td > < td行宽=“2”> Byte4 < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td行宽= " 2 " > < / td > < / tr > < tr > < td > R3 < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > R4 < / td > < td > R2 < / td > < td > R1 < / td > < td > R0 < / td > < td > G5 < / td > < td > < / td > < td > G4 < / td > < td > G3 G2 < / td > < td > G1 < / td > < td > G0 < / td > < td > B4 < / td > < td > B3 < / td > < td > B2 < / td > < td > B1 < / td > < td > B0 < / td > < td > R4 | R3 < / td > < td > R2 < / td > < td > R1 < / td > < td > R0 < / td > < td > G5 < / td > < td > G4G3 < / td > < td > G2id:) < / td > < td > G1 < / td > < td > G0 < / td > < td > B4B3 < / td > < td > B2 < / td > < td > B1 < / td > < td > < / td > < td > B0 < / td > < / tr > < /表> < /身体> < / html >

对于18位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> MSB & lt; /戴利的小于戴利colspan =“6”> >字节& lt; /戴利的小于戴利rowspan =“2”> > LSB & lt; /戴利的小于戴利rowspan =“2”> > MSB Byte2 & lt; /戴利的小于戴利colspan =“8”> > & lt; /戴利的小于戴利colspan =“6”> > MSB & lt; /戴利的小于戴利colspan =“10”> > LSB MSB & lt; /戴利的小于戴利>rowspan =“2”> LSB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > LSB & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > Byte3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > Byte4 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> R5 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G5 & lt; /戴利的小于戴利> > G4 & lt; /戴利的小于戴利> > G3 & lt; /戴利的小于戴利> > G2G1钠& lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > B5 & lt; /戴利的小于戴利> > B4 & lt; /戴利的小于戴利> > B3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > B2 & lt; /戴利的小于戴利> > B1 & lt; /戴利的小于戴利> > B0 R5 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R4R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利的小于戴利> > G5 & lt; /戴利的小于戴利> > G4 & lt; /戴利的小于戴利> > G3 & lt; /戴利的小于戴利> > G2d & lt; /戴利的小于戴利> > G1钠& lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > B5 & lt; /戴利的小于戴利> > B4 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

对于每像素24位的数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> MSB & lt; /戴利的小于戴利colspan =“6”> >字节& lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“8”> > MSB Byte2 & lt; /戴利的小于戴利colspan =“6”> > MSB & lt; /戴利的小于戴利colspan =“6”> > LSB MSB & lt; /戴利的小于戴利colspan =“7”> > & lt; /戴利的小于戴利rowspan =“2”> > LSB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> R7 & lt; /戴利的小于戴利> > R6 & lt; /戴利的小于戴利> > R5 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R0 & lt; /戴利> & lt;戴利G7 & lt; /戴利的小于戴利> > > G6G5 & lt; /戴利的小于戴利> > G4 & lt; /戴利的小于戴利> > G3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G2 & lt; /戴利的小于戴利> > G1钠& lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > B7 & lt; /戴利的小于戴利> > B6 & lt; /戴利的小于戴利> > B5 & lt; /戴利的小于戴利> > B4 & lt; /戴利的小于戴利> > B3 & lt; /戴利的小于戴利> > B2 & lt; /戴利的小于戴利> > B1 & lt; /戴利的小于戴利> > B0 & lt; /戴利的小于戴利> > R7 & lt; /戴利的小于戴利> > R6 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R5 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

下面是通过串行链接发送显示数据的顺序，当结束位为0且$\ mathbf{c}$bit为0时。

对于16位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

表身体< html > < > < > < tr > < td > MSB < / td > < td colspan =“6”>字节< / td > < td colspan =“2”> LSB < / td > < td colspan =“2”> MSB Byte2 < / td > < td colspan =“6”> LSB < / td > < td colspan =“6”> MSB < / td > < td colspan =“2”> LSB MSB < / td > < td colspan =“6”> Byte4 < / td > < / tr > < tr > < td > G2 < / td > < td > G1 < / td > < td > G0 < / td > < td > B4 < / td > < td > B3 < / td > < td > B2 < / td > < td > B1 < / td > < td > B0 < / td > < td > R4 < / td > < td > R3 td > < / R2 < td > < / td > < td > R1 < / td > < td > R0 < / td > < td > G5 < / td > < td > G4 < / td > < td > G3 < / td > < td > G2 < / td > < td > G1 < / td > < td > G0 < / td > < td > B4 < / td > < td > B3 < / td > < td > B2B1 < / td > < td > B0 < / td > < td > R4 < / td > < td > R3 < / td > < td > R2d R1 < / td > < td > R0 < / td > < td > < / td > < td > G5 < / td > < td > G4G3 < / td > < / tr > < /表> < /身体> < / html >

对于18位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

表身体< html > < > < > < tr > < td行宽=“2”> MSB < / td > < td colspan =“5”>字节1 < / td > < td行宽=“2”> LSB < / td > < td行宽=“2”> MSB < / td > < td行宽=“2”> Byte2 < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td行宽=“2”> MSB < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> Byte3 < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td colspan =“2”行宽=“2”> MSB < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> Byte4 < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < td行宽=“2”> LSB < / td > < td行宽= " 2 " > < / td > < td行宽= " 2 " > < / td > < / tr > < tr > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td行宽= " 2 " > < / td > < td > G0 B5 < / td > < td > B4 < / td > < td > B3 < / td > < td > B2 < / td > < td > B1 < / td > < td > < / td > < td > B0 R3 < / td > < td > R2 < / td > < td > R1 < / td > < td > R0 < / td > < td > G5 < / td > < td > G4 < / td > < td > G3 < / td > < td > G2 < / td > < td > B5 < / td > < td > B4 < / td > < td > B3 < / td > < td > B2 < / td > < td > B1 < / td > < td > B0R5 < / td > < td > R4 < / td > < td > R1 < / td > < td > < / td > < td > R0G5G4 < / td > < td > < / td > < td > G3G2 < / td > < td > G1 < / td > < td > < / td > < td > G0 < / td > < / tr > < tr > < td > G1 < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

Time

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 162/168 < / td > < td > 2013年3月< / td > < td >所罗门Systec < / td > < / tr > < /表> < /身体> < / html >

对于每像素24位的数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

<html><body><table><tr><td rowspan="2">MSB</td><td colspan="5">Byte</td><td rowspan="2">LSB</td><td rowspan="2">MSB Byte2</td><td rowspan="2"></td><td colspan="7"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">Byte3</td><td rowspan="2"></td><td rowspan="2">LSB</td><td rowspan="2">MSB</td><td rowspan="2"></td><td rowspan="2">Byte4</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">LSB</td><td rowspan="2"></td></tr><tr><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">B2</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">LSB</td><td rowspan="2"></td><td rowspan="2">MSB</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td></tr><tr><td>B0</td><td>G6</td><td>G3</td><td>G1</td><td>R7</td><td>R5</td><td>R4</td><td></td><td></td></tr><tr><td>B7 B6</td><td>B5</td><td>B4</td><td>B3</td><td></td><td>B1</td><td></td><td>G7</td><td></td><td>G5</td><td>G4</td><td></td><td>G2</td><td></td><td>G0</td><td></td><td>R6</td><td></td><td>R3</td><td>R2</td><td>R1</td><td>R0</td><td>R7</td><td>R6d R5</td><td>R4</td><td>R3</td><td>R2</td><td>R1</td><td></td><td>R0</td></tr></table></body></html>

Below is the order to send the display data over the serial link, when the END bit is 1 and CO bit is 1.

对于16位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

<html><body><table><tr><td rowspan="2">MSB</td><td colspan="5">Byte</td><td rowspan="2">LSB</td><td rowspan="2">MSB</td><td rowspan="2"></td><td rowspan="2">Byte2</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">LSB</td><td rowspan="2">MSB</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">Byte3</td><td rowspan="2"></td><td rowspan="2">LSB</td><td colspan="2"></td><td rowspan="2">MSB</td><td rowspan="2">Byte4</td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2"></td><td rowspan="2">LSB</td><td rowspan="2"></td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td></tr><tr><td rowspan="2"></td><td>B3 B2</td><td>B1</td><td>B0</td><td>G5</td><td>G4</td><td>G3</td><td>G2</td><td>G1</td><td>G0</td><td>R4</td><td>R3</td><td>R2</td><td>R1</td><td>R0</td><td>B4 B3</td><td>B2</td><td>B1</td><td>B0|</td><td>G5d</td><td>G4</td><td>G2</td><td>G1</td><td>G0</td><td>R3</td><td></td><td>R1</td><td></td><td></td></tr><tr><td>B4</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>G3</td><td></td><td></td><td>R4</td><td></td><td>R2</td><td></td><td>R0</td></tr></table></body></html>

对于18位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

<html><body><table><tr><td>MSB</td><td colspan="6">Byte</td><td colspan="2">LSB</td><td colspan="8">MSB Byte2</td><td colspan="6">MSB</td><td colspan="6">LSB MSB</td><td colspan="7"></td><td rowspan="2">LSB</td></tr><tr><td>B5</td><td>B4</td><td>B3</td><td>B2</td><td>B1</td><td>B0</td><td>G5</td><td></td><td>G4G3</td><td>G2</td><td>G1</td><td>G0</td><td>R5</td><td>R4</td><td>R3</td><td></td><td>R2</td><td>R1</td><td>R0id</td><td>B5</td><td>B4</td><td>B3</td><td>B2</td><td>B1</td><td>B0</td><td>G5</td><td>G4</td><td>G3</td><td>G2</td><td>G1</td><td></td><td>G0</td><td>R5</td><td>R4</td></tr></table></body></html>

对于每像素24位的数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

<html><body><table><tr><td>MSB</td><td colspan="6">Byte</td><td colspan="2">LSB</td><td colspan="7">MSB Byte2</td><td colspan="7">LSB MSB</td><td colspan="6">LSB MSB</td><td></td><td></td><td></td><td></td><td>LSB</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>R1</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td>R0</td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td><td></td></table></body></html>

Below is the order to send the display data over the serial link, when the END bit is 0 and $\mathbf { C O }$ bit is 1.

对于16位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

Time   

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> MSB & lt; /戴利的小于戴利colspan =“6”> >字节& lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“2”> > MSB Byte2 & lt; /戴利的小于戴利colspan =“2”> > & lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“2”> > MSB & lt; /戴利的小于戴利colspan =“2”> > Byte3 & lt; /戴利的小于戴利colspan =“2”> > & lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“2”> > MSB Byte4 & lt; /戴利的小于戴利colspan =“4”> > & lt; /戴利的小于戴利>colspan =“2”> LSB & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> G2 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利的小于戴利> > B4 & lt; /戴利的小于戴利> > B3 & lt; /戴利的小于戴利> > B2 & lt; /戴利的小于戴利> > B1 & lt; /戴利的小于戴利> > B0 & lt; /戴利的小于戴利> > G5 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> & lt; /戴利的小于戴利> > G1钠& lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G4 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G2G1钠& lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利的小于戴利> > B4 B3 & lt; /戴利的小于戴利> > B2 & lt; /戴利的小于戴利> > B1 & lt; /戴利的小于戴利> > B0 & lt; /戴利的小于戴利> > G5 & lt; /戴利的小于戴利> > G4G3 & lt; /戴利的小于戴利> > & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

对于18位每像素数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利>字节MSB & lt; /戴利的小于戴利> > LSB & lt; /戴利的小于戴利> > MSB & lt; /戴利的小于戴利> > Byte2 & lt; /戴利的小于戴利> > LSB & lt; /戴利的小于戴利> > MSB & lt; /戴利的小于戴利> > Byte3 & lt; /戴利的小于戴利> > LSB & lt; /戴利的小于戴利> > MSB Byte4 & lt; /戴利的小于戴利> > LSB & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> SSD2828QN4 & lt; /戴利的小于戴利> > Rev 130 & lt; /戴利的小于戴利> > P 163/168 & lt; /戴利的小于戴利> >名词& lt; / 2013是戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

对于每像素24位的数据，以下是字节顺序。每个字节的数据是按照LSB第一和MSB最后的顺序发送的。

![](images/2dcf724cd1b082d955347531209d6803426d7601479aa119fc908b44fdc9c78c.jpg)  
Time

小于桌子写成的小于医学上的小于html > > > & lt tr > & lt;戴利> MSB & lt; /戴利的小于戴利colspan =“6”> >字节& lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“8”> > MSB Byte2 & lt; /戴利的小于戴利colspan =“6”> > MSB & lt; /戴利的小于戴利colspan =“2”> > LSB & lt; /戴利的小于戴利colspan =“2”> > MSB & lt; /戴利的小于戴利colspan =“6”> > Byte4 & lt; /戴利> & lt; / tr > & lt tr > & lt;戴利> R7 & lt; /戴利的小于戴利> > R6 & lt; /戴利的小于戴利> > R5 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > R0 & lt; /戴利> & lt;戴利G7 & lt; /戴利的小于戴利> > > G6 & lt; /戴利的小于戴利> > G5 G4 & lt; /戴利的小于戴利> > G3 & lt; /戴利的小于戴利> > & lt; /戴利的小于戴利> > G2 & lt; /戴利的小于戴利> > G1钠& lt; /戴利的小于戴利> > G0 & lt; /戴利的小于戴利> > B7 & lt; /戴利的小于戴利> > B6 & lt; /戴利的小于戴利> > B5 & lt; /戴利的小于戴利> > B4 & lt; /戴利的小于戴利> > B3 & lt; /戴利的小于戴利> > B2B1 & lt; /戴利的小于戴利> > B0d & lt; /戴利的小于戴利> > R7 & lt; /戴利的小于戴利> > R6id & lt; /戴利的小于戴利> > R5 & lt; /戴利的小于戴利> > R4 & lt; /戴利的小于戴利> > R3 & lt; /戴利的小于戴利> >,R2 & lt; /戴利的小于戴利> > R1 & lt; /戴利的小于戴利> > R0 & lt; /戴利> & lt; / tr > & lt; /桌子写成> & lt; /医学上的小于/ html > >

#19.1用于SSD2828QN4的维度

![](images/cdfefcdaee82b15122590c8e14025c623de4bc3fd459473402667e2f42a32924.jpg)  
图19-1-包装资料

![](images/1538c30dd57d359aa1519ca506165ddc4299df1ec409b30cbbfc2bee0b12b874.jpg)  
图19-2标记信息

![](images/7deba170d1413a71f90c40676a9ee691e96478e602f5922af245c4f7fafba6b8.jpg)  
图19-3-托盘信息

索罗门系统保留对任何产品作出更改而不另行通知的权利。Solomon Systech对其产品是否适合任何特定用途不作任何保证、陈述或保证，也不承担因应用或使用任何产品或电路而产生的任何责任，并明确拒绝承担任何责任，包括但不限于后果性或附带性损害。“典型”参数在不同的应用程序中可以而且确实有所不同。所有的操作参数，包括“典型”，必须由客户的技术专家针对每个客户的应用进行验证。Solomon Systech不根据其专利权或他人的权利转让任何许可。Solomon Systech产品的设计、用途或授权不用于外科植入人体系统的组件，或用于支持或维持生命的其他应用，或用于Solomon Systech产品故障可能造成人身伤害或死亡的任何其他应用。如果买方购买或使用Solomon Systech产品用于任何此类意外或未经授权的应用，买方应赔偿并使Solomon Systech及其办事处、员工、子公司、附属公司和分销商免受因此类意外或未经授权的使用而直接或间接产生的任何人身伤害或死亡索赔、成本、损害赔偿和费用以及合理的律师费的损害。即使该索赔声称Solomon Systech在零件的设计或制造方面存在疏忽。

本数据表中列出的产品符合欧洲议会和理事会2004年1月27日关于限制在电气和电子设备中使用某些有害物质的指令2002/95/EC和中华人民共和国电子工业标准SJ/T 11363-2006《电子信息产品中某些有害物质的浓度限值要求》。有害物质检测报告可根据要求提供。

表身体< html > < > < > < tr > < td > SSD2828QN4 < / td > < td >转速1.3 < / td > < td > P 168/168 < / td > < td > 2013年3月所罗门Systech < / td > < / tr > < /表> < /身体> < / html >