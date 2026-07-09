#include "MPU6050.h"
#include "MPU6050_Reg.h"
#define MPU6050_I2C_PORT GPIOB
#define MPU6050_SCL_PIN GPIO_Pin_10
#define MPU6050_SDA_PIN GPIO_Pin_11
#define MPU6050_SCL_PINSOURCE GPIO_PinSource10
#define MPU6050_SDA_PINSOURCE GPIO_PinSource11

#define MPU6050_ADDRESS 0xD0
#define HARDWARE 0
#define SOFTWARE 1
#define MPU6050_I2C_Mode SOFTWARE
#define MPU6050_I2C I2C2


#if MPU6050_I2C_Mode == SOFTWARE
void I2C_W_SCL (uint8_t BitValue) {
    GPIO_WriteBit (MPU6050_I2C_PORT, MPU6050_SCL_PIN, (BitAction)(BitValue));
    Delay_Us (5);
}

void I2C_W_SDA (uint8_t BitValue) {
    GPIO_WriteBit (MPU6050_I2C_PORT, MPU6050_SDA_PIN, (BitAction)(BitValue));
    Delay_Us (5);
}

uint8_t I2C_R_SCL (void) {
    Delay_Us (5);
    uint8_t BitValue = GPIO_ReadInputDataBit (MPU6050_I2C_PORT, MPU6050_SCL_PIN);
    return BitValue;
}

uint8_t I2C_R_SDA (void) {
    Delay_Us (5);
    uint8_t BitValue = GPIO_ReadInputDataBit (MPU6050_I2C_PORT, MPU6050_SDA_PIN);
    return BitValue;
}

void I2C_Start (void) {
    I2C_W_SDA (1);
    I2C_W_SCL (1);
    I2C_W_SDA (0);
    I2C_W_SCL (0);
}

void I2C_Stop (void) {
    I2C_W_SDA (0);
    I2C_W_SCL (1);
    I2C_W_SDA (1);
}

void I2C_SendByte (uint8_t Byte) {
    for (uint8_t i = 0; i < 8; ++i) {
        I2C_W_SDA (Byte & (0x80 >> i));
        I2C_W_SCL (1);
        I2C_W_SCL (0);
    }
}

uint8_t I2C_ReceiveByte (void) {
    uint8_t Byte = 0x00;
    for (uint8_t i = 0; i < 8; ++i) {
        I2C_W_SDA (1);
        I2C_W_SCL (1);
        if (I2C_R_SDA() == 1) {
            Byte |= (0x80 >> i);
        }
        I2C_W_SCL (0);
    }
    return Byte;
}

void I2C_SendAck (uint8_t AckBit) {
    I2C_W_SDA (AckBit);
    I2C_W_SCL (1);
    I2C_W_SCL (0);
}

uint8_t I2C_ReceiveAck (void) {
    uint8_t AckBit;
    I2C_W_SDA (1);
    I2C_W_SCL (1);
    AckBit = I2C_R_SDA();
    I2C_W_SCL (0);
    return AckBit;
}
#endif


void MPU6050_WaitEvent (I2C_TypeDef *I2Cx, uint32_t I2C_EVENT) {
    uint32_t Timeout = 1000000;
    while (I2C_CheckEvent (I2Cx, I2C_EVENT) != SUCCESS) {
        Timeout--;
        if (Timeout == 0) {
            // 错误日志、紧急停机等操作
            return;
        }
    }
}

void MPU6050_WriteReg (uint8_t RegAddress, uint8_t Data) {
#if MPU6050_I2C_Mode == SOFTWARE
    I2C_Start();
    I2C_SendByte (MPU6050_ADDRESS);
    I2C_ReceiveAck();
    I2C_SendByte (RegAddress);
    I2C_ReceiveAck();
    I2C_SendByte (Data);
    I2C_ReceiveAck();
    I2C_Stop();
#else
    I2C_GenerateSTART (MPU6050_I2C, ENABLE);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress (MPU6050_I2C, MPU6050_ADDRESS, I2C_Direction_Transmitter);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData (MPU6050_I2C, RegAddress);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING);

    I2C_SendData (MPU6050_I2C, Data);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTOP (MPU6050_I2C, ENABLE);
#endif
}

uint8_t MPU6050_ReadReg (uint8_t RegAddress) {
    uint8_t Data;
#if MPU6050_I2C_Mode == SOFTWARE
    I2C_Start();
    I2C_SendByte (MPU6050_ADDRESS);
    I2C_ReceiveAck();
    I2C_SendByte (RegAddress);
    I2C_ReceiveAck();

    I2C_Start();
    I2C_SendByte (MPU6050_ADDRESS | 0x01);
    I2C_ReceiveAck();
    Data = I2C_ReceiveByte();
    I2C_SendAck (1);
    I2C_Stop();
#else
    I2C_GenerateSTART (MPU6050_I2C, ENABLE);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress (MPU6050_I2C, MPU6050_ADDRESS, I2C_Direction_Transmitter);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData (MPU6050_I2C, RegAddress);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTART (MPU6050_I2C, ENABLE);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress (MPU6050_I2C, MPU6050_ADDRESS, I2C_Direction_Receiver);
    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    I2C_AcknowledgeConfig (MPU6050_I2C, DISABLE);
    I2C_GenerateSTOP (MPU6050_I2C, ENABLE);

    MPU6050_WaitEvent (MPU6050_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED);
    Data = I2C_ReceiveData (MPU6050_I2C);

    I2C_AcknowledgeConfig (MPU6050_I2C, ENABLE);
#endif

    return Data;
}

void MPU6050_Init (void) {
    RCC_HB1PeriphClockCmd (RCC_HB1Periph_I2C2, ENABLE);
    RCC_HB2PeriphClockCmd (RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MPU6050_SCL_PIN | MPU6050_SDA_PIN;  // SCL | SDA
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
#if MPU6050_I2C_Mode == SOFTWARE
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init (MPU6050_I2C_PORT, &GPIO_InitStructure);

#else
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init (MPU6050_I2C_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig (MPU6050_I2C_PORT, MPU6050_SCL_PINSOURCE, GPIO_AF4);
    GPIO_PinAFConfig (MPU6050_I2C_PORT, MPU6050_SDA_PINSOURCE, GPIO_AF4);

    I2C_InitTypeDef I2CInitStructure;
    I2CInitStructure.I2C_Mode = I2C_Mode_I2C;
    I2CInitStructure.I2C_ClockSpeed = 1000000;
    I2CInitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2CInitStructure.I2C_Ack = I2C_Ack_Enable;
    I2CInitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2CInitStructure.I2C_OwnAddress1 = 0x00;
    I2C_Init (MPU6050_I2C, &I2CInitStructure);

    I2C_Cmd (MPU6050_I2C, ENABLE);
#endif

	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);				//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);				//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);				//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);					//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);			//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);			//加速度计配置寄存器，选择满量程为±16g
}

uint8_t MPU6050_GetID (void) {
    return MPU6050_ReadReg (MPU6050_WHO_AM_I);
}

void MPU6050_GetData (int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                      int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ) {
    int16_t DataH, DataL;                            // 定义数据高8位和低8位的变量

    DataH = MPU6050_ReadReg (MPU6050_ACCEL_XOUT_H);  // 读取加速度计X轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_ACCEL_XOUT_L);  // 读取加速度计X轴的低8位数据
    *AccX = (int16_t)((DataH << 8) | DataL);         // 数据拼接，通过输出参数返回

    DataH = MPU6050_ReadReg (MPU6050_ACCEL_YOUT_H);  // 读取加速度计Y轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_ACCEL_YOUT_L);  // 读取加速度计Y轴的低8位数据
    *AccY = (int16_t)((DataH << 8) | DataL);         // 数据拼接，通过输出参数返回

    DataH = MPU6050_ReadReg (MPU6050_ACCEL_ZOUT_H);  // 读取加速度计Z轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_ACCEL_ZOUT_L);  // 读取加速度计Z轴的低8位数据
    *AccZ = (int16_t)((DataH << 8) | DataL);         // 数据拼接，通过输出参数返回

    DataH = MPU6050_ReadReg (MPU6050_GYRO_XOUT_H);   // 读取陀螺仪X轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_GYRO_XOUT_L);   // 读取陀螺仪X轴的低8位数据
    *GyroX = (int16_t)((DataH << 8) | DataL);        // 数据拼接，通过输出参数返回

    DataH = MPU6050_ReadReg (MPU6050_GYRO_YOUT_H);   // 读取陀螺仪Y轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_GYRO_YOUT_L);   // 读取陀螺仪Y轴的低8位数据
    *GyroY = (int16_t)((DataH << 8) | DataL);        // 数据拼接，通过输出参数返回

    DataH = MPU6050_ReadReg (MPU6050_GYRO_ZOUT_H);   // 读取陀螺仪Z轴的高8位数据
    DataL = MPU6050_ReadReg (MPU6050_GYRO_ZOUT_L);   // 读取陀螺仪Z轴的低8位数据
    *GyroZ = (int16_t)((DataH << 8) | DataL);        // 数据拼接，通过输出参数返回
}
