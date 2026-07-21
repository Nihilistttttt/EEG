# TPS

# MIE1 W0505BGL VH 5V, 1W，稳压，2.5kVrms隔离DC-DC模块

初步规格可能会更改

#描述

MIE1W0505BGLVH是一种隔离，稳压，DC到DC模块。可支持3V至5.5V输入电压应用。MIE1W0505BGLVH具有优良的负载调节、线路调节，支持高达1W的输出功率。

MIE1W0505BGLVH将功率MOS、变压器和反馈电路集成在一块芯片上，性能优异，体积小。

MIE1W0505BGLVH支持稳压输出，当Vout下降并低于目标电压时，IC开始切换，将功率从Vin输出到Vout，直到Vout再次达到目标输出。

MIE1W0505BGLVH集成输出电压反馈模块，无需传统的光耦合器和TL431，即可调节输出电压。与传统的隔离电源模块相比，该模块具有体积小、运行可靠性高的特点。

MIE1W0505BGLVH具有连续短路保护和过温保护。它采用小型LGA-12 （4mmx5mm）封装。

# FEATURES

输入电压工作范围3V至5.5V
可选5V或3.3V输出电压VOUT 5V ~ 5V:≥200mA可用负载电流5V ~ 3.3V:≥200mA可用负载电流3.3V ~ 3.3V:≥75mA可用负载电流
2.5kVrms隔离电压
支持无限容性负载
0美元。4 \%$负载调节
0美元。3 \%$线路调节
连续短路保护
过热保护
CB认证符合IEC62368-1
(Ongoing)   
符合EN55032 B级排放要求
操作温度：$\scriptscriptstyle - 4 0 ^ {\circ} \mathsf {C}$到$1 2 5 ^ {\circ} \mathsf {C}$
LGA-12 （4mmx5mm）封装

#应用程序

工业自动化系统数字隔离器隔离偏置电源隔离RS485/RS422/CAN接口隔离电源隔离传感器电源电信网络设备（5G RRU，工业CPE，网络网关等）

#典型应用

![](images/843a779ffcf8d40b1efd2e89bb77d41541edf50229abcca72f0bf8e84e86972c.jpg)

![](images/f684a27d400cd416fcf18dbbe06d59e83b468256d9701e5fc7db96663f86cfe5.jpg)

#初步规格可能会有变化

#订购信息

表身体< html > < > < > < tr > < td >零件号码* < / td > < td >包< / td > < td >顶部标记< / td > < td >实验室评级< / td > < / tr > < tr > < td > MIE1W0505BGLVH-3R < / td > < td > LGA-12 (4 mmx5mm) < / td > < td >见下文< / td > < td > 3 < / td > < / tr > < /表> < /身体> < / html >

对于磁带和卷轴，添加后缀-Z（例如MIE1W0505BGLVH-3R-Z）。

# top标记

[au:] [au：

MPS: MPS前缀Y：年代码WW：周代码1W0505：部件号LLLLLL：批号BH：部件号后缀

#包引用

![](images/47337189ca91f26990a04ccf34a8401a5e0e791fba839b49433e446a38431822.jpg)

# pin函数

表身体< html > < > < > < tr > < td > LGA-12 (4 mmx5mm)销# < / td > < td >名称< / td > < td >描述< / td > < / tr > < tr > < td > 1, 11、12 < / td > < td > GND1 < / td > < td > 1地面销。< / td > < / tr > < tr > < td > 2、3、4 < / td > < td > GND2 < / td > < td > 2地面销。< / td > < / tr > < tr > < td > 5、6 < / td > < td >输出电压< / td > < td >功率输出。通常在VOUT和GND2 （pin3和pin4）之间连接22uF + 0.1μF，以降低VOUT纹波和噪声。< / td > < / tr > < tr > < td > < / td > < td > VSEL < / td > < td >输出电压设置销。5V输出必须接VOUT或float， 3.3V输出必须接GND2。请勿将VSEL与其他电源偏置，启动后5V输出不能切换到3.3V输出。< / td > < / tr > < tr > < td > 8 < / td > < td >在< / td > < td >低禁用浮动。< / td > < / tr > < tr > < td > 9, 10 < / td > < td > VIN < / td > < td > < / td > < / tr > < /表> < /身体> < / html >

#绝对最大评级(1)

VIN/EN到GND1 ..$\mathsf{- 0}。3 \mathsf {V}$到$+ 65 \mathsf {V}$ VOUT/VSEL到GND2。-0.3V至$+ 6。5 \v $连续功耗$(T _ {\mathsf {A}} = + 2 5 ^ {\circ} \mathsf {C}$)(2)(4) 1.78W结温$ 1.5 0 ^ {\circ} \mathsf {C}$引线温度$2 6 0 ^ {\circ} \mathsf {C}$存储温度。$\mathsf {- 6 5 ^ {\circ} C}$到$+ 1 5 0 ^ {\circ} \mathsf C$

# ESD等级

人体模型（HBM）±5000V充电器件模型（CDM）±2000V

#推荐操作条件(3)

电源电压$V _ {\mathsf {I N}}$ 3V至5.5V输出电压VOUT . ...5/3.3V工作结温度$(\mathsf {T} _ {\mathsf {J}})$ $\scriptscriptstyle - 4 0 ^ {\circ} \mathsf {C}$到$+ 1 2 5 ^ {\circ} \mathsf {C}$

#热阻θJA θJC

LGA-12 (4mmx5mm) EV1W0505B-LVH-00A(4) 70 ..22 .……°c / w jesd51-7 (5) 61 .....19 .……°c / w

# Notes:

超过这些额定值可能会损坏设备。最大允许功耗是最大结温$\mathsf { T } _ { \mathsf { J } }$ （MAX）、结对环境热阻$\mathsf { \theta } _ { \mathsf { J A } }$和环境温度$\mathsf { T } _ { \mathsf { A } }$的函数。任意环境温度下允许的最大连续功耗由$\mathsf { P } _ { \mathsf { D } }$$\mathsf { \Lambda } _ { \mathsf { D } } \ \left( \mathsf { M A X } \right) \ = \ \left( \mathsf { T } _ { \mathsf { J } } \right.$$( M A X ) \cdot T _ { A } )$ (id) $/ \theta _ { \mathsf { J A } }$计算。超过最大允许功耗会产生过高的模具温度，导致调节器进入热关闭状态。内部热关闭电路保护设备免受永久性损坏。
3)不保证设备在其工作条件之外的功能。
4)在EV1W0505B-LVH-00A (51mmx51mm), 1oz， 2层PCB上测量。
5)本表中给出的$\mathsf { \theta } _ { \mathsf { J A } }$值仅用于与其他包的比较，不能用于设计目的。这些数值是根据JESD51-7计算的，并在指定的JEDEC板上进行了模拟。它们不代表在实际应用程序中获得的性能。

#电气特性

V $ \ mathsf {} _ {\ mathsf{我N}} = 5 \ mathsf {V} $, $ \ mathsf {V o u r} = 5 \ mathsf {V} $, $ T _ {\ mathsf {J}} = - 4 0 ^{\保监会}{\ mathsf {C}} $ $ \下午1 - 2 5 ^{\保监会}加元(6),测试典型值T美元_ {\ mathsf {J}} = 2 5 ^{\保监会}{\ mathsf {C}} $,除非另有注明。

表身体< html > < > < > < tr > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td >参数< / td > < td >符号< / td > < td >条件< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td >电源< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < / tr > < tr > < td > Vin挖洞者锁定电压上升< / td > < td > VINUVLO-R < / td > < td >文< / td上升> < td > < / td > < td > 2.6 < / td > < td > 2.8 < / td > < td > V < / td > < / tr > < tr > < td > < / td > < td > VINHYS < / td > < td > < / td > < td > < / td > < td > 220 < / td > < td > < / td > < td > mV < / td > < / tr > < tr > < td >关闭当前< / td > < td > ISD < / td > < td > VEN = 0 V,衡量VIN销< / td > < td > < / td > < td > 7 < / td > < td > < / td > < td >μ一个< / td > < / tr > < tr > < td行宽=“2”>输入电流< / td > < td行宽= " 2 " > < / td >时候< td >加载OA < / td > < td > < / td > < td > 8 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td >负载0.2 < / td > < td > < / td > < td > 395 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td >在输入高阈值< / td > < td > < / td > < td > < / td > < td > < / td > < td > < / td > < td > 2 < / td > < td > V < / td > < / tr > < tr > < td >在低输入电压阈值< / td > < td > < / td > < td > < / td > < td > 0.4 < / td > < td > < / td > < td > < / td > < td > V < / td > < / tr > < tr > < td >输入电流泄漏< / td > < td > < / td > < td >在连接到GND1 Tj = 25°C, Vin = 4.5 V至5.5 V, l0 = 0, < / td > < td > < / td > < td > 5 5 < / td > < td > 5.075 < / td > < td >μ一个< / td > < / tr > < tr > < td >输出电压精度< / td > < td > Vo_ _ACC < / td > < td >有限公司= 22 uf Tj = -40°C + 125°C, Vin = 4.5 V至5.5 V, l0 = 0,有限公司= 22μF < / td > < td > 4.925 - 4.9 < / td > < td > 5 < / td > < td > 5.1 < / td > < td > V V < / td > < / tr > < tr > < td >负载调整率< / td > < td > < / td > < td >负荷= 0到0.2一个< / td > < td > < / td > < td > 0.4 < / td > < td > 2.5 < / td > < td > % < / td > < / tr > < tr > < td >行监管< / td > < td > < / td > < td > Vin = 4.5 V至5.5 V,负荷= 0.2 < / td > < td > < / td > < td > 0.3 < / td > < td > 2.5 < / td > < td > % < / td > < / tr > < tr > < td >效率< / td > < td > < / td > < td >加载= 0.2 < / td > < td > < / td > < td > 50.5 < / td > < td > < / td > < td > % < / td > < / tr > < tr > < td >涟漪< / td > < td > < / td > < td > TA = 25°C < / td > < td > < / td > < td > 60 < / td > < td > 100 < / td > < td > mV < / td > < / tr > < tr > < td行宽=“2”>隔离电压< / td > < td行宽=“2”> Viso < / td > < td >短主要销和所有二次销两个终端部分,测试< / td > < td > 2.5 < / td > < td > < / td > < td > < / td > < td > kVrms < / td > < / tr > < tr > < td >销两个终端部分,测试时间= 1,测试中,100%的测试生产< / td > < td > 3 < / td > < td > < / td > < td > < / td > < td > kVrms < / td > < / tr > < tr > < td > < / td > < td > C-0 < / td > < td > = 1 mhz频率< / td > < td > < / td > < td > 5 < / td > < td > < / td > < td > pF < / td > < / tr > < tr > < td > < / td > < td > R1-o < / td > < td > = 500伏直流电压< / td > < td > 50 < / td > < td > < / td > < td > < / td > < td > GΩ< / td > < / tr > < tr > < td colspan =“7”>热关机(7)< / td > < / tr > < tr > < td >热关闭温度< / td > < td > TsD中< / td > < td > < / td > < td > < / td > < td > 150 < / td > < td > < / td > < td > ' C < / td > < / tr > < tr > < td >热关闭磁滞< / td > < td > TSD-HYS < / td > < td > < / td > < td > < / td > < td > 20 < / td > < td > < / td > < td > C < / td > < / tr > < tr > < td colspan =“6”> < / td > < / tr > < /表> < /身体> < / html >

#电气特性（续）

$\mathsf {V} _ {\mathsf {I N}} = 5 \mathsf {V}$, $\mathsf {V o u r} = 3。3 \mathsf {V}$, $\mathsf {T} _ {\mathsf {J}} = \mathsf {-} \pmb {\ mathm {4 0}} ^ {\circ} \pmb {\ mathm {C}}$至$\yen 123,456$(6)，在$T _ {\mathsf {J}} = 2 5 ^ {\circ} {\mathsf {C}}$处测试典型值，除非另有说明。

表身体< html > < > < > < tr > < td >参数< / td > < td >符号< / td > < td >条件< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td colspan =“7”>电源< / td > < / tr > < tr > < td >关闭当前< / td > < td > ISD < / td > < td > Ven = 0 v,测量在VIN销< / td > < td > < / td > < td > 7 < / td > < td > < / td > < td >μ一个< / td > < / tr > < tr > < td >输入电流< / td > < td > < / td > < td >加载= 0 < / td > < td > < / td > < td > 5 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td > < / td > < td > < / td > < td >加载= 0.2 < / td > < td > < / td > < td > 354 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td >在输入电流泄漏< / td > < td > < / td > < td >在连接到GND1 TJ = 25°C, VIN = 4.5 v至5.5 v,l0 = 0, < / td > < td > < / td > < td > 5 < / td > < td > < / td > < td > < / td > < / tr > < tr > < td行宽=“2”>输出电压精度< / td > < td行宽=“2”> VO_ACC < / td > < td >有限公司= 22μF < / td > < td > 3.2 < / td > < td > 3.3 < / td > < td > < / td > 3.4 V < td > < / td > < / tr > < tr > < td > TJ = -40°C + 125°C, ViN = 4.5 V至5.5 V, l0 = 0,有限公司= 22μF < / td > < td > 3.18 < / td > < td > 3.3 < / td > < td > < / td > 3.42 V < td > < / td > < / tr > < tr > < td >负载调整率< / td > < td > < / td > < td > 0.2负荷= 0 < / td > < td > < / td > < td > 0.4 < / td > < td > 2.5 < / td > < td > % < / td > < / tr > < tr > < td >行监管< / td > < td > < / td > < td > ViN = 4.5 V至5.5 V,负荷= 0.2 < / td > < td > < / td > < td > 0.2 < / td > < td > 2 < / td > < td > % < / td > < / tr > < tr > < td >效率< / td > < td > < / td > < td >加载= 0.2 < / td > < td > < / td > < td > 37 < / td > < td > < / td > < td > % < / td > < / tr > < tr > < td >涟漪< / td > < td > < / td > < td > TA = 25°C < / td > < td > < / td > < td > 50 < / td > < td > 90 < / td > < td > mV < / td > < / tr > < /表> < /身体> < / html >

#电气特性（续）

$V _ {\parallel \parallel} = 3。3 . V$, $\mathsf {V o u r} = 3。3 \mathsf {V}$, ${\sf T} _ {\mathsf {J}} = {\mathsf {-}} 4 0 ^ {\circ} {\mathsf {C}}$到$\pm 1 2 5 ^ {\circ} C$(6)，在$T _ {\mathsf {J}} = 2 5 ^ {\circ} {\mathsf {C}}$处测试典型值，除非另有说明。

表身体< html > < > < > < tr > < td >参数< / td > < td >符号< / td > < td >条件< / td > < td > Min < / td > < td > Typ < / td > < td > Max < / td > < td >单位< / td > < / tr > < tr > < td colspan =“7”>电源< / td > < / tr > < tr > < td >关闭当前< / td > < td > ISD < / td > < td > VEN = 0 v,测量在VIN销< / td > < td > < / td > < td > 5 < / td > < td > < / td > < td > uA < / td > < / tr > < tr > < td >输入电流< / td > < td > < / td > < td >加载= 0 < / td > < td > < / td > < td > 5 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td > < / td > < td > < / td > < td >加载= 0.075 < / td > < td > < / td > < td > 150 < / td > < td > < / td >马< td > < / td > < / tr > < tr > < td >在输入电流泄漏< / td > < td > < / td > < td >在连接到GND1 TJ = 25°C, VIN = 3 v 3.6 v,l0 = 0, < / td > < td > < / td > < td > -3.3 < / td > < td > < / td > < td >μ一个< / td > < / tr > < tr > < td行宽=“2”>输出电压精度< / td > < td行宽=“2”> VO_ACC < / td > < td >有限公司= 22μF < / td > < td > 3.2 < / td > < td > 3.3 < / td > < td > < / td > 3.4 V < td > < / td > < / tr > < tr > < td > TJ = -40°C + 125°C, ViN = 3 V 3.6 V, l0 = 0,有限公司= 22 uf < / td > < td > 3.18 < / td > < td > 3.3 < / td > < td > < / td > 3.42 V < td > < / td > < / tr > < tr > < td >负载调节线监管< / td > < td > < / td > < td > 0.075负荷= 0 < / td > < td > < / td > < td > 0.3 < / td > < td > 2 < / td > < td > % < / td > < / tr > < tr > < td > < / td > < td > < / td > < td > ViN = 3 V 3.6 V,负荷= 0.075 < / td > < td > < / td > < td > 0.2 < / td > < td > 1.5 < / td > < td > % < / td > < / tr > < tr > < td >效率< / td > < td > < / td > < td >加载= 0.075 < / td > < td > < / td > < td > 50 < / td > < td > < / td > < td > % < / td > < / tr > < tr > < td >涟漪< / td > < td > < / td > < td > TA = 25°C < / td > < td > < / td > < td > 30 < / td > < td > 60 < / td > < td > mV < / td > < / tr > < /表> < /身体> < / html >

# Notes:

67)超温相关保证。未在生产中测试。样品特性保证，不进行生产测试。

典型特征$\mathsf {V} _ {\mathsf {I N}} = 5 \mathsf {V}$, $\mathsf {V o u r} = 5 \mathsf {V}$, $\mathsf {C} _ {^ {\mathsf {I N}}} = 10 \mu \mathsf {F}$, $\mathsf {C _ {o u T}} = 0。1 \mu \mathsf {F} + 2 \mu \mathsf {F}$, $T _ {A} = 2 5 ^ {\circ} C$，除非另有说明。

#输出电压精度vs。

#输出电压精度vs。

![](images/6d972c6cbc3afdeac5ee2109883e6dd84d4f9cab6853017a843ffc827f1c8ce2.jpg)

![](images/67f2ca8fee0a3597959361188d1bbc36c80204034731331fc85b3b62b51ceb81.jpg)

#典型性能特点（续）

![](images/325d2a91f146dcd4055542186374f6a34aab32e540da969645ec7fcdcef19950.jpg)

#初步规格可能会有变化

#典型性能特征（续）$\mathsf {V} _ {\mathsf {I N}} = 5 \mathsf {V}$, $\mathsf {V o u r} = 5 \mathsf {V}$, $\mathsf {C _ {o u T}} = 0。1 \mu \mathsf {F} + 2 \mu \mathsf {F}$, $T _ {A} = 2 5 ^ {\circ} C$，除非另有说明。

加载暂态IOUT $= 0 \mathsf {A}$至0.2A

![](images/16d8a22843b88c8a6ea1eba15bc290929cb3331255a5cc27dc67b995f139f900.jpg)  
CH4: IOUT

![](images/9ee93e2827fbccac32447f73f09777e202cd410c563f61c040b01735274cb40e.jpg)

#通过VIN IOUT = 0.2A启动

#通过VIN IOUT = 0.2A关机

CH1: VOUT

![](images/59b74cd38058f615398176b1c4c415bd8e12052f77840570d035d7a434aaba0d.jpg)

CH2: VIN CH4: IOUT

CH1: VOUT

![](images/ccf72e39d123e093a0978a519f74569a83babd0fb642bcad0a4968be8e88c4de.jpg)

CH2: VIN CH4: IOUT

# Start-Up through EN IOUT = 0.2A

#通过EN关闭

Iout $= 0。2 \mathsf {A}$

CH1: VOUT

![](images/cbd1c84d13765a12dedb1901bf427216a03ff8c65949b25781b026b2824579fa.jpg)

CH1: VOUT

![](images/4993af5f78e4b9dbaf99f51d94d1eae3fc86a890c2f4f55013f3fe1cb8dadcee.jpg)

Ch2: vinch3: ench4: iout

Ch2: vin ch3: en ch4: iout

#典型性能特征（续）$\mathsf {V} _ {\mathsf {I N}} = 5 \mathsf {V}$, $\mathsf {V o u r} = 3。3 \mathsf {V}$, $\mathsf {C _ {0 U T}} = 0。1 \mu \mathsf {F} + 2 \mu \mathsf {F}$, $T _ {A} = 2 5 ^ {\circ} C$，除非另有说明。

![](images/298406740ff4110b8806c698b2ede41d6e3599e86f1ea0a0e35820e1cc361391.jpg)

加载暂态IOUT $= 0 \mathsf {A}$至0.2A

CH1:输出电压/ AC

![](images/342d6ad1ad33aa9a8466305ce2340b10e654cacdeaae8c8c6b0e088df5595dd0.jpg)

CH4: IOUT

通过VIN IOUT = 0.2A启动

#通过VIN关闭

CH1: VOUT

![](images/2055e5d21052c5a5d52cdf2305a833c596c6e24fe0b1fe6f6f4556d01b31199d.jpg)

CH2: VIN CH4: IOUT

CH1: VOUT

CH2: VIN CH4: IOUT

![](images/723d700dbe76003b1b6089c1e200d38948e201a10e9246e975d2ebcb70eb0b33.jpg)

#通过IOUT $= 0启动。2 \mathsf {A}$

#通过EN关闭

Iout $= 0。2 \mathsf {A}$

CH1: VOUT

![](images/e5fa151f4dbdcbe93a32a057971dddb36bea9f5c61eafb9b6521a86a8ee51d06.jpg)

CH1: VOUT

![](images/e1b7eff7cc765a86c4710cd05106f8c5b68af80c8f04821005749c7f91308a86.jpg)

Ch2: vinch3: ench4: iout

Ch2: vin ch3: en ch4: iout

#功能框图

![](images/7873db5cf09a475f4b6ad6497022887ac677c11682edd5735896d3f5bd92b155.jpg)  
图1：功能框图

# OPERATION

MIE1W0505BGLVH是一种稳压、隔离的直流到直流模块。它可以支持3V到5.5V的输入电压应用，在$- 4 0 ^ {\circ} \mathsf {C}$到$1 2 5 ^ {\circ} \mathsf {C}$工作温度。具有优良的负载调节、线路调节性能，支持高达1W的输出功率。

#隔离电源转换

MIE1W0505BGLVH将功率MOS、变压器和反馈电路集成在一块芯片上，性能优异，体积小。

当VOUT低于目标输出电压时，IC将开始切换输出功率从$V _ {\mathsf {I N}}$到VOUT。另一方面，如果$\mathsf {V} _ {\mathsf {O U T}}$上升到目标输出电压，则开关将停止。

#输出电压设置

将VSEL引脚连接到VOUT或浮动VSEL引脚，输出电压设置为5V。VOUT可输出200mA负载，输入范围为4.5V至5.5V。

将VSEL引脚连接到GND2，输出电压设置为3.3V。VOUT可以输出200mA的负载，输入范围为4.5V至5.5V或75mA的负载，输入范围为3V至3.6V。VSEL逻辑在启动过程中被锁定。启动后，输出电压固定甚至改变VSEL逻辑。

欠压闭锁保护（UVLO）

MIE1W0505BGLVH具有输入欠压闭锁保护（UVLO），确保可靠的输出功率。当输入电压超过UVLO上升阈值时，MIE1W0505BGLVH上电。当输入电压低于UVLO下降阈值时，设备下电。此功能可防止设备在电压不足时工作。这是一个非闩锁保护。

# 电源 使能 （EN）

EN引脚使能和禁用MIE1W0505BGLVH。当施加高于2V的电压且输入电压高于$\mathsf {V} _ {\mathsf {I N}}$ UVLO时，MIE1W0505BGLVH将使能所有功能并开始开关操作。当EN电压低于其下阈值时，开关操作停止；当$E N < 0时，开关操作停止。4 . v $。自动启动时，EN引脚直接或通过电阻分压器连接到$V _ {\mathsf {I N}}$。

#电源转换器软启动和SCP

为了避免启动过程中的超调和涌流，MIE1W0505BGLVH内置了一个内部软启动（SS），可以逐渐限制输出电流从低到高。

MIE1W0505BGLVH启动时采用恒流（CC）充电模式，在此模式下，电流限制会折回，输出端会对输出电容进行CC充电，直到输出电压上升到2.7V左右。经过CC充电期后，MIE1W0505BGLVH的限流恢复正常，并具有较高的输出电流能力。这些特性保证了无限的容性负载。

在过载或输出短路时，由于内部限流，输出电压下降。当VOUT降到约2.2V以下时，MIE1W0505BGLVH进入CC充电模式。过流或短路情况消除后，MIE1W0505BGLVH在VOUT升至2.7V左右时恢复正常工作。

#超温保护

MIE1W0505BGLVH集成了一个温度监测电路。一旦结温高于$ 1.5 0 ^ {\circ} \ maththrm {C}$， MIE1W0505BGLVH关闭。待温度降至$1 0 ^ {\circ} \mathsf {C}$以下时，电源恢复正常运行。

#应用信息

#输入和输出电容器

为了稳定工作，输入端的VIN和GND1引脚之间以及输出端的VOUT和GND2引脚之间都需要去耦电容。去耦电容器必须尽可能靠近VIN和VOUT引脚。建议加10uF $+ ~ 0。1$ uF陶瓷电容在输入端，加上$ 22 \mathsf {uF} + 0。1$ uF陶瓷电容在输出侧，较大的一个用于使纹波合适，较小的一个用于高频噪声滤波。

# PCB布局指南

PCB板的布局对正常工作非常重要。参考图2和PCB布局指南。

1)出于安全考虑，一次侧和二次侧必须物理分开。爬电/间隙必须满足特定应用的标准。2)最小化VIN、输入电容和GND1之间的环路面积；VOUT，输出电容和GND2，最大限度地减少输出噪声。3)在GND1引脚上放置足够的铜和通孔，以提高IC的热性能；不建议在GND2和VOUT引脚上放置大铜，否则会使EMI更差。VOUT和GND2上的铜越小，EMI性能越好。

4)建议使用四层PCB，以获得良好的EMI性能。因为很容易建立一个低ESL重叠Y-CAP和EMI噪声将被绕过。在GND1和GND2上放置充分和密集的通道，以减少Y-CAP的ESL重叠。参考图2的中间层2和中间层3。

5)建议增加外部Y-CAP选项进行EMI调试，由于ESL较低，SMD封装优于带铅封装。

![](images/2c2b9bf451fce7a5cf78e945a8da08d19ffcb07d02c3cd12dc312ff8fa73c61e.jpg)  
初步规格可能会更改
图2：推荐的PCB布局

#典型应用电路

![](images/0c911fda2acc9490f992bad01340dd4938ca9afc596468e0e0a8f382c000a5b9.jpg)  
图3:5V输出应用电路

![](images/c33b0c5bcd6c1c761db5c7b3d1a31b44ac689b5a483d64e41c0e6fa37c24a853.jpg)  
图4:3.3V输出应用电路

#包信息

# LGA-12 （4mmx5mm）

![](images/b18eb9424231eec365bfa421e950f4e38087699f486ea6a719914497827864bb.jpg)

![](images/3bd882c5c12886aa19275563e99665a613a16a31a2454bb60a2a3246a0e37f47.jpg)

# TOP VIEW

![](images/789d53853399a6ee23b958919cfb2207e5ed28b5e3267d0e45ce5138fafefccf.jpg)

#底部视图

# SIDE VIEW

![](images/0f3ca25501701cbc999617e818e68f5d2114cd22686c6efc43d744c5b1c94178.jpg)

#推荐的土地格局

# NOTE:

所有尺寸均以毫米为单位。
2)导联共面为0.10
毫米马克斯。
3) jedec参考是mo-303。
4)绘图不按比例。

#载波信息

LGA-12 (4 mmx5mm)

![](images/470899469ca8c5b4b3e19fbaa53bc1271739c5728ace2f07f4cb681347be3b8b.jpg)

![](images/c924100f7e3e9abe2604df8f10bd7b151a82b75d67051773c4dc750abe694ef6.jpg)

表身体< html > < > < > < tr > < td >零件号< / td > < td >包描述< / td > < td >数量/卷< / td > < td >数量/管< / td > < td >数量/托盘< / td > < td >卷筒直径< / td > < td > Carrerr宽度< / td > < td > Carerr音高< / td > < / tr > < tr > < td > MIE1W0505BGLVH - 3 r-z < / td > < td > LGA-12 (4 mmx5mm) < / td > < td > 2500 < / td > < td > N / A < / td > < td > N / A < / td > < td > 13。< / td > < td > 12毫米< / td > < td > 8毫米< / td > < / tr > < /表> < /身体> < / html >