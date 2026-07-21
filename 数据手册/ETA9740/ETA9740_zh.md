# 3A 开关充电器、2.4A 升压及电量计，单电感 ESOP8 封装

# 描述

# 特性

ETA9740 是一款开关型锂离子电池充电器，可向电池提供高达 3A 的充电电流，在升压模式下可提供高达 5V/2.4A 的输出，充电模式和升压模式均具有高效率。它还包含一个电量计系统用于电量指示。充电方面，它采用专有控制方案，消除了传统恒流控制中的电流检测电阻，从而最大化效率、缩短充电时间并降低成本。它还能从电池升压反向输出 5V 电压。凭借专有的自动模式检测和切换方案，只需一个电感即可实现双向功率传输。ETA9740 是电池充放电应用的理想一体化解决方案，适用于移动电源、智能手机和平板电脑等仅用一个 USB 端口即可实现电池充电功能的产品。

ETA9740 适用于 4.2V 锂离子电池充电。采用 ESOP8 封装。

双向单电感功率转换 · 自动模式切换 \* 开关充电器 / 5V 同步升压，效率高达 96% \* 最大 3A 充电电流和 2.4A 放电 \* 无电池检测 \* 无外部检测电阻 \* 4 段 LED 电量计

# 应用

平板电脑、MID、智能手机、移动电源

# 典型应用

![](images/b2a99c3c96da59298c5003bdaddbd25f22de18ad9f06f9efac079911cfd2e00b.jpg)

订购信息：型号 ETA9740E8A，封装 ESOP8，顶标 ETA9740，每卷 4000 片

# 引脚配置

![](images/6a811b0694435066c9a0c8b32ec35c2d173dc4a1dc3a2c3abe6bea75340f78cb.jpg)

# 绝对最大额定值

（注：超过这些限值可能会损坏器件。长时间暴露在绝对最大额定条件下可能会影响器件可靠性。）

OUT、SW 电压 –0.3V ~ 6V
所有其他引脚电压 –0.3V ~ 6V
SW 对地电流：内部限制
工作温度范围 –40°C ~ 85°C
存储温度范围 –55°C ~ 150°C
热阻 θJA / θJC：ESOP8 10 / 50 °C/W
引线温度（焊接，10s）：260°C
ESD HBM（人体模式）：2KV
ESD MM（机器模式）：200V

# 电气特性

VIN = 5V，除非另有说明。典型值在 TA = 25°C 条件下测得。

<html><body><table><tr><td>PARAMETER</td><td>CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNITS</td></tr><tr><td colspan="6">BUCK MODE</td></tr><tr><td>USB Range</td><td></td><td>4.5</td><td></td><td>5.5</td><td>V</td></tr><tr><td>USB UVLO Voltage</td><td>Rising, Hys=500mV</td><td></td><td>4.5</td><td></td><td>V</td></tr><tr><td rowspan="2">USB Operating Current as BUCK</td><td>Switcher Enable, Switching</td><td></td><td>5</td><td></td><td>mA</td></tr><tr><td>Switcher Enable, No Switching</td><td></td><td>800</td><td></td><td>μA</td></tr><tr><td colspan="6">BATTERY CHARGER</td></tr><tr><td>Battery CV Voltage</td><td>IBAT =OmA, default</td><td>4.17</td><td>4.21</td><td>4.25</td><td>V</td></tr><tr><td>Charger Restart Threshold</td><td>From DONE to Fast Charge</td><td></td><td>-160</td><td></td><td>mV</td></tr><tr><td>Battery Pre-Condition Voltage</td><td>VBAT Rising Hys=250mV</td><td></td><td>2.8</td><td></td><td>V</td></tr><tr><td>Pre-Condition Charge Current</td><td></td><td></td><td>200</td><td></td><td>mA</td></tr><tr><td rowspan="2">Fast Charge Current</td><td>Riset=56K</td><td></td><td></td><td></td><td>A</td></tr><tr><td>Riset=91K</td><td></td><td>2</td><td></td><td>A</td></tr><tr><td>Charge Termination Current</td><td></td><td></td><td>200</td><td></td><td>mA</td></tr><tr><td>Charge Termination Blanking time</td><td></td><td></td><td>1</td><td></td><td>S</td></tr><tr><td colspan="6">BOOST MODE</td></tr><tr><td>BATT Ok Threshold</td><td>Rising. HYS=0.4 V</td><td></td><td>3.2</td><td></td><td>v</td></tr><tr><td>Dutput Voltage Range</td><td>lout=0</td><td>5.05</td><td>5.1</td><td>5.15</td><td>V</td></tr><tr><td>Quiescent Current At BATT</td><td>Vbat=3.6V</td><td></td><td>80</td><td></td><td>μA</td></tr><tr><td>Switching Frequency</td><td>VIN<4.3V</td><td>550</td><td>650</td><td>750</td><td>KHz</td></tr><tr><td>Inductor Peak Current Limit</td><td></td><td></td><td>5.0</td><td></td><td>A</td></tr><tr><td>Maximum Duty Cycle</td><td></td><td></td><td>90</td><td></td><td>%</td></tr><tr><td>High side Pmos Rdson</td><td>Isw =500mA</td><td></td><td>75</td><td></td><td>mΩ</td></tr></table></body></html>

<html><body><table><tr><td>PARAMETER</td><td>CONDITIONS</td><td>MIN TYP MAX</td><td></td><td>UNITS</td></tr><tr><td>Low side Nmos Rdson</td><td>Isw =500mA</td><td></td><td></td><td>mΩ</td></tr><tr><td>Short Circuit Hiccup Current</td><td></td><td>3.</td><td></td><td>A</td></tr><tr><td rowspan="2">Short Circuit Hiccup Timer</td><td>On Time</td><td>45</td><td></td><td rowspan="2">mS</td></tr><tr><td>Off Time</td><td>2000</td><td></td></tr><tr><td>Charging Thermal Regulation threshold</td><td></td><td>85</td><td></td><td>℃</td></tr><tr><td>Thermal Shutdown</td><td>Hys=20°℃ Rising.</td><td>150</td><td></td><td></td></tr></table></body></html>

# 引脚描述

<html><body><table><tr><td>PIN #</td><td>NAME</td><td>DESCRIPTION</td></tr><tr><td></td><td>SW</td><td>Inductor Connection. Connect an inductor Between SW and the regulator output</td></tr><tr><td></td><td>LED1</td><td>Fuel gauge LEDI, LED2 connection pin</td></tr><tr><td></td><td>LED2</td><td>Fuel gauge LED3, LED4 connection pin</td></tr><tr><td>2345</td><td>LED3</td><td>Fuel gauge LED1, LED2, LED3, LED4 connection pin</td></tr><tr><td></td><td>ISET</td><td>Buck Charging current setting pin. Connect a resistor between this pin and analog to set the current level. ground</td></tr><tr><td>F7</td><td>BAT</td><td>Battery pin. Connect a Battery to this pin, and with a bypass capacitor 1Ouf.</td></tr><tr><td></td><td>OUT</td><td>Dutput pin. Bypass with a 22uf or larger ceramic capacitr closely between this pin and GND</td></tr><tr><td></td><td>PGND</td><td>Ground Pin Power</td></tr><tr><td>9 / Exposed Pad</td><td>AGND</td><td>Analog Ground Pin</td></tr></table></body></html>

# 典型特性

（VIN = 5V，TA = 25°C，除非另有说明）

## 充电模式：2.1A 和 3A 充电电流下效率 vs 电池电压

![](images/edd648da947baf65389175c361f91663135ec159ffe53c0a4f642c2bc2bb6d5f.jpg)

![](images/f6b390d395221f76a705982d50be86a676bcbb3943c47d80f4faeed26f78f16b.jpg)

## 升压模式

![](images/8ee79a4981a107b719aa163e45ee57fe1655182e90a9f922caf6e6409582ce0b.jpg)

# 应用支持

请联系当地经销商或 ETA 销售代表获取技术支持。

# PCB 设计指南

![](images/f72cfa8ad3c1d2bfb12fbf1532b23935fadcaf311599faf64344eb34515861dc.jpg)

请将 CIN、COUT 和 L 放置紧靠 IC 引脚，使电源走线尽可能短，以获得 ETA9740 的最佳性能和良好的 EMI。

# 封装外形

封装：ESOP-8

![](images/fbe0c013a0cadb35e8f089b5ceb315dc4fc0e30398c4e399b4b237de9880ad72.jpg)

AA 不包含模具溢料或突出物。
