#include "main.h"
#include "RCC\bsp_rcc.h"
#include "KEY_LED\bsp_key_led.h"
#include "LCD\bsp_lcd.h"
#include "UART\bsp_uart.h"
#include "I2C\bsp_i2c.h"
#include "ADC\bsp_adc.h"
//#include "TIM\bsp_tim.h"
#include "RTC\bsp_rtc.h"

//***全局变量声明区
//*减速变量
__IO uint32_t uwTick_Key_Set_Point = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Led_Set_Point = 0;//控制Led_Proc的执行速度
__IO uint32_t uwTick_Lcd_Set_Point = 0;//控制Lcd_Proc的执行速度
__IO uint32_t uwTick_Usart_Set_Point = 0;//控制Usart_Proc的执行速度



//*按键扫描专用变量
uint8_t ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;

//*LED专用变量
uint8_t ucLed;

//*LCD显示专用变量
uint8_t Lcd_Disp_String[21];//最多显示20个字符

//*串口专用变量
uint16_t counter = 0;
uint8_t str[40];
uint8_t rx_buffer;
uint8_t rx_buf[100];//接收到的指令临时存放缓冲区
uint8_t rx_buf_index = 0;//控制数据往buf里边存储的顺序。


//*EEPROM的相关变量
//uint8_t EEPROM_String_1[5] = {0x11,0x22,0x33,0x44,0x55};
//uint8_t EEPROM_String_2[5] = {0};
//uint8_t K_Storage_EEPROM;

//*4017相关变量
//uint8_t RES_4017;

//*pwm相关变量
//uint16_t PWM_T_Count;
//uint16_t PWM_D_Count;
//float PWM_Duty;


//*rtc相关变量
RTC_TimeTypeDef H_M_S_Time;
RTC_DateTypeDef Y_M_D_Date;


//用户自定义变量区
uint8_t Interface_Num;//0x00-显示界面，0x10-设置上报时间的小时，0x11-设置分钟，0x12-设置秒。
float R37_Voltage;
uint8_t k_int = 1;
uint8_t LED_Ctrl = 0;// 0-打开，1关闭，控制LED报警功能
uint8_t Clock_Comp_Disp[3] = {0,0,0};//闹钟比较值的初值（显示专用）
uint8_t Clock_Comp_Ctrl[3] = {0,0,0};//闹钟比较值的初值（控制专用）
__IO uint32_t uwTick_SETTING_TIME_Set_Point = 0;//控制待设置的时间数值闪烁
uint8_t SETTING_TIME_Ctrl = 0;// 0-亮，1-灭，控制时间设置界面的待设置值的闪烁功能
uint8_t Ctrl_Uart_Send_Time_Data_Times = 0;// 控制只允许到闹钟时间后只上报一次
__IO uint32_t Uart_Rev_Data_Delay_Time = 0;//控制串口接收数据的等待时间
_Bool Start_Flag;//起始位判断
__IO uint32_t uwTick_LED_bulingbuling_Set_Point = 0;//控制LED报警闪烁的打点变量
	
//***子函数声明区
void Key_Proc(void);
void Led_Proc(void);
void Lcd_Proc(void);
void Usart_Proc(void);


//***系统主函数
int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

	/*bsp资源的初始化*/
	KEY_LED_Init();
	
	LCD_Init();
	LCD_Clear(White);
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);	
	
	UART1_Init();
	I2CInit();
//  ADC1_Init();
  ADC2_Init();
//	
//	BASIC_TIM6_Init();
//	PWM_INPUT_TIM2_Init();
//	SQU_OUTPUT_TIM15_Init();
//	PWM_OUTPUT_TIM3_Init();
//	PWM_OUTPUT_TIM17_Init();
  RTC_Init();



	/*外设使用基本配置*/	
	//*EEPROM测试
//	iic_24c02_write(EEPROM_String_1, 0, 5);
//	HAL_Delay(1);	
//	iic_24c02_read(EEPROM_String_2, 0, 5);
	iic_24c02_read(&k_int,0,1);//去EEPOM中读取k的初值
	
  //*MCP4017测试
//	write_resistor(0x77);
//	RES_4017 = read_resistor();	
	
	//*串口接收中断打开
	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 1);
	
	//*打开基本定时器
//	HAL_TIM_Base_Start_IT(&htim6);//每100ms触发一次中断
		
	//*输入捕获PWM启动
//	HAL_TIM_Base_Start(&htim2);  /* 启动定时器 */
//  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);		  /* 启动定时器通道输入捕获并开启中断 */
//  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);		
		
	//*输出方波PA2引脚	
//  HAL_TIM_OC_Start_IT(&htim15,TIM_CHANNEL_1);
	
	//*启动定时器3和定时器17通道输出
//  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6
//  HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PA7
		

		
		
		
		
  while (1)
  {
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
		Usart_Proc();
		
		
		
  }

}


//***按键扫描子函数
void Key_Proc(void)
{
	if((uwTick -  uwTick_Key_Set_Point)<50)	return;//减速函数
		uwTick_Key_Set_Point = uwTick;

	ucKey_Val = Key_Scan();
	unKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val); 
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);	
	ucKey_Old = ucKey_Val;
	
//	if(unKey_Down == 4)
//	{
//		ucLed = 0x88;
//	}
//	if(unKey_Down == 3)
//	{
//		ucLed = 0x00;
//	}	
//uint8_t Interface_Num;//0x00-显示界面，0x10-设置上报时间的小时，0x11-设置分钟，0x12-设置秒。
	
	if(unKey_Down == 1)//B1完成LED报警功能的打开和关闭
	{
		LED_Ctrl ^= 0x01;//让最后一位翻滚
	}
	
	if(unKey_Down == 2)//B2完成两个界面的切换
	{
		if(Interface_Num == 0x00)
		{
			LCD_Clear(White);//清屏
			Interface_Num = 0x10;
			
			
		}
		else if((Interface_Num>>4) == 0x1)
		{
			LCD_Clear(White);//清屏
			Interface_Num = 0x00;	

			Clock_Comp_Ctrl[0] = Clock_Comp_Disp[0];	//更新闹钟显示值到控制值
			Clock_Comp_Ctrl[1] = Clock_Comp_Disp[1];	//更新闹钟显示值到控制值	
			Clock_Comp_Ctrl[2] = Clock_Comp_Disp[2];	//更新闹钟显示值到控制值				
		}			
	}
	if(unKey_Down == 3)//B3切换时分秒
	{
	 if((Interface_Num>>4) == 0x1)
			if(++Interface_Num == 0x13)
					Interface_Num = 0x10;
	}	
	if(unKey_Down == 4)//B4调整设定值的时分秒
	{
		if(Interface_Num == 0x10)
		{
			if( ++Clock_Comp_Disp[0] == 24)
				Clock_Comp_Disp[0] = 0;
		}
		else if(Interface_Num == 0x11)
		{
			if( ++Clock_Comp_Disp[1] == 60)
				Clock_Comp_Disp[1] = 0;		
		}
		else if(Interface_Num == 0x12)
		{
			if( ++Clock_Comp_Disp[2] == 60)
				Clock_Comp_Disp[2] = 0;			
		}
	}		
	
	
	
	
}


//***LED扫描子函数
void Led_Proc(void)
{
	if((uwTick -  uwTick_Led_Set_Point)<50)	return;//减速函数
		uwTick_Led_Set_Point = uwTick;

	if(LED_Ctrl == 0x1)//关闭LED的功能的时候
	{
		ucLed = 0x00;
	}
	else//开启LED功能的时候
	{
		if(R37_Voltage>=(3.3*k_int*0.1))
		{
			if((uwTick-uwTick_LED_bulingbuling_Set_Point)>=200)
			{
				uwTick_LED_bulingbuling_Set_Point = uwTick;
				ucLed ^= 0x1;
			}
		}
		else
			ucLed = 0x00;			
	}
//	
	LED_Disp(ucLed);
}


void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<100)	return;//减速函数
		uwTick_Lcd_Set_Point = uwTick;
	
//	//开机屏幕测试代码
//	sprintf((char *)Lcd_Disp_String, "TEST :%02X",Interface_Num);
//	LCD_DisplayStringLine(Line9, Lcd_Disp_String);	
//	
//	//*EEPROM测试
//	sprintf((char *)Lcd_Disp_String, "EE:%x%x%x%x%x",EEPROM_String_2[0],EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4]);
//	LCD_DisplayStringLine(Line0, Lcd_Disp_String);

//	//*ADC测试
////	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",((((float)getADC1())/4096)*3.3));
////	LCD_DisplayStringLine(Line1, Lcd_Disp_String);	
////	
//	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",((((float)getADC2())/4096)*3.3));
//	LCD_DisplayStringLine(Line2, Lcd_Disp_String);	
//	
//	//*PWM输入捕获测试，测量占空比和频率
////	sprintf((char *)Lcd_Disp_String, "R40P:%05dHz,%4.1f%%",(unsigned int)(1000000/PWM_T_Count),PWM_Duty*100);
////	LCD_DisplayStringLine(Line3, Lcd_Disp_String);		
//	
//	//*RTC内容显示
//	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
//	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
//	sprintf((char *)Lcd_Disp_String, "Time:%02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
//	LCD_DisplayStringLine(Line4, Lcd_Disp_String);		

		//数据采集区（借用位置）
		R37_Voltage = ((((float)getADC2())/4096)*3.3);
		HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
		HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);

		//数据显示区
		if(Interface_Num == 0x00)//内容显示界面
		{
			sprintf((char *)Lcd_Disp_String, "    V1:%4.2fV",R37_Voltage);
			LCD_DisplayStringLine(Line2, Lcd_Disp_String);	
			
			sprintf((char *)Lcd_Disp_String, "    k:%3.1f",(k_int*0.1));
			LCD_DisplayStringLine(Line4, Lcd_Disp_String);	
			
			if(LED_Ctrl == 0)
				sprintf((char *)Lcd_Disp_String, "    LED:ON ");		
			else
				sprintf((char *)Lcd_Disp_String, "    LED:OFF");						
		  LCD_DisplayStringLine(Line6, Lcd_Disp_String);	
		
			sprintf((char *)Lcd_Disp_String, "    T:%02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
			LCD_DisplayStringLine(Line8, Lcd_Disp_String);			
		}
		
		//时间设置区
		if((Interface_Num>>4) == 0x1)//进入设置界面
		{		
			sprintf((char *)Lcd_Disp_String, "       Setting");
			LCD_DisplayStringLine(Line2, Lcd_Disp_String);					
			sprintf((char *)Lcd_Disp_String, "      %02d-%02d-%02d",(unsigned int)Clock_Comp_Disp[0],(unsigned int)Clock_Comp_Disp[1],(unsigned int)Clock_Comp_Disp[2]);
			if((uwTick - uwTick_SETTING_TIME_Set_Point)>=500)
			{
				uwTick_SETTING_TIME_Set_Point = uwTick;
				SETTING_TIME_Ctrl ^= 0x1;
			}
			
			if(SETTING_TIME_Ctrl == 0x1)//用来控制闪烁，时间设置时候的闪烁。
			{
				if(Interface_Num == 0x10)//设置时	
					{
						Lcd_Disp_String[6] = ' ';
						Lcd_Disp_String[7] = ' ';						
					}
				else if(Interface_Num == 0x11)//设置分
				{
						Lcd_Disp_String[9] = ' ';
						Lcd_Disp_String[10] = ' ';					
				}
				else if(Interface_Num == 0x12)//设置秒
				{
						Lcd_Disp_String[12] = ' ';
						Lcd_Disp_String[13] = ' ';					
				}
			}
			LCD_DisplayStringLine(Line5, Lcd_Disp_String);		

			
		}
		






}

void Usart_Proc(void)
{
	if((uwTick -  uwTick_Usart_Set_Point)<30)	return;//减速函数
	uwTick_Usart_Set_Point = uwTick;
	
//	sprintf(str, "%04d:Hello,world.\r\n", counter);
//	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);
//	
//	if(++counter == 10000)
//		counter = 0;
	
	//闹钟时间到
	if((H_M_S_Time.Hours == Clock_Comp_Ctrl[0])&&(H_M_S_Time.Minutes == Clock_Comp_Ctrl[1])&&(H_M_S_Time.Seconds == Clock_Comp_Ctrl[2]))
	{

		if(Ctrl_Uart_Send_Time_Data_Times == 0)//控制只发送一次数据，虽然通过扫描进来了五次
		{
				Ctrl_Uart_Send_Time_Data_Times = 1;
				sprintf(str, "%4.2f+%3.1f+%02d%02d%02d\n", R37_Voltage,(k_int*0.1),(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
				HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);			
		}
	}
	else 
		Ctrl_Uart_Send_Time_Data_Times = 0;	//当时间变化或者控制值变化，两者不等的时候，恢复下一次数据发送允许。

	//串口接收的数据处理
		if(((uwTick - Uart_Rev_Data_Delay_Time)<=300)&&(uwTick - Uart_Rev_Data_Delay_Time)>=200)//200ms~300ms之内处理数据
	{
		if(rx_buf_index==6)//接收到了6个数据
		{
			if((rx_buf[0] == 0x6B)&&(rx_buf[1] == 0x30)&&(rx_buf[2] == 0x2E)&&(rx_buf[4] == 0x5C)&&(rx_buf[5] == 0x6E))
			{
				if((rx_buf[3]>=0x31)&&(rx_buf[3]<=0x39))
				{
					k_int = rx_buf[3] - 0x30; 
					sprintf(str, "OK\n");
					HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);		
					iic_24c02_write(&k_int, 0, 1);					
				}
			}			
		}
	  rx_buf_index = 0;	
		Start_Flag = 0;		
	}

}


//串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if((rx_buffer == 0x6B)&&(rx_buf_index == 0))
	{
		Uart_Rev_Data_Delay_Time = uwTick;//接收到第一个数据启动计时		
		Start_Flag = 1;
	}
	if(Start_Flag == 1)
	{		
		rx_buf[rx_buf_index] = rx_buffer;
		rx_buf_index++;		
	}
		
	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 1);
	
}



//基本定时器6更新回调函数
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance==TIM6)
//  {
////		if(++counter == 10)
////		{
////			counter = 0;
////			sprintf(str, "Hello,world.\r\n");
////			HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);	
////		}
//	}
//}


//输入捕获PWM中断回调
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//	  if(htim->Instance==TIM2)
//  {
//				if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//			{
//				PWM_T_Count =  HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1)+1;
//				PWM_Duty = (float)PWM_D_Count/PWM_T_Count;
//			}
//				else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
//			{
//				PWM_D_Count =  HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2)+1;
//			}			
//	}	
//}




//方波输出回调函数
//void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  if(htim->Instance==TIM15)
//  {
//				if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//			{			
//			  __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,(__HAL_TIM_GetCounter(htim)+500));//1Khz
//			}
//	}
//}







/******************************************************************************
/******************************************************************************
/******************************************************************************
/******************************************************************************
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}
