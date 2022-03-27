#include "main.h"
#include "RCC\bsp_rcc.h"
#include "KEY_LED\bsp_key_led.h"
#include "LCD\bsp_lcd.h"
//#include "UART\bsp_uart.h"
#include "I2C\bsp_i2c.h"
//#include "ADC\bsp_adc.h"
#include "TIM\bsp_tim.h"
//#include "RTC\bsp_rtc.h"

//***全局变量声明区
//*减速变量
__IO uint32_t uwTick_Key_Set_Point = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Led_Set_Point = 0;//控制Led_Proc的执行速度
__IO uint32_t uwTick_Lcd_Set_Point = 0;//控制Lcd_Proc的执行速度
//__IO uint32_t uwTick_Usart_Set_Point = 0;//控制Usart_Proc的执行速度

//*按键扫描专用变量
uint8_t ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;

//*LED专用变量
uint8_t ucLed;

//*LCD显示专用变量
uint8_t Lcd_Disp_String[21];//最多显示20个字符

//*串口专用变量
//uint16_t counter = 0;
//uint8_t str[40];
//uint32_t rx_buffer;

//*EEPROM的相关变量
//uint8_t EEPROM_String_1[5] = {0x11,0x22,0x33,0x44,0x55};
//uint8_t EEPROM_String_2[5] = {0};

//*4017相关变量
//uint8_t RES_4017;

//*pwm相关变量
//uint16_t PWM_T_Count;
//uint16_t PWM_D_Count;
//float PWM_Duty;


//*rtc相关变量
//RTC_TimeTypeDef H_M_S_Time;
//RTC_DateTypeDef Y_M_D_Date;


//全局变量
__IO uint32_t uwTick_Long_Short_Judge = 0;//短长按键判断专用
uint8_t ucState;//系统整体运行状态
uint8_t ucHour_Min_Sec[3];//小时分钟秒
uint8_t Storage_Num = 1;//存储序号
__IO uint32_t Sec_Boss = 0;//全局的秒变量
__IO uint32_t uwTick_Sec_Boss = 0;//控制秒的递减
__IO uint32_t uwTick_LED_BULING = 0;//控制LED闪烁


//***子函数声明区
void Key_Proc(void);
void Led_Proc(void);
void Lcd_Proc(void);
//void Usart_Proc(void);


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
	
//	UART1_Init();
	I2CInit();
//  ADC1_Init();
//  ADC2_Init();
//	
//	BASIC_TIM6_Init();
//	PWM_INPUT_TIM2_Init();
//	SQU_OUTPUT_TIM15_Init();
	PWM_OUTPUT_TIM3_Init();
//	PWM_OUTPUT_TIM17_Init();
//	RTC_Init();



	/*外设使用基本配置*/	
	//*EEPROM测试
//	iic_24c02_write(EEPROM_String_1, 0, 5);
//	HAL_Delay(1);	
//	iic_24c02_read(EEPROM_String_2, 0, 5);	
	iic_24c02_read(ucHour_Min_Sec, 0, 3);	
	
	
	
  //*MCP4017测试
//	write_resistor(0x77);
//	RES_4017 = read_resistor();	
	
	//*串口接收中断打开
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 4);
	
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
//		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);	//PA6

		
		
		
		
  while (1)
  {
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
//		Usart_Proc();
		
		
		
  }

}


void B3_Ctrl(void)
{
	switch(ucState)
	{
		case 1:
			if( ++ucHour_Min_Sec[0] >= 24 )
				ucHour_Min_Sec[0] = 0;
			break;
			
		case 2:
			if( ++ucHour_Min_Sec[1] >= 60 )
				ucHour_Min_Sec[1] = 0;
			break;			
			
		case 3:
			if( ++ucHour_Min_Sec[2] >= 60 )
				ucHour_Min_Sec[2] = 0;
			break;			
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
	
	if(unKey_Down)
	{
		uwTick_Long_Short_Judge = uwTick;
	}	
	
	if((uwTick - uwTick_Long_Short_Judge) < 800)//短按识别
	{
		switch(ucKey_Up)
		{
			case 2://如果B2短按
					if( ++ucState == 4)
						ucState = 1;
					
				break;
			
			case 4://如果B4短按
				if(ucState != 4)
				{	
					ucState = 4;
				  Sec_Boss = (__IO uint32_t)(3600*ucHour_Min_Sec[0] + 60*ucHour_Min_Sec[1] +ucHour_Min_Sec[2]);
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6，启动PWM的产生
				}
				else 
				{
					ucLed = 0x00;		//关闭LED闪烁
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1); //关闭PWM生成
					ucState = 5;
				}
			break;
				
			case 3://如果B3短按,时分秒进行+1操作（一会儿写）
				B3_Ctrl();
			break;
			
			case 1://如果B1短按,切换存储位置（一会儿写）
				if(++Storage_Num == 6) Storage_Num = 1;
				iic_24c02_read(ucHour_Min_Sec, (3*(Storage_Num-1)), 3);					
			break;		
	
		
		}
	}
	else //长按
	{
		switch(ucKey_Val)
		{
			case 2://如果B2长按，进入状态0
				if((ucState == 1)||(ucState == 2)||(ucState == 3))
				{				
					iic_24c02_write(ucHour_Min_Sec,(3*(Storage_Num-1)) , 3);			
					ucState = 0;
				}
				break;
			
			case 3://如果B3长按,时分秒连续进行+1操作（一会儿写）
				B3_Ctrl();				
			break;	
			
			case 4://如果B4长按,运行状态下进入状态0
				if(ucState == 4)
				{
					ucLed = 0x00;		//关闭LED闪烁
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1); //关闭PWM生成		
					iic_24c02_read(ucHour_Min_Sec, (3*(Storage_Num-1)), 3);						
					ucState = 0;
				}				
			break;			
		
		
		
		}
	
	
	
	
	}

	
	
	
	
	
	
	
	
}


//***LED扫描子函数
void Led_Proc(void)
{
	if((uwTick -  uwTick_Led_Set_Point)<200)	return;//减速函数
		uwTick_Led_Set_Point = uwTick;
	
	LED_Disp(ucLed);
}


void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<100)	return;//减速函数
		uwTick_Lcd_Set_Point = uwTick;
	
//	//开机屏幕测试代码
//	sprintf((char *)Lcd_Disp_String, "TEST CODE:%02d",ucState);
//	LCD_DisplayStringLine(Line9, Lcd_Disp_String);	
//	
//	//*EEPROM测试
//	sprintf((char *)Lcd_Disp_String, "EE:%x%x%x%x%x ",EEPROM_String_2[0],EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4]);
//	LCD_DisplayStringLine(Line0, Lcd_Disp_String);

	//*ADC测试
//	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",((((float)getADC1())/4096)*3.3));
//	LCD_DisplayStringLine(Line1, Lcd_Disp_String);	
//	
//	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",((((float)getADC2())/4096)*3.3));
//	LCD_DisplayStringLine(Line2, Lcd_Disp_String);	
	
	//*PWM输入捕获测试，测量占空比和频率
//	sprintf((char *)Lcd_Disp_String, "R40P:%05dHz,%4.1f%%",(unsigned int)(1000000/PWM_T_Count),PWM_Duty*100);
//	LCD_DisplayStringLine(Line3, Lcd_Disp_String);		
	
	//*RTC内容显示
//	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
//	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
//	sprintf((char *)Lcd_Disp_String, "Time:%02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
//	LCD_DisplayStringLine(Line4, Lcd_Disp_String);		


//借用位置，进行数值递减操作
// Sec_Boss = (__IO uint32_t)(3600*ucHour_Min_Sec[0] + 60*ucHour_Min_Sec[1] +ucHour_Min_Sec[2]);
	if(ucState == 4)//此状态控制递减操作
	{
		if((uwTick - uwTick_Sec_Boss) >= 1000)//1s到来了，控制倒计时
		{
			uwTick_Sec_Boss = uwTick;
			Sec_Boss--;
			
			ucHour_Min_Sec[0] = Sec_Boss/3600;
			ucHour_Min_Sec[1] = Sec_Boss%3600/60;		
			ucHour_Min_Sec[2] = Sec_Boss%3600%60;		
		}
	
		if((uwTick - uwTick_LED_BULING) >= 500)//0.5s到来了，控制LED闪烁
		{
			uwTick_LED_BULING = uwTick;		
			ucLed ^= 0x01;	
		}
	}





//	if(ucState == 0)//待机
//	{
		sprintf((char *)Lcd_Disp_String, "  No %1d",Storage_Num);
		LCD_DisplayStringLine(Line3, Lcd_Disp_String);	
		
		
		if(ucState == 1)   LCD_SetBackColor(Yellow);
		LCD_DisplayChar(Line5, 224, (ucHour_Min_Sec[0]/10 + 0x30));
		LCD_DisplayChar(Line5, 208, (ucHour_Min_Sec[0]%10 + 0x30));	
		LCD_SetBackColor(White);
		
		LCD_DisplayChar(Line5, 192, ':');	

		if(ucState == 2)   LCD_SetBackColor(Yellow);
		LCD_DisplayChar(Line5, 176, (ucHour_Min_Sec[1]/10 + 0x30));
		LCD_DisplayChar(Line5, 160, (ucHour_Min_Sec[1]%10 + 0x30));	
		LCD_SetBackColor(White);
		
		LCD_DisplayChar(Line5, 144, ':');		
		
		if(ucState == 3)   LCD_SetBackColor(Yellow);
		LCD_DisplayChar(Line5, 128, (ucHour_Min_Sec[2]/10 + 0x30));
		LCD_DisplayChar(Line5, 112, (ucHour_Min_Sec[2]%10 + 0x30));			
		LCD_SetBackColor(White);
	
	
	
	switch(ucState)//仅用来表示状态信息
	{
		case 0:
			sprintf((char *)Lcd_Disp_String, "    Standby     ");
			LCD_DisplayStringLine(Line7, Lcd_Disp_String);			
			break;
		
		case 1:
		case 2:
		case 3:
			sprintf((char *)Lcd_Disp_String, "    Setting     ");
			LCD_DisplayStringLine(Line7, Lcd_Disp_String);		
			break;	
	
		case 4:
			sprintf((char *)Lcd_Disp_String, "    Running     ");
			LCD_DisplayStringLine(Line7, Lcd_Disp_String);			
			break;

		case 5:
			sprintf((char *)Lcd_Disp_String, "    Pause     ");
			LCD_DisplayStringLine(Line7, Lcd_Disp_String);			
			break;
	}










}

//void Usart_Proc(void)
//{
//	if((uwTick -  uwTick_Usart_Set_Point)<1000)	return;//减速函数
//	uwTick_Usart_Set_Point = uwTick;
//	
////	sprintf(str, "%04d:Hello,world.\r\n", counter);
////	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);
//	
////	if(++counter == 10000)
////		counter = 0;
//}


//串口接收中断回调函数
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
////	counter++;
////	sprintf(str, "%04d:Hello,world.\r\n", counter);
////	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);	
////	
////	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 4);

//}



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
