#include "main.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);

//24C02的相关代码
void iic_24c02_write(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);
void iic_24c02_read(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);


//MCP4017的相关代码
void write_resistor(uint8_t value);
uint8_t read_resistor(void);