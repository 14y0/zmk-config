# ThreeKey —— 基于 nRF52840 + ZMK 的超小型无线可编程键盘

<p align="center">
一个三键极简无线键盘项目
</p>


---

# 1. 项目简介

ThreeKey 是一个基于 **Nordic nRF52840 MCU** 和 **ZMK 固件框架** 开发的超小型无线可编程键盘项目。

该项目采用：

- nRF52840 ProMicro 兼容控制板
- ZMK Keyboard Firmware
- Bluetooth Low Energy（BLE）
- USB HID 有线模式
- SSD1306 128×64 OLED 显示屏
- 3.7V 锂电池供电

设计目标是在极简硬件结构下，实现一个完整的现代无线键盘系统。


与传统机械键盘采用矩阵扫描不同，本项目采用：

```
每个按键独立连接 MCU GPIO
```

的方式。

因此：

- 无需矩阵扫描
- 硬件结构简单
- 固件配置简单
- 功耗更低
- 易于扩展


---

# 2. 项目设计目标


## 2.1 硬件目标

- 构建最小化无线输入设备
- 支持蓝牙无线连接
- 支持 USB 有线连接
- 支持锂电池供电
- 集成 OLED 信息显示
- 降低待机功耗


## 2.2 软件目标

- 基于 ZMK 开源键盘固件
- 支持完整键位自定义
- 支持 Layer 层切换
- 支持高级按键行为
- 支持持续扩展


---

# 3. 系统整体架构


```
                     SSD1306 OLED
                     128×64
                         |
                         |
                        I2C
                         |
                         |
+------------------------------------------------+
|                                                |
|              ProMicro nRF52840                 |
|                                                |
|                                                |
|    GPIO输入              BLE          USB       |
|       |                   |            |        |
|       |                   |            |        |
|    三个按键          蓝牙通信     USB HID       |
|                                                |
|                                                |
|              3.7V Li-ion Battery               |
|                                                |
+------------------------------------------------+

```


系统组成：

```
nRF52840
    |
    +-- 三个GPIO按键输入
    |
    +-- I2C OLED显示
    |
    +-- BLE无线通信
    |
    +-- USB有线通信
    |
    +-- 锂电池供电
```


---

# 4. 硬件配置


## 4.1 主控 MCU


| 项目 | 参数 |
|-|-|
| 芯片 | Nordic nRF52840 |
| 架构 | ARM Cortex-M4F |
| 主频 | 64MHz |
| Flash | 1MB |
| RAM | 256KB |
| 蓝牙 | BLE 5.x |
| 工作电压 | 3.3V |


开发板：

```
国产 ProMicro nRF52840
```


ZMK 固件目标：

```
nice_nano_v2
```


---

# 5. 按键硬件设计


## 5.1 按键布局


本键盘包含三个独立按键：

```

+------+-------+------+
| KEY1 | KEY2  | KEY3 |
+------+-------+------+

```


不采用矩阵扫描。


每个按键：

```
GPIO ---- 按键 ---- GND
```


MCU 使用内部上拉：

```
GPIO_PULL_UP
```

按下状态：

```
LOW
```


## 5.2 旋转编码器（EC11）硬件设计

> 扩展可选器件：EC11 增量式旋转编码器，带按压开关。

编码器共 5 个引脚：A(CLK)、B(DT)、C(公共端)、D/SW(开关一端)、E(开关另一端)。**无源器件，不需要 VCC 供电，依靠 MCU 内部上拉。**

电路逻辑：

- A 相、B 相：GPIO 输入，开启**内部上拉**，输出正交脉冲，用于检测旋转方向和步数
- C：A/B 相公共端，接 GND
- D/SW：按压开关一端，接 GPIO 输入，内部上拉
- E：按压开关另一端，接 GND，并并联 100nF 电容用于硬件消抖

按下开关时，对应 GPIO 被拉低到 GND。

```
A(CLK)  ---- GPIO (内部上拉)
B(DT)   ---- GPIO (内部上拉)
C       ---- GND
D(SW)   ---- GPIO (内部上拉)
E       ---- GND (并联 100nF 消抖电容)
```


---

# 6. GPIO 引脚映射


## 6.1 按键


|功能|Arduino编号|nRF52840 GPIO|
|-|-|-|
|KEY1|D2|P0.17|
|KEY2|D3|P0.20|
|KEY3|D4|P0.22|


ZMK 配置：

```dts
GPIO_ACTIVE_LOW
GPIO_PULL_UP
```


## 6.2 旋转编码器（EC11）

| 功能 | Arduino 编号 | nRF52840 GPIO | 说明 |
|-|-|-|-|
| EC11 A 相 (CLK) | D6 | P1.00 | 旋转编码 A 相，内部上拉 |
| EC11 B 相 (DT) | D7 | P0.11 | 旋转编码 B 相，内部上拉 |
| EC11 公共端 (C) | — | GND | A/B 相公共地 |
| EC11 开关 (SW) | D5 | P0.24 | 按压按键，内部上拉，按下拉低 |
| EC11 开关另一端 (E) | — | GND | 并联 100nF 消抖电容 |

ZMK 配置要点：

```dts
/* 编码器使用 alps_ec11 驱动，A/B 相接入传感器节点 */
/* 开关作为普通按键，GPIO_ACTIVE_LOW + GPIO_PULL_UP */
```


---

# 7. OLED 显示模块


## 7.1 OLED 参数


型号：

```
SSD1306
```


参数：

|项目|参数|
|-|-|
|分辨率|128×64|
|接口|I2C|
|地址|0x3C|
|供电|3.3V|


---

## 7.2 OLED连接


|OLED引脚|MCU|
|-|-|
|SDA|D20 / P0.29|
|SCL|D21 / P0.31|
|VCC|3.3V|
|GND|GND|


OLED用于显示：

- 蓝牙连接状态
- 当前 Layer
- 键盘状态
- 电池信息（后续支持）


---

# 8. 电源系统


## 8.1 电池


规格：

```
3.7V 500mAh Li-ion
```


供电结构：

```

             USB 5V
                |
                |
            充电管理
                |
                |
          Li-ion Battery
                |
                |
             3.3V系统
                |
                |
          nRF52840 + OLED


```


---

# 9. USB 与电池同时工作


系统支持：

```
USB + 电池同时连接
```


工作逻辑：

## USB连接时

- USB HID模式工作
- MCU由USB供电
- 电池进行充电（取决于板载充电电路）


## 拔掉USB

- 自动切换电池供电
- BLE无线模式工作


---

# 10. 固件架构


本项目使用：

```
ZMK Firmware
```


软件架构：

```

应用层
 |
 |
ZMK Keyboard Framework
 |
 |
Zephyr RTOS
 |
 |
nRF52840 Hardware

```


---

# 11. 工程目录结构


```
zmk-config

├── boards
│   └── shields
│       └── threekey
│           |
│           ├── Kconfig.shield
│           ├── Kconfig.defconfig
│           ├── threekey.conf
│           ├── threekey.keymap
│           └── threekey.overlay
│
├── config
│
├── zephyr
│
└── build.yaml

```


---

# 12. ZMK配置


## Board

```yaml
board: nice_nano_v2
```


## Shield

```yaml
shield: threekey
```


---

# 13. 当前键位映射


当前测试布局：

|物理按键|输出|
|-|-|
|KEY1|A|
|KEY2|B|
|KEY3|C|


配置：

```dts
bindings = <
    &kp A
    &kp B
    &kp C
>;
```


---

# 14. 固件编译流程


项目使用：

```
GitHub Actions
```


流程：


```

提交代码

    |

GitHub Actions

    |

ZMK自动编译

    |

生成UF2固件

```


生成文件：

```
threekey.uf2
```


---

# 15. 固件烧录


## 进入Bootloader


快速双击：

```
RESET
RESET
```


约：

```
0.5秒内
```


设备进入：

```
NICENANO
```


复制：

```
threekey.uf2
```

进入U盘即可完成烧录。


---

# 16. 蓝牙连接


烧录完成后：

1. 打开键盘电源

2. 打开电脑蓝牙

3. 搜索：

```
ThreeKey
```

4. 完成配对


---

# 17. 当前开发状态


## 已完成

- [x] nRF52840硬件平台搭建
- [x] ZMK环境配置
- [x] 三GPIO按键设计
- [x] OLED硬件设计
- [x] USB/BLE双模式架构


## 调试中

- [ ] 固件首次编译
- [ ] OLED显示验证
- [ ] 蓝牙连接测试
- [ ] 功耗测试


---

# 18. 后续开发计划


## 固件方向

计划增加：

- 多Layer支持
- Tap Dance
- Mod-Tap
- 宏功能
- 自定义快捷键


## 硬件方向

计划增加：

- 更小PCB尺寸
- 电源开关
- 电池电量检测
- RGB状态灯
- 3D打印外壳


---

# 19. 项目总结


ThreeKey 是一个基于现代嵌入式架构的小型无线键盘项目：

```
nRF52840
    +
ZMK
    +
BLE
    +
OLED
    +
Li-ion Battery
    +
GPIO Direct Keys
```


通过极简硬件设计，实现：

- 可编程
- 低功耗
- 无线化
- 可扩展

的小型智能输入设备平台。


---

# License

Open Hardware / Open Firmware Project
