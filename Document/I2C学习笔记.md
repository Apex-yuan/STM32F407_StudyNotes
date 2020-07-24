---
typora-root-url: ..
---

I2C学习笔记



# 一、学习《I2C总线规范》

### I2C总线特征：

- 两条总线：一条串行数据线（SDA）；一条串行时钟线（SCL）
- 传输速率：
  - 标准模式：100kbit/s
  - 快速模式：400kbit/s
  - 高速模式：3.4Mbit/s
- 连接到总线的IC数量只受到总线的最大电容400pF的限制。
- 每个连接到总线的设备都有唯一的地址

### 重要知识点

- SDA和SCL都是双向线路，**都要通过上拉电阻接到电源的正极**。总线空闲时，两条线路都是高电平。
- 连接到总线的器件必须配置为**开漏输出**
- 每传输一个数据位就需要产生一个时钟脉冲
- SDA 线上的数据必须在时钟的高电平周期保持稳定 **数据线的高或低电平状态只有在 SCL 线的时钟信号是低电平时才能改变**  （如图4）

![image-20200718124052182](Image/image-20200718124052182.png)

- 起始和停止条件（如图5）：
  - 起始条件：**SCL线是高电平时，SDA线从高电平向低电平切换**
  - 停止条件：**SCL线是高电平时，SDA线从低电平向高电平切换**

![image-20200718124552347](Image/image-20200718124552347.png)

- 数据传输
  - **发送到SDA线上的每个字节必须为8位。**
  - 每次传输可以发送的字节数不受限制。
  - **每个字节后必须跟一个响应位。**
  - 首先传输的是数据的最高位（MSB）

![image-20200718130546326](Image/image-20200718130546326.png)

- 响应
  - 数据传输必须带响应（ACK或NACK），响应的时钟脉冲由主机产生。
  - **在响应的时钟脉冲期间，发送器释放SDA总线（即将SDA总线设置为高电平）**
  - **被寻址的接收器在接收到每个字节后，必须产生一个响应（ACK或NACK）。**
  - **主机作为接收器时，主机必须在从机发送完最后一个字节数据后产生一个NACK响应，通知从机数据结束。从机必须释放数据总线，允许主机产生一个停止或重复起始条件。**

![image-20200718130750822](Image/image-20200718130750822.png)

- **同步**、**仲裁**等内容待需要时再补充

### 数据传输

数据的传输遵循图 10 所示的格式。**在起始条件（S）后 发送了一个从机地址。 这个地址共有 7 位，紧接着的第 8 位是数据方向位（R/ W）--- ‘0’表示发送（写），‘1’表示请求数据（读）。数据传输一般由主机产生的停止位（P）终止**   

![image-20200718131502986](Image/image-20200718131502986.png)

# 二、实现软件I2C

根据上述I2C总线规范的内容一步步实现软件I2C通信的代码

这里以STM32F103为例编写实现代码

## 实现

### 1. 预处理

主要包含引脚定义，和一些便于IO操作的宏。

```c
#include "stm32f10x.h"

#define I2C_GPIO_PORT    GPIOB
#define I2C_GPIO_CLK     RCC_APB2Periph_GPIOB
#define I2C_SCL_PIN      GPIO_Pin_6
#define I2C_SDA_PIN      GPIO_Pin_7

#define HIGH ((uint32_t)1)
#define LOW  (uint32_t)0

#define SCL_OUTPUT(x)  if(x == 0) RDA5807M_GPIO_PORT->BRR = RDA5807M_SCL_PIN; else RDA5807M_GPIO_PORT->BSRR = RDA5807M_SCL_PIN
#define SDA_OUTPUT(x)  if(x == 0) RDA5807M_GPIO_PORT->BRR = RDA5807M_SDA_PIN; else RDA5807M_GPIO_PORT->BSRR = RDA5807M_SDA_PIN
#define SDA_INPUT()     RDA5807M_GPIO_PORT->IDR & RDA5807M_SDA_PIN
```

后面会用到的延时函数。后面可以通过更改延时时间的长短调整I2C通信的频率。

```c
void i2c_delay(void) {
    delay_us(4);
}
```



### 2. `i2c_init()`

初始化用到的GPIO引脚，配置为**开漏输出**，并分别释放SDA总线和SCL总线（即将总线设置为高电平）

```c
static void i2c_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_APB2PeriphClockCmd(RDA5807M_GPIO_CLK, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = RDA5807M_SCL_PIN | RDA5807M_SDA_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RDA5807M_GPIO_PORT, &GPIO_InitStruct);
  SCL_OUTPUT(HIGH); //释放SCL总线
  SDA_OUTPUT(HIGH); //释放SDA总线
}
```

### 3. `i2c_start()`

实现起始信号：**SCL线是高电平时，SDA线从高电平向低电平切换**

<img src="Image/image-20200718134022778.png" alt="image-20200718134022778" style="zoom:50%;" />

```c
void i2c_start(void) {
  SCL_OUTPUT(HIGH);
  SDA_OUTPUT(HIGH);
  i2c_delay();
  SDA_OUTPUT(LOW);
  i2c_delay();
}
```

### 4.`i2c_stop()`

实现停止条件：**SCL线是高电平时，SDA线从低电平向高电平切换**

<img src="Image/image-20200718134532728.png" alt="image-20200718134532728" style="zoom:50%;" />

```c
static void i2c_stop(void) {
  SCL_OUTPUT(HIGH);
  SDA_OUTPUT(LOW);
  i2c_delay();
  SDA_OUTPUT(HIGH);
  i2c_delay();
}
```

### `5.i2c_ack() i2c_nack()`

实现主机向从机发送ACK或NACK响应的函数，当主机作为接收器时才会用到。主机作为接收器时，从机每向主机发送一个字节的数据主机便需要向从机发送一个响应（ACK/NACK）。

![image-20200718130750822](Image/image-20200718130750822.png)

ACK：SCL时钟处于高电平期间，SDA保持低电平

NACK：SCL时钟处于高电平期间，SDA保持高电平

```c
static void i2c_ack(void) {
  SCL_OUTPUT(LOW);
  SDA_OUTPUT(LOW);
  i2c_delay();
  SCL_OUTPUT(HIGH);
  i2c_delay();
  SCL_OUTPUT(LOW);
  i2c_delay();
}

static void i2c_nack(void) {
  SCL_OUTPUT(LOW);
  SDA_OUTPUT(HIGH);
  i2c_delay();
  SCL_OUTPUT(HIGH);
  i2c_delay();
  SCL_OUTPUT(LOW);
  i2c_delay();
}
```

### 6. `i2c_waitACK()`

主机作为发送器时，在发送完一个字节的数据后需要等待从机发来的响应信号（ACK/NACK）。

```c
uint8_t i2c_waitAck(void) {
  uint8_t res;
  
  SDA_OUTPUT(HIGH); //主机释放SDA总线，让从机占用
  SCL_OUTPUT(LOW); //主机产生响应的时钟脉冲 
  i2c_delay();
  i2c_delay();
  SCL_OUTPUT(HIGH);
  i2c_delay();
  res = SDA_INPUT(); //读取从机产生的ACK(0)还是NACK(1)信号
  i2c_delay();
  SCL_OUTPUT(LOW);
  return res;
}
```

### 7.`i2c_writeByte()`

实现主机发送一个字节的数据到从机。**SDA 线上的数据必须在时钟的高电平周期保持稳定**. **数据线的高或低电平状态只有在 SCL 线的时钟信号是低电平时才能改变**

![image-20200718140725216](Image/image-20200718140725216.png)

```c
void i2c_writeByte(uint8_t byte) {
  for(uint8_t i = 0; i < 8; ++i) {
    SCL_OUTPUT(LOW);  //在SCL低电平期间改变数据线高低电平的状态
    i2c_delay();
    if(byte & 0x80) {
      SDA_OUTPUT(HIGH);
    } else {
      SDA_OUTPUT(LOW);
    }
    byte <<= 1;
    i2c_delay();
    SCL_OUTPUT(HIGH);
    i2c_delay();
    i2c_delay();
  }
  SCL_OUTPUT(LOW);
  //SDA_OUTPUT(HIGH);
}
```

### 8.`i2c_readByte()`

实现主机接收从机发来的一个字节数据。

```c
static uint8_t i2c_readByte(void) {
  uint8_t byte = 0;
  
  SDA_OUTPUT(HIGH); //主机释放SDA总线
  for(uint8_t i = 0; i < 8; ++i) {
    byte <<= 1;
    SCL_OUTPUT(LOW);
    i2c_delay();
    i2c_delay();
    SCL_OUTPUT(HIGH);
    i2c_delay();
    if(SDA_INPUT()) {
      byte |= 0x01;
    }
    i2c_delay();
  }
  SCL_OUTPUT(LOW);
  return byte;
}
```



## 测试

通过该软件I2C向RDA5807收音机模块中写入和读出数据来测试该协议是否可用！

关于RDA5807读写时序的信息可参考下面博客：

https://blog.csdn.net/m0_38101413/article/details/90116119

移植上面博客中的读写RDA5807寄存器的函数

```c
void rda5807_writeReg(u8 reg_Addr, u16 value)
{
  i2c_start();
  i2c_writeByte(0x22);
  i2c_waitAck();
  i2c_writeByte(reg_Addr);
  i2c_waitAck();
  i2c_writeByte(value >> 8);
  i2c_waitAck();
  i2c_writeByte(value);
  i2c_waitAck();
  i2c_stop();
}

//读写寄存器
unsigned int rda5807_readReg(u8 reg_Addr)
{
  unsigned int buf = 0;
  i2c_start();
  i2c_writeByte(0x22);
  i2c_waitAck();
  i2c_writeByte(reg_Addr);
  i2c_waitAck();
  i2c_start();
  i2c_writeByte(0x23);
  i2c_waitAck();
  buf = i2c_readByte();
  i2c_ack();
  buf = buf << 8;
  buf = buf | i2c_readByte();
  i2c_nack();
  i2c_stop();
  return buf;
}
```

通过软件I2C向RDA5807收音机模块写入数据：（向0x06寄存器写入0x1111）。通过逻辑分析仪测得的写入时序如下：

![image-20200718151617455](Image/image-20200718151617455.png)

  通过软件I2C读出之前写入的数据：（读取0x06寄存器的值）。通过逻辑分析仪测得的写入时序如下：

![image-20200718152026879](/Image/image-20200718152026879.png)

从逻辑分析仪的分析结果可以看出，当前软件I2C的通信速度约为90kHz(即90kbit/s,一个时钟周期传输一个数据)

## 总结

最原始的资料才是最好的资料，学习I2C认真看《I2C总线协议》才是王道。