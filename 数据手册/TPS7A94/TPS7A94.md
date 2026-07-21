# TPS7A94 1-A, Ultra-Low Noise, Ultra-High PSRR, Low-Dropout Regulator

# 1 Features

Ultra-low output noise: $0 . 4 6 \mu \mathsf { V } _ { \mathsf { R M S } }$ (typ, $1 0 H z$ to $1 0 0 k H z )$

High power-supply ripple rejection (PSRR): 102 dB at 100Hz 110 dB at 1 kHz 95 dB at $1 0 ~ \mathsf { k H z }$ 78 dB at $1 0 0 k H z$ 50 dB at 1 MHz

Accuracy over line, load, and temperature: $1 \%$   
Low dropout: $1 5 0 ~ \mathrm { m V }$ at 1 A   
Wide input voltage range: $1 . 7 \mathrm { V }$ to $5 . 7 \vee$   
Wide output voltage range: $0 \vee$ to $5 . 5 \vee$   
Parallelable for lower noise and higher current   
Fast transient response   
Precision enable and UVLO   
Programmable current limit   
Programmable PG threshold   
Adjustable start-up inrush control   
Open-drain, power-good (PG) output   
Package: $3 . 0 0 \mathrm { - } \mathsf { m m } \times 3 . 0 0 \mathrm { - } \mathsf { m m }$ , 10-pin WSON: JEDEC RθJA: $4 6 . 1 ^ { \circ } \mathsf { C } / \mathsf { W }$ EVM RθJA: $2 5 . 6 ^ { \circ } \mathsf { C } N$

# 3 Description

The TPS7A94 is an ultra-low-noise $( 0 . 4 6 ~ \mu \ V _ { \sf R M S } )$ , lowdropout (LDO) voltage regulator capable of sourcing 1 A with only $1 5 0 ~ \mathsf { m V }$ of dropout. The low dropout, in conjunction with a wide bandwidth error amplifier, allows for very high PSRR (110 dB at $1 \mathsf { k H z }$ and 50 dB at 1 MHz) under low operating headroom $( 5 0 0 ~ \mathrm { m V } )$ and high output current $( 7 5 0 \mathsf { m A } )$ .

The device output is adjustable from $\textsf { 0 } \textsf { V }$ to 5.5 V with an external resistor. With the wide input voltage range, the device supports operation as low as 1.7 V and up to $5 . 7 ~ \lor .$ The device includes programmable current limit, programmable PG threshold, and precision enable, allowing better control in the application.

With the high-accuracy reference and wide-bandwidth topology, the device can be easily paralleled to achieve lower noise and higher current.

With $1 \%$ output voltage accuracy (over line, load, and temperature) and soft-start capabilities to reduce inrush current, the device is an excellent choice for powering sensitive analog low-voltage devices.

# 2 Applications

Macro remote radio units (RRU)   
Outdoor backhaul units   
Active antenna system mMIMO (AAS)   
Ultrasound scanners   
Lab and field instrumentation   
Sensor, imaging, and radar   
(1) For all available packages, see the orderable addendum at the end of the data sheet.   
(2) The package size (length $\times$ width) is a nominal value and includes pins, where applicable.

![](images/46a3ad6be241a563b5d01fbc2176e0ef96733859a5f753781bfeb3e3395d28d3.jpg)  
Ultra-Low Output Noise Independent of Output Voltage $( 1 0 \mathsf { H z } - 1 0 0 \mathsf { k H z } )$

Package Information   

<html><body><table><tr><td>PART NUMBER</td><td>PACKAGE(1)</td><td>PACKAGE SIZE(2)</td></tr><tr><td>TPS7A94</td><td>DSC (WSON, 10)</td><td>3.00 mm × 3.00 mm</td></tr></table></body></html>

![](images/f30fd6f4f55a37023d108bf6dfb1664df3773fd19ad9dbc0f5506be6683ffa7c.jpg)

# Table of Contents

1 Features.   
2 Applications........   
3 Description.   
4 Revision History. ......... 2   
5 Pin Configuration and Functions.   
6 Specifications. 6.1 Absolute Maximum Ratings. 6.2 ESD Ratings.. 4 6.3 Recommended Operating Conditions. 5 6.4 Thermal Information. 5 6.5 Electrical Characteristics. , . ... 6 6.6 Typical Characteristics............................................... 8   
7 Detailed Description. 26 7.1 Overview. .... 26 7.2 Functional Block Diagram. ............... 27 7.3 Feature Description. .28 7.4 Device Functional Modes. . 30

# 8 Application and Implementation. 31

8.1 Application Information. 31   
8.2 Typical Application...... 49   
8.3 Power Supply Recommendations. .51   
8.4 Layout.. 51

# 9 Device and Documentation Support. 53

9.1 Device Support.. 53   
9.2 Documentation Support. 53   
9.3 Receiving Notification of Documentation Updates....53   
9.4 Support Resources. 53   
9.5 Trademarks. 54   
9.6 Electrostatic Discharge Caution. .......................... . 54   
9.7 Glossary. 54

# 10 Mechanical, Packaging, and Orderable

Information. 54   
10.1 Mechanical Data. 55

# 4 Revision History

NOTE: Page numbers for previous revisions may differ from page numbers in the current version.

Changes from Revision B (February 2023) to Revision C (June 2023) Page

Deleted note regarding output current range from Optimizing Noise and PSRR section................................. 40

# Changes from Revision A (May 2022) to Revision B (February 2023) Page

Changed GND Pin Current vs IOUT and Temperature for $V _ { O U T } = 3 . 3 ~ V$ to $100 \%$ Current Limit vs Temperature for $V _ { O U T } = 1 . 8 V$ curves in Typical Characteristics section. .............. ......................... 8 Changed Equation 1, Equation 2, and the discussion of these equations in Precision Enable (External UVLO) section. cid:) …… 」 … 32 Changed parallel impedance value from $1 0 k \Omega$ to $1 2 . 5 k \Omega$ in Power-Good Feedback (FB_PG Pin) and PowerGood Threshold (PG Pin) section. .34 Added Relationship Between Threshold Voltage, Output Voltage, IFAST_SS, and INR/SS During Start-Up figure to Programmable Soft-Start and Noise-Reduction (NR/SS Pin) section.......................................................... 37 Changed discussion of VON and VOFF in Detailed Design Procedure section. 50

# 5 Pin Configuration and Functions

![](images/2c23cf789c1bb91a20990baecadb8a83d0972a467ab1e4c599e5f18a564caabc.jpg)  
Figure 5-1. DSC Package, 10-Pin WSON (Top View)

# Pin Functions

<html><body><table><tr><td colspan="2">PIN</td><td rowspan="2">I/O(1)</td><td rowspan="2">DESCRIPTION</td></tr><tr><td>NAME</td><td>WSON</td></tr><tr><td>EN_UV</td><td>3</td><td>section</td><td>Precision enable and undervoltage lockout pin; see the Precision Enable (External UVLO) for details.</td></tr><tr><td>FB_PG</td><td></td><td></td><td>Power-good feedback pin. This pin has a dual function: this pin programs the PG pin output 100%,</td></tr><tr><td>GND</td><td>6</td><td>G</td><td>Pin) and Power-Good Threshold (PG Pin) section for details. Ground pin; see the Board Layout section for details.</td></tr><tr><td>IN</td><td>1,2</td><td></td><td>r Types section and the Recommended</td></tr><tr><td>NR/SS</td><td>7</td><td></td><td>Output voltage set and noise-reduction pin; see the Programmable Soft-Start and Noise- Reduction (NR/SS Pin) section for details.</td></tr><tr><td>OUT</td><td>9, 10</td><td></td><td>Regulated output pin; see the Load Transient Response section for additional information.</td></tr><tr><td>PG</td><td></td><td></td><td>Power-</td></tr><tr><td>SNS</td><td>8</td><td></td><td>noninverting terminal of the error amplifier; see the</td></tr><tr><td colspan="2">Thermal pad</td><td>G</td><td>The the ral pad is electrically connected to the GND pin; see the Board Layout section for</td></tr></table></body></html>

(1) $\mid \mid =$ input, $0 =$ output, $1 / \mathsf { O } =$ input or output, $\mathsf { G } =$ ground, $\mathsf { P } = \mathsf { p o w e r }$

# 6 Specifications 6.1 Absolute Maximum Ratings

over operating junction temperature range and all voltages with respect to GND(unless otherwise noted)(1)

<html><body><table><tr><td colspan="2"></td><td>MIN MAX</td><td>UNIT</td></tr><tr><td rowspan="4">Voltage</td><td>IN, PG, EN_UV</td><td>−0.3 6.0</td><td rowspan="4"></td></tr><tr><td>FB_PG</td><td>−0.3 1.5</td></tr><tr><td>OUT</td><td>−0.3 + 0.3 VIN</td></tr><tr><td>NR/SS, SNS</td><td>−0.3 6.0</td></tr><tr><td rowspan="2">Current</td><td>OUT</td><td>Internally limited</td><td></td></tr><tr><td>PG (sink current into the device)</td><td>5</td><td>mA</td></tr><tr><td rowspan="2">Temperature</td><td>Operating junction, TJ</td><td>-55 150</td><td></td></tr><tr><td>Storage, Tstg</td><td>-55 150</td><td></td></tr></table></body></html>

(1) Operation outside the Absolute Maximum Ratings may cause permanent device damage. Absolute Maximum Ratings do not imply functional operation of the device at these or any other conditions beyond those listed under Recommended Operating Conditions. If used outside the Recommended Operating Conditions but within the Absolute Maximum Ratings, the device may not be fully functional, and this may affect device reliability, functionality, performance, and shorten the device lifetime.

# 6.2 ESD Ratings

<html><body><table><tr><td colspan="3"></td><td>VALUE</td><td>UNIT</td></tr><tr><td rowspan="2">V(ESD)</td><td rowspan="2">Electrostatic discharge</td><td>Human model (HBM), ANSI/ESDA/JEDEC JS-001(1) body per</td><td>±2000</td><td rowspan="2"></td></tr><tr><td>device model CDM) per per ANSI/ESDA/JEDEC JS-002(2)</td><td>±500</td></tr></table></body></html>

(1) JEDEC document JEP155 states that 500-V HBM allows safe manufacturing with a standard ESD control process.   
(2) JEDEC document JEP157 states that 250-V CDM allows safe manufacturing with a standard ESD control process.

# 6.3 Recommended Operating Conditions

over operating junction temperature range (unless otherwise noted)

<html><body><table><tr><td colspan="2"></td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNIT</td></tr><tr><td>ViN</td><td>Input supply voltage range</td><td>1.7</td><td></td><td>5.7</td><td>V</td></tr><tr><td>VoUT</td><td>Output voltage range</td><td>0.4</td><td></td><td>VDo VIN</td><td>V</td></tr><tr><td>louT</td><td>Output current</td><td>0</td><td></td><td></td><td>A</td></tr><tr><td>CIN</td><td>Input capacitor</td><td>4.7</td><td>10</td><td>1000</td><td>μF</td></tr><tr><td>CoUT</td><td>Output capacitor</td><td>4.7</td><td>10</td><td>1000</td><td>μF</td></tr><tr><td>_ESR CouT</td><td>Output capacitor ESR</td><td></td><td></td><td>20</td><td>mΩ</td></tr><tr><td>_ESL ZOUT_</td><td>Total output loop impedance</td><td></td><td></td><td>2</td><td>nH</td></tr><tr><td>CNR/SS</td><td>Noise-reduction capacitor</td><td></td><td>4.7</td><td>100</td><td>μF</td></tr><tr><td>RPG</td><td>Power-good pull-up resistance</td><td>10</td><td></td><td>100</td><td>kΩ2</td></tr><tr><td>TJ</td><td>Junction temperature</td><td>-40</td><td></td><td>125</td><td>C</td></tr></table></body></html>

# 6.4 Thermal Information

<html><body><table><tr><td rowspan="4">THERMAL METRIC(1)</td><td rowspan="2"></td><td colspan="2">TPS7A94</td><td rowspan="4">UNIT</td></tr><tr><td>DSC (WSON)(2)</td><td>DSC (WSON)(3)</td></tr><tr><td rowspan="2"></td><td>10 PINS</td><td>10 PINS</td></tr><tr><td>46.1</td><td></td><td>C/W</td></tr><tr><td>RθJA</td><td>Junction-to-ambient thermal resistance</td><td></td><td>25.6</td><td></td></tr><tr><td>RθJC(top)</td><td>Junction-to-case (top) thermal resistance</td><td>35.2</td><td></td><td>°C/W</td></tr><tr><td>RθJB</td><td>Junction-to-board thermal resistance</td><td>19.1</td><td></td><td>CW</td></tr><tr><td>4JT</td><td>Junction-to-top characterization parameter</td><td>0.5</td><td>0.3</td><td>CW</td></tr><tr><td>YJB</td><td>Junction-to-board characterization parameter</td><td>19</td><td>11.5</td><td>CW</td></tr><tr><td>ReJC(bot)</td><td>Junction-to-case (bottom) thermal resistance</td><td>3.9</td><td></td><td>C/W</td></tr></table></body></html>

(1) For more information about traditional and new thermal metrics, see the Semiconductor and ICPackage Thermal Metrics application report.   
(2) JEDEC standard. (2s2p)   
(3) EVM thermal model using JEDEC measurement methodology, see TPS7A94EVM-046 thermal analysis.

# 6.5 Electrical Characteristics

over operating temperature range $( \mathsf { T } _ { \mathsf { J } } = - 4 0 ^ { \circ } \mathsf { C }$ to $+ 1 2 5 ^ { \circ } \mathsf C$ ), $\mathsf { V } _ { \sf I N ( N O M ) } = \mathsf { V } _ { \sf O U T ( N O M ) } + 0 . 5 \mathsf { V }$ , $\mathsf { V } _ { \mathsf { O U T } ( \mathsf { N O M } ) } = 3 . 3 \mathsf { V } ,$ $\mathsf { l _ { O U T } } = 1 \mathsf { m A }$ , $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V } _ { : }$ $\mathsf { C } _ { \mathsf { I N } } = \mathsf { C } _ { \mathsf { O U T } } = 1 0 \ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R } / \mathsf { S } \mathsf { S } } = 0$ nF, and PG pin pulled up to $\mathsf { V } _ { \mathsf { I N } }$ with $1 0 0 ~ \mathsf { k } \Omega$ (4) (unless otherwise noted); typical

<html><body><table><tr><td colspan="7">EN CIN COUT NR/SS values are at Tj = 25°C</td></tr><tr><td></td><td>PARAMETER</td><td>TEST CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNIT</td></tr><tr><td>VN</td><td>Input supply voltage range</td><td></td><td></td><td></td><td>5.7</td><td>V</td></tr><tr><td>VUVLO</td><td>Input supply UVLO</td><td>Vin rising, no load</td><td></td><td>1.6</td><td>1.7</td><td>V</td></tr><tr><td>VHYS(UVLO)</td><td>Input supply UVLO hysteresis</td><td>No load</td><td></td><td>53</td><td></td><td>mV</td></tr><tr><td rowspan="3">INR/SS</td><td rowspan="3">NR/SS pin current</td><td>ViN = 1.7 V, louT = 1 mA, VouT = 1.2 V</td><td></td><td>150</td><td></td><td>μA</td></tr><tr><td>1.V≤VN≤5.5V,0.4V≤VOUT<1.2V,1mA≤IOUT</td><td>-1.5</td><td></td><td>1.5</td><td></td></tr><tr><td>1.7 V≤ VIN≤5.5 V, 1.2 V ≤V ≤ 5.1 V, mA V 1A VoUT ≤ louT</td><td></td><td></td><td></td><td>%</td></tr><tr><td>IFAST_SS</td><td>NR/SS fast start-up charging</td><td>GND, ViN ≥ 2.5 V, VFB_PG < 0.2 V, IouT = 0 mA VNR/SS</td><td></td><td>2.1</td><td></td><td>mA</td></tr><tr><td></td><td>current Output voltage range</td><td>= GND, ViN = 1.7 V, < 0.2 V, IoUT 0mA VNR/SS VFB_PG</td><td></td><td>1.5</td><td></td><td></td></tr><tr><td>VouT</td><td>Output ofset volage s</td><td>1.7V≤ViN≤5.7V, 1.2V≤VoUT≤5.1V,</td><td></td><td>±0.1</td><td>5.5 2</td><td></td></tr><tr><td rowspan="2">Vos</td><td rowspan="2">VouT)</td><td>≤1A 1 mA≤ loUT</td><td></td><td></td><td></td><td rowspan="2">mV</td></tr><tr><td>1MA≤IOUTS1V, 4V≤VOUT<12V</td><td></td><td>±0.2</td><td></td></tr><tr><td rowspan="5">(iVOUT(VIN)d)</td><td>Line regulation: ∆INR/SS</td><td>= 1.2VandV louT 1mA, ViN VouT VouT VoUT =3.3 V,</td><td></td><td>-0.9</td><td></td><td>nAV</td></tr><tr><td></td><td>ViN = (VouT + 0.5V) to 5.7V</td><td></td><td></td><td></td><td></td></tr><tr><td rowspan="3">Line regulation: ΔVos</td><td></td><td></td><td>-4.5</td><td></td><td>μVN</td></tr><tr><td>= 1mA,</td><td></td><td>2.1</td><td></td><td></td></tr><tr><td>ViN = 1.7V, VoUT= 1.2V, mA ≤1A ≤louT</td><td></td><td>2.3</td><td></td><td></td></tr><tr><td rowspan="4">(iVOUT(∆IOUT) NR/SS(VNR/SS)</td><td rowspan="2">Load regulation: ∆InR/ss (1)</td><td>ViN = 3.8 V, VouT = 3.3 V, 1 mA ≤ loUT ≤ 1 A</td><td></td><td>-3.6</td><td></td><td>nA</td></tr><tr><td>= 5.1V, mA≤louT ≤1A ViN = 5.6V,VoUT</td><td></td><td>-21</td><td></td><td></td></tr><tr><td>Load regulation: : ∆Vos(1)</td><td>ViN = VOUT(NOM) 0.5V, 1.2V≤Vouτ≤5.1V, 1 mA≤ louT≤1A</td><td></td><td>0.03</td><td></td><td>mV</td></tr><tr><td rowspan="2">e in INR/Ss Vs VNR/SS Changei</td><td>0.4 V ≤ VNR/SS ≤ 1.5 V, ViN = 5.7 V, IoUT = 1 mA</td><td></td><td>6.3</td><td></td><td>nA</td></tr><tr><td></td><td>1.5V≤VNR/SS≤5V, VIN= 5.7V, IOUT = 1mA</td><td>-3.3</td><td></td><td></td><td>nA</td></tr><tr><td>∆VOS(∆VNR/SS)</td><td>Change in Vos vs VNR/ss</td><td>0.4 V ≤ VNR/Ss ≤1.5 V, ViN = 5.7 V, IoUT = 1 mA ≤5V, ViN = 5.7V, IoUT = 1mA 1.5V≤VNR/SS</td><td></td><td>0.033</td><td></td><td>mV mV</td></tr><tr><td rowspan="5">VDo</td><td rowspan="5">Dropout voltage(2)</td><td>1.7 V ≤ ViN < 2.0 V, louT = 1 mA,</td><td></td><td>0.013</td><td></td><td></td></tr><tr><td>VouT = 99% × VoUT(NOM)</td><td></td><td>160</td><td></td><td></td></tr><tr><td>1.7 V ≤ ViN < 2.0 V, ouT = 1 A, VoUT = 99%x VOUT(NOM)</td><td></td><td>165</td><td>220</td><td>mV</td></tr><tr><td>ViN ≥ 2.0 V, louT = 1 mA, VouT =99% × VoUT(NOM)</td><td></td><td>140</td><td></td><td></td></tr><tr><td>ViN ≥ 2.0 V, IouT = 1 A, 99% VoUT × VOUT(NOM)</td><td></td><td>150</td><td>240</td><td></td></tr><tr><td rowspan="5">IC</td><td rowspan="3">Output current limit</td><td>Vout forced at 90% of VOUTOM) 200 mVOr ViN 1.7 V whichever is greater, VouT(Nom) ≥ 1.2 V, ≤ 12.5 kΩ (±1%) VOUT(NOM), ViN= ,RPGFB-to-GND</td><td></td><td></td><td></td><td></td></tr><tr><td>forced at 90% of ViN = VOUT(NOM) 200mV or ViN 1.7 V whichever is greater, ≥1.2V, = 50 kΩ (±1%) VouT VOUT(NOM), RpGFB-</td><td>0.96</td><td>1.04</td><td>1.12</td><td></td></tr><tr><td>3-to-GND VouT forced at 90% of VN = 200mVor ViN 1.7 V whichever is VOUT(NOM) f VoUT(NOM), VOUT(NOM)</td><td>0.72</td><td>0.78</td><td>0.84</td><td></td></tr><tr><td>Output current limit variation (3)</td><td>greater, VOUTNOM)≥1.2V, RPGFB-to-GND 100 kΩ (±1%) 1.7V whichever is</td><td></td><td></td><td></td><td></td></tr><tr><td>∆Isc</td><td></td><td>VN = 5.7V, VOUT = 5.1V, OUT= 0.1 mA</td><td></td><td>15</td><td>22</td><td></td></tr><tr><td>IGND</td><td>GND pin current</td><td>ViN = 1.7 V, 1oUT = 1A, 1.2V ,VouT</td><td>34</td><td>41</td><td>51</td><td>mA</td></tr></table></body></html>

# 6.5 Electrical Characteristics (continued)

over operating temperature range $( \mathsf { T } _ { \mathsf { J } } = - 4 0 ^ { \circ } \mathsf { C }$ to $+ 1 2 5 ^ { \circ } \mathsf C$ , $\mathsf { V } _ { \mathsf { I N ( N O M ) } } = \mathsf { V } _ { \mathsf { O U T ( N O M ) } } + 0 . 5 \mathsf { V }$ V, $\mathsf { V } _ { \mathsf { O U T } ( \mathsf { N O M } ) } = 3 . 3 \mathsf { V } ,$ ${ \mathsf { I } } _ { { \mathsf { O U T } } } = { \mathsf { 1 } } { \mathsf { m A } } _ { \mathrm { ; } }$ $\mathsf { V } _ { \mathsf { E N } } = 1 . 8 \mathsf { V } ,$ $\mathsf { C } _ { \mathsf { I N } } = \mathsf { C } _ { \mathsf { O U T } } = 1 0 \ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 0 \ \mathsf { 1 }$ nF, and $\mathsf { P G }$ pin pulled up to $\mathsf { V } _ { \mathsf { I N } }$ with $1 0 0 ~ \mathsf { k } \Omega$ (4) (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

<html><body><table><tr><td></td><td>PARAMETER</td><td>TEST CONDITIONS</td><td>MIN</td><td>TYP</td><td>MAX</td><td>UNIT</td></tr><tr><td>SDN</td><td>Shutdown GND pin current</td><td>PG = (open), ViN = 5.7 V, = 0.4 V , VEN_UV</td><td></td><td>0.1</td><td>30</td><td>μA</td></tr><tr><td>IEN_UV</td><td>EN_UV pin current</td><td>ViN = 5.7V, 0V≤ V /≤5.5V VEN_UV</td><td></td><td></td><td>1</td><td>μA</td></tr><tr><td>VIH(EN_UV)</td><td>EN_UV trip rising (turn-on) point</td><td>ViN = 1.7 V, no load</td><td>1.20</td><td>1.22</td><td>1.25</td><td></td></tr><tr><td>VHYS(EN_UV)</td><td>EN_UV trip point hysteresis</td><td>ViN = 1.7 V, no load</td><td></td><td>150</td><td></td><td>mV</td></tr><tr><td></td><td>PG delay time rising</td><td>Time frotme Vau crossing PG threshold% to PG reaching</td><td></td><td></td><td></td><td>ms</td></tr><tr><td>pGDL</td><td>PG delay time falling</td><td>Time from 90% to 80% of PG ofVouT</td><td></td><td>3</td><td></td><td>us</td></tr><tr><td>VFB_PG</td><td>FB_PG pin trip point (rising)</td><td>1.7 V≤ ViN≤ 5.7 V</td><td>0.19</td><td>0.2</td><td>0.21</td><td>√</td></tr><tr><td>VHYS(FB_PG)</td><td>FB_PG pin hysteresis</td><td>1.7V≤ViN≤5.7 V</td><td></td><td>6</td><td></td><td>mV</td></tr><tr><td>VOL(PG)</td><td>PG pin low-level output voltage</td><td>ViN = 1.7 V, VoUT PG(threshold), IPG -1 mA (current into device) <VFB</td><td></td><td></td><td>0.4</td><td></td></tr><tr><td>IPG(LKG)</td><td>PG pin leakage current</td><td>ViN = 5.7 V, PG(threshold), VPG =5.5V VouT VFB_</td><td></td><td></td><td></td><td>μA</td></tr><tr><td>PG IFB_</td><td>FB_PG pin leakage current</td><td>VN = 5.7V, VFB_PG = 0.2V</td><td>-100</td><td></td><td>100</td><td>nA</td></tr><tr><td>PSRR</td><td>Power-supply ripple rejection</td><td>f = 1 MHz, ViN = 3.8 V, =3.3V， IOuT = 750 mA, = 4.7 F VOUT(NOM) CNR/SS</td><td></td><td>51</td><td></td><td>dB</td></tr><tr><td rowspan="2"></td><td rowspan="2">Output noise voltage</td><td>BW = 10 Hz to 100kHz, 1.7 V≤ VN ≤5.7 V, =1.2 V, loUT = 1.0 A, CNR/SS = 4.7 μF VOUT(NOM)</td><td colspan="3">0.46</td><td rowspan="2">VRMS</td></tr><tr><td>1.8V, VoUT(NOM) = 0.8 V,</td><td colspan="3">0.835</td></tr><tr><td rowspan="4"></td><td rowspan="4">Noise spectral density</td><td>f= 100 Hz, 1.7V≤ViN ≤5.7V, VoUT(NOM) = 1.2V, louT = 1.0 A, = 4.7 μF CNR/SS</td><td colspan="3">6.6</td><td rowspan="4">nV/VHz</td></tr><tr><td>f = 1 kHz, 1.7V ≤VN ≤ 5.7V, VOUT(NOM) = 1.2V,</td><td colspan="3">1.3</td></tr><tr><td>IOuT = 1.0 A, = 4.7 μF f = 10 kHz, 1.7 V ≤ VN ≤ 5.7 V, CNR/SS VOUT(NOM)</td><td colspan="3">1.1</td></tr><tr><td>louT = 1.0 A, CNR/sS = 4.7 μF</td><td colspan="3"></td></tr><tr><td>NRSS RpULLDOWN</td><td>NRSS active discharge resistance</td><td>VN = 1.7 V, VEN_UV = GND</td><td colspan="3">15</td><td>Ω</td></tr><tr><td>RPULLDOWN</td><td>Output active discharge resistance</td><td>ViN = 1.7 V, VEN_UV = GND</td><td colspan="3">195</td><td>Q</td></tr><tr><td>TSD(shutdown)</td><td>Thermal shutdown temperature</td><td>Shutdown, temperature increasing</td><td colspan="3">175</td><td>°C</td></tr><tr><td>TSD(reset)</td><td>Thermal shutdown reset temperature</td><td>Reset, temperature decreasing</td><td colspan="3">160</td><td></td></tr></table></body></html>

(1) The device not tested under conditions where $\mathsf { V } _ { \mathsf { I N } } > \mathsf { V } _ { \mathsf { O U T } ( \mathsf { N O M } ) } + 2 . 5 \mathsf { V }$ and $\mathsf { I } _ { \mathsf { O U T } } = 1$ A because the junction temperature is higher than $+ 1 2 5 ^ { \circ } \mathsf { C }$ . Also, this accuracy specification does not apply on any application condition that exceeds the maximum junction temperature.   
(2) Measured when output voltage drops $1 \%$ below targeted value.   
(3) Brickwall current limit: $\begin{array} { r } { | _ { \mathsf { C L } _ { - } \circ _ { \% } } = ( | _ { \mathsf { S C } } - | _ { \mathsf { C L } _ { - } \ @ 0 . 9 \times \mathsf { V O U T } } ) / \mathsf { I } _ { \mathsf { C L } _ { - } \ @ 0 . 9 \times \mathsf { V O U T } } \times 1 0 0 . } \end{array}$   
(4) Additional information on setting the PG pullup resistor can be found in the application section.

# 6.6 Typical Characteristics

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } ,$ $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \ \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/7e36cb07c8ca3960c6997b7f7e09f33070a149a5bc2d68a4af905e0507c514eb.jpg)  
Figure 6-1. PSRR vs Frequency and IOUT for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/c4a7edcd4f6f8a82b0b7eb84b9a11c8e121337ab50f971f1631b5c935a23b948.jpg)  
Figure 6-3. PSRR vs Frequency and IOUT for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { v }$

![](images/281144cbf393e219c36fd5d43a03debfc676596e2c24f829076090665081f09b.jpg)  
Figure 6-5. PSRR vs Frequency and Input Pairs

![](images/a01d40038db8b6b076159e8db7d93e42ae8056ba9a58854ad2719bcf68448b60.jpg)  
Figure 6-2. PSRR vs Frequency and IOUT for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

![](images/d41d8d3b05f81d2e61e965ece35bc97f1627f795f4aead02b94e859c156b67ce.jpg)  
Figure 6-4. PSRR vs Frequency and IOUT for $\mathsf { v } _ { 0 \mathsf { U T } } = 5 . 0 \mathsf { V }$

![](images/c4e03b7f4f0aa6b51e7f3ea34751faca0c307360c4b92b802a5bd6e31d0f0a67.jpg)  
Figure 6-6. PSRR vs Frequency for Operating Headroom $( \mathsf { V } _ { \mathsf { O p H r } } )$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { 1 \mathsf { N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/39be3c0637672746595a44217112df4b541665fbae204f0b22bd3b6a39f52dbd.jpg)

$$
\mathsf { V } _ { \mathsf { O U T } } = 3 . 3 \mathsf { V } , \mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 \mathsf { \mu F } , \mathsf { C } _ { \mathsf { O U T } } = 1 0 \mathsf { \mu F }
$$

![](images/a5276b356626c221a8ec610cfb481af986ce83e60d598a069f3cb4909238aefd.jpg)  
Figure 6-8. PSRR vs Frequency for Operating Headroom $( \mathsf { v } _ { \mathsf { o p H r } } )$ and IOUT

$$
\mathsf { V } _ { \mathsf { O U T } } = 3 . 3 \mathsf { V } , \mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 \mathsf { \mu F } , \mathsf { C } _ { \mathsf { O U T } } = 1 0 \mathsf { \mu F }
$$

![](images/19978273ad47c2c2ea15f61acd6bad40690c150e28f64839ec6239415076e637.jpg)  
Figure 6-7. PSRR vs Frequency for Operating Headroom $( \mathsf { v } _ { \mathsf { o p H r } } )$ and IOUT

![](images/3495c959676b6eff6175e6b96b56bca6ecdb7cb300bf3e9da41c80b81fd65ba2.jpg)  
Figure 6-9. PSRR vs Frequency for Operating Headroom $( \mathsf { v } _ { \mathsf { o p H r } } )$ and IOUT

![](images/f1e03f020dae44deba2365b630cab7b747c7b8a5516b3740629f9a4501a225d0.jpg)  
Figure 6-10. PSRR vs Frequency for Operating Headroom $( \mathsf { V } _ { \mathsf { O p H r } } )$ and IOUT

![](images/9b37f86920d7a593b488ff74948db9abe33d2b7adc92470999c836a1f67a1466.jpg)  
Figure 6-11. PSRR vs Frequency and CNR/SS   
Figure 6-12. PSRR vs Frequency and COUT

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , ${ \mathsf { C } } _ { { \mathsf { O U T } } } = 1 0 \mu { \mathsf { F } } ,$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/f9fb994d01824c47f1e553d77ebf6abc8ffc4467e60b88ed8a05f5b62aca752f.jpg)  
Figure 6-13. Output Voltage Noise (RMS) vs Output Voltage

![](images/8b091e0c678710a939b0154d93f310b6d21979917036971f730dd8d5d914bdea.jpg)  
Figure 6-14. Output Voltage Noise (RMS) vs CNR/SS

![](images/34a81046316996e4543521cb8bf0aa888513878f0917ee239d892eea42c84238.jpg)  
Figure 6-15. Output Voltage Noise Density vs Frequency for IOUT and $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/f574a77ab91dcaa23e836c3f3cb70aa7d63b9d2a735250892edf6b4a44e4e418.jpg)  
Figure 6-16. Output Voltage Noise Density vs Frequency for IOUT and $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

![](images/8243f6cb921559577622efb4bd8df9a9feedd2b8f26636fda45fbe0d0cd51f13.jpg)  
Figure 6-17. Output Voltage Noise Density vs Frequency for IOUT and $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ \mathsf { V }$

![](images/157ac777bc075dbbfe68ccdd589fe3154c4e32ca83cf79de0acec56fb03e340f.jpg)  
Figure 6-18. Output Voltage Noise Density vs Frequency for IOUT and $\mathsf { v } _ { 0 \mathsf { U T } } = 5 . 0 \mathsf { V }$

# 6.6 Typical Characteristics (continued)

VIN VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/a026295407caedb4616f0375f883a4e3a614b551b7b805e6cdc9f1d2fc451701.jpg)  
Figure 6-19. Output Noise vs CNR/SS for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

![](images/87759688312b7d9eb02839c02ebd03cb82847a34042e786bf6b40fc3b3b57ae6.jpg)  
Figure 6-21. Output Voltage Noise Density vs Frequency and COUT for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ \mathsf { V }$

![](images/e01fa488ff27dfe04034714d305d9c10b525c0a3aa12a534ba115bcc98ad02c9.jpg)  
Figure 6-23. Load Transient Response for VOUT = 1.2 V, $\mathsf { l o u r } = 1 0 0 \mathrm { m A }$ to ${ \pmb 5 0 0 } \mathbf { m A }$

![](images/8608497501adfdce96b05231230aac52800994dbcc821d5ce280b5bf67f85ae8.jpg)  
Figure 6-20. Output Voltage Noise (RMS) vs CNR/SS for VOUT = 3.3 V

100 VouT, VRMS 50 1.2 V, 0.47 μVRMS   
30 0.8 V,0.82 VRMS 20 0.4 V, 0.78 μVRMS   
10 5 0.5 0.3 0.2 10 100 1k 10k 100k 1M 10M Frequency (Hz) CIN = COUT = 10 μF, CNR/SS = 4.7 μF, IOUT = 750 mA (10 Hz–100 kHz)

![](images/d082a21f40bc9e5048871e142a62bbcfabd9d652c1c01260f4a39c02e38f2ec3.jpg)  
Figure 6-22. Output Voltage Noise Density vs Frequency and Input Pairs   
Figure 6-24. Load Transient Response for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v } ,$ IOUT = 100 mA to 1 A

# 6.6 Typical Characteristics (continued)

![](images/7cd4eb54cf8240abf3af585eb2f7f2cd44aefa9207e76f614a1bba9ecd58ee15.jpg)  
$5 0 0 m A$

# 6.6 Typical Characteristics (continued)

![](images/2a2e7da96e45a43478c683e3cafc42a40f8595e505021c34e277c93ac2c0f1ca.jpg)

![](images/20dae56b769facc9854d1d72f2bfd02f9152cb32ee3ec1c522376361a1ac95d5.jpg)

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { 1 \mathsf { N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/cc4e897cf7a3a350cf340529615dcc5fef35f16603f4f37f0571b3300c66ef84.jpg)  
Figure 6-43. Dropout Voltage vs IOUT for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

![](images/35d98153c08d5fca344c2ec69fc6e430ba85ee87570230f4fea84f8bb6a1a0f5.jpg)  
Figure 6-45. INR/SS Distribution

![](images/d9aa16fcd1659ec47550731e144a764f37d2325fc678b28fc8bb6a30731f7672.jpg)  
Figure 6-44. Dropout Voltage vs IOUT for $\mathsf { v } _ { \mathsf { o u r } } = 5 . 1 \mathrm { \ v }$

![](images/77e7c426ff4e2f776ad5fa5cc0371b69b63831a31853ac99737b1233e44b81a6.jpg)  
Figure 6-47. INR/SS Load Regulation

![](images/72dac81c24fc220ce41a3031eef5ed43a4d5034075b23c222005e3e0c86c150b.jpg)  
Figure $6 { \cdot } 4 6 . \mathsf { V } _ { 0 } \mathsf { s }$ Distribution

![](images/6a99ad1e890a312698281f1c251a05053371cec1de86e05c78f8320fc56ab372.jpg)  
Figure $6 { \cdot } 4 8 . \mathsf { V } _ { 0 5 }$ Load Regulation

# 6.6 Typical Characteristics (continued)

VIN VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , ${ \mathsf { C } } _ { { \mathsf { O U T } } } = 1 0 \mu { \mathsf { F } } ,$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/295aa38eacc0beb555c92abd9c95507f50f9e43598c36658468b40e780634615.jpg)  
Figure 6-49. INR/SS Line Regulation

![](images/ccaaaae758a58c77da801fd4ad65ae8d93d13b605a0276376f83fedfbb804d48.jpg)  
Figure ${ \tt6 . 5 0 . } \tt V _ { 0 . 5 }$ Line Regulation

![](images/2b24897d4d21d317f4136485854aa14aab31db207eded57a12ff9f1b0084dafe.jpg)  
Figure 6-51. INR/SS vs VNR/SS and Temperature

![](images/ddb2b4de3b2990d093d6e72d0ce5cb0b176b533f6abdc44279154d00de0872b3.jpg)  
Figure 6-52. VOS vs VNR/SS and Temperature

![](images/6896f90d749716adad5f02413efee8f582d39e72613edb6f85b0f789289583e8.jpg)  
Figure 6-53. INR/SS vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 2 \ : \mathsf { V }$

![](images/c5795f395e65f4c2d37680c61d3941c8afc728f2e3ec65be8fdf2372cc1593eb.jpg)  
Figure 6-54. $\mathtt { v _ { \infty } }$ vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 2 \mathsf { V }$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { 1 \mathsf { N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/132b15e176005b07d95fa909930140d56cc097d5ae735508c6a40dbfeb5ed0d7.jpg)  
Figure 6-55. INR/SS vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 8 \ : \mathsf { V }$

![](images/a6824a43fbb9ddb6f443ed41b904df201f36720bf38cb57c529d002f4ead91a6.jpg)  
Figure 6-57. INR/SS vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 3 . 3 \ : \mathsf { V }$

![](images/8240a6781f02679227456452568b11677b75271541eafbbdb80b181ba8ecd289.jpg)  
Figure 6-59. INR/SS vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 5 . 1 \mathrm { ~ V }$

![](images/c3793678a60acfefbfc23564860aba31e0a1731d27f2254df60608c059a329a5.jpg)  
Figure 6-56. $\mathtt { v _ { 0 s } }$ vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 8 \ : \mathsf { V }$

![](images/c82cbcd8516a0fbc94e7798050ebdd14b83ffd63298a8b500f3f3b1d78dd1c3b.jpg)  
Figure 6-58. $\mathtt { v _ { 0 5 } }$ vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 3 . 3 \ : \mathsf { V }$

![](images/8a6ebcf599584688b3f950c8e47b06cb6525f6d7df3f9dd4a2830929e79d7d5f.jpg)  
Figure 6-60. $\mathtt { v _ { \infty } }$ vs IOUT and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 5 . 1 \ : \mathsf { V }$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { 1 \mathsf { N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , ${ \mathsf { C } } _ { { \mathsf { O U T } } } = 1 0 \mu { \mathsf { F } } ,$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/2fa2413587058dd137e9bdb1956d510b31b7eb68bd92f003f2948b0636bbf696.jpg)  
Figure 6-61. INR/SS vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/7192cb3f16c0cb85c26175699397276554848b2d823ecc4902561a5eaf8d55bf.jpg)  
Figure 6-63. INR/SS vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { v }$

![](images/2ce302975bc9c32b8d97523d401e22d88f65b9373ba8142b947197fa25d169be.jpg)  
Figure 6-65. INR/SS vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { v }$

![](images/2be9cd19c021f56dd0b3334046fdafa75843c706a384dc5e3c53c79165ad8f69.jpg)  
Figure 6-62. $\mathtt { v _ { 0 s } }$ vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/0b6617a6a3fd9b1850c18baac30b30d93383e8715b807ee2eeb413628a93dc80.jpg)  
Figure $\phantom { + } 6 . 6 4 . \phantom { + } \mathsf { V } _ { 0 5 }$ vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

![](images/2a0df5fe1f7fa6928048cf97cdd3ec746ee2148c48cb63e5aa1221e4623d96b7.jpg)  
Figure 6-66. $\mathtt { v _ { \infty } }$ vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/5fb428c058a4a2645e1aa9acaf61ef3e7edadfc56cb9588c2dca22f02152d89a.jpg)  
Figure 6-67. INR/SS vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v _ { 0 u \tau } } = 5 . 1 \mathrm { \ v }$

![](images/5f9d90db88bf4da704d85b2f100cace67880c7538250c2e19d44ef1d6b1e1bb6.jpg)  
Figure 6-69. INR/SS vs Temperature for $\mathsf { V } _ { \mathsf { I N \_ M i n } } , \mathsf { V } _ { \mathsf { O p H r } } = 0 . 2 \mathsf { V }$

![](images/237b6caaec40380ff6f461e6fb5ef5c9a15320d60afcd969c755a8ed62bc668d.jpg)  
Figure 6-71. INR/SS Fast-Start vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature

![](images/8d42192db060b8095914f14cc5e5f243996cdf1498182a987bee27d2d1311fa9.jpg)  
Figure 6-68. $\mathtt { v _ { 0 s } }$ vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 5 . 1 \mathrm { \ v }$

![](images/7ab9b3e86205c93faa92dbbd31f2b0d0514a6ea37c76cd9df43a642d34869a76.jpg)  
Figure $\phantom { + } 6 . 7 0 . \phantom { + } \mathsf { v } _ { \infty }$ vs Temperature for $\mathsf { V } _ { \mathsf { I N \_ M i n } } , \mathsf { V } _ { \mathsf { O p H r } } = 0 . 2 \mathsf { V }$

![](images/e22cffa47780d7fa2b39ebaa111ef7acbe7ed9ecd291db394bb015346a8875e7.jpg)  
Figure 6-72. GND Pin Current vs $\mathsf { I } _ { \mathsf { O U T } }$ and Temperature for

# 6.6 Typical Characteristics (continued)

VIN VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/e881c8d7901d26ce310a51eeb8fc92af51ef68dc4883f97bb8d644d3ca0ae38b.jpg)  
Figure 6-73. GND Pin Current vs IOUT and Temperature for

![](images/6b281a636483e33a425a5dbff1184d818bdfdec8c21dd7342a58676e51103c0c.jpg)  
Figure 6-74. GND Pin Current vs IOUT and Temperature for

![](images/1dfa98684c94ecf9a6516e3bfdc1798008baf5ce315e949c88e5ce616d73678d.jpg)  
Figure 6-75. GND Pin Current vs VNR/SS and Temperature

![](images/53a6c4a28938de013328dcc2e571bf212dad61974b12a2b81093e80764470caf.jpg)  
Figure 6-76. GND Pin Current vs $\mathsf { \mathbf { v } } _ { \mathsf { I N } }$ and Temperature for $\mathsf { l } _ { \mathsf { O U T } } = 1 \mathsf { m A }$

![](images/3c2df464d31632a6467d0a8a6dcccd63ccafb1251bcd0eb209cc5bb7ed6c04cd.jpg)  
Figure 6-77. GND Pin Current vs $\mathsf { \mathbf { v } } _ { \mathsf { I N } }$ and Temperature for $\mathsf { l o u r } = 5 0 0 ~ \mathsf { m A }$

![](images/3f4fc65b1993dbbf802690b4b216da05e8020bf1a7957786d2203772d59f5583.jpg)  
Figure 6-78. GND Pin Current vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature for

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/6936f683c1adf8447c99a0ffd576e8bb89a64bbed926eb768f74c693dac1adf6.jpg)  
Figure 6-79. GND Pin Current vs $\boldsymbol { \mathsf { v } } _ { \mathsf { E N } }$ and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 7 \ : \mathsf { V }$ (Dropout Operation)

![](images/fbd010db8fa0fdb13ea54331abd7d765ce55c47af106c025e7a34a89f3c75eae.jpg)  
Figure 6-81. $100 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = \pmb { 0 . 4 } \mathsf { V }$

![](images/5cdd10d8ae3598811893345e816bc7c8db2511d2e3e8ad082a2035b3dbd41a08.jpg)  
Figure 6-83. $100 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/6c8eca3960676519edb686e2e233d30b9e387b568ac2b671b9c91e8382dd8219.jpg)  
Figure 6-80. GND Pin Current vs $\ V _ { E N }$ and Temperature for

![](images/b71e7231230002952aa0af205b01e7f0d2feb5870510894d7f8505f4d4def9b3.jpg)  
Figure 6-82. $100 \%$ Current Limit vs Temperature for $\mathsf { V } _ { \mathsf { O U T } } = \mathsf { 0 } . 8 \mathsf { V }$

![](images/77fd1c646d8573f2a0bd6ac3273a11374b1efb276fc2da8c99b899842b3beaf7.jpg)  
igure 6-84. $100 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/273f68e0cfb72f130ba23d1510f782e8755dfee74905dc086f3a4c968d247124.jpg)  
Figure 6-85. $100 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { v }$

![](images/66be7f01b98e6fad4099b74e54c8c1d41895feeda5cddc13f212cc947a5e0ea1.jpg)  
Figure $6 . 8 7 . 8 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/ffb7b30adb761d9c436e692b9bafb1d9ee0c16d7eb8d164331eef7acf0d53c30.jpg)  
Figure $6 . 8 9 . 8 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

![](images/0b4edaa2daa7dda6b6e84d553dcdd80d2b0f856910eaf87e63ad15470bfbfe07.jpg)  
Figure 6-86. $100 \%$ Current Limit vs Temperature for $\mathsf { v } _ { 0 \mathsf { U T } } = 5 . 1 \ \mathsf { V }$

![](images/15bc83ef92c2708a3424b9aa57dfed30aeff2ccf5e323963c892d13f5091a69c.jpg)  
Figure $6 . 8 8 . 8 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

![](images/c49bc6e38c93fb4825f8d44f1650863dbe023866b0b38762d0f32271291da714.jpg)  
Figure $6 . 9 0 . 8 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 5 . 1 \mathrm { v }$

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/9f044d0c281c400cb663f7910c5d0fcbefbb57e1f45f044ec272968db93065e6.jpg)  
Figure 6-91. $60 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 2 \mathsf { v }$

![](images/2c1af1aaf754cb8a5cfe9eab646ceb05c93c84a5e611c8d23a6b3cc7ce2a3fb3.jpg)  
Figure $6 . 9 3 . 6 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

![](images/883be45b3e06fffa62bf17be7b5ad6499e41aede2b6e63c82237c4d9cfbb505b.jpg)  
Figure 6-95. $\mathsf { U V L O } _ { \mathsf { I N } }$ vs Temperature

![](images/c9c12084e296aebf17b5666972a13a23aeb0666d18208882cca8a91e7f3feb50.jpg)  
Figure 6-92. $60 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 1 . 8 \mathsf { V }$

![](images/29a0b2d9210d9c61ac7a212622b1ccc665c3efffeb0446602e0f41185cc0e41f.jpg)  
Figure $6 . 9 4 . 6 0 \%$ Current Limit vs Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 5 . 1 \mathrm { \ v }$

![](images/2e51c09b7d5d082aa70f640d00e88be2eb418c34fead8ce8183dad76146e3b8b.jpg)  
Figure 6-96. $\mathsf { v } _ { \mathsf { E N } }$ Hysteresis and Threshold vs Temperature

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { 1 \mathsf { N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/80d3f413a25198c8b0cb67685c249be28ab3246ded76b1ebf9cc531a779e1c37.jpg)  
Figure 6-97. EN Pin Current vs Enable Voltage and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 1 . 7 \ : \mathsf { V }$

![](images/02562ccfd90dda59ad0e88cb46584d2e72aa052a9bb733211452dec08c96bb80.jpg)  
Figure 6-98. EN Pin Current vs Enable Voltage and Temperature for $\mathsf { V } _ { \mathsf { I N } } = 5 . 7 \ : \mathsf { V }$

![](images/bd90dcb8da1d7725ef24839db80362eabe80eccd2d5adc6ee5e214b65bbf56e5.jpg)  
Figure 6-99. VFB_PG Hysteresis and Threshold vs Temperature

![](images/ecb4cad00fb2caa89f3da0e6b9d412da38c5468c02394adea36555de3f0e8521.jpg)  
Figure 6-101. $\mathsf { \Delta } \mathsf { v } _ { \mathsf { P G } }$ Low-Level Output Voltage vs Temperature

![](images/ac479075fc0df896f39b24426310bd0547304c9be3456fe9ff95c56d7d16b8de.jpg)  
Figure 6-100. FB_PG Pin Current vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature

![](images/49c204d3abe7f2b4d8d214aa37ce1b6c8a5e383e303492c89bcb8071bdebc019.jpg)  
Figure 6-102. PG Pin Current vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature

# 6.6 Typical Characteristics (continued)

VIN = VOUT(NOM) + 0.5 V, $V _ { \mathsf { E N } } = 1 . 8 ~ \mathsf { V }$ , $\mathsf { C } _ { \mathsf { I N } } = 1 0 ~ \mu \mathsf { F }$ , $\mathsf { C } _ { \mathsf { N R / S S } } = 4 . 7 ~ \mu \mathsf { F } _ { ; }$ , $\mathsf { C } _ { \mathsf { O U T } } = 1 0 \mu \mathsf { F } _ { : }$ and $\mathsf { I _ { O U T } } = 1 \mathsf { m A }$ (unless otherwise noted); typical values are at ${ \sf T } _ { \mathsf { J } } = 2 5 ^ { \circ } { \sf C }$

![](images/48821630664d0027ae87cc80d96eed1cb1fc247066e99bdc652a12e5ad1eae3b.jpg)  
Figure 6-105. Shutdown Current vs $\mathsf { v } _ { \mathsf { I N } }$ and Temperature

# 7 Detailed Description 7.1 Overview

The TPS7A94 is an ultra-low-noise $( 0 . 4 6 ~ \mu \mathsf { V } _ { \mathsf { R M S } }$ over $1 0 - H z$ to $1 0 0 - k H z$ bandwidth), ultra-high PSRR $\left( > 5 0 \right.$ dB to 2 MHz), high-accuracy $( 1 \% )$ , low-dropout (LDO) linear voltage regulator with an input range of $1 . 7 \ : \mathrm { V }$ to $5 . 7 \mathrm { V }$ and an output voltage range from $0 \vee$ to $\mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { D O } }$ and is fully specified above $0 . 4 \lor _ { \mathrm { O U T } }$ . This LDO regulator uses innovative circuitry to achieve wide bandwidth and high loop gain, resulting in ultra-high PSRR even when operating under very low operational headroom $( \mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { O U T } } )$ . At a high level, the device has two main blocks (the current reference and the unity-gain LDO buffer) and a few secondary features (such as the precision enable, current limit, and PG pin).

The current reference is controlled by the NR/SS pin. This pin sets the output voltage with a single resistor, sets the start-up time, and filters the noise generated by the reference and external set resistor.

The unity-gain LDO buffer is controlled by the OUT pin. The ultra-low-noise does not increase with output voltage and provides wideband PSRR. As such, the SNS pin is only used for remote sensing of the load.

The EN_UV pin sets the precision enable feature. Select the optimal input voltage at which the LDO starts at.   
There are two independent UVLO voltages in this device: the internal IN rail UVLO and the EN_UV pin.

The FB_PG pin sets the current limit and power-good (PG) features. A voltage divider on this pin programs both the current limit and the PG trip point.

An ultra-low-noise current reference ( $1 5 0 \mu \mathsf { A }$ , typical) is used in conjunction with an external resistor $( R _ { \mathsf { N R / S S } } )$ to set the output voltage. This process allows the output voltage range to be set from $0 . 4 ~ \mathsf { V }$ to $( \mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { D O } } )$ . To achieve this ultra-low noise, an external capacitor CNR/SS (typically $4 . 7 ~ \mu \mathsf { F } )$ (id:) is placed in parallel to the RNR/SS resistor used to set the output voltage. The unity-gain architecture provides ultra-high PSRR over a wide frequency range without compromising load and line transients.

This regulator offers programmable current-limit, thermal protection, is fully specified from $- 4 0 ^ { \circ } \mathsf { C }$ to $+ 1 2 5 ^ { \circ } \mathsf { C }$ above $0 . 4 \mathsf { V } _ { \mathsf { O U T } }$ and is offered in a thermally efficient 10-pin, 3-mm × 3-mm WSON package.

# 7.2 Functional Block Diagram

![](images/3d2df046335aac7ec18aaf925df687f833cf3a26e575dd412b25beff75367b52.jpg)

A. See the RPULLDOWN output active discharge resistance value in the Electrical Characteristics table.   
B. See the delay value in the Electrical Characteristics table.

# 7.3 Feature Description

# 7.3.1 Output Voltage Setting and Regulation

Figure 7-1 shows a simplified regulation circuit, where the input signal $( V _ { \mathsf { N R / S S } } )$ is generated by the internal current source (INR/SS) and the external resistor $( R _ { \mathsf { N R / S S } } )$ . Because the error amplifier is always operating in unity-gain configuration, the LDO output voltage is directly programmed by the VNR/SS voltage. The VNR/SS reference voltage is generated by an internal low-noise current source driving the RNR/SS resistor and is designed to have very low bandwidth at the input to the error amplifier through the use of a low-pass filter (CNR/SS || RNR/SS).

![](images/6ec0e72e7c3cf65275550a614de79ace0161e0a0c64673bc30f8ede606ad7a8d.jpg)  
Figure 7-1. Simplified Regulation Circuit

This unity-gain configuration, along with the highly accurate INR/SS reference current, enables the device to achieve excellent output voltage accuracy; though, the RNR/SS accuracy can become the limiting factor when operating at low output voltage. The low dropout voltage $( \mathsf { V } _ { \mathsf { D O } } )$ enables reduced thermal dissipation and achieves robust performance. This combination of features makes this device an excellent voltage source for powering sensitive analog low-voltage devices.

# 7.3.2 Ultra-Low Noise and Ultra-High Power-Supply Rejection Ratio (PSRR)

The architecture features a highly accurate, high-precision, low-noise current reference followed by a state-ofthe-art error amplifier (1.1 nV/√Hz at $1 0 - k H z$ noise for $\mathsf { V } _ { \mathsf { O U T } } \geq 1 . 2 \mathrm { ~ V } ,$ ) comparable to, if not better than, that of a precision amplifier. The unity-gain configuration ensures ultra-low noise over the entire output voltage range. Additional noise reduction and higher output current can be achieved by placing multiple TPS7A94 LDOs in parallel.

# 7.3.3 Programmable Current Limit and Power-Good Threshold

The brick-wall current limit can be programmed to either $100 \%$ , $80 \%$ , or $60 \%$ of the nominal factory-programmed value by setting the input impedance for the FB_PG pin. Similarly, the power-good indication threshold can also be adjusted between $8 5 \%$ and $9 5 \%$ of the nominal output voltage by changing the FB_PG resistor divider ratio; see the Adjusting the Factory-Programmed Current Limit section for details.

# 7.3.4 Programmable Soft Start (NR/SS Pin)

The device features a programmable, monotonic, voltage-controlled, soft-start circuit that uses the CNR/SS capacitor to minimize inrush current into the output capacitor and load during start up. This circuitry can also reduce the start-up time for some applications that require the output voltage to reach at least $90 \%$ of the set value for fast system start up. See the Programmable Soft-Start and Noise-Reduction (NR/SS Pin) section for more details.

# 7.3.5 Precision Enable and UVLO

Two independent UVLO (undervoltage lockout) voltage circuits are present. An internally set UVLO on the input supply (IN pin) automatically disables the LDO when the input voltage reaches the minimum threshold. A precision EN function (EN_UV pin) can also be used as a user-programmable UVLO.

The input supply voltage undervoltage lockout (UVLO) circuit prevents the regulator from turning on when the input voltage is not high enough, see the Electrical Characteristics table for more details. 2. The precision enable circuit allows a simple sequencing of multiple power supplies with a resistor divider from another supply. This enable circuit can be used to set an external UVLO voltage at which the device is enabled using a resistor divider on the EN_UV pin; see the Precision Enable (External UVLO) section for more details.

# 7.3.6 Active Discharge

The device incorporates two internal pulldown metal-oxide semiconductor field effect transistors (MOSFETs). The first pulldown MOSFET connects a resistor (RPULLDOWN) from OUT to ground when the device is disabled to actively discharge the output capacitor. The second pulldown MOSFET connects a resistor (RPULLDOWN_ NR/SS) from NR/SS to ground when the device is disabled and discharges the NR/SS capacitor. Both pulldown MOSFETs are activated by any one or more of the following:

12. Driving the EN_UV pin below the VEN(LOW) threshold The IN pin voltage falling below the undervoltage lockout $\mathsf { V } _ { \mathsf { U V L O } }$ threshold 3. Having the output voltage greater than the input voltage

# 7.3.7 Thermal Shutdown Protection $( \overline { { \mathsf { T } } } \mathsf { s } \mathsf { \mathsf { D } } )$

A thermal shutdown protection circuit disables the LDO when the junction temperature $( \mathsf { T } _ { \mathsf { J } } )$ of the pass transistor rises to TSD(shutdown) (typical). Thermal shutdown hysteresis assures that the device resets (turns on) when the temperature falls to TSD(reset) (typical). The thermal time constant of the semiconductor die is fairly short, thus the device can cycle off and on when thermal shutdown is reached until power dissipation is reduced. Power dissipation during start up can be high from large $\mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { O U T } }$ voltage drops across the device or from high inrush currents charging large output capacitors.

Under some conditions, the thermal shutdown protection can disable the device before start up completes. For reliable operation, limit the junction temperature to the maximum listed in the Electrical Characteristics table. Operation above this maximum temperature causes the device to exceed operational specifications. Although the internal protection circuitry of the device is designed to protect against thermal overload conditions, this circuitry is not intended to replace proper heat sinking. Continuously running the device into thermal shutdown or above the maximum recommended junction temperature reduces long-term reliability.

# 7.4 Device Functional Modes

Table 7-1 shows the conditions that lead to the different modes of operation. See the Electrical Characteristics table for parameter values.

Table 7-1. Device Functional Mode Comparison   

<html><body><table><tr><td rowspan="2">OPERATING MODE</td><td colspan="4">PARAMETER</td></tr><tr><td>ViN</td><td>_UV VEN.</td><td>louT</td><td>TJ</td></tr><tr><td>Normal operation</td><td>VN> + VDo and VN> VoUT(nom) ViN(min)</td><td>VEN_UV > VIH(EN_UV)</td><td>IouT < louT(max)</td><td>TJ < TSD(shutdown)</td></tr><tr><td>Dropout operation</td><td><VN< VDo ViN(min) VouT(nom) ×</td><td>VEN_UV _UV) VIH(EN</td><td>louT < loUT(max)</td><td>TJ <TSD(shutdown)</td></tr><tr><td>Disabled (any true condition disables the device)</td><td>ViN < VuvLo or ViN < VOUT + 90 mV or VN< + 20mV VNR/SS</td><td>VEN_UV <VIL(EN_UV)</td><td>Not applicable</td><td>TJ> TSD(shutdown)</td></tr><tr><td>Current-limit operation</td><td>VN> + VDo and ViN> VoUT(nom) ViN(min)</td><td>VEN_UV (7d: VIH(EN_UV)</td><td>IouT ≥ICL(min)</td><td>TJ < TSD(shutdown)</td></tr></table></body></html>

# 7.4.1 Normal Operation

The device regulates to the nominal output voltage when the following conditions are met:

The input voltage is greater than the nominal output voltage plus the dropout voltage $( \mathsf { V } _ { \mathsf { O U T } ( \mathsf { n o m } ) } + \mathsf { V } _ { \mathsf { D O } } )$   
The output current is less than the current limit $( \mathsf { I } _ { \mathsf { O U T } } < \mathsf { I } _ { \mathsf { C L } } )$ (d)   
The device junction temperature is less than the thermal shutdown temperature $( \mathsf { T } _ { \mathsf { J } } < \mathsf { T S D } _ { ( \mathsf { s h u t d o w n } ) } )$   
The voltage on the EN_UV pin has previously exceeded the $\mathsf { V } _ { \mathsf { I H } ( \mathsf { E N } \_ \mathsf { U V } ) }$ threshold voltage and has not yet   
decreased to less than the $\mathsf { V } _ { \mathsf { I L } ( \mathsf { E N \_ U V } ) }$ falling threshold

# 7.4.2 Dropout Operation

If the input voltage is lower than the nominal output voltage plus the specified dropout voltage, but all other conditions are met for normal operation, the device operates in dropout mode. In this mode, the output voltage tracks the input voltage. During this mode, the transient performance of the device becomes significantly degraded because the pass transistor is in the ohmic or triode region, and acts as switch. Line or load transients in dropout can result in large output-voltage deviations.

<html><body><table><tr><td>Note</td></tr><tr><td>While in dropout, if a heavy load transient event forces ViN < + 90 mV or ViN < VNR/SS + VOUT(NOM)</td></tr><tr><td></td></tr><tr><td>20 mV, the device restarts to prevent the output voltage from overshooting to protect the device and load.</td></tr></table></body></html>

When the input voltage returns to a value greater than or equal to the nominal output voltage plus the dropout voltage $( \mathsf { V } _ { \mathsf { O U T } ( \mathsf { N O M } ) } + \mathsf { V } _ { \mathsf { D O } } )$ , the output voltage can overshoot for a short period of time while the device pulls the pass transistor back into the linear region.

For additional information, see the Output Voltage Restart (Overshoot Prevention Circuit) section.

# 7.4.3 Disabled

The output of the device can be shutdown by forcing the voltage of the EN_UV pin to less than the VIL(EN_ UV) threshold (see the Electrical Characteristics table). When disabled, the pass transistor is turned off, internal circuits are shutdown, and both the NR/SS pin and OUT pin voltages are actively discharged to ground by internal discharge circuits to ground when the IN pin voltage is higher than or equal to a diode-drop voltage.

# 7.4.4 Current-Limit Operation

If the output current is greater than or equal to the minimum current limit, $( \mathsf { l } _ { \mathsf { C L } ( \mathsf { M i n } ) } )$ , then the device is operating in current-limit mode. The current limit is brick-wall and is programmable with the PG_FB pin. For additional information, see the Adjusting the Factory-Programmed Current Limit section.

# 8 Application and Implementation

# Note

Information in the following applications sections is not part of the TI component specification, and TI does not warrant its accuracy or completeness. TI’s customers are responsible for determining suitability of components for their purposes, as well as validating and testing their design implementation to confirm system functionality.

# 8.1 Application Information

Successfully implementing a low-dropout regulator (LDO) in an application depends on the application requirements. This section discusses key device features and how to best implement them to achieve a reliable design.

# 8.1.1 Output Voltage Restart (Overshoot Prevention Circuit)

Wide bandwidth linear regulators suffer from an undesirable excessive overshooting of the output voltage during restart events that occur when the CNR/SS and $\mathsf { C } _ { \mathsf { O U T } }$ capacitors are not fully discharged. In this device, and as shown in Figure 8-1, this undesirable behavior is mitigated by implementing low hysteresis circuitry consisting of two ORed comparators to detect when the input voltage is either $2 0 ~ \mathsf { m V }$ (typical) lower than the VNR/SS reference voltage or $3 0 0 ~ \mathrm { m V }$ (typical) lower than VOUT.

![](images/6f7e2911ce07b205c3a44147a35a6c647caf2133b8fc538eea27a540a8fb6e9b.jpg)  
Figure 8-1. Overshoot Prevention Circuit

When the device is operating in dropout, transient events (such as an input voltage brownout, heavy load transient, or short-circuit event) can force the device in a reversed bias condition where the input voltage is either $2 0 ~ \mathsf { m V }$ (typical) lower than the VNR/SS reference voltage or $3 0 0 ~ \mathrm { m V }$ (typical) lower than $\mathsf { V } _ { \mathsf { O U T } }$ . The output overshoot prevention circuit can be triggered, as shown in Figure 8-2, thus forcing the device to shutdown and restart, thereby preventing output voltage overshoot. If the device is still operating in dropout and the error condition that triggered this circuit is still present, an additional restart can occur until these conditions are removed or the device is no longer in dropout. The restart always occurs from a discharged state and always has the same characteristics as the initial LDO power-up, so the start-up time, VOUT ramp rate, and VOUT monotonicity are all predictable.

![](images/1f392fda7d96de037f06bed17bddd858fa2ab28f3bc1e3c43f67d0b814ce094e.jpg)  
Figure 8-2. Device Behavior in Dropout

Figure 8-3 and Figure 8-4 show examples of a soft brownout and a brownout event, respectively.

The brownout overshoot is present with higher $\mathsf { V } _ { \mathsf { I N } }$ slew rates. A $1 - V / \mu \leq$ slew rate was used in Figure 8-5.

![](images/70e7f37ace9e3ecfb90f00fcdc79a2b6478f5190e310e436188b98937f108c57.jpg)  
Figure 8-3. Example: Soft Brownout to VNR/SS

![](images/15917ec4abee534a3886361db63496790b377b9a4d3f00c69084350a60862040.jpg)  
Figure 8-4. Example: Brownout

![](images/e568d2b7cad436cc42a0287d57bf1e372215b2ef65001df4bba598fce295c0d5.jpg)  
Figure 8-5. Example: Brownout With Overshoot Recovery

The overshoot prevention circuit is implemented to provide a predictable start-up and shutdown of the device without output overshoot if the EN_UV external UVLO is not used as described in this section. This circuit can be prevented from triggering by:

1. Using an input supply capable of handling heavy load transients or a larger value input capacitor   
2. Increasing the operating headroom between VIN and $\mathsf { V } _ { \mathsf { O U T } }$ (for example, when using a battery as an input supply to make sure that VIN stays higher than $\mathsf { V } _ { \mathsf { O U T } }$ even when the battery is near the full discharge state)   
3. Using an input supply with a ramp rate faster than the set output voltage time constant formed by CNR/SS || RNR/SS   
4. Discharging the input supply slower than the discharge time formed by COUT || (Load RPULLDOWN) or by the CNR/SS | (RNR/SS RPULLDOWN_ NR/SS)

# 8.1.2 Precision Enable (External UVLO)

The precision enable circuit is used to turn the device on and off. This circuit can be used to set an external undervoltage lockout (UVLO) voltage (see Figure 8-6) to turn on and off the device using a resistor divider between IN, EN_UV, and GND.

$\mathsf { V } _ { \mathsf { E N \_ U V } } \geq \mathsf { V } _ { \mathsf { I H } ( \mathsf { E N \_ U V } ) } .$ , the regulator is enabled. $\mathsf { V } _ { \mathsf { E N \_ U V } } \leq \mathsf { V } _ { \mathsf { I L } ( \mathsf { E N \_ U V } ) }$ , the regulator is disabled. The EN_UV pin does not incorporate an internal pulldown resistor to GND and must not be left floating. Use the precision enable circuit for this pin to set an external undervoltage lockout (UVLO) input supply voltage to turn on and off the device using a resistor divider between IN, EN_UV, and GND.

![](images/0176a034e69ac731b0822b09fcb06d3b8c9936c21633c0f89a912f631bf49363.jpg)  
Figure 8-6. Precision EN Used as External UVLO

This external UVLO configuration prevents the LDO from turning on when the input supply voltage is insufficient and places the device in dropout operation.

Using the EN_UV pin as an externally set UVLO allows simple sequencing of cascaded power supplies. An additional benefit is that the EN_UV pin is never left floating. The EN_UV pin does not have an internal pulldown resistor. In addition to the resistor divider, a zener diode can be needed between the EN_UV pin and ground to comply with the absolute maximum ratings on this pin.

When $V _ { \mathsf { I N } }$ exceeds the targeted $\mathsf { V } _ { \mathsf { O N } }$ voltage and the R(BOTTOM) resistor is set, Equation 1 and Equation 2 provide the R(TOP) resistor value and the VOFF voltage at which the input voltage must drop below to disable the LDO.

$$
\begin{array} { r l } & { \mathsf { R } _ { ( \mathsf { T O P } ) } \leq \mathsf { R } _ { ( \mathsf { B O T T O M } ) } \times ( \mathsf { V } _ { \mathsf { O N } } / \mathsf { V } _ { \mathsf { I H } ( \mathsf { E N } _ { - } \cup \mathsf { V } ) } - 1 ) } \\ & { \mathsf { V } _ { \mathsf { O F F } } < [ 1 + \mathsf { R } _ { ( \mathsf { T O P } ) } / \mathsf { R } _ { ( \mathsf { B O T T O M } ) } ] \times ( \mathsf { V } _ { \mathsf { I H } ( \mathsf { E N } _ { - } \cup \mathsf { V } ) } - \mathsf { V } _ { \mathsf { H Y S } ( \mathsf { E N } _ { - } \cup \mathsf { V } ) } ) } \end{array}
$$

where:

$\mathsf { V } _ { \mathsf { O F F } }$ is the input voltage where the regulator shuts off $\mathsf { V } _ { \mathsf { O N } }$ is the voltage where the regulator turns on

Consider the EN_UV current pin when selecting the ${ \mathsf { R } } _ { ( { \mathsf { T O P } } ) }$ and R(BOTTOM) values.

# 8.1.3 Undervoltage Lockout (UVLO) Operation

The UVLO circuit, present on the IN pin, ensures that the device remains disabled before the input supply reaches the minimum operational voltage range, and that the device shuts down when the input supply falls too low.

The $\mathsf { U V L O } _ { | \mathsf { N } }$ circuit has a minimum response time of several microseconds to fully assert. During this time, a downward line transient below approximately $1 . 6 \mathrm { ~ V ~ }$ causes the input supply UVLO to assert for a short time. However, the $\mathsf { U V L O } _ { | \mathsf { N } }$ circuit can possibly not have enough stored energy to fully discharge the internal circuits inside of the device. When the $\mathsf { U V L O } _ { | \mathsf { N } }$ circuit does not fully discharge, internal circuitry is not fully disabled.

The effect of the downward line transient can trigger the overshoot prevention circuit and can be easily mitigate by using the solution proposed in the Precision Enable (External UVLO) section.

Figure 8-7 illustrates the $\mathsf { U V L O } _ { | \mathsf { N } }$ circuit response to various input voltage events. This diagram can be separated into the following regions:

Region A: The device does not turn on until the input reaches the UVLO rising threshold.   
Region B: Normal operation with a regulated output.   
Region C: Brownout event above the UVLO falling threshold (UVLO rising threshold – UVLO hysteresis). The output can fall out of regulation but the device is still enabled.   
Region D: Normal operation with a regulated output.   
Region E: Brownout event below the UVLO falling threshold. The device is disabled in most cases and the output falls because of the load and active discharge circuit. The device is re-enabled when the UVLO rising threshold is reached by the input voltage and a normal start up then follows.   
Region F: Normal operation followed by the input falling to the UVLO falling threshold.

Region G: The device is disabled when the input voltage falls below the UVLO falling threshold to 0 V. The output falls because of the load and active discharge circuit.

![](images/dbac06effd4fdf18cc37fc56565377bfa13db04e29db9053a91848418069d982.jpg)  
Figure 8-7. Typical UVLO Operation

# 8.1.4 Dropout Voltage $( \mathsf { v } _ { \mathsf { D } \mathsf { 0 } } )$

The dropout voltage refers to the minimum voltage difference between the input and output voltage $( \mathsf { V } _ { \mathsf { D } 0 } = \mathsf { V } _ { \mathsf { I N } }$ $- \mathsf { V } _ { \mathsf { O U T } } )$ that is required for regulation. When the input voltage $( \mathsf { V } _ { \mathsf { I N } } )$ drops to or below the maximum dropout voltage $( \mathsf { V } _ { \mathsf { D O } ( \mathsf { M a x } ) } )$ for the given load current, see the Electrical Characteristics table, the device functions as a resistive switch and does not regulate the output voltage. When the device is operating in dropout, the output voltage tracks the input voltage. For high current, the dropout voltage $( \mathsf { V } _ { \mathsf { D O } } )$ is proportional to the output current because the device is operating as a resistive switch. For low current, internal nodes are saturating and the dropout plateaus to the minimum value. As mentioned in the Output Voltage Restart (Overshoot Prevention Circuit) section, transient events such as an input voltage brownout, heavy load transient, or short-circuit event can trigger the overshoot prevention circuit. Operating the device at or near dropout significantly degrades both transient performance and PSRR, and can also trigger the overshoot prevention circuit. Maintaining sufficient operating headroom $( \mathsf { V } _ { \mathsf { O p H r } } = \mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { O U T } } )$ significantly improves the device transient performance and PSRR, and prevents triggering the overshoot prevention circuit.

# Note

For this device, the pass element is not the limiting dropout voltage factor. Because the reference voltage is generated by a current source and the NR/SS resistor, and because the operating headroom is reducing (even at low load), the internal current source (INR/SS) saturates faster than the pass transistor. This behavior is described in the dropout voltage plot (Figure 6-43). Notice that the dropout does not go to $0 \vee$ at light loads.

# 8.1.5 Power-Good Feedback (FB_PG Pin) and Power-Good Threshold (PG Pin)

For proper device operation, the resistor divider network input to the FB_PG pin must be connected. The FB_PG pin must not be left floating because this pin represents an analog input to the device internal logic and the input impedance is sampled during device start up.

The PG pin is an output indicating whether the LDO is ready to provide power. This pin is implemented using an open-drain architecture. The FB_PG pin is used to program the PG pin and serves a dual purpose of programming the PG threshold assert voltage and adjusting the current limit, ICL.

The PG pin must use the minimum value or larger pullup resistor from PG to IN, see Figure 8-8, or the external rail as listed in the Electrical Characteristics table. If PG functionality is not used, leave this pin floating or connected to GND.

The FB_PG pin uses the parallel impedance formed by the resistor divider $\mathsf { R } _ { \mathsf { F B \_ P G ( T O P ) } }$ and RFB _PG(BOTTOM) to program the current limit value during LDO initialization. If this impedance is less than $1 \dot { 2 } . 5 \mathsf { k } \Omega$ , then the nominal factory-programmed, current-limit value is selected. If the input impedance is less than $5 0 ~ \mathsf { k } \Omega$ , but greater than $1 2 . 5 ~ { \mathsf { k } } \Omega$ , then $80 \%$ of the nominal factory-programmed current limit is selected. If the input impedance is less than $1 0 0 ~ \mathsf { k } \Omega$ , but greater than $5 0 ~ \mathsf { k } \Omega$ , then $60 \%$ of the nominal factory-programmed current limit is selected.

Connect the RFB_PG(TOP) and RFB_PG(BOTTOM) resistors as indicated in this section for proper operation of the LDO. Do not float this pin.

When initialization is complete, the voltage divider provides the necessary feedback to the PG pin by setting the PG assert threshold voltage.

To properly select the values of the $\mathsf { R } _ { \mathsf { F B \_ P G ( T O P ) } }$ and RFB PG(BOTTOM) resistors, see the Adjusting the FactoryProgrammed Current Limit section for detailed explanation and calculation.

# Note

The $\mathsf { R } _ { \mathsf { F B \_ P G ( T O P ) } }$ and $\mathsf { R } _ { \mathsf { F B \_ P G } ( \mathsf { B O T T O M } ) }$ resistor divider ratio sets the power-good assert threshold voltage between $8 5 \%$ to $9 5 \%$ of the $V _ { F B \_ P G }$ voltage for $60 \%$ and $80 \%$ of the nominal factoryprogrammed current limit.

If the current limit is set for $100 \%$ of the nominal factory-programmed current limit, the PG threshold range is not limited. A PG threshold greater than $80 \%$ is common for system where start-up inrush current must be minimized. Lower PG thresholds can be needed in systems with fast start-up time constraints.

Setting the PG threshold based off the $V _ { F B } \textsf { P G }$ voltage sets the PG to assert when the output voltage reaches the corresponding percentage level of $V _ { F B \_ P G }$ because $V _ { F B } \mathsf { \ p } _ { \mathsf { G } }$ is a scaled version of the output voltage. Figure 8-8 shows the internal circuitry for both the FB_PG and PG pins.

![](images/a3168c4c2552237dd17235bc57e926e9148e037234032ffcefd42ebd6132d5fe.jpg)  
Figure 8-8. Programmable Power-Good Threshold Simplified Schematic

The PG pin pullup resistor value must be between $1 0 ~ \mathsf { k } \Omega$ and $1 0 0 ~ \mathsf { k } \Omega$ . The lower limit of $1 0 ~ \mathsf { k } \Omega$ results from the maximum pulldown strength of the power-good transistor, and the upper limit of $1 0 0 ~ \mathsf { k } \Omega$ results from the maximum leakage current at the power-good node. If the pullup resistor is outside of this range, then the power-good signal can possibly not read a valid digital logic level.

The state of the PG signal is only valid when the FB_PG pin resistor divider network is set properly and the device is in normal operating mode.

# 8.1.6 Adjusting the Factory-Programmed Current Limit

The current limit is a brick-wall scheme and the factory-programmed current limit value can be programmed to a set of discrete value ( $100 \%$ , $80 \%$ , or $60 \%$ of the default value), as specified in the Electrical Characteristics table. This adjustment can be done by changing the input impedance of the FB_PG pin represented by the parallel resistance of $\mathsf { R } _ { \mathsf { F B \_ P G } ( \mathsf { T O P } ) }$ $\mathsf { 1 | R _ { F B \_ P G ( B O T T O M ) } }$ . The FB_PG pin has dual functionality: adjusting the $\mathsf { I } _ { \mathsf { C L } }$ value and setting the power-good (PG) assert threshold.

Prior to start up, the input impedance of the FB_PG pin is sampled and the $\mathsf { I } _ { \mathsf { C L } }$ value is adjusted based on the input impedance.

# Note

The current limit programmability is dependent on the output voltage. For voltages below $0 . 4 ~ \mathsf { V } ,$ , the current limit cannot be programmed. For voltages between $0 . 4 \mathrm { ~ V ~ }$ and $1 . 2 \lor ,$ , the current limit cannot be adjusted and is always set to $100 \%$ . Programmable Current Limit vs Output Voltage describes this behavior.

Programmable Current Limit vs Output Voltage   

<html><body><table><tr><td>NOVONTAGE TPUT</td><td>RFB_PG(BOTTOM) (k2)</td><td>PG(TOP) (kΩ) RFB_</td><td>IcL SETTING</td></tr><tr><td rowspan="3">1.2V VoUT(nom) M</td><td>PG(BOTTOM) 0.2 V / 16 μA RFB</td><td rowspan="4">PG(TOP) = PG(BOTTOM)× 0.2V × K 1) with K = PG threshold (%VouT) RFB RFB VOuT(nom)</td><td>100</td></tr><tr><td>PG(BOTTOM) 0.2 V / 4 μA RFB</td><td>80</td></tr><tr><td>PG(BOTTOM) 0.2 V / 2 μA RFB</td><td>60</td></tr><tr><td>< 1.2 V 0.4 V ≤ VoUT(nom) RFB</td><td>PG(BOTTOM) 0.2 V / 6 μA</td><td>100</td></tr><tr><td>< 0.4V VouT(nom)</td><td>N/A</td><td>N/A</td><td>N/A</td></tr></table></body></html>

able 8-1 provides values for various output voltages using $1 \%$ resistors.   
Table 8-1. Programmable Current Limit Voltage-Divider Current Settings   

<html><body><table><tr><td>NOMINAL OUTPUT VOLTAGE (V)</td><td>RFB_PG(BOTTOM) (kΩ)</td><td>PG(TOP) (kΩ) RFB_</td><td>IcL SETTING (%)</td><td>PG THRESHOLD (%)</td></tr><tr><td rowspan="3">= 1.2V VouT(nom)</td><td>12.4</td><td>51.1</td><td>100</td><td>85</td></tr><tr><td>49.9</td><td>205</td><td>80</td><td>85</td></tr><tr><td>100</td><td>412</td><td>60</td><td>85</td></tr><tr><td rowspan="3">=3.3V VouT(nom)</td><td>12.4</td><td>187</td><td>100</td><td>95</td></tr><tr><td>49.9</td><td>732</td><td>80</td><td>95</td></tr><tr><td>100</td><td>1470</td><td>60</td><td>95</td></tr><tr><td rowspan="3">= 5.1 V VouT(nom)</td><td>12.4</td><td>287</td><td>100</td><td>95</td></tr><tr><td>49.9</td><td>1150</td><td>80</td><td>95</td></tr><tr><td>100</td><td>2320</td><td>60</td><td>95</td></tr></table></body></html>

![](images/f2bb268ccd6ce2074597adcb3cd3e34f594bcdd6d7ec70de16ede9c49091d192.jpg)  
Figure 8-9 shows the different $\mathsf { I } _ { \mathsf { C L } }$ settings for a nominal $3 . 3 – \lor$ output voltage.   
Figure 8-9. Programmable Current Limit Behavior (Typical) for a 3.3-VOUT(nom)

# 8.1.7 Programmable Soft-Start and Noise-Reduction (NR/SS Pin)

The NR/SS pin is the input to the inverting terminal of the error amplifier, see the Functional Block Diagram. A resistor connected from this pin to GND sets the output voltage by the pin internal reference current INR/SS, VOUT $= \mathsf { I } _ { \mathsf { N R } / \mathsf { S S } } \times \mathsf { R } _ { \mathsf { N R } / \mathsf { S S } }$ Connecting a capacitor from this pin to GND significantly reduces the output noise, limits the input inrush-current, and soft-starts the output voltage. Use the minimum value or larger capacitor from NR/SS to ground as listed in the Electrical Characteristics table and place the NR/SS capacitor as close to the NR/SS and GND pins of the device as possible.

The device features a programmable, monotonic, voltage-controlled, soft-start circuit that is set to work with an external capacitor (CNR/SS). In addition to the soft-start feature, the CNR/SS capacitor also lowers the output voltage noise of the LDO. The soft-start feature can be used to eliminate power-up initialization probl ems. The controlled output voltage ramp also reduces peak inrush current during start up, minimizing start-up transients to the input power bus.

To achieve a monotonic start up, the device output voltage tracks the VNR/SS reference voltage until this reference reaches the set value (the set output voltage). The VNR/SS reference voltage is set by the RNR/SS resistor and, during start up, using a fast charging current (IFAST_SS) in addition to the INR/SS current, as shown in Figure 8-10, to charge the CNR/SS capacitor.

![](images/6983386a677f09f0d3671c68c2444102b7693bc397c9ac7f307f9a8f5be941d5.jpg)  
Figure 8-10. Simplified Soft-Start Circuit

The 2.1-mA (typical) IFAST_ SS current and $1 5 0 \mu \mathsf { A }$ (typical) INR/SS current quickly charge CNR/SS until the voltage reaches approximately $93 \%$ of the set output voltage, then the IFAST _SS current disengages and only the INR/SS current continues to charge CNR/SS to the set output voltage level. If there is any error during start up or the output overshoot prevention circuit is triggered, the NR/SS discharge FET turns on, thus discharging the CNR/SS capacitor to protect both the LDO and the load.

The soft-start ramp time depends on the fast start-up (IFAST_SS) charging current, the reference current (INR/SS), CNR/SS capacitor value, and the set (targeted) output voltage (VOUT(target)). Equation 3 calculates the soft-start ramp time.

Soft-Start Time $( \mathsf { t } _ { \mathsf { S S } } ) = ( \mathsf { V } _ { \mathsf { O U T } ( \mathsf { t a r g e t } ) } \times \mathsf { C } _ { \mathsf { N R } / \mathsf { S S } } ) / ( \mathsf { I } _ { \mathsf { N R } / \mathsf { S S } } + \mathsf { I } _ { \mathsf { F A S T } \_ \mathsf { S S } } )$

The INR/SS current is provided in the Electrical Characteristics table and has a value of $1 5 0 ~ \mu \mathsf { A }$ (typical). The IFAST_SS current has a value of $2 \mathsf { m A }$ (typical) for $\mathsf { V } _ { \mathsf { I N } } > 2 . 5 \mathsf { V } .$ Figure 8-11 and Figure 8-12 depict the INR/SS and IFAST SS current versus $\mathsf { V } _ { \mathsf { I N } }$ and temperature.

![](images/494c03f552a5895038deb8fec7216568b637b279102050d83811a1ef5cb2f424.jpg)  
Figure 8-11. INR/SS Reference vs Input Voltage and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

![](images/bce84103533ef41cb96925dd11ae3f4dceaaee8bf14f3f39f217fcd726367e92.jpg)  
Figure 8-12. IFAST_SS Reference vs Input Voltage and Temperature for $\mathsf { v } _ { \mathsf { o u r } } = 3 . 3 \ : \mathsf { V }$

Because the error amplifier is always operating in unity-gain configuration, the output voltage noise can only be adjusted by increasing the CNR/SS capacitor. The CNR/SS capacitor and RNR/SS resistor form a low-pass filter (LPF) that filters out the noise from the VNR/SS voltage reference, thereby reducing the device noise floor. The LPF is a single-pole filter and Equation 4 calculates the LPF cutoff frequency. Increasing the CNR/SS capacitor can significantly lower output voltage noise; however, doing so greatly lengthens start-up time. For low-noise applications, use a $4 . 7 \mathsf { - } \mu \mathsf { F }$ CNR/SS for optimal noise and start-up time trade off.

The Typical Characteristics section illustrates the impact of the CNR/SS capacitor on the LDO output voltage noise.

Figure 8-13 illustrates the relationship, timing, and output voltage value during the start-up phase.

![](images/b3eaa7e6b1e78393495230dd943cb9edac3c8bc4d7ea2ebc86656c6de09bc2ad.jpg)  
Figure 8-13. Relationship Between Threshold Voltage, Output Voltage, IFAST_SS, and INR/SS During StartUp

# 8.1.8 Inrush Current

Inrush current is defined as the current into the LDO at the IN pin during start up. Inrush current then consists primarily of the sum of load current and the current used to charge the output capacitor. This current is difficult to measure because the input capacitor must be removed. Operating without an input capacitor is not recommended because this capacitor is required for stability. However, Equation 5 can be used to estimate this current.

$$
\mathsf { I } _ { \mathsf { O U T } ( \mathsf { t } ) } = \left[ \frac { \mathsf { C } _ { \mathsf { O U T } } \times \mathsf { d V } _ { \mathsf { O U T } } ( \mathsf { t } ) } { \mathsf { d t } } \right] + \left[ \frac { \mathsf { V } _ { \mathsf { O U T } } ( \mathsf { t } ) } { \mathsf { R } _ { \mathsf { L O A D } } } \right]
$$

where:

$\mathsf { V } _ { \mathsf { O U T } } ( \mathsf { t } )$ is the instantaneous output voltage of the turn-on ramp $\mathsf { d V } _ { \mathsf { O U T } } ( \mathsf { t } ) I$ dt is the slope of the VOUT ramp $R _ { \mathsf { L O A D } }$ is the resistive load impedance

As illustrated in Figure 8-10, the external capacitor at the NR/SS pin (CNR/SS) sets the output start-up time by setting the rise time of the VNR/SS reference voltage.

Inrush current for a no-load condition is given in Figure 6-37 to Figure 6-40.

# 8.1.9 Optimizing Noise and PSRR

Noise can be generally defined as any unwanted signal combining with the desired signal (such as the regulated LDO output). Noise can easily be noticed in audio as a hissing or popping sound. Noise produced from an external circuit or the 50- to 60-hertz power-line noise (spikes), along with the harmonics, is an excellent representative of extrinsic noise. Intrinsic noise is produced by components within the device circuitry, such as resistors and transistors. The two dominating sources of intrinsic noise are the error amplifier and the internal reference voltage $( V _ { \mathsf { N R / S S } } )$ . Extrinsic noise, including the switching mode power-supply ac ripple voltage, coupled onto the input supply of the LDO is attenuated by the LDO power-supply rejection ratio, or PSRR. PSRR is a measurement of the noise attenuation from the input to the output of the LDO.

Optimize the intrinsic noise and PSRR by carefully selecting:

CNR/SS for the low-frequency range up to the device bandwidth   
COUT for the high-frequency range close to and higher than the device bandwidth   
Operating headroom, $\mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { O U T } } ( \mathsf { V } _ { \mathsf { D O } } )$ , mainly for the low-frequency range up to the device bandwidth, but   
also for higher frequencies to a lesser effect

These behaviors are described in the Typical Characteristics curves.

Figure 8-14 and Figure 8-15 show the measured $1 0 - H z$ to 100-kHz RMS noise for a 3.3-V device output voltage with a $0 . 5 – \lor$ headroom for different CNR/SS and COUT capacitors and a 1-A load current. Table 8-2 lists the typical output noise for these capacitors.

![](images/a7c2ea3e67cb8b2361b6bd8fd49f83345d14758ff70a20d5d3fb28d64358e9ca.jpg)  
Figure 8-14. PSRR vs Frequency and IOUT for VOUT

![](images/2e86873dc5e26015f03fec8b208e0d366a7c5461b7bc88274488026074e1d49c.jpg)  
Figure 8-15. PSRR vs Frequency and IOUT for VOUT = 3.3 V, COUT = 4.7 μF || 4.7 μF|| 1.0 μF

Table 8-2. Typical Output Noise for 3.3-VOUT vs CNR/SS, COUT, and Typical Start-Up Time   

<html><body><table><tr><td>Vn (μVRms), 10-Hz to 100-kHz BW</td><td>(μF) CNR/SS</td><td>(μF) CouT</td><td>START-UP TIME (ms)</td></tr><tr><td>0.98</td><td></td><td>10</td><td>3.73</td></tr><tr><td>0.62</td><td>2.2</td><td>10</td><td>6.21</td></tr><tr><td>0.46</td><td>4.7</td><td>10</td><td>13.97</td></tr><tr><td>0.42</td><td>10</td><td>10</td><td>28.21</td></tr></table></body></html>

PSRR can be viewed as being simply the ratio of the output capacitor impedance by the LDO output impedance. At low frequency, the output impedance is very low whereas the output impedance of the capacitor is high, resulting in high PSRR. As the frequency increases, the output capacitor impedance reduces and reaches a minima set by the ESR.

As shown in Figure 8-14 and Figure 8-15, and in order to achieve high PSRR at high frequencies, ensure that the output capacitor ESR and ESL are minimal. These figures compare the use of a single $1 0 \mathrm { - } \mu \mathsf { F }$ output capacitor with a 4.7-μF || 4.7-μF || $1 . 0 \mathrm { - } \mu \mathsf { F }$ implementation. Notice that below $2 0 0 k H z$ , there is no impact on performance but above $2 0 0 ~ \mathsf { k H z }$ , the PSRR improves by 5 dB to 7 dB.

Minimizing the ESR, ESL generated resonance point in the output capacitance allows for a smoother transition between the LDO active PSRR component to the passive PSRR of the capacitors.

# 8.1.10 Adjustable Operation

As shown in Figure 8-16, the output voltage of the device can be set using a single external resistor (RNR/SS).   
Equation 6 calculates the output voltage.

$$
\mathsf { V } _ { \mathsf { O U T } } = \mathsf { I } _ { \mathsf { N R / S S ( N O M ) } } \times \mathsf { R } _ { \mathsf { N R / S S } }
$$

![](images/834048315ed5c4b7dddb509e91cd8c937cddbf9d53f99d1a3e0dac4176f1deac.jpg)  
Figure 8-16. Typical Circuit

Table 8-3 shows the recommended RNR/SS resistor values to achieve several common rails using a standard $1 \%$ -tolerance resistor.

Table 8-3. Recommended RNR/SS Values   

<html><body><table><tr><td>TARGETED OUTPUT VOLTAGE (M)</td><td>RNR/Ss (kΩ)</td><td>CALCULATED OUTPUT VOLTAGE (M)</td></tr><tr><td>0.4</td><td>2.67</td><td>0.4005</td></tr><tr><td>0.5</td><td>3.32</td><td>0.498</td></tr><tr><td>0.6</td><td>4.02</td><td>0.603</td></tr><tr><td>0.7</td><td>4.64</td><td>0.696</td></tr><tr><td>0.8</td><td>5.36</td><td>0.804</td></tr><tr><td>0.9</td><td>6.04</td><td>0.906</td></tr><tr><td>1.0</td><td>6.65</td><td>0.9975</td></tr><tr><td>1.2</td><td>8.06</td><td>1.209</td></tr><tr><td>1.5</td><td>10.0</td><td>1.5</td></tr><tr><td>2.5</td><td>16.5</td><td>2.475</td></tr><tr><td>3.0</td><td>20.0</td><td>3.0</td></tr><tr><td>3.3</td><td>22.1</td><td>3.315</td></tr><tr><td>3.6</td><td>24.3</td><td>3.645</td></tr><tr><td>4.7</td><td>31.6</td><td>4.74</td></tr><tr><td>5.0</td><td>33.2</td><td>4.98</td></tr></table></body></html>

# Note

To avoid engaging the current limit during start-up with a large $\mathsf { C o u r }$ capacitor, make sure tha

1. A minimum NR/SS capacitor of $1 \mu \mathsf { F }$ is used   
2. When the output capacitor is greater than ${ 1 0 0 \mu \mathsf { F } } _ { \cdot }$ , maintain a COUT to CNR/SS ratio < 100

Because the set resistor is also placed on the NR/SS pin, consider using a thin-film resistor and provide enough resistor temperature drift to ensure the targeted accuracy.

# 8.1.11 Paralleling for Higher Output Current and Lower Noise

Achieving higher output current and lower noise is achievable by paralleling two or more LDOs. Implementation must be carefully planned out to optimize performance and minimize output current imbalance.

Because the TPS7A94 output voltage is set by a resistor driven by a current source, the NR/SS resistor and capacitor must be adjusted as per the following:

RNR/SS _parallel $\mathbf { \sigma } = \mathbf { \sigma }$ VOUT _TARGET (n × INR/SS)

CNR/SS _parallel = n × CNR/SS single

where:

n is the number of LDOs in parallel   
INR/SS is the NR/SS current as provided in the data sheet Electrical Characteristics table   
CNR/SS single is the NR/SS capacitor for a single LDO

When connecting the input and NR/SS pin together, and with the LDO being a buffer, the current imbalance is only affected by the error offset voltage of the error amplifier. As such, the current imbalance can be expressed as:

$$
\varepsilon _ { \mathsf { I } } = { \mathsf { V } } _ { \mathsf { O S } } \times 2 \times { \mathsf { R } } _ { \mathsf { B A L L A S T } } / ( { \mathsf { R } } _ { \mathsf { B A L L A S T } } ^ { 2 } - \Delta { \mathsf { R } } _ { \mathsf { B A L L A S T } } ^ { 2 } ) ^ { 2 }
$$

where:

$\mathfrak { E } _ { \mathrm { l } }$ is the current imbalance   
VOS is the LDO error offset voltage   
RBALLAST is the ballast resistor   
ΔRBALLAST is the deviation of the ballast resistor value from the nominal value

With the typical offset voltage of $2 0 0 ~ \mu \nu ,$ considering no error from the design of the PCB ballast resistor $( \Delta R _ { \mathsf { B A L L A S T } } = 0 )$ ) and a 100-mA maximum current imbalance, the ballast resistor must be $4 \ m \Omega$ or greater; see Figure 8-17.

Using the configuration described, the LDO output noise is reduced by:

$$
\mathsf { e } _ { 0 \_ \mathsf { p a r a l l e l } } = ( 1 \mathrm { ~ / ~ } \sqrt { \mathsf { n } } ) \times \mathsf { e } _ { 0 \_ \mathsf { s i n g l e } }
$$

where:

n is the numbers of LDOs in parallel eO _single is the output noise density from a single LDO eO_ parallel is the output noise density for the resulting parallel LDO

In Figure 8-17, the noise is reduced by 1 / √2.

![](images/b70fafe60a4f8c9d6a16fcbf87937d6ac678ce81deac0031f151abcff786957e.jpg)  
Figure 8-17. Paralleling Multiple TPS7A94 Devices

# 8.1.12 Recommended Capacitor Types

The device is designed to be stable using low equivalent series resistance (ESR) and low equivalent series inductance (ESL) ceramic capacitors at the input, output, and noise-reduction pin. Multilayer ceramic capacitors have become the industry standard for these applications and are recommended, but must be used with good judgment. Ceramic capacitors that employ X7R-, X5R-rated, or better dielectric materials provide relatively good capacitive stability across temperature. The use of Y5V-rated capacitors is discouraged because of large variations in capacitance.

Regardless of the ceramic capacitor type selected, ceramic capacitance varies with operating voltage and temperature. The input and output capacitors recommended herein account for a capacitance derating of approximately $50 \%$ , but at high $\mathsf { V } _ { \mathsf { I N } }$ and $\mathsf { V } _ { \mathsf { O U T } }$ conditions $( \mathsf { V } _ { | \mathsf { N } } = 5 . 5 ~ \mathsf { V }$ to $\mathsf { V } _ { \mathsf { O U T } } = 5 . 0 \ \mathsf { V } ,$ ), the derating can be greater than $50 \%$ , which must be taken into consideration.

The device requires input, output, and noise-reduction capacitors for proper operation of the LDO. Use the nominal or larger than the nominal input, and output capacitors as specified in the Recommended Operating Conditions table. Place input and output capacitors as close as possible to the corresponding pin and make the capacitor GND connections as close as possible to the device GND pin to minimize PCB loop inductance, thus reducing transient voltage spikes during a load step.

As illustrated in Figure 8-15, multiple parallel capacitors can be used to lower the impedance present on the line. This capacitor counteracts input trace inductance, improves transient response, and reduces input ripple and noise. Using an output capacitor larger than the typical value can also improve the transient response.

# 8.1.13 Load Transient Response

# Note

For best transient response, use the nominal value or larger capacitor from OUT to ground as listed in the Recommended Operating Conditions table. Place the output capacitor as close to the OUT and GND pins of the device as possible.

For best transient response and to minimize input impedance, use the nominal value or larger capacitor from IN to ground as listed in the Recommended Operating Conditions table. Place the input capacitor as close to the IN and GND pins of the device as possible.

The load-step transient response is the LDO output voltage response to load current changes. There are two key transitions during a load transient response: the transition from a light to a heavy load, and the transition from a heavy to a light load. The regions shown in Figure 8-18 are broken down in this section. Regions A, E, and H are where the output voltage is in steady-state regulation.

![](images/64093565fccbb84f1a2f4bdfbf36d2f12462f9ad78c6e4a2118df4430a8cb74b.jpg)  
Figure 8-18. Load Transient Waveform

During transitions from a light load to a heavy load:

The initial voltage dip is a result of the depletion of the output capacitor charge and parasitic impedance to the output capacitor (region B)   
Recovery from the dip results from the LDO increasing the sourcing current, and leads to output voltage regulation (region C)

During transitions from a heavy load to a light load:

The initial voltage rise results from the LDO sourcing a large current, and leads to the output capacitor charge   
to increase (region F)   
Recovery from the rise results from the LDO decreasing the sourcing current in combination with the load   
discharging the output capacitor (region G)

Transitions between current levels changes the internal power dissipation because the device is a high-current device (region D). The change in power dissipation changes the die temperature during these transitions, and leads to a slightly different voltage level. This temperature-dependent output voltage level shows up in the various load transient responses.

A larger output capacitance reduces the peaks during a load transient but slows down the response time of the device. A larger dc load also reduces the peaks because the amplitude of the transition is lowered and a higher current discharge path is provided for the output capacitor.

# 8.1.14 Power Dissipation $( P _ { \mathsf { D } } )$

Circuit reliability demands that proper consideration be given to device power dissipation, location of the circuit on the printed circuit board (PCB), and correct sizing of the thermal plane. The PCB area around the regulator must be as free as possible of other heat-generating devices that cause added thermal stresses.

As a first-order approximation, power dissipation in the regulator depends on the input-to-output voltage difference and load conditions. Equation 11 calculates $\mathsf { P } _ { \mathsf { D } }$ :

$$
\mathsf { P } _ { \mathsf { D } } = ( \mathsf { V } _ { \mathsf { O U T } } - \mathsf { V } _ { \mathsf { I N } } ) \times \mathsf { I } _ { \mathsf { O U T } }
$$

# Note

Power dissipation can be minimized, and thus greater efficiency achieved, by proper selection of the system voltage rails. Proper selection allows the minimum input-to-output voltage differential to be obtained. The low dropout of the device allows for maximum efficiency across a wide range of output voltages.

The primary heat conduction path for the package is through the thermal pad to the PCB. Solder the thermal pad to a copper pad area under the device. This pad area contains an array of plated vias that conduct heat to any inner plane areas or to a bottom-side copper plane.

The power dissipation by the device determines the junction temperature $( \mathsf { T } _ { \mathsf { J } } )$ for the device. Power dissipation and junction temperature are most often related by the junction-to-ambient thermal resistance $( \mathsf { R } _ { \Theta \ J { \mathsf { A } } } )$ of the combined PCB and device package and the temperature of the ambient air $( T _ { \mathsf { A } } )$ , according to Equation 12. This equation is rearranged for output current in Equation 13.

$$
\begin{array} { r l } & { \mathsf { T } _ { \mathsf { J } } = \mathsf { T } _ { \mathsf { A } } = ( \mathsf { R } _ { \mathsf { \theta J A } } \times \mathsf { P } _ { \mathsf { D } } ) } \\ & { \mathsf { I } _ { \mathsf { O U T } } = ( \mathsf { T } _ { \mathsf { J } } - \mathsf { T } _ { \mathsf { A } } ) / [ \mathsf { R } _ { \mathsf { \theta J A } } \times ( \mathsf { V } _ { \mathsf { I N } } - \mathsf { V } _ { \mathsf { O U T } } ) ] } \end{array}
$$

This thermal resistance $( \mathsf { R } _ { \Theta \ J \mathsf { A } } )$ is highly dependent on the heat-spreading capability built into the particular PCB design, and therefore varies according to the total copper area, copper weight, and location of the planes. The $\mathsf { R } _ { \Theta \ J _ { A } }$ recorded in the Thermal Information table is determined by the JEDEC standard, PCB, and copperspreading area, and is only used as a relative measure of package thermal performance. For a well-designed thermal layout, RθJA is actually the sum of the DSC package junction-to-case (bottom) thermal resistance $( \mathsf { R } _ { \Theta \mathsf { J C b o t } } )$ plus the thermal resistance contribution by the PCB copper.

# 8.1.15 Estimating Junction Temperature

The JEDEC standard now recommends the use of psi $( \Psi )$ thermal metrics to estimate the junction temperatures of the LDO when in-circuit on a typical PCB board application. These metrics are not strictly speaking thermal resistances, but rather offer practical and relative means of estimating junction temperatures. These psi metrics are determined to be significantly independent of the copper-spreading area. The key thermal metrics ( $\cdot \Psi _ { \ J \top }$ and ΨJB) are used in accordance with Equation 14 and are given in the Thermal Information table.

$$
\begin{array} { c } { { \Psi _ { \mathrm { J T } } \mathrm { : } \mathrm { } \mathrm { } \sf T _ { \mathrm { J } } = \sf T _ { \mathrm { T } } + \Psi _ { \mathrm { J T } } \mathrm { \times } \sf P _ { \mathrm { D } } } } \\ { { \Psi _ { \mathrm { J B } } \mathrm { : } \mathrm { } \sf T _ { \mathrm { J } } = \sf T _ { \mathrm { B } } + \Psi _ { \mathrm { J B } } \mathrm { \times } \sf P _ { \mathrm { D } } } } \end{array}
$$

where:

$\mathsf { P } _ { \mathsf { D } }$ is the power dissipated as explained in the Power Dissipation $( P _ { D } )$ section   
$\mathsf { T } _ { \mathsf { T } }$ is the temperature at the center-top of the device package   
$T _ { \mathsf { B } }$ s the PCB surface temperature measured 1 mm from the device package and centered on the package   
edge

# 8.1.16 TPS7A94EVM-046 Thermal Analysis

The TPS7A94EVM-046 EVM was used to develop the TPS7A9401DRC thermal model. The DRC package is a ${ 3 \cdot \mathsf { m m } } \times 3 \cdot { \mathsf { m m } }$ , 10-pin VQFN with $2 5 \AA - \mu \ m$ plating on each via. The EVM is a 2.85-inch $\times$ 3.35-inch $( 7 2 . 3 9 \ \mathsf { m m } \ \times \$ $8 5 . 0 9 ~ \mathrm { { m m } } ,$ PCB comprised of four layers. Table 8-4 lists the layer stackup for the EVM. Figure 8-19 to Figure 8-23 illustrate the various layer details for the EVM.

Table 8-4. TPS7A94EVM-046 PCB Stackup   

<html><body><table><tr><td>LAYER</td><td>NAME</td><td>MATERIAL</td><td>THICKNESS (mil)</td></tr><tr><td></td><td>Top overlay</td><td></td><td></td></tr><tr><td>2</td><td>Top solder</td><td>Solder resist</td><td>0.4</td></tr><tr><td>3</td><td>Top layer</td><td>Copper</td><td>2.8</td></tr><tr><td>4</td><td>Dielectric 1</td><td>FR-4 Tg high</td><td>10</td></tr><tr><td>5</td><td>Mid layer</td><td>Copper</td><td>2.8</td></tr><tr><td></td><td>Dielectric 2</td><td>FR-4 high Tg</td><td>30</td></tr><tr><td>67</td><td>Mid layer 2</td><td>Copper</td><td>2.8</td></tr><tr><td>8</td><td>Dielectric 3</td><td>FR-4 high Tg</td><td>10</td></tr><tr><td>9</td><td>Bottom layer</td><td>Copper</td><td>2.8</td></tr><tr><td>10</td><td>Bottom solder</td><td>Solder resist</td><td>0.4</td></tr></table></body></html>

![](images/1cc3f02b7b6fc43a3ad0a7bb25c5c9a9951ae2381ee47c28f0a8d4417af2302f.jpg)  
Figure 8-19. Top Composite View

![](images/59b0961fbea211025ff481697ea90ac8d74fa3d9b1c49732d590b3b0f17e165d.jpg)  
Figure 8-20. Top Layer Routing

![](images/98b7acf80514dddef1ee5c1698bc229a2ab58469df4f5d457df31eddeb5478c2.jpg)  
Figure 8-21. Mid Layer Routing

![](images/aa4ec3b80dc2df5211234513c86f94df26ba9e744a953efd4d04b4b0ad81cd68.jpg)  
Figure 8-22. Mid Layer 2 Routing

![](images/118284ccd966f089461e6c22da8a2df09109e2614fe169bd396f8d18d7917db0.jpg)  
Figure 8-23. Bottom Layer Routing

Figure 8-24 to Figure 8-26 show the thermal gradient on the PCB and device that results when a 1-W power dissipation is used through the pass transistor with a $2 5 ^ { \circ } \mathsf { C }$ ambient temperature. Table 8-5 shows thermal simulation data for the TPS7A94EVM-046.

Table 8-5. TPS7A94EVM-046 Thermal Simulation Data   

<html><body><table><tr><td>DUT</td><td>RθJA (°C/W)</td><td>ψJB (°C/W)</td><td>ψJT (°C/W)</td></tr><tr><td>TPS7A94EVM-046</td><td>25.6</td><td>11.5</td><td>0.3</td></tr></table></body></html>

![](images/dcb1d8e47ac0d6996a6c91e0a81150ca02fb715c5272e94434f9c64e84f36d9f.jpg)  
Figure 8-24. TPS7A94EVM-046 3D View

![](images/a352070e5e433c9208257e44e7f362210c436d53708d879d345878f820e5c331.jpg)  
Figure 8-25. TPS7A94EVM-046 PCB Thermal Gradient

![](images/d49311bd526879f7719e4a0c4b863aea312a689cb274db85f6af7aa83e5e550a.jpg)  
Figure 8-26. TPS7A94EVM-046 Device Thermal Gradient

# 8.2 Typical Application

![](images/f5ffd3f66fb1eede768359f8c7bbfb40b05b42c7bdf67c0bd2ac66b11c24b805.jpg)  
Figure 8-27. Typical Application Circuit

![](images/6f93a63ce84f810cdcf511dd530da0c3f17f1d06067dc3c478f2f4463615f917.jpg)  
Figure 8-28. Typical Application Circuit With Added Pi-Filter

# 8.2.1 Design Requirements

Table 8-6 lists the required application parameters for this design example.

Table 8-6. Design Parameters   

<html><body><table><tr><td>PARAMETER</td><td>DESIGN REQUIREMENT</td></tr><tr><td>Input voltage</td><td>Vin ≥ 5 V, ±3%, provided by the dc/dc converter switching at 1 MHz</td></tr><tr><td>Output voltage</td><td>3.3V, ±1%</td></tr><tr><td>Output current</td><td>500 mA (maximum), 300 mA (minimum)</td></tr><tr><td>Current limit</td><td>750mA</td></tr><tr><td>PG threshold</td><td>95%</td></tr><tr><td rowspan="2">Targeted spectral noise</td><td>Targeted noise compliance mask Zone 1 (10 Hz to 100 Hz): Spectral noise ≤ 100 nV/VHz</td></tr><tr><td>Zone 2 (100 Hz to kHz): Spectral noise ≤ 10 nV/VHz</td></tr><tr><td>PSRR at 1 MHz</td><td>Zone 3 (> kHz): Spectral noise ≤ 3 nV/VHz 50 dB at max load current 7</td></tr><tr><td>Start-up environment</td><td>Device to be enabled when VN ≥ 80% × Target Device to be disabled when Vin <80% × Target VN_T ViN</td></tr></table></body></html>

# 8.2.2 Detailed Design Procedure

In this design example, the device is powered by a ${ \mathsf { d c } } / { \mathsf { d c } }$ convertor switching at 1 MHz. The load requires a 3.3-V clean rail with the spectral noise mask versus frequency shown in Figure 8-29 and a maximum load of $5 0 0 ~ \mathsf { m A }$ . The typical $1 0 \mathrm { - } \mu \mathsf { F }$ input and output capacitors and $4 . 7 \mathsf { - } \mu \mathsf { F }$ NR/SS capacitors are used to achieve a good balance between fast start-up time and excellent noise and PSRR performance.

![](images/0232a6a3b55b26cc1a743a4bff1c83b24de29c211620d33f45f94f6d0808bb8b.jpg)  
Figure 8-29. Noise Compliance Mask

The output voltage is set using a $2 2 . 1 \mathrm { - } \mathsf { k } \Omega$ , thin-film resistor value calculated as described in the Adjustable Operation section. To set the current limit to a value close to the $7 5 0 ~ \mathsf { m A }$ required by the application, and to set the PG threshold to $9 5 \%$ , use Table 8-1 to set the $\mathsf { R } _ { \mathsf { F B \_ P G } }$ top and bottom resistors values at 1.47 MΩ and $1 0 0 \mathsf { k } \Omega$ , respectively.

Setting $R _ { \mathsf { B } }$ to $1 0 0 ~ \mathsf { k } \Omega$ and using a 4-V $\mathsf { V } _ { \mathsf { O N } }$ and Equation 1 provide the $\mathsf { R } _ { \mathsf { T } }$ value of $2 2 6 ~ \mathsf { k } \Omega$ . VOFF is calculated with Equation 2 to be $3 . 5 \mathsf { V } .$

Figure 8-30 shows that the device meets all design noise requirements except for the noise peaking at $9 0 0 ~ \mathsf { k H z }$ . However, this noise peaking can be easily attenuated to the required noise level by means of a pi-filter positioned after the LDO. Figure 8-31 shows that this design is very close to the PSRR level at 1 MHz and can require more margin. Fortunately, both requirements are easily achieved by inserting a pi-filter consisting of a ferrite bead and a small capacitor beyond the LDO and before the load; see Figure 8-28.

The ferrite bead was selected to have a very small dc resistance of less than $5 0 ~ \mathsf { m } \Omega$ , 1 A of current rating, and a relatively small footprint. The added pi-filter components have almost no impact on the LDO accuracy performance and no significant increase in the design total cost.

![](images/314cd97ec442b19dadd2332d4a81e2a8699915b0d35b69b1fd4f1ecf8890176d.jpg)  
Figure 8-30. Output Noise vs Frequency

![](images/9e4734042c552bc8f69c3f43b5db9939f5dd1cdce030de3a6049d0c34f81c4c8.jpg)  
Figure 8-31. PSRR vs Frequency

# 8.2.3 Application Curves

Figure 8-32 and Figure 8-33 show the design noise and PSRR performance after inserting the pi-filter.

![](images/7e1db96fab04aa003ad753cc5b68374ce6bade219a2204b18b86a25da42ee491.jpg)  
Figure 8-32. Noise vs Frequency

![](images/ca356e1a268c1c413b0707f9a54f3d9de0808ed341c50d9c4d46156c732853c4.jpg)  
Figure 8-33. PSRR vs Frequency

# 8.3 Power Supply Recommendations

The device is designed to operate from an input voltage supply ranging from $1 . 7 \mathrm { V }$ to ${ \mathsf { 5 . 7 ~ V . } }$ Ensure that the input voltage range provides adequate operational headroom for the device to have a regulated output. This input supply must be well regulated. If the input supply is noisy, use additional input capacitors with low ESR and increase the operating headroom to achieve the desired output noise, PSRR, and load transient performance.

# 8.4 Layout

# 8.4.1 Layout Guidelines

# 8.4.1.1 Board Layout

For good thermal performance, connect the thermal pad to a large-area GND plane.

Kelvin connects the SNS pin through a low-impedance connection to the output capacitor and load for optima transient performance. Do not float this pin.

Connect the GND pin to the device thermal pad and connect both this pin and the thermal pad to the ground on the board through a low-impedance connection.

For best overall performance, place all circuit components on the same side of the circuit board and as near as practical to the respective LDO pin connections. Place ground return connections to the input and output capacitor, and to the LDO ground pin as close to each other as possible, connected by a wide, component-side, copper surface. To avoid negative system performance, do not use vias or long traces to the input and output capacitors. The grounding and layout scheme described in Figure 8-34 minimizes inductive parasitics, and thereby reduces load-current transients, minimizes noise, and increases circuit stability.

To improve performance, use a ground reference plane, either embedded in the printed circuit board (PCB) or placed on the bottom side of the PCB opposite the components. This reference plane serves to assure accuracy of the output voltage, shield noise, and behaves similar to a thermal plane to spread (or sink) heat from the LDO device when connected to the thermal pad. In most applications, this ground plane is necessary to meet thermal requirements.

# 8.4.1.2 Layout Example

![](images/def647c84c46a5f994e79a9a6ba486dbd82b58f3d687901d2193e9cd3690a7b8.jpg)  
Figure 8-34. Example Layout

# 9 Device and Documentation Support 9.1 Device Support

# 9.1.1 Development Support

# 9.1.1.1 Evaluation Modules

An evaluation module (EVM) is available to assist in the initial circuit performance evaluation using the PS7A94. Table 9-1 shows the summary information for this fixture.

Table 9-1. Design Kits and Evaluation Modules   

<html><body><table><tr><td>NAME</td><td>LITERATURE NUMBER</td></tr><tr><td>TPS7A94EVM-046 evaluation module</td><td>SBVU070</td></tr></table></body></html>

The EVM can be requested at the Texas Instruments web site through the TPS7A94 product folder.

# 9.1.1.2 Spice Models

Computer simulation of circuit performance using SPICE is often useful when analyzing the performance of analog circuits and systems. A SPICE model for the TPS7A94 is available through the TPS7A94 product folder under simulation models.

# 9.1.2 Device Nomenclature

Table 9-2. Ordering Information(1)   

<html><body><table><tr><td>PRODUCT</td><td>DESCRIPTION</td></tr><tr><td>TPS7A9401 yyy z</td><td></td></tr></table></body></html>

(1) For the most current package and ordering information see the Package Option Addendum at the end of this document, or see the device product folder at www.ti.com.

# 9.2 Documentation Support

# 9.2.1 Related Documentation

For related documentation see the following:

Texas Instruments, TPS3702 High-Accuracy, Overvoltage and Undervoltage Monitor data sheet Texas Instruments, TPS7A94EVM-046 Evaluation Module user guide Texas Instruments, High-Current, Low-Noise Parallel LDO reference design

# 9.3 Receiving Notification of Documentation Updates

To receive notification of documentation updates, navigate C the device product folder on ti.com. Click on Subscribe to updates to register and receive a weekly digest of any product information that has changed. For change details, review the revision history included in any revised document.

# 9.4 Support Resources

TI E2E™ support forums are an engineer's go-to source for fast, verified answers and design help — straight from the experts. Search existing answers or ask your own question to get the quick design help you need.

Linked content is provided "AS IS" by the respective contributors. They do not constitute TI specifications and do not necessarily reflect TI's views; see TI's Terms of Use.

# 9.5 Trademarks

TI E2E™ is a trademark of Texas Instruments.   
All trademarks are the property of their respective owners.

# 9.6 Electrostatic Discharge Caution

![](images/aa93e0c23597de89c6f5ccf8ac251ed8f78ff79b0dd0acb419744fd9d80c9111.jpg)

This integrated circuit can be damaged by ESD. Texas Instruments recommends that all integrated circuits be handled with appropriate precautions. Failure to observe proper handling and installation procedures can cause damage.

ESD damage can range from subtle performance degradation to complete device failure. Precision integrated circuits may be more susceptible to damage because very small parametric changes could cause the device not to meet its published specifications.

# 9.7 Glossary

# TI Glossary

This glossary lists and explains terms, acronyms, and definitions.

# 10 Mechanical, Packaging, and Orderable Information

The following pages include mechanical, packaging, and orderable information. This information is the most current data available for the designated devices. This data is subject to change without notice and revision of this document. For browser-based versions of this data sheet, refer to the left-hand navigation.

# 10.1 Mechanical Data

![](images/4b661345be8133a8418122c8277b45dabe2ff40f28b70302039a161c252e8dfe.jpg)

PLASTIC SMALL OUTLINE NO LEAD

![](images/50025c3b5dec6e5423a05068b6ffb15bd1fd9acb71304fe68b31a9e9ea6dea9a.jpg)

NOTES:

1. All linear dimensions are in millimeters. Any dimensions in parenthesis are for reference only. Dimensioning and tolerancing per ASME Y14.5M.   
2. This drawing is subject to change without notice.   
3. The package thermal pad must be soldered to the printed circuit board for thermal and mechanical performance.

# EXAMPLE BOARD LAYOUT

WSON 0.8 mm max height

PLASTIC SMALL OUTLINE NO LEAD

![](images/43bb80d929d983e853429e2bb518af37004f85c921ca43e4b33cb8eec02899e8.jpg)  
NOTES: (continued)

4. This package is designed to be soldered to a thermal pad on the board. For more information, see Texas Instruments literature number SLUA271 (www.ti.com/lit/slua271).

# EXAMPLE STENCIL DESIGN

WSON 0.8 mm max height

PLASTIC SMALL OUTLINE NO LEAD

![](images/06443fc0f20d4807cfd81e46301dbbb8a146fe471a22e25e23dbd02e982bb272.jpg)

12/2014

NOTES: (continued)

5. Laser cutting apertures with trapezoidal walls and rounded corners may offer better paste release. IPC-7525 may have alternate design recommendations.

# PACKAGING INFORMATION

<html><body><table><tr><td rowspan="2">Orderable Device</td><td rowspan="2">Status (1)</td><td rowspan="2">Package Type</td><td rowspan="2">Package Drawing</td><td rowspan="2">Pins</td><td rowspan="2">Package Qty</td><td rowspan="2">Eco Plan (2)</td><td rowspan="2">Lead finish/ Ball material</td><td rowspan="2">MSL Peak Temp (3)</td><td rowspan="2">Op Temp (°C)</td><td rowspan="2">Device Marking (4/5)</td><td rowspan="2">Samples</td></tr><tr><td></td></tr><tr><td>TPS7A9401DSCR</td><td>ACTIVE</td><td>WSON</td><td>DSC</td><td></td><td>3000</td><td>RoHS & Green</td><td>NIPDAU</td><td>Level- -260C-UNLIM</td><td>40 to 125</td><td>7A9401</td><td>Samples</td></tr></table></body></html>

(1) The marketing status values are defined as follows:

ACTIVE: Product device recommended for new designs.   
LIFEBUY: TI has announced that the device will be discontinued, and a lifetime-buy period is in effect.   
NRND: Not recommended for new designs. Device is in production to support existing customers, but TI does not recommend using this part in a new design.   
PREVIEW: Device has been announced but is not in production. Samples may or may not be available.

(2) RoHS:  TI defines "RoHS" to mean semiconductor products that are compliant with the current EU RoHS requirements for all 10 RoHS substances, including the requirement that RoHS substance do not exceed $0 . 1 \%$ by weight in homogeneous materials. Where designed to be soldered at high temperatures, "RoHS" products are suitable for use in specified lead-free processes. TI may types of products as "Pb-Free".

RoHS Exempt: TI defines "RoHS Exempt" to mean products that contain lead but are compliant with EU RoHS pursuant to a specific EU RoHS exemption. Green: TI defines "Green" to mean the content of Chlorine (Cl) and Bromine (Br) based flame retardants meet JS709B low halogen requirements of $\scriptstyle < = 1 0 0 ($ 0ppm threshold. Antimony trioxide based flame retardants must also meet the $\scriptstyle < = 1 0 0 0$ ppm threshold requirement.

(3) MSL, Peak Temp. The Moisture Sensitivity Level rating according to the JEDEC industry standard classifications, and peak solder temperature.

(4) There may be additional marking, which relates to the logo, the lot trace code information, or the environmental category on the device.

(5) Multiple Device Markings will be inside parentheses. Only one Device Marking contained in parentheses and separated by "\~" will appear on a device. If a line is indented then it is a continuation of the previous line and the two combined represent the entire Device Marking for that device.

(6) Lead finish/Ball material Orderable Devices may have multiple material finish options. Finish options are separated by vertical ruled line. Lead finish/Ball material values may wrap to two lines if the finish value exceeds the maximum column width.

Important Information and Disclaimer:The information provided on this page represents TI's knowledge and belief as of the date that it is provided. TI bases its knowledge and belief on information provided by third parties, and makes no representation or warranty as to the accuracy of such information. Efforts are underway to better integrate information from third parties. TI has taken and continues to take reasonable steps to provide representative and accurate information but may not have conducted destructive testing or chemical analysis incoming materials and chemicals. TI and TI suppliers consider certain information to be proprietary , and thus CAS numbers and other limited information may not be available for release.

no event shall TI's liability arising out of such information exceed the total purchase price of the TI part(s) at issue in this document sold by TI to Customer on an annual basis.

# TAPE AND REEL INFORMATION

![](images/5bde564b261467e81b8850fc653be860399e42ffb1c731068d05c327fdaeaa36.jpg)

![](images/e9c4e1295d9b8017a9c1ef1353ab9f03155e5f9952d2bfab07e8c01b0636327f.jpg)

<html><body><table><tr><td>A0</td><td>Dimension designed to accommodate the component width</td></tr><tr><td>B0</td><td>Dimension designed to accommodate the component length</td></tr><tr><td>K0</td><td>Dimension designed to accommodate the component thickness</td></tr><tr><td>W</td><td>Overall width of the carrier tape</td></tr><tr><td>P1</td><td>Pitch successive cavity centers between</td></tr></table></body></html>

# QUADRANT ASSIGNMENTS FOR PIN ORIENTATION IN TAPE

![](images/f3d50601ad96d9725a54982f9ddba4990e3abcbb0a78b5c2d879bbb01533a45f.jpg)

\*All dimensions are nominal

<html><body><table><tr><td>Device</td><td>Type</td><td>Package Package Drawing</td><td>Pins</td><td>SPQ</td><td>Reel Diameter (mm)</td><td>Reel Width W1 (mm)</td><td>AO (mm)</td><td>B0 (mm)</td><td>KO (mm)</td><td>P1 (mm)</td><td>W (mm)</td><td>Pin1 Quadrant</td></tr><tr><td>TPS7A9401DSCR</td><td>WSON</td><td>DSC</td><td>10</td><td>3000</td><td>330.0</td><td>12.4</td><td>3.3</td><td>3.3</td><td></td><td>8.0</td><td>12.0</td><td>Q2</td></tr></table></body></html>

![](images/b53f786f7b61ee34a8f742984ccaef370e0879b8967c5b8cf5ded8166eb55b29.jpg)

\*All dimensions are nominal

<html><body><table><tr><td>Device</td><td>Package Type</td><td>Package Drawing</td><td>Pins</td><td>SPQ</td><td>Length (mm)</td><td>Width (mm)</td><td>Height (mm)</td></tr><tr><td>TPS7A9401DSCR</td><td>WSON</td><td>DSC</td><td>10</td><td>3000</td><td>367.0</td><td>367.0</td><td>35.0</td></tr></table></body></html>

# IMPORTANT NOTICE AND DISCLAIMER

TI PROVIDES TECHNICAL AND RELIABILITY DATA (INCLUDING DATA SHEETS), DESIGN RESOURCES (INCLUDING REFERENCE DESIGNS), APPLICATION OR OTHER DESIGN ADVICE, WEB TOOLS, SAFETY INFORMATION, AND OTHER RESOURCES “AS IS” AND WITH ALL FAULTS, AND DISCLAIMS ALL WARRANTIES, EXPRESS AND IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS.

These resources are intended for skilled developers designing with TI products. You are solely responsible for (1) selecting the appropriate TI products for your application, (2) designing, validating and testing your application, and (3) ensuring your application meets applicable standards, and any other safety, security, regulatory or other requirements.

These resources are subject to change without notice. TI grants you permission to use these resources only for development of an application that uses the TI products described in the resource. Other reproduction and display of these resources is prohibited. No license is granted to any other TI intellectual property right or to any third party intellectual property right. TI disclaims responsibility for, and you will fully indemnify TI and its representatives against, any claims, damages, costs, losses, and liabilities arising out of your use of these resources.

TI’s products are provided subject to TI’s Terms of Sale or other applicable terms available either on ti.com or provided in conjunction with such TI products. TI’s provision of these resources does not expand or otherwise alter TI’s applicable warranties or warranty disclaimers for TI products.

TI objects to and rejects any additional or different terms you may have proposed.

Mailing Address: Texas Instruments, Post Office Box 655303, Dallas, Texas 75265 Copyright $\circledcirc$ 2023, Texas Instruments Incorporated