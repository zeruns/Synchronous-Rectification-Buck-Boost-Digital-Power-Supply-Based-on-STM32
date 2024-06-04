# Synchronous Rectification Buck-Boost Digital Power Supply Based on STM32

**A four-switch Buck-Boost digital power supply based on STM32G474, supporting TypeC interface PD decoy input and DC5.5 interface input, with input/output voltage up to 48V10A. This is my graduation project, now open-sourced, including schematic, PCB, program source code, case 3D model, etc.**

做得一般，勿喷，欢迎友好交流。

**Demo video：[https://www.bilibili.com/video/BV1Ui421y7ip/](https://www.bilibili.com/video/BV1Ui421y7ip/)**

Based on CH32V307 smart electronic load open source, embedded competition work open source：[https://blog.zeruns.tech/archives/785.html](https://blog.zeruns.tech/archives/785.html)

本项目的立创开源平台开源链接：[https://url.zeruns.tech/noGf0](https://oshwhub.com/zeruns/ji-yu-stm32-de-buck-boost-xing-shu-zi-dian-yuan)

**电子/单片机技术交流群：[820537762](https://qm.qq.com/q/ZmTfBbFM4Y)**

The download address of the materials is at the end of the article.

## Introduction

This article designs a synchronous rectification Buck-Boost digital power supply based on STM32, the power circuit of which consists of MOSFET driving circuit, 4 switch Buck-Boost circuit, signal conditioning circuit, PD fast charging protocol circuit, auxiliary power circuit, MCU control circuit, etc.

The power supply can be powered through DC interface or Type-C interface, and the Type-C interface supports communication with the charger through PD fast charging protocol, automatically requesting and obtaining the maximum 20V working voltage. The power supply uses STM32G474 MCU to monitor the input and output voltage and current in real time, and adjusts the output PWM duty cycle through PID control algorithm, as well as realizes overvoltage and overcurrent protection. It can also sample the motherboard temperature to achieve overtemperature protection. In addition, it can display the power parameters in real time through the OLED screen, and set the output voltage and current through the rotary encoder and button, or communicate with the host computer through another Type-C interface to view the power parameters and waveforms in real time.

### Design Performance Parameters

​	电源设计性能参数如下表：

|     Item     |   Specification   |
| :----------: | :----------: |
| Input Voltage Range | 12Vdc~48Vdc  |
| Input Current Range |    0~10A     |
| Maximum Output Power |     450W     |
| Output Voltage Range	 | 0.5Vdc~48Vdc |
| Output Current Range |    0~10A     |
| Output Voltage Ripple	 | 峰峰值≤200mV |
|   Switching Frequency   |  181.333kHz  |

## Physical image

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240604_220453_ac82c6fcd7e80d46681be3d549251c7f.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_013241_cb8d07d69fbdbe4d98049f16cbf7eb64.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014039_182d63136cd729aaff26ee2fa4ded03a.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014104_1dee309cb5e9803cfefc8ed601e9395d.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014959_2647d66ede40c603ae14932cccb08439.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_015953_21e43de0555ec3c291de980127045d08.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_020420_9078a09c0869463c1af1573612a2ec09.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_020240_3b13fb354f8c36c48cdef4a072f9d7b7.jpg)

Using VOFA+ as the host computer software, you can monitor real-time power supply parameters (input voltage and current, output voltage and current, motherboard temperature, MCU temperature, power conversion efficiency, etc.) and waveform changes as shown below.

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604221547444_25ea3121b3626cacf8827ffbfa5792dd.png)

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604221601891_83cc3f6152fc28edfc50212ebe1802d0.png)

## System Framework Diagram

![](https://tc2.zeruns.tech/2024/06/04/image.png)


## Hardware Circuit Design

### Component Selection Calculation

#### Inductor Calculation

​In a synchronous Buck-Boost power supply, the calculation of inductance needs to consider the working conditions under both Buck and Boost modes to ensure that the circuit can meet the requirements in both modes. Typically, the selection of inductance is based on the mode that requires the higher inductance.

​When the power supply operates in Buck mode (voltage reduction), assuming the maximum input voltage is 48V and the minimum output voltage is 5V, the minimum PWM duty cycle is calculated:

$$
D_{min} = \frac{V_{out\_min}}{V_{in\_max}} = \frac{5V}{48V} = 10.417\%
$$

​Define the inductance current ripple (25% of the maximum rated current ripple, with the maximum rated current set as 10A):

$$
\Delta I_L = 25\% \cdot I_{out\_nom} = 25\% \times 10A = 2.5A
$$

​Calculate the minimum required BUCK inductor:

$$
L_{minBuck} = \frac{V_{out\_min}}{\Delta I_L \cdot f_{switch}} 
= \frac{5V}{2.5A \times 181333Hz} \times (1 - 10.417\%) 
\approx 9.88\mu H
$$

​When the power supply operates in BOOST mode (voltage boosting), with the rated input voltage of 24V as the calculation point, calculate the PWM duty cycle:

$$
D_{Bo} = 1 - \frac{V_{in\_nom}}{V_{out\_max}} = 1 - \frac{24V}{48V} = 50\%
$$

​In Boost mode, when taking 1A ($I_{minb}$), Boost enters continuous conduction mode (CCM), calculate inductance:

$$
L_{minBoost} = \frac{V_{out\_max} \cdot D_{Bo}(1-D_{Bo})^2}{2 \cdot I_{minb} \cdot f_{switch}}  
= \frac{48V \times 50\% \times (1 - 50\%)^2}{2 \times 1A \times 181333Hz} 
= 16.544\mu H
$$

​Based on the calculated results derived from the formula, the minimum inductance value required to meet the predetermined ripple current requirement is determined. To ensure that this condition is met, an inductor with a value slightly higher than the calculated value should be selected. At the same time, it is essential to ensure that the saturation current of the selected inductor can withstand the highest peak current in the circuit. Since the efficiency factor was not considered in the calculation process, the actual duty cycle and peak current may be lower than the theoretical calculated values. Therefore, when making a selection, a certain safety margin should be taken into account to adapt to various conditions that may arise under actual working conditions.

​Taking into account the above factors, this design chooses a 1770 surface-mount package inductor with a saturation current of more than 10A and an inductance of 22μH as the inductor component for the BUCK-BOOST circuit.

#### Capacitance Calculation

To achieve a better output voltage ripple, the designed voltage ripple is taken as 50mV.

$$
\Delta V_{out} = 0.05V
$$

Given the previously selected inductor of 22μH, the calculation here will also use this inductance value:

$$
L_{BB}=22\mu H
$$

Calculate the minimum capacitance required for operation in BUCK step-down mode and BOOST step-up mode, respectively, as $C_{minBuck}$ and $C_{minBoost}$:

$$
C_{minBuck}=\frac{V_{out\_min}\cdot(1-\frac{V_{out\_min}}{V_{in\_max}})}{8\cdot L_{BB}\cdot \Delta V_{out}\cdot {f_{switch}}^2}=15.48\mu F
$$

$$
C_{minBoost}=\frac{I_{out\_nom}\cdot(1-\frac{Vin\_min}{Vout\_max})}{\Delta Vout\cdot fswitch}=413.6\mu F
$$

The design requires a certain margin, as well as lower output ripple, so a low ESR 220μF solid-state capacitor is selected in conjunction with a 470μF conventional electrolytic capacitor, totaling 690μF.

#### MOS Selection Calculation

Within the rated input and output voltage range, calculate the effective value of the input MOS current as:

$$
i_{mos\_rms}=7.098A
$$

In the MOSFET selection, the rated current $I_D$ of the MOS tube must be at least twice the maximum current that flows through the MOS tube (to prevent damage from excessive current during fault or short circuit conditions).

$$
2×i_{mos\_rms}=14.196A
$$

In the selection of MOSFET, the rated voltage $V_{DS}$ should be greater than 1.5 times the maximum input voltage (to prevent spike breakdown).

$$
1.5×Vin\_max=72V
$$

After referring to the previous calculations, for conventional application scenarios, a MOSFET with a rated current exceeding 15 amperes (A) and a voltage rating of 100 volts (V) can be selected. The selection for the lower MOSFET should be the same as for the upper MOSFET. Considering the heat dissipation situation, to minimize energy loss during conduction and switching processes, MOSFETs with low on-resistance ($R_{DS(on)}$) and low output capacitance ($C_{oss}$) should be given priority.

The MOSFET model selected for this design is the CJAC80SN10, produced by Jiangsu Changjing Technology Co., Ltd., which is a domestically produced MOSFET. It has a drain-source voltage rating ($V_{DS}$) of 100V and a maximum drain-source current ($I_D$) of 80A. Its on-resistance ($R_{DS(on)}$) is as low as 6.2mΩ, which is beneficial for reducing power loss when the device is in the on-state. Additionally, the output capacitance ($C_{oss}$) of this device has a typical value of 420pF, which is advantageous for reducing dynamic losses during switching transitions. Therefore, the CJAC80SN10 not only meets the electrical characteristics required by the project but also effectively reduces energy loss and improves the overall efficiency of the system.

### Power Supply Board Circuit Design

#### Main Power Circuit

The following diagram represents the main power circuit of the synchronous BUCK-BOOST power supply. The input is on the left side; the synchronous BUCK step-down circuit consists of MOSFETs Q2 and Q4 and inductor L1. The synchronous BOOST step-up circuit is made up of MOSFETs Q3 and Q5 and inductor L1. Each MOSFET has a 10kΩ resistor in parallel between the gate and source terminals to ensure that the gate is not floating and to prevent unintended conduction.

![](https://tc2.zeruns.tech/2024/06/04/imagee4eecc6aa41fc7c6.png)

The main power circuit has a symmetrical structure on the left and right sides. Both the input and output ends are equipped with one 470μF/63V aluminum electrolytic capacitor and one 220μF/63V solid-state aluminum electrolytic capacitor. In addition, there are two small surface-mount MLCCs (Multilayer Ceramic Capacitors) with parameters of 10μF/50V to filter high-frequency noise interference at the ports. R9 and R10 serve as dummy loads for the input and output ports, allowing for the rapid dissipation of residual energy in the circuit when the power is disconnected. R13 and R14 are high-precision 5mΩ resistors used for current sampling, with a subsequent differential amplification circuit to amplify the input and output current signals. CNT1 and CNT2 are the output terminals. L1 and L2 are surface-mount 1770 package and through-hole magnetic core inductor packages, respectively; only one should be soldered, and both are drawn to facilitate testing the effects and performance of different inductors.

#### Power Supply Input and Fast Charging Protocol Circuit

The following diagram represents the power supply's input interface and PD fast charging protocol communication circuit.

The power supply input features two types of interfaces: a DC5.5*2.5mm female connector and a Type-C female connector. The Type-C interface supports various fast charging protocols such as BC1.2, PD3.0/2.0, etc. The fast charging protocol chip used is the CH224K, which can communicate with fast charging chargers to output a voltage as high as 20V and supports up to 100W of power.

Diodes D1, D3, and D4 serve to prevent voltage backflow from the DC interface to the Type-C interface; the design does not allow both interfaces to be connected simultaneously. Diode D5 is used for reverse polarity protection. FH1 is the power input fuse holder, which accommodates a 12A fuse.

![](https://tc2.zeruns.tech/2024/06/04/image6389c97e68ccc163.png)

#### Driver Circuit

In the design of a synchronous BUCK-BOOST power supply, both the BUCK and BOOST circuits include high-side N-MOSFETs. Traditionally, the driving of these high-side N-MOSFETs is accomplished using transformer-isolated drivers, but this method increases the complexity of the circuit and enlarges the size of the circuit board.

This design selects two MOSFET driver chips with built-in bootstrap circuit functions, EG3112, to drive the MOSFETs of the BUCK and BOOST circuits. The EG3112 is a non-isolated complementary dual-channel driver chip; its 2A output driving current capability ensures that the MOSFET can turn on quickly; the chip also has a built-in dead-time control function to prevent shoot-through of the output driving signals, thereby enhancing the stability of the system. The specific circuit is shown in the figure below.

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604212659943_bbd9654002c450331cd56f50547128a7.png)

Taking the driving of the MOSFET in the BOOST step-up circuit as an example, PWM2L and PWM2H are PWM signals output from the STM32G474 microcontroller, which are sent to the LIN and HIN pins of the EG3112 driver chip. LO is the lower MOSFET driving signal output, and the driving resistor has a value of 10Ω. HO is the upper MOSFET driving signal output, and the driving resistor also has a value of 10Ω. D7 is the bootstrap diode for the upper MOSFET driving circuit, while diodes D9 and D11 are used to quickly discharge the gate charge, speeding up the turn-off speed of the MOSFET. C14 is the bootstrap capacitor.

The role of the driving resistor is to reduce oscillations that may occur on the PCB traces, distributed capacitance, inductors, and other components during the MOSFET switching process. By using a series resistor, these oscillations can be mitigated, improving the stability and reliability of the system.

#### Auxiliary Power Supply

The following diagram is the schematic of the power supply board's auxiliary power supply circuit.

![](https://tc2.zeruns.tech/2024/06/04/image2feef08fe62bfb2d.png)

The first-stage 12V output auxiliary power circuit uses the BUCK-type power chip TPS54360B, which integrates a high-side MOSFET, for its design. According to the chip's datasheet, a pull-down resistor on the RT pin can set the switching frequency; a 110kΩ resistor is selected here, corresponding to a switching frequency of 876.5kHz. A higher switching frequency allows for the use of a smaller inductor to save space. Based on this frequency, the inductance value should be greater than 9.75μH, so a 10μH inductor is selected, and SS310 is chosen as the freewheeling diode. C26 and C27 are input filter capacitors; feedback divider resistors R19 and R25 create a 0.8V reference voltage for the chip's FB pin to ensure the output voltage is maintained at 12V. C18 and C19 are the filter capacitors for the first-stage 12V output auxiliary power supply. The first-stage 12V output is mainly used to supply the input of the second-stage 6V step-down circuit, as well as the MOSFET driver circuit and cooling fan.

The second-stage 6V output auxiliary power circuit uses the synchronous rectification BUCK-type power chip SY8205, which integrates a MOSFET, for its design. According to the datasheet, the chip's switching frequency is fixed at 500kHz, and a 10μH inductor is selected for the auxiliary power BUCK circuit. C21 and C22 are input filter capacitors; feedback divider resistors R23 and R26 create a 0.6V reference voltage for the chip's FB pin to ensure the output voltage is maintained at 6V. C24 and C25 are the filter capacitors for the second-stage 6V output auxiliary power supply. Since directly using a linear regulator to step down from 12V to 5V would result in significant loss, a switch-mode power supply is first used to step down to a voltage close to 5V, and then a linear regulator is used to step down to the target voltage. This approach ensures higher efficiency and lower output ripple.

The second-stage output of 6V is stepped down to 5V by the linear voltage regulator chip AMS1117-5 to serve as the third-level auxiliary power supply. The voltage reference chip REF3033 is used for signal conditioning, OLED display, USB communication, and other functional circuits.

5V DC is stepped down to 3.3V by the linear voltage regulator chip AMS1117-3.3 to serve as the fourth-level auxiliary power supply, used for MCU, buzzer, Flash chip, and other circuits.

The voltage reference chip REF3033 outputs a 3.3V reference voltage for the MCU's VREF pin, serving as the reference voltage for the MCU's built-in ADC to improve the accuracy of ADC sampling.

#### Signal Conditioning Circuit

![](https://tc2.zeruns.tech/2024/06/04/image5235dbc3d2789367.png)


As shown in the diagram above, the input and output voltage sampling and signal conditioning circuit of this design utilizes differential amplification technology. The differential amplification topology can effectively reduce the interference of high-frequency noise in the switching power supply on the signal conditioning circuit, enhancing the stability and reliability of the signal. In this circuit, the low-offset operational amplifier GS8558-SR is selected to improve the precision of the conversion. The GS8558-SR op-amp has excellent DC accuracy and low bias current characteristics, which are crucial for improving the performance of the entire signal conditioning circuit.

Differential amplification circuit feedback resistor:

$$
R_{38}=4.7kΩ
$$

$$
R_{36}=75kΩ
$$

Calculate the differential amplification gain:

$$
K_V=\frac{R_{38}}{R_{36}}=0.062667
$$

The MCU's ADC has a maximum sampling voltage of 3.3V, which is the reference voltage output by the voltage reference chip, so the maximum input and output voltage values that can be sampled can be calculated as follows:

$$
V_{max}=\frac{3.3V}{K_V}=52.66V
$$

To better suppress the interference of high-frequency noise from the switching power supply on the differential amplification circuit, decoupling capacitors C40 and C41 are added to the power supply pins of the operational amplifier for filtering. Capacitors are selected as MLCCs (Multilayer Ceramic Capacitors), which have a lower ESR (Equivalent Series Resistance) and ESL (Equivalent Series Inductance), thus providing better filtering effects for high-frequency noise. Additionally, an RC low-pass filter circuit is formed by adding resistor R35 and capacitor C39 at the output end to filter out high-frequency noise.

![](https://tc2.zeruns.tech/2024/06/04/imagef80efbbbbfc7722d.png)

As shown in the diagram above, the input and output current conditioning circuits use a differential amplification method, and the output current sampling resistor is:

$$
R_{14}=5mΩ
$$

The feedback resistors in the differential amplification circuit are:

$$
R_{48}=6.2kΩ
$$

$$
R_{46}=100Ω
$$

Calculate the differential amplification gain:

$$
K_I=\frac{R_{48}}{R_{46}}=62
$$

That is to say, for every 1A of current output, there is a voltage of 310mV.

Given that the MCU's ADC has a maximum sampling voltage of 3.3V, provided by the voltage reference chip, you can calculate the maximum input and output current values that can be sampled as follows:

$$
I_{max}=\frac{3.3V}{K_I \times R_{14}}=10.65A
$$

#### MCU Control Circuit

![](https://tc2.zeruns.tech/2024/06/04/imagedb4de164208aadb6.png)

The digital power supply design in this project uses the STM32G474RET6 chip as the controller, which has the notable features of a simple peripheral circuit, diverse control methods, and excellent expansion capabilities. To achieve an accurate clock signal, the controller uses an external crystal oscillator X1, which is a quartz crystal oscillator with a frequency of 25 megahertz (MHz). In addition, the circuit also includes multiple filtering capacitors, including C45, C51, C56, C46, and C52, which are used for different digital power supply pins of the microcontroller (MCU) to ensure power stability and reduce noise interference. Resistor R49 and capacitor C49 form a power-on reset circuit, and SW1 is the MCU reset button. U11 is a USB-to-serial chip, model CH340C, connected to the second Type-C interface, with the serial port connected to the MCU's USART1 interface. U10 is a Flash memory chip, model W25Q64, used to store parameter settings and other information, connected to the MCU's SPI3 interface. Q6 is the driving MOSFET for the buzzer, used to control the buzzer, with the MOSFET gate connected to the MCU's PB5 port. Terminal H1 is the SWD programming port. H2 and H4 are PH2.0 connectors, used to connect with the control panel, with connectors reserved for the USART2 interface, which can conveniently replace the control panel with a serial screen, and can also add an ESP32 to increase networking and wireless control functions. D13 is a 5V reverse polarity protection diode.

#### Cooling Fan Drive and Motherboard Temperature Sampling Circuit

The schematic diagram of the motherboard temperature sampling circuit and the cooling fan drive circuit is shown below. The principle of the power supply motherboard temperature sampling is to use an NTC thermistor R2 in series with a pull-down resistor R4 to output a voltage divider to the MCU's ADC port for sampling. The NTC thermistor used has a resistance value of 10kΩ and a B value of 3950K.

The cooling fan is driven using an N-MOSFET, model AO3400. A diode D2 is reverse-biased in parallel on the fan interface to prevent damage caused by the back electromotive force (back EMF) generated by the motor. When the motor is powered off, due to rotational inertia, the rotor of the motor will not stop immediately but will continue to rotate and generate a voltage. This voltage can potentially damage the transistors or integrated circuits in the circuit, especially when the motor is connected to these components through a semiconductor switch, such as an MOSFET.

![](https://tc2.zeruns.tech/2024/06/04/image8481a78abdd1262b.png)

### Control Panel Circuit Design

The schematic diagram of the control panel circuit is shown below. SW1 is a rotary encoder used for setting parameters and the like, while SW2 and SW3 are buttons; SW2 is used to switch settings, and SW3 is used to control the power output's on and off. LED1 is the system operation status indicator light, which blinks at intervals of 500ms during normal operation. LED2 is the output status indicator light, which lights up when the output is on and turns off when the output is off. OLED1 is the OLED screen, used to display power supply parameters and status information, etc.

![](https://tc2.zeruns.tech/2024/06/04/imageac5187c46ec927e9.png)

## User Manual

You can adjust the output voltage and current values using the buttons and rotary encoder. As shown in the figure below, the inverse color display indicates the current setting digit. By rotating the encoder, you can increase or decrease the value. Pressing the encoder button allows you to switch to the next digit for setting. The SW2 button is used to switch between different settings. The set data will be automatically saved to the Flash memory chip, and the data will be read from the chip during the next power-up.

The SW3 button is used to turn the power output on or off.

![](https://tc2.zeruns.tech/2024/06/04/image048512af72675ee7.png)

You can switch to the data display page to view the current input and output voltage and current of the power supply, as well as information such as motherboard temperature and MCU temperature, as shown in the figure below.

![](https://tc2.zeruns.tech/2024/06/04/image472a77ba11a65622.png)

You can switch to the settings page to configure the threshold values for over-temperature, over-current, and over-voltage protection, as shown in the figure below. The data set will be automatically saved to the Flash memory chip, and the data will be retrieved from the chip during the next startup.

![](https://tc2.zeruns.tech/2024/06/04/image13c3475597eb3015.png)

## Ripple Test

Use an electronic load and oscilloscope to test the power supply output performance and output ripple, as shown in the figure below. At an input of 36V and an output of 12V at 2A, the peak-to-peak ripple measured is approximately 42mV, as illustrated in the figure below.

![](https://tc2.zeruns.tech/2024/06/04/image1aec2ab4f0416d21.png)

![](https://tc2.zeruns.tech/2024/06/04/image5c172981426b5675.png)

## Efficiency Test

The power supply conversion efficiency when inputting 20V and outputting 12V at 10A is measured to be 92%, as shown in the figure below.

![](https://tc2.zeruns.tech/2024/06/04/image214e1bc17e0a1bec.png)

## MOSFET Gate Waveform

The following table shows the conversion efficiency at various input and output voltages, with the highest efficiency reaching 94.3%.

| Input Voltage (V) | Input Current (A) | Input Power (W) | Output Voltage (V) | Output Current (A) | Output Power (W) | Efficiency (%) |
|:------------------:|:------------------:|:---------------:|:------------------:|:------------------:|:---------------:|:--------------:|
| 20.003             | 4.035              | 80.712          | 15.010            | 5.000              | 75.050          | 92.985         |
| 47.999             | 5.335              | 256.075         | 24.040            | 9.900              | 237.996         | 92.940         |
| 48.000             | 7.875              | 378.000         | 36.020            | 9.900              | 356.598         | 94.338         |
| 48.000             | 9.860              | 473.280         | 45.030            | 9.900              | 445.797         | 94.193         |
| 23.998             | 8.835              | 212.022         | 48.070            | 4.000              | 192.280         | 90.689         |
| 23.998             | 9.830              | 235.900         | 35.998            | 6.001              | 216.024         | 91.574         |
| 12.099             | 9.166              | 110.899         | 24.070            | 4.000              | 96.280          | 86.817         |
| 20.008             | 2.645              | 52.921          | 4.970             | 9.000              | 44.730          | 84.522         |
| 20.008             | 10.550             | 211.084         | 24.030            | 8.000              | 192.240         | 91.073         |
| 36.000             | 6.418              | 231.048         | 24.010            | 9.000              | 216.090         | 93.526         |
| 36.000             | 10.540             | 379.440         | 35.950            | 9.800              | 352.310         | 92.850         |

## Efficiency Test

The power supply conversion efficiency when inputting 20V and outputting 24V is tested on the gate waveforms of each MOSFET.

BUCK circuit upper and lower tube to ground voltage waveform diagram: 

![](https://tc2.zeruns.tech/2024/06/04/image2dbfee46c6cb8b48.png)

BOOST Circuit Upper and Lower Tube to Ground Voltage Waveform Diagram:

![](https://tc2.zeruns.tech/2024/06/04/image270e2362e47d3fa4.png)

## 元器件购买地址

这个项目用到的大部分元件购买地址都在这里：

- 0805电阻电容样品本：[https://s.click.taobao.com/begdskt](https://s.click.taobao.com/t?e=m%3D2%26s%3Dq2Hk1NsmK39w4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2pNnTGGq1akD0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAmCODajZzqkiGm1Ouoto%2FLv1SarTXhIOTUdDn0Cj7EhNdd%2B7%2B%2F2bfk3kp5tu88bnmfiF9hqvFc1tI6b3MeFr5Pad0gyySBx6sy0vsV50aX2CTSqV%2Fo%2F6pRZGZ9wPRcXV%2BQ%2FMlsmagC3QYR0t0%2BifKcrYkzU4LehszxiXvDf8DaRs%3D&union_lens=lensId%3APUB%401717514892%4021085110_0c76_18fe3dd7443_be53%40026T237i0kBOLVaGKkksmY1a%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_2ae668ea-589c-46f9-b924-e800d331f527_)
- STM32G474RE芯片：[https://s.click.taobao.com/C5yYWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3D1bgCRCrwjx5w4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2TW057QpX8%2BP0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZtx9GJkz0%2BY1U9sc3Rp3DHQpnTkqo0s9O9whTAWnxDu4BDY4s85gbiFOq%2FH7gu2ofDvAzCOFWANvnkZn3A9FxdX4F%2B6qWNoTfGk3O4wtQ3K90YW9F2JTC2Z5wzd6EJTYJCiGFCzYOOqAQ&union_lens=lensId%3APUB%401717514947%402133ebe8_0d2d_18fe3de4c3f_7045%40024c6z8mbO9v25a8FLTiNdWs%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_886caec8-cf82-4684-a45b-8f09c3b05f58_)
- CH224K芯片：[https://s.click.taobao.com/4t4bskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DgfsQ0GFJ2tBw4vFB6t2Z2ueEDrYVVa64YUrQeSeIhnK53hKxp7mNFrMfIvbtZ%2F%2B2aeIFlo3v5770JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMA569A3bhmEVLoDOSx%2FyKnTaLWMw3EOEsyei2B3s1dHL%2Fc6mknC3l62D0HjFcQZN9NLRZ6oWQv2hGH69%2Fe7lOk8bA2OU2M8Pv8gkJgaGyGQf2XXBQGhVAY3B%2FQ0Xk2XvDuj4MrAUsR31Dl1SxDw1i9uP7nyHmkoZi7sfoqE3cFzAWIPus6xSRp2cYMXU3NNCg%2F&union_lens=lensId%3APUB%401717515106%4021664126_0c65_18fe3e0b7d9_186a%40024wx7nw1EhCyKmNvo9ewksX%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_dca72704-9422-4c38-8fe3-ada2813d8dfc_)
- CJAC80SN10 MOS管：[https://s.click.taobao.com/aPsWWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3Dwxu343JFN%2B1w4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B22yhJas5yPn%2F0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZt8ilz31ouGxzOSkiDdkQCUnrS9V%2BHCu7QKUkuSvlMqhUU5w6aGHQTtaV%2BYFvVA%2BPfYwmLWBgEm80VKxcI130SjETn2JhMaQNtYWLhXkoGmYyH23HiUahG8wAZ0e0kWnrXcYOae24fhW0&union_lens=lensId%3APUB%401717515288%402154669d_0c44_18fe3e3801a_27d7%40024DYvoAgiZES9sfLBip6mUq%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_16dcef80-a9d8-4a4b-94bf-eaa099d80850_)
- EG3112芯片：[https://s.click.taobao.com/1k9Zskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DSJ9XXUy9t8Vw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B25SFJHbJ7ieL0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMA4GZmwfE1QcUz9FTg%2BKHsN%2F1SarTXhIOTUdDn0Cj7EhNQ2Fa1i0diqXZhQ4CbRsm6fYgDMeHSfS%2BrUGYT%2BU1A%2F5FVPJXkdQvjUDO4cIJOgpVlTUdH%2F4v4MZGZ9wPRcXV%2BQ%2FMlsmagC3Tvd09ovMkWf8xsxzwWvLn6xiXvDf8DaRs%3D&union_lens=lensId%3APUB%401717515471%4021675e4c_0c04_18fe3e64bec_b5f5%400260SL9dLsnYJv9Wlnnu543H%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_7e2558ea-cf0b-4ced-a202-de24587d6ae1_)
- 220μF 63V 固态电容：[https://s.click.taobao.com/n60Yskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DR5rt6anE7ZRw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B20aoW6RtNUqP0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAAXwSJMeMTMnYgv019zNeJf1SarTXhIOT%2FcbizUjsed9UZYQMFccYYXWbdBRJPRxU4TzWKfJFiEwEv0DolshoXy%2BYmh1ufDU2PSSAUUXsF3ejO9AJYjY8CXJ%2BwEVkOqHFdIW9JNkz7%2FgDXc3BRboPm2unxbrMbb1gvhyupOv2QT1MdIp1rr4P3Q%3D%3D&union_lens=lensId%3APUB%401717515591%402127ecb3_0bc4_18fe3e81f4d_6429%400248sGfUEErF9pDX6wqNw05y%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_83c3d358-b7fc-46c8-a0f0-c82ead873bc4_)
- 470μF 63V红宝石电容：[https://s.click.taobao.com/WTRUWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3DoNJakc%2Fgas5w4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2eJS%2FlGX%2FHCz0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAdamTXq5RvrvEpBUtJrIEF5ESMSIvSnZOMN7NXhFCiylOOKaYnA%2B4Jkzslj1VfbK4BLgAura4EdF%2FuWfGlvEogka7UFonYxBlEWaTHuE9qf%2BMJi1gYBJvNFSsXCNd9Eox5xRFlLBC%2BAxOyCI34m0ia1O%2FTXMSxOWDomfkDJRs%2BhU%3D&union_lens=lensId%3APUB%401717515690%40213e4428_0d9f_18fe3e9a202_52bb%40025rg7dhXV4Xfdnt1Vyv49BH%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_c659fa20-89ab-4b22-ac73-0da30923b20a_)
- GS8558运放：[https://s.click.taobao.com/usSVskt](https://s.click.taobao.com/t?e=m%3D2%26s%3Dc9VXontUpHNw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2hJoAxuL33Zv0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMADX9AtIWVKayQbZBrMLqCrJESMSIvSnZOMN7NXhFCiymJOiceX4kwGqVko1ZC2rhGVNyuMVuyel9VM46YmcV9eY6VscdQk%2FYnhxNPRD%2BmAGGQs8JddDGu2JGZ9wPRcXV%2BQ%2FMlsmagC3RjzDzXxRT3kHngecbybC1rxiXvDf8DaRs%3D&union_lens=lensId%3APUB%401717515948%4021087db4_0c33_18fe3ed930b_bf55%400212Xaf1oZXvUdqh9heND8SB%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_b8b8257d-0f4e-483f-9838-a44839b41a76_)
- TPS54360芯片：[https://s.click.taobao.com/r6lSWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3Dy0f%2FxajlIo5w4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2T5WL1XDPsq%2F0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZt8%2FsN2Ci7Whz7mu2K%2BGUDFTbVWvgSIEiA1KQ0yA8VKIq8gcz9%2BlLgUWSVlpOWSbwpowmLWBgEm80VKxcI130SjETn2JhMaQNtYWLhXkoGmYyP4Y2sRYqTc5DYIzGTSSGOcYOae24fhW0&union_lens=lensId%3APUB%401717516095%40212cbfc1_0c11_18fe3efce60_d39f%400225wXjg2EqacBra1P1g6deI%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_2d2369c2-9e16-4ff6-91a1-883369690869_)
- SY8205芯片：[https://s.click.taobao.com/GxMUskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DZGEmYEO9YLRw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B26%2BNd%2BN%2BIi9H0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZt6TgkhyowXMyqA47Z7Hdv4UGGCfCsQltpZjyPwNyKiIax0%2BLv%2FbjgC%2BNldy0a4OGvowmLWBgEm80VKxcI130SjETn2JhMaQNtYWLhXkoGmYy%2FtqYTP%2FAUGaFV8fPH4mFxsYOae24fhW0&union_lens=lensId%3APUB%401717516143%40210480f0_0bfe_18fe3f08d52_572e%400210k1bOlwNNWZNzVKQYCOye%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_9d9dabea-cb7f-4acb-b8ae-8e85ee9cc6fd_)
- AMS1117芯片：[https://s.click.taobao.com/mxASWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3DfTBy54Xe8mVw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2E39dAI5s37v0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMA%2FXC6lFkCpYM56ZoUzm0cdkhUSIdDrY3ktWAKfhx9%2FCLZz2VyWhvCA6lOFDm9DwWRMp%2FSwSPLD5%2FLF0ViIzaPpm2mKLdbAARyR2PBc9zLFeXIJucKztczwrTO8hVXH49zb%2FnUHMQd61%2FGCUoWt8%2BaTPRVj1yieHrVKkt59nVASuRWpJNtDYCweQ%3D%3D&union_lens=lensId%3APUB%401717516188%400bf8e0ed_0bc8_18fe3f13d08_27dd%40027Dt0ALKAvsEk2d86AZYkIY%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_cbac6e1b-5a7b-41f8-bef2-e18d5863cc95_)
- REF3033基准芯片：[https://s.click.taobao.com/eOcTskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DRJVQF%2F5P04Bw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2%2FgNUgnorhu30JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZt8RGrZ9kV4RyT4AFqDdlbY9K1eoNjGZEjOx6ZWEhlN9t4JcDqiAhAzum81xyrpWuH4wmLWBgEm80VKxcI130SjETn2JhMaQNtYWLhXkoGmYysFprkEssBcyGEhgVkfeC8MYOae24fhW0&union_lens=lensId%3APUB%401717516244%400bf8e0b9_0bc8_18fe3f2152f_0ad5%400254PnYk2hYNmcHmrmHL5iRZ%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_d5f34af2-712b-4e2e-b740-66662c991aba_)
- W25Q64JVSSIQ芯片：[https://s.click.taobao.com/QX7Tskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DsbuN4BLtmWNw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2xMmwWQOqqnr0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAhscfsB2%2FyzZJq71CBMBeP%2F1SarTXhIOTsgIpc1WFZiJNubylQlnZtz0cAZG65YSbp%2BzQyz%2FP%2FUxz81MO6NheiuB%2FbIPTn9JMs5Au8iOrbFu3myZCws9xpowmLWBgEm80VKxcI130SjETn2JhMaQNtYWLhXkoGmYyNmQ3hnDHPY0a287yaLvIX8YOae24fhW0&union_lens=lensId%3APUB%401717516345%4021662ab2_0bc6_18fe3f3a073_5ff0%40025mivRkEVTpx35MqUhuUje8%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_22c188ef-8bbf-4b12-9b45-7a938e019b19_)
- CH340C芯片：[https://s.click.taobao.com/QYRQWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3D%2Fx8xa5pVDTpw4vFB6t2Z2ueEDrYVVa64YUrQeSeIhnK53hKxp7mNFrMfIvbtZ%2F%2B2gAH%2FUlmsVA70JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAbyJ13E4KPCIWqyf4HTdqkv1SarTXhIOTUdDn0Cj7EhP3CWHO8JvakzwJULvwzLdCjolU2dLqpzgt3wxv%2FcK7MjQgOpVn9n7QsuczrrH5oS7780OnliQlp3B6Jd9pUfrR1KilmKsn0wxlH30cIrZrlVqk39LHGAlDxg5p7bh%2BFbQ%3D&union_lens=lensId%3APUB%401717516425%40213d2ad3_0c0e_18fe3f4d8c4_7ea6%40026yvWtwsijH9NrIeGmDudmQ%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_876a4f84-da14-495b-b2d9-b24971507089_)
- 3.3V有源蜂鸣器：[https://s.click.taobao.com/sovPWlt](https://s.click.taobao.com/t?e=m%3D2%26s%3DN6rGO5JHzyNw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B2qTxl3%2BveaDz0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMAl4EuqVQA9Kc56ZoUzm0cdngJMd2yjs373q5%2F6sUdbJKdk2iv391z8P8GAnsUm%2FW94ZJROVsXZxPLkYvRdVCblwe6Czg4%2F%2BipPybtkeJ4cBSjO9AJYjY8CXJ%2BwEVkOqHF%2FasCdS0uCG6XnNpQB3cavRFf1GB1Osr%2F&union_lens=lensId%3APUB%401717516525%400b521ecc_0bee_18fe3f65e3a_7147%4002Y2qcMIgCxLtyFV9SEx2Vl%40eyJmbG9vcklkIjo4MDY3NCwiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfaW5kZXhfaHRtIiiwiic3JjRmxvb3JJZCI6IjgwNjc0In0ie%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_4e8883ce-1ee9-46f3-af86-e3482aaba171_)
- TypeC母座 16P：[https://s.click.taobao.com/vjLRskt](https://s.click.taobao.com/t?e=m%3D2%26s%3DAGnANCpGehpw4vFB6t2Z2ueEDrYVVa64g3vZOarmkFi53hKxp7mNFrMfIvbtZ%2F%2B248nGIg2io7D0JlhLk0Jl4QTquP0kWxBLBDnvz6xo38xspWc9%2BCL4bTGF1ceZMhPo8mL8HhJ3EdVrH4ks4QyiY4z4rjZDGVMA%2FXC6lFkCpYM56ZoUzm0cdjFnKNBxtUOF56yhindHoRjQ1RhlcRSa%2B9TWeX3YAPjccKhcNivwaiqdb3PHAdZ0AL2D05cS5bWpeRtW%2Fi37v0RweiXfaVH60dSopZirJ9MMzsAzH1xYDH4SIz%2BVIb9tlJ9t1Kc%2BZBNy%2BIup62zqUgk%3D&skuId=4662437423099&union_lens=lensId%3APUB%401717516627%402104517b_0c0f_18fe3f7ecb7_3aba%40033KxVhjeXvjw3DllGDd2Uju%40eyJmbG9vcklkIjo4NTQ2Nywiic3BtQiiI6Il9wb3J0YWxfdjJfcGFnZXNfcHJvbW9fZ29vZHNfZGV0YWlsX2h0bSIsInNyY0Zsb29ySWQiiOiiI4MDY3NCJ9%3BtkScm%3AselectionPlaza_site_4358%3Bscm%3A1007.30148.329090.pub_search-item_c7809811-3a4a-4842-83d4-cc0b509b279b_)

建议在[立创商城](https://activity.szlcsc.com/invite/D03E5B9CEAAE70A4.html)里购买元器件：[https://activity.szlcsc.com/invite/D03E5B9CEAAE70A4.html](https://activity.szlcsc.com/invite/D03E5B9CEAAE70A4.html)

**在立创开源链接里的BOM表那点立即到立创商城下单可将用到的元器件一键导入到购物车。**

## Download Links for Materials

The following links include materials such as: EasyEDA project files, schematic diagram PDFs, datasheets for each chip, source code project compressed package, and some reference program codes.

**123云盘不限速下载地址：**[https://www.123pan.com/ps/2Y9Djv-8yevH.html](https://www.123pan.com/ps/2Y9Djv-8yevH.html)

百度网盘下载地址：[https://url.zeruns.tech/MW2d1](https://url.zeruns.tech/MW2d1)

**项目程序Gitee开源地址：**[https://gitee.com/zeruns/STM32-Buck-Boost](https://gitee.com/zeruns/STM32-Buck-Boost)

项目程序GitHub开源地址：

**求点个Star和点个赞。**


## Known Issues

1. The SY8205 chip in the auxiliary power supply enters PFM (Pulse Frequency Modulation) mode at low load, resulting in a lower frequency and thus generating slight noise.
2. The PID control program for constant current mode is not well written; it only remains stable under pure resistive loads. (Constant voltage mode is not problematic.)

## Other Recommended Open Source Projects

- An open-source three-phase power meter has been created to conveniently monitor home electricity usage：[https://blog.zeruns.tech/archives/771.html](https://blog.zeruns.tech/archives/771.html)
- Based on the STM32F407, there is an LVGL project template for the MSP3526 display, which includes both FreeRTOS and bare-metal versions.：[https://blog.zeruns.tech/archives/788.html](https://blog.zeruns.tech/archives/788.html)
- 沁恒CH32V307VCT6最小系统板开源：[https://blog.zeruns.tech/archives/726.html](https://blog.zeruns.tech/archives/726.html)
- LM25118自动升降压可调DCDC电源模块：[https://blog.zeruns.tech/archives/727.html](https://blog.zeruns.tech/archives/727.html)
- EG1164大功率同步整流升压模块开源，最高效率97%：[https://blog.zeruns.tech/archives/730.html](https://blog.zeruns.tech/archives/730.html)
- 基于合宙Air700E的4G环境监测节点（温湿度、气压等数据），通过MQTT上传阿里云物联网平台：[https://blog.zeruns.tech/archives/747.html](https://blog.zeruns.tech/archives/747.html)
- 基于HT32F52352的智能电子负载开源，合泰杯作品开源：[https://blog.zeruns.tech/archives/784.html](https://blog.zeruns.tech/archives/784.html)

## 推荐阅读

- **高性价比和便宜的VPS/云服务器推荐:** [https://blog.zeruns.tech/archives/383.html](https://blog.zeruns.tech/archives/383.html)
- 我的世界开服教程：[https://blog.zeruns.tech/tag/mc/](https://blog.zeruns.tech/tag/mc/)
- 免代码搭建博客网站！超详细个人博客搭建教程：[https://blog.zeruns.tech/archives/783.html](https://blog.zeruns.tech/archives/783.html)
- 内网穿透服务器搭建教程，NPS搭建和使用教程：[https://blog.zeruns.tech/archives/741.html](https://blog.zeruns.tech/archives/741.html)
- 雨云 宁波 8272CL 大带宽高防云服务器性能测评，最高500兆带宽和1TB云盘：[https://blog.zeruns.tech/archives/789.html](https://blog.zeruns.tech/archives/789.html)
- 抖音商城2.6元的120W充电器测试和拆解：[https://blog.zeruns.tech/archives/786.html](https://blog.zeruns.tech/archives/786.html)

