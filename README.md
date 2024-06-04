# 基于STM32的同步整流Buck-Boost数字电源 开源

**一款基于STM32G474的四开关Buck-Boost数字电源，支持TypeC接口PD诱骗输入和DC5.5接口输入，输入/输出最高48V10A，这是我的毕业设计，现在开源出来，含原理图、PCB、程序源码、外壳3D模型等资料。**

做得一般，勿喷，欢迎友好交流。

**作品演示视频：[https://www.bilibili.com/video/BV1Ui421y7ip/](https://www.bilibili.com/video/BV1Ui421y7ip/)**

基于CH32V307的智能电子负载开源，嵌入式大赛作品开源：[https://blog.zeruns.tech/archives/785.html](https://blog.zeruns.tech/archives/785.html)

本项目的立创开源平台开源链接：[https://url.zeruns.tech/noGf0](https://oshwhub.com/zeruns/ji-yu-stm32-de-buck-boost-xing-shu-zi-dian-yuan)

**电子/单片机技术交流QQ群：[820537762](https://qm.qq.com/q/ZmTfBbFM4Y)**

资料下载地址在文章末尾。

## 简介

​	本文设计基于STM32的同步整流Buck-Boost数字电源，该电源电路由MOSFET驱动电路、4开关Buck-Boost电路、信号调理电路、PD快充协议电路、辅助电源电路、单片机控制电路等部分组成。

​	该电源可通过DC接口或Type-C接口供电，而且Type-C接口支持与充电器通过PD快充协议通讯，自动请求并获取最高20V的工作电压。该电源使用STM32G474单片机对输入输出的电压和电流实时监测，并通过PID控制算法来调整输出的PWM占空比，以及实现过压过流保护，还可采样主板温度来实现过温保护。此外可通过OLED屏实时查看电源的参数，并通过旋转编码器和按钮来设置输出电压和电流，还可以通过另一个Type-C接口与上位机通信，通过上位机软件可实时查看电源的各项参数和波形。

### 设计性能参数

​	电源设计性能参数如下表：

|     项目     |   参数指标   |
| :----------: | :----------: |
| 输入电压范围 | 12Vdc~48Vdc  |
| 输入电流范围 |    0~10A     |
| 输出最大功率 |     450W     |
| 输出电压范围 | 0.5Vdc~48Vdc |
| 输出电流范围 |    0~10A     |
| 输出电压纹波 | 峰峰值≤200mV |
|   开关频率   |  181.333kHz  |


## 实物图

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240604_220453_ac82c6fcd7e80d46681be3d549251c7f.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_013241_cb8d07d69fbdbe4d98049f16cbf7eb64.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014039_182d63136cd729aaff26ee2fa4ded03a.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014104_1dee309cb5e9803cfefc8ed601e9395d.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_014959_2647d66ede40c603ae14932cccb08439.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_015953_21e43de0555ec3c291de980127045d08.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_020420_9078a09c0869463c1af1573612a2ec09.jpg)

![](https://tc2.zeruns.tech/Pic/2024/06/IMG_20240501_020240_3b13fb354f8c36c48cdef4a072f9d7b7.jpg)

使用VOFA+作为上位机软件，可实时查看电源各项参数（输入电压和电流、输出电压和电流、主板温度、MCU温度、电源转换效率等）和波形变化。如下图所示。

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604221547444_25ea3121b3626cacf8827ffbfa5792dd.png)

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604221601891_83cc3f6152fc28edfc50212ebe1802d0.png)

## 系统框架图

![](https://tc2.zeruns.tech/2024/06/04/image.png)

## 硬件电路设计

### 器件选型计算

#### 电感计算

​	在同步BUCK-BOOST电源中，电感的计算需要考虑BUCK模式和BOOST模式下的工作情况，以确保在两种模式下都能满足电路的工作要求。通常，电感的选择会基于两种模式中对电感要求更高的那个模式来进行。

​	当电源工作于BUCK降压模式时，假设输入电压最大为48V，输出电压最小为5V，计算最小的PWM占空比：
$$
D_{min} = \frac{V_{out\_min}}{V_{in\_max}} = \frac{5V}{48V} = 10.417\%
$$

​	定义电感电流波动量(25%的最大额定电流脉动，设最大额定电流为10A)：

$$
\Delta I_L = 25\% \cdot I_{out\_nom} = 25\% \times 10A = 2.5A
$$

​	计算最小需要的BUCK电感：

$$
L_{minBuck} = \frac{V_{out\_min}}{\Delta I_L \cdot f_{switch}} 
= \frac{5V}{2.5A \times 181333Hz} \times (1 - 10.417\%) 
\approx 9.88\mu H
$$

​	当电源工作于BOOST升压模式时，以输入额定电压24V为计算点，计算PWM占空比：

$$
D_{Bo} = 1 - \frac{V_{in\_nom}}{V_{out\_max}} = 1 - \frac{24V}{48V} = 50\%
$$

​	Boost 模式下，取1A（$I_{minb}$）时Boost进入连续模式（CCM），计算电感：

$$
L_{minBoost} = \frac{V_{out\_max} \cdot D_{Bo}(1-D_{Bo})^2}{2 \cdot I_{minb} \cdot f_{switch}}  
= \frac{48V \times 50\% \times (1 - 50\%)^2}{2 \times 1A \times 181333Hz} 
= 16.544\mu H
$$

​	基于公式得出的计算结果，确定了为满足预定纹波电流要求所需的最小电感值。为了确保满足这一条件，应选用一个电感值略高于计算值的电感元件。同时，必须确保所选电感的饱和电流能够承载电路中的最高电流峰值。由于在计算过程中未将效率因素纳入考虑，实际的占空比和峰值电流可能会低于理论计算值。因此，在进行选择时，应该考虑到一定的安全余量，以适应实际工作条件下可能出现的各种情况。

​	综合考虑以上因素，本设计选用1770贴片封装的饱和电流10A以上的22μH的电感作为BUCK-BOOST电路的电感元件。

#### 电容计算

​	为了获得较好的输出电压纹波，取设计的电压纹波为50mV。

$$
\Delta V_{out} = 0.05V
$$

​	上面选用的电感为 22μH 的，所以这里计算也用这个电感值：

$$
L_{BB}=22\mu H
$$

​	分别计算工作于BUCK降压模式下和BOOST升压模式下所需要的最小电容量 $C_{minBuck}$ 和 $C_{minBoost}$ ：

$$
C_{minBuck}=\frac{V_{out\_min}\cdot(1-\frac{V_{out\_min}}{V_{in\_max}})}{8\cdot L_{BB}\cdot \Delta V_{out}\cdot {f_{switch}}^2}=15.48\mu F
$$

$$
C_{minBoost}=\frac{I_{out\_nom}\cdot(1-\frac{Vin\_min}{Vout\_max})}{\Delta Vout\cdot fswitch}=413.6\mu F
$$

​	设计需要留有一定余量，以及为更低的输出纹波，所以选用一颗低ESR的220μF的固态电容加一颗470μF的普通电解电容，共690μF。

#### MOS选型计算

​	在额定输入输出电压范围内，计算输入MOS电流的有效值为：

$$
i_{mos\_rms}=7.098A
$$

​	MOSFET选型中MOS管的额定电流值$I_D$需按流过MOS管最大电流的2倍以上（以防止故障或短路状态下电流过大造成冲击损坏）。

$$
2×i_{mos\_rms}=14.196A
$$

​	MOSFET选型的额定耐压值$V_{DS}$需大于最大输入电压的1.5倍（以防止尖峰击穿）

$$
1.5×Vin\_max=72V
$$

​	在参考了之前的计算之后，对于常规的应用场景，可以选择额定电流超过15安培（A）且耐压等级为100伏特（V）的MOSFET，下管MOSFET选型和上管一样，考虑到发热的情况，为了最大限度地减少导通和开关过程中的能量损耗，应优先选用具有低导通电阻（$R_{DS(on)}$）和低输出电容（Coss）的MOSFET。

​	本设计选用的MOSFET型号为CJAC80SN10，该器件由江苏长晶科技股份有限公司生产，是一款国产化的MOSFET。它具备100V的漏源耐压（$V_{DS}$）和最大80A的漏源电流（$I_D$）。其导通电阻（$R_{DS(on)}$）仅为6.2mΩ，这一低阻特性有利于减少器件在导通状态下的功率损耗。同时，该器件的输出电容Coss具有420pF的典型值，这一较低的电容量有利于降低开关转换过程中的动态损耗。因此，CJAC80SN10在满足项目所需的电气特性的同时，还能有效减少能量损耗，提高系统的整体效率。

### 电源板电路设计

#### 主功率电路

​	下图为同步BUCK-BOOST电源主功率电路图，左侧为输入，同步 BUCK降压电路由MOS管Q2和Q4与电感L1组成，同步BOOST升压电路由MOS管Q3和Q5与电感L1组成，每颗MOS管的栅极和源极间均并有10kΩ的电阻，以确保MOS的栅极不悬空，防止误动作导通。

![](https://tc2.zeruns.tech/2024/06/04/imagee4eecc6aa41fc7c6.png)

​	主功率电路整体上左右对称结构，输入输出端都设有1个470μF/63V的铝电解电容器和1个220μF/63V的固态铝质电解电容，此外，还有2个小的贴片MLCC（多层陶瓷电容器），参数为10μF/50V，用以过滤端口处的高频噪声干扰。R9和R10作为输入输出端口的假负载，可以在电源断开时能够迅速耗散电路内残留的能量。R13与R14是高精度的5mΩ电阻，用于电流采样，后级接差分放大电路来放大输入输出的电流信号。CNT1和CNT2为输出端子。L1和L2分别是贴片1770封装和直插磁环电感封装，选其中一个焊接即可，两个都画是为了方便测试不同电感的效果和性能。

#### 电源输入和快充协议电路

​	下图为电源的输入接口和PD快充协议通信的电路图。

​	电源输入采用了两种接口，分别是DC5.5\*2.5mm的母口和TypeC母口，其中TypeC接口支持BC1.2，PD3.0/2.0等多种快充协议，使用的快充协议芯片型号是CH224K，可以与快充充电器通信使充电器输出最高20V的电压，最高支持100W功率。

​	D1、D3、D4二极管作用是防止DC接口电压倒灌TypeC接口，设计上是不允许两个接口同时接入的。D5是用于反接保护的二极管。FH1为电源输入保险座，插12A的保险丝。

![](https://tc2.zeruns.tech/2024/06/04/image6389c97e68ccc163.png)

#### 驱动电路

​	在同步BUCK-BOOST电源的设计中，BUCK电路和BOOST电路均包含高侧N-MOSFET。传统上，对于这些高侧N-MOSFET的驱动通常采用变压器隔离驱动的方式，但这种方法会增加电路的复杂性，并且扩大了电路板的尺寸。

​	本设计选用两颗自带自举电路功能的MOS管驱动芯片EG3112用以驱动BUCK电路和BOOST电路的MOS管。EG3112是一款非隔离型的互补双通道驱动芯片；其2A的输出驱动电流能力确保了MOSFET能够迅速导通；芯片还内置了死区时间控制功能，以防止输出驱动信号发生直通，从而提高了系统的稳定性。具体电路如下图所示。

![](https://tc2.zeruns.tech/Pic/2024/06/image-20240604212659943_bbd9654002c450331cd56f50547128a7.png)

​	以BOOST升压电路的MOS管驱动为例，PWM2L与PWM2H是来自STM32G474微控制器输出的PWM信号，送入EG3112驱动芯片的LIN和HIN引脚，LO为下管驱动信号输出，驱动电阻阻值为10Ω，HO为上管驱动信号输出，驱动电阻阻值为 10Ω，D7为上管驱动电路自举二极管，D9和D11二极管用于快速释放栅极电荷，加快MOS管的关断速度，C14为自举电容。

​	驱动电阻作用是在MOSFET开关过程中，驱动信号可能会在PCB走线、分布电容、电感等元件上产生反射和振荡，通过串联电阻可以减少这种振荡，提高系统的稳定性和可靠性。

#### 辅助电源

​	下图是电源板辅助电源供电电路原理图。

![](https://tc2.zeruns.tech/2024/06/04/image2feef08fe62bfb2d.png)

​	第一级12V输出的辅助电源电路选用了集成高侧MOSFET的BUCK型电源芯片TPS54360B来设计。按照芯片手册，RT引脚接一个下拉电阻可以设置开关频率，这里选取110kΩ的电阻，对应开关频率是876.5kHz，较高的开关频率可以选择小一点的电感节省空间，根据这个频率计算电感取值应大于9.75μH，这里选取10μH的电感，选取SS310为续流二极管。C26与C27是输入滤波电容；反馈分压电阻R19和R25分压出0.8V的基准电压给芯片FB引脚后保证输出电压在12V时。C18和C19为第一级12V输出辅助电源的滤波电容。第一级12V输出主要供第二级6V降压电路的输入，以及MOSFET驱动电路和散热风扇使用。

​	第二级6V输出的辅助电源电路选用了集成MOSFET的同步整流BUCK型电源芯片SY8205来设计。按照芯片手册说明，芯片开关频率固定为500kHz，选取辅助电源BUCK电路的电感为10μH。C21与C22为输入滤波电容；反馈分压电阻R23和R26分压出0.6V的基准电压给芯片FB引脚后保证输出电压在6V时。C24和C25为第二级6V输出辅助电源的滤波电容。由于直接使用线性稳压器从12V降压到5V会造成比较大的损耗，所以采用先用开关电源降压至接近5V的电压再用线性稳压器降压至目标电压，这样可以保证较高的效率和较低的输出纹波。

​	第二级输出的6V电压经线性稳压芯片AMS1117-5降压成5V作为第三级辅助电源，电压基准芯片REF3033，供信号调理，OLED显示，USB通信等功能电路使用。

5V直流电经线性稳压芯片AMS1117-3.3降压成3.3V作为第四级辅助电源，供MCU，蜂鸣器，Flash芯片等电路使用。

​	电压基准芯片REF3033输出3.3V的基准电压供MCU的VREF端口使用，作为MCU内置ADC的参考电压，提高ADC的采样准确性。

#### 信号调理电路

![](https://tc2.zeruns.tech/2024/06/04/image5235dbc3d2789367.png)

​	如上图所示，本设计的输入和输出电压的采样和信号调理电路采用了差分放大技术。差分放大拓扑可以有效地降低开关电源中高频噪声对信号调理电路的干扰，提高了信号稳定和可靠性。在此电路中，选用了低零偏运算放大器GS8558-SR，以提升转换的精度。GS8558-SR运放具有优异的直流精度和低偏置电流特性，这对于提高整个信号调理电路的性能至关重要。

​	差分放大电路的反馈电阻：

$$
R_{38}=4.7kΩ
$$

$$
R_{36}=75kΩ
$$

​	计算差分放大倍数：

$$
K_V=\frac{R_{38}}{R_{36}}=0.062667
$$

​	MCU的ADC最大采样电压为电压基准芯片输出的3.3V，则可以计算出能采样到的输入和输出电压最大值为：

$$
V_{max}=\frac{3.3V}{K_V}=52.66V
$$

​	为了更好抑制开关电源中的高频噪声对差分放大电路的干扰，在运放的供电管脚添加去耦电容C40和C41用以滤波，电容选用MLCC（多层陶瓷电容器），MLCC由于具有较低的ESR和ESL，在高频噪声的滤波中具有更好的效果，同时输出端添加R35电阻和C39电容构成RC低通滤波器电路，用以滤除高频噪声。

![](https://tc2.zeruns.tech/2024/06/04/imagef80efbbbbfc7722d.png)

​	如上图所示，输入和输出电流调理电路采用差分放大的方式，输出电流采样电阻为：

$$
R_{14}=5mΩ
$$

​	差分放大电路的反馈电阻：

$$
R_{48}=6.2kΩ
$$

$$
R_{46}=100Ω
$$

​	计算差分放大倍数：

$$
K_I=\frac{R_{48}}{R_{46}}=62
$$

​	也就是每1A电流输出310mV的电压。

​	MCU的ADC最大采样电压为电压基准芯片输出的3.3V，则可以计算出能采样到的输入和输出电流最大值为：

$$
I_{max}=\frac{3.3V}{K_I \times R_{14}}=10.65A
$$

#### MCU控制电路

![](https://tc2.zeruns.tech/2024/06/04/imagedb4de164208aadb6.png)

​	本项数字电源设计选用了STM32G474RET6芯片作为控制器，该控制器具有外围电路简洁、控制方式多样、扩展能力优越等显著特点。为了实现精确的时钟信号，控制器采用了外部晶振X1，即一个频率为25兆赫兹（MHz）的石英晶体振荡器。此外，电路中还包含了多个滤波电容器，包括C45、C51、C56、C46和C52，它们分别用于微控制器（MCU）的不同数字电源引脚，以确保电源的稳定性和减少噪声干扰。R49电阻和C49电容构成上电复位电路，SW1为MCU复位按钮。U11是USB转串口的芯片，型号为CH340C，与第二个TypeC接口连接，串口与MCU的USART1接口连接。U10是Flash存储芯片，型号为W25Q64，用来存储参数设置等信息，与MCU的SPI3接口连接。Q6为蜂鸣器的驱动MOS管，用于控制蜂鸣器，MOS管栅极与MCU的PB5端口连接。H1接线端是SWD烧录口。H2和H4是PH2.0接线座，用于与控制面板连接，接线座预留有USART2接口，可方便将控制面板方案更换为串口屏，还可外加ESP32以增加联网和无线控制等功能。D13为5V反接保护二极管。

#### 散热风扇驱动和主板温度采样电路

​	主板温度采样电路和散热风扇驱动电路原理图如下图所示。电源主板温度采样的原理是使用NTC热敏电阻R2与下拉电阻R4串联分压输出到MCU的ADC端口进行采样，使用的NTC热敏电阻阻值为10kΩ的，B值为3950K。

​	散热风扇是使用一个N-MOS管来驱动，型号为AO3400，散热风扇接口上反向并联一个二极管D2用于防止由于电机产生的反电动势（back EMF）造成的损害。当电机断电时，由于旋转惯量，电机的转子不会立即停止转动，而是会继续旋转并产生电动势。这个电动势可能会导致电路中的晶体管或集成电路损坏，特别是当电机与这些元件通过半导体开关（如MOSFET）连接时。

![](https://tc2.zeruns.tech/2024/06/04/image8481a78abdd1262b.png)

### 控制面板电路设计

​	控制面板的电路原理图如下图所示。SW1是旋转编码器，用于设置参数等，SW2和SW3是按钮，SW2用于切换设置项，SW3用于控制电源输出的开启和关闭。LED1是系统运行状态指示灯，正常运行时以500mS的间隔闪烁，LED2是输出状态指示灯，输出开启时亮灯，输出关闭是灭灯。OLED1是OLED屏幕，用于显示电源参数和状态等信息。

![](https://tc2.zeruns.tech/2024/06/04/imageac5187c46ec927e9.png)

## 使用说明

​	通过按钮和旋转编码器可以设置输出的电压和电流值。如下图所示，反色显示的数值为当前要设置的位，通过旋转编码器可以增加或减少，按下编码器可以切换到下一位进行设置，通过SW2按键可以切换要设置的项目。设置的数据会自动保存到Flash存储芯片里，下次开机会从存储芯片里读出数据。

​	SW3按钮开启/关闭电源输出。

![](https://tc2.zeruns.tech/2024/06/04/image048512af72675ee7.png)

​	可切换到数据显示页面查看电源当前的输入输出电压和电流，以及主板温度和MCU温度等信息，如下图所示。

![](https://tc2.zeruns.tech/2024/06/04/image472a77ba11a65622.png)

​	可切换到设置页面设置过温/过流/过压保护的阈值，如下图所示。设置的数据会自动保存到Flash存储芯片里，下次开机会从存储芯片里读出数据。

![](https://tc2.zeruns.tech/2024/06/04/image13c3475597eb3015.png)

## 纹波测试

​	使用电子负载和示波器测试电源输出性能和输出纹波，如下图所示。在36V输入，12V2A输出时纹波峰峰值测得42mV左右，如下图所示。

![](https://tc2.zeruns.tech/2024/06/04/image1aec2ab4f0416d21.png)

![](https://tc2.zeruns.tech/2024/06/04/image5c172981426b5675.png)

## 转换效率测试

​	测试20V输入，12V10A输出时的电源转换效率为92%，如下图所示。

![](https://tc2.zeruns.tech/2024/06/04/image214e1bc17e0a1bec.png)

​	下表为各个不同的输入和输出电压下的转换效率，最高效率为94.3%。

| 输入电压(V) | 输入电流(A) | 输入功率(W) | 输出电压(V) | 输出电流(A) | 输出功率(W) | 转换效率(%) |
| :---------: | :---------: | :---------: | :---------: | :---------: | :---------: | :---------: |
|   20.003    |    4.035    |   80.712    |   15.010    |    5.000    |   75.050    |   92.985    |
|   47.999    |    5.335    |   256.075   |   24.040    |    9.900    |   237.996   |   92.940    |
|   48.000    |    7.875    |   378.000   |   36.020    |    9.900    |   356.598   |   94.338    |
|   48.000    |    9.860    |   473.280   |   45.030    |    9.900    |   445.797   |   94.193    |
|   23.998    |    8.835    |   212.022   |   48.070    |    4.000    |   192.280   |   90.689    |
|   23.998    |    9.830    |   235.900   |   35.998    |    6.001    |   216.024   |   91.574    |
|   12.099    |    9.166    |   110.899   |   24.070    |    4.000    |   96.280    |   86.817    |
|   20.008    |    2.645    |   52.921    |    4.970    |    9.000    |   44.730    |   84.522    |
|   20.008    |   10.550    |   211.084   |   24.030    |    8.000    |   192.240   |   91.073    |
|   36.000    |    6.418    |   231.048   |   24.010    |    9.000    |   216.090   |   93.526    |
|   36.000    |   10.540    |   379.440   |   35.950    |    9.800    |   352.310   |   92.850    |

## MOS管栅极波形

​	测试20V输入，24V输出时的各个MOS栅极波形。

​	BUCK电路上下管对地电压波形图：

![](https://tc2.zeruns.tech/2024/06/04/image2dbfee46c6cb8b48.png)

​	BOOST电路上下管对地电压波形图：

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

## 资料下载地址

下面链接包含的资料有：立创EDA工程、原理图PDF文件、各个芯片的数据手册、源代码工程压缩包、一些参考的程序代码。

**123云盘不限速下载地址：**[https://www.123pan.com/ps/2Y9Djv-8yevH.html](https://www.123pan.com/ps/2Y9Djv-8yevH.html)

百度网盘下载地址：[https://url.zeruns.tech/MW2d1](https://url.zeruns.tech/MW2d1)

**项目程序Gitee开源地址：**[https://gitee.com/zeruns/STM32-Buck-Boost](https://gitee.com/zeruns/STM32-Buck-Boost)

项目程序GitHub开源地址：

**求点个Star和点个赞。**


## 已知存在的问题

1. 辅助电源中的SY8205芯片在低负载时会进入PFM模式，频率较低，从而产生了轻微的噪音。
2. 恒流模式的PID控制程序写的不行，只有在纯电阻负载下恒流才稳定。（恒压模式没有问题）



## 其他开源项目推荐

- 做了个三相电量采集器开源出来，可以方便监测家里用电情况：[https://blog.zeruns.tech/archives/771.html](https://blog.zeruns.tech/archives/771.html)
- 基于STM32F407的LVGL工程模板（MSP3526屏幕），包含FreeRTOS版和裸机版：[https://blog.zeruns.tech/archives/788.html](https://blog.zeruns.tech/archives/788.html)
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

